# Nível Clean — Circuit Designer + Firebase + GitHub

Painel web para controlar e acompanhar o LED do projeto **Nível Clean**.

## Como funciona

```text
Painel no GitHub Pages  ⇄  Firebase Realtime Database  ⇄  Circuit Designer
                           /led.json
```

O Firebase é o intermediário em tempo real. O GitHub guarda e publica o painel; ele não substitui o banco de dados.

- Firebase: `https://nivelclean-2bc3b-default-rtdb.firebaseio.com/led.json`
- Circuit Designer: `https://app.cirkitdesigner.com/project/723acf31-be30-40e0-9bad-f043e2df169c`
- Painel: será disponibilizado pelo GitHub Pages.

O painel consulta o valor a cada 2 segundos e grava com uma requisição REST `PUT`. Ele reconhece valores booleanos, `0/1`, `"0"/"1"`, `"ON"/"OFF"` e `"ligado"/"desligado"`.

## Publicar o painel

1. Abra **Settings → Pages** neste repositório.
2. Em **Build and deployment**, selecione **GitHub Actions**.
3. Abra a aba **Actions** e acompanhe a execução “Publicar painel no GitHub Pages”.
4. Ao finalizar, o endereço esperado é:
   `https://natan2026.github.io/nivelclean/`

## Código mínimo no Circuit Designer

O programa do circuito deve ler e/ou escrever o mesmo nó:

```cpp
const char* firebaseUrl =
  "https://nivelclean-2bc3b-default-rtdb.firebaseio.com/led.json";
```

Leitura esperada:
- `1` liga o LED;
- `0` desliga o LED.

Se o código atual já consulta esse endereço, nenhuma troca de URL é necessária.

## Segurança importante

O painel usa o endpoint REST diretamente no navegador. Para protótipo/simulação, regras públicas podem funcionar, mas não são recomendadas para um equipamento real. Em produção, habilite Firebase Authentication e regras que exijam usuário autenticado. Nunca coloque token administrativo, senha ou chave privada neste repositório.

## Diagnóstico

- **HTTP 401/403:** as regras do Firebase não permitem a operação.
- **Painel muda, circuito não:** o circuito não está consultando exatamente `/led.json` ou usa outro formato.
- **Circuito muda, painel não:** confirme que a gravação ocorre no mesmo nó.
- **Valor aparece como objeto:** ajuste o circuito para manter `led` como valor simples, por exemplo `0` ou `1`.
