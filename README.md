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
