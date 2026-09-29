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

// Dados informados da caixa. 500 L e a capacidade NOMINAL, nao volume calculado.
constexpr float TANK_HEIGHT_CM = 100.0f;
constexpr float TANK_DIAMETER_CM = 85.0f;
constexpr float NOMINAL_CAPACITY_L = 500.0f;
// Referencia: sensor 10 cm ACIMA do nivel maximo; altura util de 100 cm.
// Confira as distancias reais da FACE DO SENSOR ate o fundo e a agua cheia.
// So depois de conferir, altere CALIBRATED para true.
constexpr float EMPTY_DISTANCE_CM = 110.0f;
constexpr float FULL_DISTANCE_CM = 10.0f;
constexpr bool CALIBRATED = false;
