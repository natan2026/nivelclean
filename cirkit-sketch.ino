// NivelClean: ESP32-S3 SOMENTE para simulacao no Cirkit.
// Dois LCDs I2C: nivel 0x27 e turbidez 0x26. Nao inserir senhas neste arquivo.
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

#include <pgmspace.h>
// Raizes publicas Google Trust Services; extraidas do trust store do sistema.
// Fonte oficial para renovacao: https://pki.goog/repository/
// Verificar/renovar se o Google alterar a cadeia TLS.
static const char FIREBASE_ROOTS[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw
MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp
Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA
A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo
27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w
Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw
TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl
qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH
szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8
Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk
MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92
wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p
aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN
VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID
AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E
FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb
C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe
QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy
h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4
7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J
ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef
MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/
Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT
6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ
0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm
2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb
bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPluILrIPglJ209ZjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjMwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjMwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AAQfTzOHMymKoYTey8chWEGJ6ladK0uFxh1MJ7x/JlFyb+Kf1qPKzEUURout736G
jOyxfi//qXGdGIRFBEFVbivqJn+7kAHjSxm65FSWRQmx1WyRRK2EE46ajA2ADDL2
4CejQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBTB8Sa6oC2uhYHP0/EqEr24Cmf9vDAKBggqhkjOPQQDAwNpADBmAjEA9uEglRR7
VKOQFhG/hMjqb2sXnh5GmCCbn9MN2azTL818+FsuVbu/3ZL3pAzcMeGiAjEA/Jdm
ZuVDFhOD3cffL74UOO0BzrEXGhF16b0DjyZ+hOXJYKaV11RZt+cRLInUue4X
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)CERT";


const uint8_t TRIG_PIN=4, ECHO_PIN=5, SDA_PIN=8, SCL_PIN=9;
const float EMPTY_CM=110.0f, FULL_CM=10.0f; // EXEMPLOS da simulacao, nao da caixa real.
LiquidCrystal_I2C lcdLevel(0x27,16,2), lcdTurbidity(0x26,16,2);
String deviceUid, sessionToken, command;
String stagedUid, stagedToken;
uint32_t lastRead=0, lastPublish=0, tokenStart=0;
float lastCm=NAN;
const char* DB="https://nivelclean-2bc3b-default-rtdb.firebaseio.com";

void line(LiquidCrystal_I2C &lcd,uint8_t row,const String &text) {
  lcd.setCursor(0,row);
  for(uint8_t i=0;i<16;i++) lcd.print(i<text.length()?text[i]:' ');
}
float measure() {
  float values[5]; uint8_t n=0;
  for(uint8_t i=0;i<5;i++) {
    digitalWrite(TRIG_PIN,LOW);delayMicroseconds(2);
    digitalWrite(TRIG_PIN,HIGH);delayMicroseconds(10);digitalWrite(TRIG_PIN,LOW);
    unsigned long pulse=pulseIn(ECHO_PIN,HIGH,30000UL);
    float cm=pulse*0.0343f/2;
    if(pulse>0 && cm>=2 && cm<=400) values[n++]=cm;
    delay(65);
  }
  if(n<3) return NAN;
  for(uint8_t i=1;i<n;i++){float v=values[i];int j=i-1;while(j>=0 && values[j]>v){values[j+1]=values[j];j--;}values[j+1]=v;}
  return n%2?values[n/2]:(values[n/2-1]+values[n/2])/2;
}
float percent(float cm){return constrain(100.0f*(EMPTY_CM-cm)/(EMPTY_CM-FULL_CM),0.0f,100.0f);}
void display(float cm) {
  line(lcdTurbidity,0,"TURBIDEZ");line(lcdTurbidity,1,"SENSOR PENDENTE");
  if(!isfinite(cm)){line(lcdLevel,0,"Nivel: --%");line(lcdLevel,1,"SEM LEITURA");Serial.println("SIMULACAO | SEM LEITURA");return;}
  const float p=percent(cm);
  line(lcdLevel,0,"Nivel: "+String(p,0)+"%");
  line(lcdLevel,1,p<=20?"NIVEL BAIXO":p>=95?"NIVEL ALTO":"NIVEL MEDIO");
  Serial.printf("SIMULACAO | %.1f cm | %.1f %% | turbidez pendente\n",cm,p);
}
bool safeUid(const String &uid){
  if(uid.length()==0 || uid.length()>128)return false;
  for(size_t i=0;i<uid.length();i++){char c=uid[i];if(!isalnum(c)&&c!='_'&&c!='-')return false;}return true;
}
// Credencial de sessao recebida SO durante a execucao; nao e salva no sketch/NVS.
// O painel autenticado copia AUTH <uid> <idToken>. Expira em ate uma hora.
void readCommand(){
  while(Serial.available()){
    char c=Serial.read();
    if(c=='\r')continue;
    if(c=='\n' || c==';'){
      Serial.printf("Serial: %u caracteres recebidos\n", unsigned(command.length()));
      if(command.startsWith("TIME ")){
        long long epoch=command.substring(5).toInt();
        if(epoch>=1767225600LL && epoch<2145916800LL){struct timeval tv={};tv.tv_sec=epoch;settimeofday(&tv,nullptr);Serial.println("Relogio da simulacao ajustado.");}
      }
      else if(command=="STATUS"){Serial.printf("Wi-Fi: %d | relogio: %lld\n",int(WiFi.status()),(long long)time(nullptr));}
      else if(command.startsWith("AUTH_BEGIN ")){
        stagedUid=command.substring(11);stagedToken="";
        if(!safeUid(stagedUid))stagedUid="";
        Serial.println(stagedUid.isEmpty()?"UID invalido.":"Inicio da sessao recebido.");
      }
      else if(command.startsWith("AUTH_PART ")){
        String part=command.substring(10);
        if(!stagedUid.isEmpty() && part.length()>0 && part.length()<=80 && stagedToken.length()+part.length()<4000){
          stagedToken+=part;Serial.printf("Sessao: %u caracteres acumulados\n",unsigned(stagedToken.length()));
        }else {stagedUid="";stagedToken="";Serial.println("Parte invalida. Reinicie a sessao.");}
      }
      else if(command=="AUTH_END"){
        if(safeUid(stagedUid)&&stagedToken.length()>100){deviceUid=stagedUid;sessionToken=stagedToken;tokenStart=millis();Serial.println("Sessao recebida. Aguardando Wi-Fi e NTP.");}
        else Serial.println("Sessao incompleta. Reinicie.");
        stagedUid="";stagedToken="";
      }
      else if(command=="SAIR"){sessionToken="";deviceUid="";stagedUid="";stagedToken="";Serial.println("Sessao encerrada.");}
      else if(command.startsWith("AUTH ")){
        int split=command.indexOf(' ',5);
        String uid=split>5?command.substring(5,split):"";
        String token=split>5?command.substring(split+1):"";
        if(safeUid(uid)&&token.length()>100&&token.length()<4000){deviceUid=uid;sessionToken=token;tokenStart=millis();Serial.println("Sessao recebida. Aguardando Wi-Fi e NTP.");}
        else Serial.println("Comando de sessao invalido.");
      }
      command="";
    }else if(command.length()<4200)command+=c;
    else command="";
  }
}
void publish(float cm){
  if(sessionToken.isEmpty())return;
  Serial.printf("Publicando: sessao com %u caracteres\n",unsigned(sessionToken.length()));
  if(uint32_t(millis()-tokenStart)>3600000UL){sessionToken="";Serial.println("Sessao expirou. Copie uma nova pelo painel.");return;}
  if(WiFi.status()!=WL_CONNECTED){Serial.println("Aguardando Wi-Fi.");return;}
  if(time(nullptr)<1767225600){Serial.println("Aguardando relogio. Envie TIME com o horario atual pelo Serial.");return;}
  // Recolhe nova amostra imediatamente antes de enviar: nao renova dado antigo.
  const bool valid=isfinite(cm);
  String body="{\"status\":\""+String(valid?"ok":"sem_eco")+"\",\"calibrated\":true,\"emptyCm\":110,\"fullCm\":10,\"timestamp\":{\".sv\":\"timestamp\"},\"rssi\":"+String(WiFi.RSSI())+",\"version\":\"simulacao-2.0.0\",\"source\":\"simulacao\",\"turbidity\":{\"status\":\"not_configured\"}";
  if(valid)body+=",\"distanceCm\":"+String(cm,1)+",\"levelPct\":"+String(percent(cm),1);
  body+="}";
  WiFiClientSecure client;client.setCACert(FIREBASE_ROOTS);client.setHandshakeTimeout(8);
  HTTPClient http;http.setTimeout(8000);http.setConnectTimeout(8000);http.useHTTP10(true);
  if(!http.begin(client,String(DB)+"/nivelclean/devices/"+deviceUid+"/telemetry.json?auth="+sessionToken+"&print=silent")){Serial.println("Falha ao iniciar HTTPS.");return;}
  http.addHeader("Content-Type","application/json");
  int code=http.PUT(body);http.end();Serial.printf("Firebase HTTP %d\n",code);
  if(code==401||code==403){sessionToken="";Serial.println("Confira permissao do UID ou renove a sessao no painel.");}
}
void setup(){
  // A sessao Firebase excede o buffer UART padrao de 256 bytes.
  Serial.setRxBufferSize(8192);
  Serial.begin(115200);pinMode(TRIG_PIN,OUTPUT);pinMode(ECHO_PIN,INPUT);
  Wire.begin(SDA_PIN,SCL_PIN);lcdLevel.init();lcdLevel.backlight();lcdTurbidity.init();lcdTurbidity.backlight();
  display(NAN);
  WiFi.begin("CirkitWifi","");configTime(0,0,"time.google.com","pool.ntp.org");
  Serial.println("SIMULACAO NivelClean. Teste local ativo; Firebase aguarda sessao AUTH pelo Serial.");
  lastRead=millis()-1000;lastPublish=millis();
}
void loop(){
  readCommand();
  if(uint32_t(millis()-lastRead)>=1000){lastRead=millis();lastCm=measure();display(lastCm);
    if(uint32_t(millis()-lastPublish)>=10000){lastPublish=millis();publish(lastCm);}
  }
  delay(10);
}
