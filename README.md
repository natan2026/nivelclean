# NívelClean — nível de água e turbidez

[Abrir o painel no celular](https://natan2026.github.io/nivelclean/) · [Guia de montagem e acesso](https://natan2026.github.io/nivelclean/esp8266/guia.html)

## Nesta versão

- **ESP8266 físico:** HC-SR04 e dois LCD 16×2 I2C, com configuração local de Wi-Fi/Firebase.
- **Cirkit:** ESP32-S3, HC-SR04, LCD de nível e LCD de turbidez. [Cópia do circuito](https://app.cirkitdesigner.com/project/05a1b113-96d5-40af-a1b9-6f9091fa4489).
- Nível em percentual de altura útil: baixo ≤20%, médio >20% e <95%, alto ≥95%.
- Sem eco ou sem calibração nunca significa caixa vazia. A página oculta medições após 45 segundos sem atualização.
- **Turbidez pendente:** o sensor está encomendado e o modelo ainda não foi confirmado. Nenhum valor NTU, ligação analógica ou classificação de água limpa é inventado.
- Página responsiva com demonstração local (não grava no Firebase) e identificação da origem real/simulada.

## Programas e segurança

`firmware/nivelclean_esp8266/`: abra o .ino no Arduino IDE, core ESP8266 3.1.2, ArduinoJson 7.4.2 e LiquidCrystal I2C 1.1.2. Copie config.example.h para nivelclean_config.h apenas localmente; não publique senhas.

`firmware/nivelclean_simulacao/`: ESP32-S3 para o Cirkit, LiquidCrystal I2C 1.1.2. A rede virtual é CirkitWifi. Envio Firebase requer uma sessão temporária enviada pela entrada Serial; nunca coloque tokens ou senhas no código público. O painel prepara comandos de até 91 caracteres, enviados um por vez. A recepção completa da sessão foi verificada no Cirkit; um comando longo era truncado em 128 caracteres.

**Validação de 03/10/2026:** o circuito e os dois displays funcionam, a sessão foi recebida e o Wi-Fi/relógio foram confirmados. A tentativa de publicação ficou presa na conexão HTTPS, com mensagens `esp_task_wdt_reset: task not found`, sem retorno HTTP. A comunicação completa Cirkit → Firebase → painel permanece pendente. A validação de certificado continua habilitada. Consulte [VALIDACAO.md](VALIDACAO.md).

As regras completas em `firebase/esp8266.rules.json` negam acesso anônimo, separam os UIDs de escrita e leitura e fecham `/Dados` e `/distancia`. A revisão não apaga dados anteriores. As regras foram publicadas no projeto NivelClean e o acesso anônimo foi conferido como negado. A raiz do site abre o painel autenticado; o sketch antigo foi desativado.

O administrador cadastra `nivelclean/writers/UID_DA_PLACA=true` e `nivelclean/viewers/UID_DO_PAINEL/UID_DA_PLACA=true`. A conta de visualização não escreve. Não conceda acesso na raiz.

## Ligações físicas propostas — NodeMCU

| Função | GPIO / pino |
|---|---|
| TRIG HC-SR04 | GPIO5 / D1 |
| ECHO HC-SR04 | GPIO4 / D2, divisor 1kΩ/2kΩ |
| SDA dos LCDs | GPIO12 / D6 |
| SCL dos LCDs | GPIO14 / D5 |
| LCD nível / turbidez | 0x27 / 0x26, confirmar adaptadores |

HC-SR04 em 5 V e GND comum. LCDs de 5 V precisam de conversor bidirecional de nível no I2C quando seus pull-ups estão em 5 V. Não aplicar 5 V aos GPIOs. Calibração física permanece desativada até medir a caixa. Os valores 110/10 cm e 500 L são referências antigas, não medições confirmadas.

## Verificação

`npm test` verifica cálculos, limites, falhas, dados antigos, origem, turbidez pendente e integridade da sessão dividida em comandos curtos. O workflow verifica a compilação e as regras no emulador Firebase. A validação no hardware real ainda depende da montagem e da calibração.

Turbidez não comprova potabilidade nem a limpeza do reservatório.
