#pragma once
// Copie este arquivo para nivelclean_config.h na MESMA pasta do .ino.
// Edite somente a copia local. Nunca publique nivelclean_config.h no GitHub.
constexpr char WIFI_SSID[] = "PREENCHA_WIFI_2_4_GHZ";
constexpr char WIFI_PASSWORD[] = "PREENCHA_SENHA_WIFI";
constexpr char FIREBASE_API_KEY[] = "PREENCHA_CHAVE_API_WEB";
// Conta exclusiva da placa, criada em Firebase Authentication (Email/senha).
constexpr char DEVICE_EMAIL[] = "PREENCHA_EMAIL_DA_PLACA";
constexpr char DEVICE_PASSWORD[] = "PREENCHA_SENHA_DA_PLACA";
constexpr char DEVICE_UID[] = "PREENCHA_UID_DA_PLACA";

// Referencias anteriores do projeto: confirme na montagem real. 500 L e a capacidade NOMINAL, nao volume calculado.
constexpr float TANK_HEIGHT_CM = 100.0f;
constexpr float TANK_DIAMETER_CM = 85.0f;
constexpr float NOMINAL_CAPACITY_L = 500.0f;
// Referencia: sensor 10 cm ACIMA do nivel maximo; altura util de 100 cm.
// Confira as distancias reais da FACE DO SENSOR ate o fundo e a agua cheia.
// So depois de conferir, altere CALIBRATED para true.
constexpr float EMPTY_DISTANCE_CM = 110.0f;
constexpr float FULL_DISTANCE_CM = 10.0f;
constexpr bool CALIBRATED = false;

// LCDs: enderecos diferentes, conferir com scanner I2C antes da montagem.
// I2C em D6/D5 para preservar TRIG D1 e ECHO D2 do projeto anterior.
constexpr uint8_t LCD_SDA_PIN = 12; // D6
constexpr uint8_t LCD_SCL_PIN = 14; // D5
constexpr uint8_t LCD_LEVEL_ADDRESS = 0x27;
constexpr uint8_t LCD_TURBIDITY_ADDRESS = 0x26;
