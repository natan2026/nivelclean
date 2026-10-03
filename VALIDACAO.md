# NivelClean — registro de validação

Atualizado em 03/10/2026.

## Entregas disponíveis

- Painel: https://natan2026.github.io/nivelclean/
- Circuito de teste: https://app.cirkitdesigner.com/project/05a1b113-96d5-40af-a1b9-6f9091fa4489
- Código e guia: https://github.com/natan2026/nivelclean
- Revisão integrada: https://github.com/natan2026/nivelclean/pull/2

O circuito anterior foi preservado. A versão simulada usa ESP32-S3; o programa para a placa física usa ESP8266.

## Conferido

- Compilação dos programas ESP8266 e ESP32-S3 no GitHub Actions.
- Testes da lógica de nível e das regras de acesso no emulador Firebase.
- Publicação do painel no GitHub Pages e apresentação em tela de celular.
- Login no painel com a conta criada pelo usuário: conexão aceita, aguardando a primeira medição.
- Regras publicadas no Firebase; acesso anônimo negado aos caminhos antigos e ao caminho de telemetria. Dados anteriores preservados.
- Simulação compilada e executada no Cirkit com HC-SR04 e dois displays independentes: nível em 0x27 e turbidez em 0x26.
- Divisor do ECHO configurado no circuito com resistores de 1 kΩ e 2 kΩ.
- Leituras locais observadas: aproximadamente 10% a 100 cm, 50% a 60 cm e 100% a 10 cm, usando a calibração ilustrativa 110/10 cm.
- Segundo display apresenta TURBIDEZ / SENSOR PENDENTE.
- Entrada do Monitor Serial verificada com o comando SAIR.
- Identificado truncamento do comando longo em 128 caracteres. Corrigido com protocolo AUTH_BEGIN / AUTH_PART / AUTH_END; confirmada a recepção integral de uma sessão temporária de 924 caracteres.
- Wi-Fi conectado e relógio atual conferidos com STATUS. O painel prepara mensagens menores que o limite do UART virtual.

## Ainda não concluído

- Teste completo Cirkit → Firebase → painel: a sessão foi transferida, mas a tentativa HTTPS ficou presa no simulador, com mensagens esp_task_wdt_reset: task not found e sem retorno HTTP. Ainda não houve gravação de telemetria confirmada pelo circuito. Não foi desativada a verificação do certificado.
- Conta independente de visualização: o código e as regras já a suportam, mas o acesso testado até aqui utiliza a conta dedicada ao dispositivo.
- Sensor de turbidez: modelo, ligação e calibração aguardam a confirmação do componente.
- Montagem física, endereços dos adaptadores LCD e calibração do reservatório precisam ser verificados no hardware.

## Retomar o teste de comunicação

1. No painel autenticado com a conta do dispositivo, abrir Conectar ao Firebase e clicar em Preparar sessão do simulador.
2. Copiar cada comando em ordem, colar somente no campo de entrada do Monitor Serial e clicar em Enviar. Avançar no painel para o próximo comando; não enviar a lista inteira de uma vez. Não colar no editor, na conversa da IA ou no repositório.
3. Confirmar a mensagem de sessão recebida e o retorno Firebase HTTP 200 ou 204. Aguardar uma atualização no painel, que deve identificar a origem como simulação.
4. Alterar a distância no HC-SR04 e verificar a mudança no display e no painel.
5. Interromper a simulação e verificar o aviso de dado desatualizado após 45 segundos sem uma medição nova.

A sessão temporária permanece apenas na execução e expira em até uma hora. O Console Serial foi limpo após os testes. Não há senha ou token neste documento nem no código público. Turbidez, quando implementada, não comprova potabilidade.
