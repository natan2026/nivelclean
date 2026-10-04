# NivelClean — tarefa de implementação e integração para o Codex

## Pedido do proprietário

O usuário solicitou que o Codex execute o trabalho, utilize a internet e configure Firebase, GitHub e Cirkit Designer, deixando o monitoramento utilizável no celular. Não entregar somente um planejamento ou instruções para o usuário programar. Implementar, testar e executar as configurações externas que forem possíveis com acesso legitimamente autorizado. Este arquivo, por si só, não comprova que uma tarefa foi iniciada, que o circuito foi salvo ou que qualquer integração foi validada.

Trabalhar primeiro na simulação. Preparar depois a transferência para o ESP físico pela Arduino IDE. Não adicionar controle de bombas: o escopo é monitoramento.

## Recursos já identificados

- Repositório: https://github.com/natan2026/nivelclean
- Branch desta tarefa: `codex/nivelclean-integracao-completa-20261001`.
- Projeto Firebase: `nivelclean-2bc3b`.
- Realtime Database fornecido pelo usuário: https://nivelclean-2bc3b-default-rtdb.firebaseio.com/
- Caminho de telemetria protegido: `/nivelclean/devices/{DEVICE_UID}/telemetry`.
- Projeto Cirkit Designer citado no README: https://app.cirkitdesigner.com/project/723acf31-be30-40e0-9bad-f043e2df169c
- Página anterior citada no README: https://natan2026.github.io/nivelclean/
- Versão ESP8266 citada no README: https://natan2026.github.io/nivelclean/esp8266/

O README foi conferido pelo conector GitHub em 1º de outubro de 2026. Os links do simulador e das páginas são referências do README, não evidências de que continuam funcionando. Inspecionar seu estado e o código antes de editar.

Arquivos existentes citados no README: `firmware/nivelclean_esp8266/nivelclean_esp8266.ino`, configuração de exemplo, `firebase/esp8266.rules.json`, `firmware/sensor_ultrassonico.ino` e página em `esp8266/`. Ler também as instruções locais, inclusive `AGENTS.md`, se existir. Não apagar as implementações anteriores.

## Acesso e execução externa

1. Verificar quais ferramentas de navegador, GitHub, terminal e Firebase estão realmente disponíveis. Usar integrações oficiais ou sessões explicitamente autorizadas; não supor que a URL do banco concede administração nem que o ambiente herda a sessão Google do usuário.
2. Quando houver navegador autorizado, abrir o projeto correto no Firebase Console e no Cirkit Designer. Aproveitar usuários, aplicativo web e configurações existentes quando adequados, sem duplicar recursos desnecessariamente.
3. Se faltar autenticação, indicar exatamente o serviço que requer login ou aprovação. O usuário deve digitar senhas e códigos de verificação na interface oficial, nunca no chat, no repositório ou em arquivos públicos. Não tentar contornar CAPTCHA, autenticação ou bloqueios de ferramentas.
4. O pedido abrange configurar e publicar o projeto, mas não autoriza contratar planos, inserir dados de pagamento, apagar dados, alterar a visibilidade do repositório, modificar outros projetos ou enfraquecer as proteções. Respeitar as aprovações exigidas pelo ambiente.
5. Trabalhar nesta branch ou em uma branch derivada. Não fazer force push nem merge automático. Apresentar diff e testes. Para publicação que exija integração na branch principal, respeitar a aprovação dessa mudança; explicar essa dependência em vez de declarar o site atualizado sem publicação.
6. Antes de alterar regras publicadas, guardar uma cópia local protegida, executar testes e preparar a migração dos consumidores. Não tornar o banco público para facilitar a simulação. Não expor backups, credenciais ou listas privadas de contas em commits.
7. Se um serviço externo estiver bloqueado, continuar código, testes locais e emuladores. Ao final, separar claramente implementado, testado localmente, publicado, verificado externamente e bloqueado. Não classificar mock como integração real.

## Requisitos funcionais

### Nível e primeiro display

Usar sensor ultrassônico para medir distância e calcular porcentagem de altura útil calibrada, de 0 a 100%. Mostrar a porcentagem e a classificação “Nível baixo”, “Nível médio” ou “Nível alto” no primeiro display e na página. Manter limites configuráveis e documentar valores provisórios. Tratar falta de eco, leituras inválidas, filtro e calibração. Não apresentar um valor antigo como leitura atual após falha.

### Turbidez e segundo display

Incluir o sensor de turbidez e mostrar sua condição no segundo display e na página. O usuário pediu as expressões “caixa limpa” e “caixa suja”; explicar na interface/documentação que a leitura se limita à turbidez, não certifica limpeza da caixa nem potabilidade. Preferir estados técnicos “Baixa turbidez”, “Alta turbidez”, “Não calibrado” e “Falha na leitura”. Não inventar escala NTU, curva do sensor ou limiar sanitário. Separar calibração de nível da calibração de turbidez.

### Página para celular

Criar ou atualizar uma página responsiva em português com login de visualizador, porcentagem, faixa de nível, turbidez, última medição, conexão e aviso de dados antigos. Receber atualizações diretamente do Firebase. GitHub hospeda o código e a página; não criar commits para cada medição. Preservar credenciais do escritor fora do navegador. Diferenciar visualmente simulação de hardware real. Testar a página em largura de celular, com e sem autenticação, conexão interrompida e dados obsoletos. Entregar link publicado apenas após verificá-lo.

## Hardware: o que permanece pendente

O usuário ainda confirmará os modelos exatos do ESP, do ultrassônico, do sensor de turbidez e dos dois displays. O README tem uma implementação NodeMCU ESP8266/HC-SR04 e outra ESP32-S3/LCD I2C; isso é histórico, não confirmação da placa atual. Não perguntar novamente a URL do Firebase ou o repositório, que já estão informados.

Prosseguir com lógica desacoplada de hardware, página, regras, testes e gerador de dados. Para simulação, selecionar componentes cuja execução seja comprovadamente suportada, identificando-os como perfil provisório. Não apresentar uma simulação ESP32-S3 como validação do ESP8266 físico.

O README registra 500 L nominais, altura 100 cm e diâmetro 85 cm. A referência anterior de distância vazio/cheio 110/10 cm não é calibração confirmada. Não equiparar automaticamente porcentagem de altura a volume de um reservatório de geometria desconhecida.

Após confirmação dos componentes, documentar pinagem, tensão de alimentação, terra comum, limites de GPIO/ADC, adaptação de níveis e endereços dos displays. Não ligar sinais de tensão incompatível ao ESP nem pressupor endereços I2C diferentes. Não incluir credenciais reais em projeto de simulação público. Não gravar no hardware antes da identificação e da autorização específica para essa etapa.

## Firebase: regras, autenticação e esquema

A cópia informada pelo usuário está no Anexo A. Ela não foi obtida por leitura administrativa do estado atual. Comparar com o console autorizado e os arquivos existentes antes da migração.

Preservar o modelo de permissão:
- `/nivelclean/writers/{DEVICE_UID}` com booleano `true` autoriza o escritor;
- `/nivelclean/viewers/{VIEWER_UID}/{DEVICE_UID}` com booleano `true` autoriza o leitor;
- o escritor grava somente na telemetria cujo UID coincide com seu `auth.uid`;
- clientes não podem editar as listas nem conceder privilégios a si mesmos;
- provisionamento de contas e autorizações é feito por um administrador legítimo, sem expor segredos.

Implementar e testar:
- migração dos consumidores legados e fechamento de leitura/gravação públicas em `/Dados` e `/distancia`, preservando os dados;
- validação explícita dos novos campos de turbidez e de origem: `telemetry/$other/.validate` atualmente rejeita campos extras; não remover essa proteção;
- esquema versionado comum a firmware, página e testes; estados e tipos coerentes; não fixar faixa de ADC sem definir representação e perfil;
- amostras atômicas; em `sem_eco`, remover valores antigos de distância/nível da amostra atual;
- timestamp em milissegundos, preferencialmente do servidor quando suportado, com teste da janela existente de -60 s/+10 s;
- detecção de amostra antiga na página, independente da validação no momento da escrita;
- proteções contra exclusão indevida e alteração parcial que remova campos obrigatórios;
- usuários distintos para simulação, dispositivo físico e visualização; sem senha de escritor no frontend;
- configuração do aplicativo web e do método de login necessário, usando recursos existentes quando adequados.

A configuração pública de aplicativo web não substitui autenticação e regras. Guardar senhas, refresh tokens, chaves privadas e credenciais administrativas somente nos mecanismos seguros autorizados do ambiente. Fornecer arquivos de exemplo e `.gitignore`; não registrar segredos em logs, URLs ou commits. Usar TLS com verificação adequada; não desabilitar a validação do servidor como atalho.

## Cirkit Designer

Abrir e inspecionar o projeto identificado. Conferir documentação atual de suporte à placa, dois displays, sensores e rede. Um componente existir na biblioteca não comprova que seja simulável.

Montar e salvar no site, quando o acesso permitir, o circuito de simulação com primeiro display para nível e segundo para turbidez. Se o sensor de turbidez não for simulável, usar entrada ajustável ou mock claramente identificado. Isso testa a lógica e a integração, não calibra o sensor real. Não trocar de simulador silenciosamente.

Testar a comunicação autenticada: simulação -> Firebase -> página. Usar identidade de teste e não incluir segredos em projetos compartilhados. Caso essa execução de rede seja incompatível com o simulador, documentar a limitação comprovada e executar os testes independentes possíveis, sem afirmar que o circuito foi validado ponta a ponta.

## Entregáveis e testes mínimos

Entregar código funcional, firmware por perfil, página responsiva, regras completas, testes automatizados, gerador local de telemetria, configuração de exemplo, instruções de montagem/simulação e guia em português para a Arduino IDE. Aproveitar a estrutura do repositório.

Executar testes reproduzíveis de:
- leitura/escrita anônima rejeitadas, inclusive caminhos legados;
- escritor autorizado limitado ao próprio UID, visualizador autorizado somente para leitura e para o dispositivo permitido;
- escritor/visualizador sem autorização rejeitados, inclusive leituras de ancestrais não autorizados;
- impossibilidade de cliente editar listas ou autoatribuir privilégios;
- tipos inválidos, campos inesperados, calibração inconsistente, nível fora de 0-100, timestamp fora da janela e exclusões indevidas rejeitados;
- novos campos válidos de turbidez aceitos e estados inconsistentes rejeitados;
- baixo/médio/alto, sem eco, turbidez, falta de calibração, falha de rede e dados antigos;
- distinção entre simulação e hardware;
- build da página e compilação dos perfis realmente preparados, relatando comandos e resultados.

Em publicação externa autorizada, registrar evidências sanitizadas da gravação pelo dispositivo de teste, leitura pelo visualizador, rejeição de acesso indevido e atualização da página publicada. Não executar testes destrutivos no banco real nem usar o banco de produção como área livre de testes.

Ao encerrar, informar mudanças efetivas, testes e resultados, links realmente verificados e pendências precisas. Não informar “tudo pronto” quando faltarem publicação, acesso, simulação ou validação física. Um bloqueio não deve impedir a entrega do restante.

## Referências para conferir na implementação

- https://firebase.google.com/docs/database/security/core-syntax
- https://firebase.google.com/docs/database/security/rules-conditions
- https://firebase.google.com/docs/emulator-suite
- https://developers.openai.com/codex/integrations/github
- https://help.openai.com/en/articles/20001277-using-the-built-in-browser-in-the-chatgpt-desktop-app

## Anexo A — regras fornecidas pelo usuário, NÃO corrigidas

Esta é uma referência de migração, não uma versão para publicar e não uma confirmação administrativa do estado atual. Nunca republicar esta cópia como solução.

```json
{
  "rules": {
    ".read": false,
    ".write": false,
    "nivelclean": {
      "writers": { ".read": false, ".write": false },
      "viewers": { ".read": false, ".write": false },
      "devices": {
        "$uid": {
          "telemetry": {
            ".read": "auth != null && ((auth.uid === $uid && root.child('nivelclean/writers').child(auth.uid).val() === true) || root.child('nivelclean/viewers').child(auth.uid).child($uid).val() === true)",
            ".write": "auth != null && auth.uid === $uid && root.child('nivelclean/writers').child(auth.uid).val() === true && newData.exists()",
            ".validate": "newData.hasChildren(['status', 'calibrated', 'timestamp', 'rssi', 'version']) && (newData.child('status').val() !== 'ok' || newData.hasChildren(['distanceCm'])) && (newData.child('status').val() !== 'sem_eco' || (!newData.child('distanceCm').exists() && !newData.child('levelPct').exists())) && (newData.child('calibrated').val() !== true || (newData.hasChildren(['emptyCm','fullCm']) && newData.child('emptyCm').val() > newData.child('fullCm').val()))",
            "status": { ".validate": "newData.val() === 'ok' || newData.val() === 'sem_eco'" },
            "calibrated": { ".validate": "newData.isBoolean()" },
            "distanceCm": { ".validate": "newData.isNumber() && newData.val() >= 2 && newData.val() <= 400" },
            "levelPct": { ".validate": "newData.isNumber() && newData.val() >= 0 && newData.val() <= 100 && newData.parent().child('calibrated').val() === true" },
            "emptyCm": { ".validate": "newData.isNumber() && newData.val() >= 2 && newData.val() <= 400" },
            "fullCm": { ".validate": "newData.isNumber() && newData.val() >= 2 && newData.val() <= 400" },
            "rssi": { ".validate": "newData.isNumber() && newData.val() >= -127 && newData.val() <= 0" },
            "version": { ".validate": "newData.isString() && newData.val().length <= 40" },
            "timestamp": { ".validate": "newData.isNumber() && newData.val() >= now - 60000 && newData.val() <= now + 10000" },
            "capacityL": { ".validate": "newData.isNumber() && newData.val() > 0 && newData.val() <= 1000000" },
            "tankHeightCm": { ".validate": "newData.isNumber() && newData.val() > 0 && newData.val() <= 400" },
            "tankDiameterCm": { ".validate": "newData.isNumber() && newData.val() > 0 && newData.val() <= 10000" },
            "$other": { ".validate": false }
          }
        }
      }
    },
    "Dados": { ".read": true, ".write": true },
    "distancia": { ".read": true, ".write": true }
  }
}
```
