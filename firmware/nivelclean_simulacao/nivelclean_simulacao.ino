// NivelClean: ESP32-S3 SOMENTE para simulacao no Cirkit.
// Dois LCDs I2C: nivel 0x27 e turbidez 0x26. Nao inserir senhas neste arquivo.
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include <math.h>
#include "firebase_roots.h"

const uint8_t TRIG_PIN=4, ECHO_PIN=5, SDA_PIN=8, SCL_PIN=9;
const float EMPTY_CM=110.0f, FULL_CM=10.0f; // EXEMPLOS da simulacao, nao da caixa real.
LiquidCrystal_I2C lcdLevel(0x27,16,2), lcdTurbidity(0x26,16,2);
String deviceUid, sessionToken, command;
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
    if(c=='\n'){
      if(command=="SAIR"){sessionToken="";deviceUid="";Serial.println("Sessao encerrada.");}
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
  if(uint32_t(millis()-tokenStart)>3600000UL){sessionToken="";Serial.println("Sessao expirou. Copie uma nova pelo painel.");return;}
  if(WiFi.status()!=WL_CONNECTED||time(nullptr)<1767225600)return;
  // Recolhe nova amostra imediatamente antes de enviar: nao renova dado antigo.
  const bool valid=isfinite(cm);
  String body="{\"status\":\""+String(valid?"ok":"sem_eco")+"\",\"calibrated\":true,\"emptyCm\":110,\"fullCm\":10,\"timestamp\":{\".sv\":\"timestamp\"},\"rssi\":"+String(WiFi.RSSI())+",\"version\":\"simulacao-2.0.0\",\"source\":\"simulacao\",\"turbidity\":{\"status\":\"not_configured\"}";
  if(valid)body+=",\"distanceCm\":"+String(cm,1)+",\"levelPct\":"+String(percent(cm),1);
  body+="}";
  WiFiClientSecure client;client.setCACert(FIREBASE_ROOTS);
  HTTPClient http;http.setTimeout(8000);http.useHTTP10(true);
  if(!http.begin(client,String(DB)+"/nivelclean/devices/"+deviceUid+"/telemetry.json?auth="+sessionToken+"&print=silent"))return;
  http.addHeader("Content-Type","application/json");
  int code=http.PUT(body);http.end();Serial.printf("Firebase HTTP %d\n",code);
  if(code==401||code==403){sessionToken="";Serial.println("Confira permissao do UID ou renove a sessao no painel.");}
}
void setup(){
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
