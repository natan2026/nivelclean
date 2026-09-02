// Nível Clean - ESP32-S3 + HC-SR04 + LCD 16x2 I2C + Firebase
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* WIFI_SSID = "CirkitWifi";
const char* WIFI_PASSWORD = "";
const char* FIREBASE_URL =
  "https://nivelclean-2bc3b-default-rtdb.firebaseio.com/distancia.json";

// Ligações do HC-SR04
const uint8_t TRIG_PIN = 4;
const uint8_t ECHO_PIN = 5;

// Ligações I2C do LCD
const uint8_t SDA_PIN = 8;
const uint8_t SCL_PIN = 9;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long ultimaLeitura = 0;
unsigned long ultimoEnvio = 0;
float distanciaCm = 0;

float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Limite de 30 ms: aproximadamente 5 metros
  unsigned long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) return -1;

  return duracao * 0.0343f / 2.0f;
}

void mostrarNoDisplay(float cm) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DISTANCIA:");

  lcd.setCursor(0, 1);
  if (cm < 0) {
    lcd.print("SEM LEITURA");
  } else {
    lcd.print(cm, 1);
    lcd.print(" cm");
  }
}

bool enviarFirebase(float cm) {
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  if (!http.begin(FIREBASE_URL)) return false;
  http.addHeader("Content-Type", "application/json");

  int codigo = http.PUT(String(cm, 1));
  String resposta = codigo > 0 ? http.getString() : "";
  http.end();

  Serial.print("Firebase HTTP: ");
  Serial.print(codigo);
  Serial.print(" | resposta: ");
  Serial.println(resposta);

  return codigo >= 200 && codigo < 300;
}

void conectarWiFi() {
  lcd.clear();
  lcd.print("CONECTANDO WIFI");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long inicio = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - inicio < 15000) {
    delay(300);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print(WiFi.status() == WL_CONNECTED ? "WIFI CONECTADO" : "WIFI OFFLINE");
  delay(800);
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("NIVEL CLEAN");
  delay(900);

  conectarWiFi();
}

void loop() {
  unsigned long agora = millis();

  // Atualiza sensor e LCD 5 vezes por segundo
  if (agora - ultimaLeitura >= 200) {
    ultimaLeitura = agora;
    float novaLeitura = medirDistancia();

    if (novaLeitura >= 2 && novaLeitura <= 400) {
      // Filtro simples para deixar a leitura mais estável
      distanciaCm = distanciaCm == 0
        ? novaLeitura
        : distanciaCm * 0.7f + novaLeitura * 0.3f;

      Serial.print("Distancia: ");
      Serial.print(distanciaCm, 1);
      Serial.println(" cm");
      mostrarNoDisplay(distanciaCm);
    } else {
      mostrarNoDisplay(-1);
    }
  }

  // Envia ao Firebase uma vez por segundo
  if (distanciaCm > 0 && agora - ultimoEnvio >= 1000) {
    ultimoEnvio = agora;

    if (WiFi.status() != WL_CONNECTED) conectarWiFi();
    enviarFirebase(distanciaCm);
  }
}