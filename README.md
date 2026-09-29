# ESP8266 + HC-SR04 — caixa de 500 L

A versão NodeMCU ESP8266 está em **[esp8266/](https://natan2026.github.io/nivelclean/esp8266/)**.

- **[Guia completo de instalação, ligação e Firebase](https://natan2026.github.io/nivelclean/esp8266/guia.html)**
- Firmware: [`firmware/nivelclean_esp8266/nivelclean_esp8266.ino`](firmware/nivelclean_esp8266/nivelclean_esp8266.ino)
- Copie `config.example.h` para `nivelclean_config.h` localmente e preencha Wi-Fi e conta Firebase. Não publique o arquivo com senhas.
- NodeMCU: TRIG em **D1/GPIO5**; ECHO em **D2/GPIO4 com divisor 1 kΩ / 2 kΩ**; HC-SR04 em 5 V e GND comum.
- Caixa: **500 L nominais, 100 cm de altura, 85 cm de diâmetro**. Referência de calibração: sensor 10 cm acima do máximo; vazio 110 cm / cheio 10 cm. Confira na instalação antes de ativar `CALIBRATED`.
- Firebase Authentication por e-mail/senha; regras em [`firebase/esp8266.rules.json`](firebase/esp8266.rules.json). Estas regras completas não autorizam o antigo `/distancia`; revise a migração conforme o guia.
- TLS com raízes Google e horário NTP, mediana de 5 leituras, publicação a cada 10 s, estado sem eco e detecção de leitura antiga.
- A porcentagem indica altura útil. Não convertemos em litros medidos: um cilindro ideal de 100 × Ø85 cm comportaria aproximadamente 567 L.
- Sem acesso físico à placa, o teste final de Wi-Fi, TLS, Firebase e sensor deve ser feito na instalação.

## Projeto anterior ESP32-S3

O painel original na raiz e o firmware anterior foram preservados. A documentação anterior está abaixo.

---

# Nível Clean — Sensor ultrassônico com Firebase

Projeto com **ESP32-S3**, sensor ultrassônico **HC-SR04** e display **LCD 16×2 I²C**. A distância aparece no display do circuito e também no painel publicado pelo GitHub Pages.

## Fluxo

```text
HC-SR04 → ESP32-S3 → LCD 16×2
                  └→ Firebase /distancia → painel GitHub Pages
```

- Painel: https://natan2026.github.io/nivelclean/
- Firebase: `https://nivelclean-2bc3b-default-rtdb.firebaseio.com/distancia.json`
- Circuit Designer: https://app.cirkitdesigner.com/project/723acf31-be30-40e0-9bad-f043e2df169c
- Firmware: `firmware/sensor_ultrassonico.ino`

## Ligações

| Componente | Pino | ESP32-S3 |
|---|---|---|
| HC-SR04 | VCC | 5V |
| HC-SR04 | GND | GND |
| HC-SR04 | TRIG | GPIO 4 |
| HC-SR04 | ECHO | GPIO 5 |
| LCD I²C | VCC | 5V |
| LCD I²C | GND | GND |
| LCD I²C | SDA | GPIO 8 |
| LCD I²C | SCL | GPIO 9 |

> Em montagem física, o ECHO do HC-SR04 pode chegar a 5 V. Use divisor resistivo ou conversor de nível antes do GPIO 5 do ESP32-S3. Na simulação, siga o comportamento do componente do Cirkit Designer.

## Funcionamento

- O sensor é lido a cada 200 ms.
- O LCD mostra a distância com uma casa decimal.
- O Firebase recebe a distância uma vez por segundo.
- O painel web consulta `/distancia.json` uma vez por segundo.
- Faixa considerada válida: 2 a 400 cm.
- Um filtro simples reduz oscilações da leitura.

## Bibliotecas

- `WiFi.h`
- `HTTPClient.h`
- `Wire.h`
- `LiquidCrystal_I2C.h`

## Segurança

O endpoint REST está no navegador. Para protótipos e simulação, regras públicas podem funcionar. Para equipamento real, utilize Firebase Authentication e regras que aceitem somente usuários/dispositivos autorizados. Não publique senhas ou tokens administrativos.

