// Firmware legado desativado: enviava sem autenticacao a /distancia.
// Use nivelclean_simulacao/nivelclean_simulacao.ino para ESP32-S3
// ou nivelclean_esp8266/nivelclean_esp8266.ino para a placa fisica.
void setup(){Serial.begin(115200);Serial.println("Use o firmware NivelClean autenticado. Consulte README.md.");}
void loop(){delay(1000);}
