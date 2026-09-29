/* Nivel Clean — NodeMCU ESP8266 + HC-SR04 + Firebase Realtime Database.
 * Arduino IDE 2.3.7; placa NodeMCU 1.0 (ESP-12E Module).
 * Core ESP8266 3.1.2; biblioteca ArduinoJson 7.x.
 * TRIG: D1/GPIO5. ECHO: D2/GPIO4 ATRAVES DE DIVISOR 1k/2k.
 * Nao conecte ECHO 5 V diretamente ao ESP8266.
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <math.h>
#include "firebase_roots.h"
#if __has_include("config.h")
#include "config.h"
#else
#include "config.example.h"
#endif

constexpr uint8_t TRIG_PIN = 5; // D1 na NodeMCU
constexpr uint8_t ECHO_PIN = 4; // D2 na NodeMCU
constexpr uint32_t CYCLE_MS = 10000;
constexpr char DATABASE_URL[] = "https://nivelclean-2bc3b-default-rtdb.firebaseio.com";
BearSSL::X509List trustAnchors(FIREBASE_ROOTS);
String idToken, refreshToken;
uint32_t tokenStarted = 0, tokenLifetimeMs = 0, lastAuthTry = 0;
uint32_t lastCycle = 0, lastWifiTry = 0;
bool configured = false, triedAuth = false;

bool clockReady() { return time(nullptr) >= 1767225600; } // 01/01/2026 UTC
bool calibrationReady() {
  return CALIBRATED && isfinite(EMPTY_DISTANCE_CM) && isfinite(FULL_DISTANCE_CM)
    && FULL_DISTANCE_CM >= 2 && EMPTY_DISTANCE_CM <= 400
    && EMPTY_DISTANCE_CM > FULL_DISTANCE_CM;
}
bool placeholder(const char* value) { return strlen(value) == 0 || String(value).startsWith("PREENCHA"); }
bool validUid() {
  const size_t n = strlen(DEVICE_UID);
  if (n == 0 || n > 128) return false;
  for (size_t i = 0; i < n; ++i) {
    const char c = DEVICE_UID[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
  }
  return true;
}
String formEncode(const String& input) {
  const char hex[] = "0123456789ABCDEF";
  String output; output.reserve(input.length() * 3);
  for (size_t i=0;i<input.length();i++) {
    const uint8_t c = static_cast<uint8_t>(input[i]);
    if ((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~') output += char(c);
    else { output += '%'; output += hex[c>>4]; output += hex[c&15]; }
  }
  return output;
}

// HTTPS com validacao de certificado e horario NTP. Nao usa setInsecure().
int httpsRequest(const String& url, const char* method, const char* contentType, const String& body, String& response) {
  response = "";
  if (WiFi.status() != WL_CONNECTED || !clockReady()) return -100;
  BearSSL::WiFiClientSecure client;
  client.setTrustAnchors(&trustAnchors);
  client.setTimeout(12000);
  client.setHandshakeTimeout(12);
  HTTPClient http;
  http.setTimeout(12000);
  http.setReuse(false);
  http.useHTTP10(true);
  if (!http.begin(client, url)) return -101;
  http.addHeader("Content-Type", contentType);
  int code = http.sendRequest(method, body);
  if (code > 0 && code != 204) response = http.getString();
  http.end();
  // Nao imprimir URL, payload, resposta de login, senha ou token.
  return code;
}

bool authenticate() {
  if (idToken.length() && uint32_t(millis()-tokenStarted) < tokenLifetimeMs) return true;
  if (triedAuth && uint32_t(millis()-lastAuthTry) < 30000) return false;
  triedAuth = true; lastAuthTry = millis();
  const bool refreshing = refreshToken.length() > 0;
  String url, body, response;
  if (refreshing) {
    url = String("https://securetoken.googleapis.com/v1/token?key=") + FIREBASE_API_KEY;
    body = "grant_type=refresh_token&refresh_token=" + formEncode(refreshToken);
  } else {
    url = String("https://identitytoolkit.googleapis.com/v1/accounts:signInWithPassword?key=") + FIREBASE_API_KEY;
    JsonDocument payload;
    payload["email"] = DEVICE_EMAIL;
    payload["password"] = DEVICE_PASSWORD;
    payload["returnSecureToken"] = true;
    serializeJson(payload, body);
  }
  const int code = httpsRequest(url, "POST", refreshing ? "application/x-www-form-urlencoded" : "application/json", body, response);
  body = "";
  if (code != 200) {
    Serial.printf("Autenticacao HTTP %d. Confira API key, Email/senha, internet e horario.\n", code);
    if (code == 400 || code == 401 || code == 403) { idToken = ""; refreshToken = ""; }
    return false;
  }
  JsonDocument result;
  if (deserializeJson(result, response)) { Serial.println(F("Resposta de autenticacao invalida.")); return false; }
  const String uid = result[refreshing ? "user_id" : "localId"].as<String>();
  if (uid != DEVICE_UID) {
    Serial.println(F("UID diferente da conta autenticada. Corrija DEVICE_UID no config.h."));
    idToken = ""; refreshToken = ""; return false;
  }
  idToken = result[refreshing ? "id_token" : "idToken"].as<String>();
  refreshToken = result[refreshing ? "refresh_token" : "refreshToken"].as<String>();
  const long expires = result[refreshing ? "expires_in" : "expiresIn"].as<long>();
  if (idToken.isEmpty() || refreshToken.isEmpty() || expires < 120) {
    idToken = ""; refreshToken = ""; Serial.println(F("Sessao invalida.")); return false;
  }
  tokenStarted = millis();
  tokenLifetimeMs = uint32_t(expires > 3600 ? 3540 : expires - 60) * 1000UL;
  Serial.println(F("Firebase autenticado."));
  return true;
}

float measureDistance() {
  float samples[5]; uint8_t count = 0;
  for (uint8_t i=0;i<5;i++) {
    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    const unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
    const float cm = duration * 0.0343f / 2.0f;
    if (duration > 0 && cm >= 2.0f && cm <= 400.0f) samples[count++] = cm;
    delay(65); // Pausa entre pulsos; tambem atende o Wi-Fi/watchdog.
  }
  if (count < 3) return NAN;
  for (uint8_t i=1;i<count;i++) {
    float value=samples[i]; int j=i-1;
    while (j>=0 && samples[j]>value) {samples[j+1]=samples[j];j--;}
    samples[j+1]=value;
  }
  return count%2 ? samples[count/2] : (samples[count/2-1]+samples[count/2])/2.0f;
}

void publishReading(float cm) {
  JsonDocument data;
  const bool valid = isfinite(cm), calibrated = calibrationReady();
  data["status"] = valid ? "ok" : "sem_eco";
  data["calibrated"] = calibrated;
  data["timestamp"][".sv"] = "timestamp"; // Horario do servidor Firebase.
  data["rssi"] = WiFi.RSSI();
  data["version"] = "esp8266-1.0.0";
  data["tankHeightCm"] = TANK_HEIGHT_CM;
  data["tankDiameterCm"] = TANK_DIAMETER_CM;
  data["capacityL"] = NOMINAL_CAPACITY_L;
  if (calibrated) { data["emptyCm"] = EMPTY_DISTANCE_CM; data["fullCm"] = FULL_DISTANCE_CM; }
  if (valid) {
    data["distanceCm"] = roundf(cm * 10.0f) / 10.0f;
    if (calibrated) {
      const float pct = constrain(100.0f*(EMPTY_DISTANCE_CM-cm)/(EMPTY_DISTANCE_CM-FULL_DISTANCE_CM),0.0f,100.0f);
      data["levelPct"] = roundf(pct * 10.0f) / 10.0f;
    }
  }
  String body, response; serializeJson(data, body);
  const String url = String(DATABASE_URL) + "/nivelclean/devices/" + DEVICE_UID + "/telemetry.json?auth=" + formEncode(idToken) + "&print=silent";
  // PUT troca o snapshot inteiro. Uma falha remove a distancia anterior.
  const int code = httpsRequest(url, "PUT", "application/json", body, response);
  Serial.printf("Firebase HTTP %d%s\n", code, code==200||code==204 ? " - medicao enviada" : " - envio falhou");
  if (code==401||code==403) {idToken="";Serial.println(F("Confira as regras do banco e a permissao do UID da placa."));}
}

void setup() {
  Serial.begin(115200); delay(200);
  pinMode(TRIG_PIN, OUTPUT); digitalWrite(TRIG_PIN, LOW); pinMode(ECHO_PIN, INPUT);
  Serial.println(F("\nNivel Clean | ESP8266 | TRIG D1/GPIO5 | ECHO D2/GPIO4 com divisor"));
  configured = !placeholder(WIFI_SSID) && !placeholder(FIREBASE_API_KEY) && !placeholder(DEVICE_EMAIL)
    && !placeholder(DEVICE_PASSWORD) && !placeholder(DEVICE_UID) && validUid();
  if (!configured) Serial.println(F("Preencha config.h para ativar Wi-Fi/Firebase. Medicao local continua no Serial."));
  if (!calibrationReady()) Serial.println(F("Percentual desativado: configure distancias vazio/cheio e CALIBRATED=true."));
  if (configured) {
    WiFi.persistent(false); WiFi.mode(WIFI_STA); WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); lastWifiTry = millis();
    configTime(0,0,"time.google.com","pool.ntp.org","time.cloudflare.com");
  }
  lastCycle = millis() - CYCLE_MS;
}

void loop() {
  if (uint32_t(millis()-lastCycle) < CYCLE_MS) {delay(10);return;}
  lastCycle = millis();
  bool ready = false;
  if (configured) {
    if (WiFi.status()!=WL_CONNECTED) {
      Serial.println(F("Wi-Fi desconectado. Aguardando reconexao na rede 2,4 GHz."));
      if (uint32_t(millis()-lastWifiTry)>=30000) {WiFi.reconnect();lastWifiTry=millis();}
    } else if (!clockReady()) Serial.println(F("Aguardando horario NTP para validar HTTPS."));
    else ready = authenticate();
  }
  const float cm = measureDistance();
  if (isfinite(cm)) Serial.printf("Distancia: %.1f cm\n",cm);
  else Serial.println(F("Sem eco valido. Verifique sensor, divisor e posicionamento."));
  if (ready) publishReading(cm);
  yield();
}
