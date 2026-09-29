# 🤖 Robô Autônomo de Resgate

Projeto de um robô autônomo de resgate utilizando:

- Arduino UNO
- Ponte H L298N
- Sensor ultrassônico HC-SR04
- 2 motores DC
- 2 servos
- Garra mecânica

O robô foi projetado para se movimentar sozinho em um ambiente com obstáculos, procurar uma possível vítima, capturá-la com uma garra e retornar ao ponto inicial de forma autônoma.

---

# 🎯 Objetivo do Projeto

O objetivo principal é simular um robô de resgate capaz de atuar em ambientes como:

- áreas com escombros;
- corredores bloqueados;
- locais estreitos;
- cenários de desabamento;
- regiões onde uma pessoa não consegue acessar com segurança.

O robô deve realizar a missão sem Bluetooth ou controle manual.

---

# 🧠 Funcionamento Geral

O robô executa automaticamente a seguinte sequência:

```text
LIGA
  ↓
ABRE A GARRA
  ↓
LEVANTA A GARRA
  ↓
COMEÇA A EXPLORAR
  ↓
ANDA PARA FRENTE
  ↓
MEDE A DISTÂNCIA
  ↓
ENCONTROU ALGO?
  ↓
        NÃO
         ↓
CONTINUA PARA FRENTE

        SIM
         ↓
ANALISA O OBJETO
         ↓
É GRANDE / LARGO?
         ↓
        SIM
         ↓
CONSIDERA OBSTÁCULO
         ↓
VIRA PARA DIREITA
         ↓
CONTINUA EXPLORANDO

É OBJETO ESTREITO?
         ↓
        SIM
         ↓
POSSÍVEL VÍTIMA
         ↓
CONFIRMA DETECÇÃO
         ↓
APROXIMA
         ↓
ABAIXA A GARRA
         ↓
FECHA A GARRA
         ↓
LEVANTA A VÍTIMA
         ↓
FAZ BACKTRACKING
         ↓
RETORNA AO PONTO A
         ↓
ABAIXA A VÍTIMA
         ↓
ABRE A GARRA
         ↓
MISSÃO CONCLUÍDA
```

---

# 🚑 Situações de Resgate

O robô pode representar quatro situações principais.

## 1. Pessoa presa atrás de escombros

A vítima pode ser posicionada atrás de caixas, blocos ou outros obstáculos.

O robô utiliza o HC-SR04 para detectar essas barreiras e contorná-las.

---

## 2. Pessoa em corredor bloqueado

O boneco fica em uma região que não possui acesso direto.

O robô identifica o bloqueio e tenta encontrar outra passagem.

---

## 3. Pessoa em uma área de desabamento

Vários obstáculos podem ser colocados ao redor da vítima.

O robô navega de forma autônoma desviando das barreiras.

---

## 4. Pessoa em local de difícil acesso

A vítima é posicionada em uma área estreita.

Quando o robô detecta um objeto com características de uma possível vítima, inicia automaticamente o procedimento de resgate.

---

# 🔩 Componentes Utilizados

- 1 Arduino UNO
- 1 Ponte H L298N
- 1 HC-SR04
- 2 Motores DC
- 2 Rodas
- 1 Servo motor para garra
- 1 Servo motor para levantar e abaixar a garra
- 1 Garra mecânica
- 1 Bateria para motores
- Fonte adequada para servos
- Jumpers
- Chassi do robô

---

# 📌 Mapa de Pinos

| Componente | Arduino UNO |
|---|---:|
| Servo Motor Garra | D2 |
| Servo Motor Base / Altura | D3 |
| Ponte H IN1 | D4 |
| Ponte H IN2 | D5 |
| Ponte H IN3 | D6 |
| Ponte H IN4 | D7 |
| HC-SR04 ECHO | D8 |
| HC-SR04 TRIG | D9 |

---

# 🔊 HC-SR04

As conexões do sensor ultrassônico são:

| HC-SR04 | Arduino |
|---|---|
| VCC | 5V |
| GND | GND |
| ECHO | D8 |
| TRIG | D9 |

Esquema:

```text
HC-SR04            Arduino UNO

VCC   ------------ 5V
GND   ------------ GND
ECHO  ------------ D8
TRIG  ------------ D9
```

---

# 🤏 Servo da Garra

O servo responsável por abrir e fechar a garra utiliza:

```text
Sinal -> D2
VCC   -> 5V externo recomendado
GND   -> GND comum
```

No código:

```cpp
#define SERVO_GARRA_PIN 2
```

---

# ↕️ Servo de Subida e Descida

O segundo servo é responsável por levantar e abaixar o mecanismo da garra.

Ligação:

```text
Sinal -> D3
VCC   -> 5V externo recomendado
GND   -> GND comum
```

No código:

```cpp
#define SERVO_BASE_PIN 3
```

---

# ⚙️ Ponte H L298N

As entradas utilizadas são:

```text
IN1 -> D4
IN2 -> D5
IN3 -> D6
IN4 -> D7
```

Ligação:

```text
Arduino          L298N

D4  ------------ IN1
D5  ------------ IN2
D6  ------------ IN3
D7  ------------ IN4
```

---

# 🚗 Motores

Um motor deve ser ligado em:

```text
OUT1
OUT2
```

O outro motor:

```text
OUT3
OUT4
```

Exemplo:

```text
Motor esquerdo
   |
   +---- OUT1
   |
   +---- OUT2


Motor direito
   |
   +---- OUT3
   |
   +---- OUT4
```

---

# ⚠️ MUITO IMPORTANTE — GND COMUM

O GND da ponte H deve estar obrigatoriamente conectado ao GND do Arduino.

Faça:

```text
Arduino GND
     |
     +---------- GND L298N
```

O mesmo vale para os servos e sensores.

O ideal é:

```text
GND Arduino
    |
    +-------- GND HC-SR04
    |
    +-------- GND L298N
    |
    +-------- GND Fonte dos Servos
    |
    +-------- Negativo da bateria
```

Sem GND comum, os sinais enviados pelo Arduino podem não ser corretamente interpretados pelos outros módulos.

Isso pode causar:

- motores parados;
- motores funcionando aleatoriamente;
- servos tremendo;
- leituras incorretas;
- resets do Arduino.

---

# 🔋 Alimentação

Não é recomendado alimentar:

- motores DC;
- dois servos;

diretamente pelo pino 5V do Arduino.

Os motores devem receber energia pela L298N.

Exemplo:

```text
Bateria +

   |
   +-------- VIN / +12V L298N


Bateria -

   |
   +-------- GND L298N
                 |
                 +-------- GND Arduino
```

Para os servos, utilize preferencialmente uma fonte externa de 5 V com corrente suficiente.

Exemplo:

```text
Fonte 5V externa
      |
      +---- VCC Servo Garra
      |
      +---- VCC Servo Base

GND Fonte
      |
      +---- GND Servos
      |
      +---- GND Arduino
```

---

# ⚡ ENA e ENB

A maioria das placas L298N possui:

```text
ENA
IN1
IN2
IN3
IN4
ENB
```

Se não estiver usando PWM para controlar velocidade, deixe os jumpers:

```text
ENA
ENB
```

instalados.

Se esses jumpers forem removidos e os pinos ENA/ENB não forem controlados, os motores podem não funcionar.

---

# 🧠 Inteligência do Robô

O robô utiliza várias estratégias para tornar o comportamento mais autônomo.

---

# 📡 Filtragem do Ultrassônico

O código não confia em uma única leitura.

São realizadas várias medições do HC-SR04.

Exemplo:

```text
20.1 cm
19.8 cm
20.2 cm
93.5 cm
20.0 cm
```

A leitura de:

```text
93.5 cm
```

provavelmente é um erro.

O algoritmo ordena os valores e utiliza a mediana.

Resultado aproximado:

```text
20.1 cm
```

Isso melhora bastante a estabilidade.

---

# 👤 Detecção de Possível Vítima

Como o projeto utiliza apenas um HC-SR04, ele não consegue identificar visualmente uma pessoa.

Por isso, é utilizada uma estratégia geométrica.

Quando o robô encontra algo na frente:

```text
        OBJETO
          X
          |
        ROBÔ
```

ele gira um pouco para a esquerda:

```text
\ X
 \
 ROBÔ
```

depois gira para a direita:

```text
 X /
  /
ROBÔ
```

Se o centro estiver ocupado, mas os dois lados estiverem relativamente livres:

```text
      espaço
        |
    \   X   /
     \     /
       ROBÔ
```

o sistema interpreta que o objeto pode ser estreito.

Isso é utilizado como indicação de uma possível vítima.

---

# 🧱 Detecção de Obstáculo

Se o robô olhar para os lados e continuar detectando objetos próximos:

```text
████████████████

      ROBÔ
```

o sistema interpreta como uma parede ou escombro.

Nesse caso:

```text
OBSTÁCULO
    ↓
PARA
    ↓
VIRA PARA DIREITA
    ↓
CONTINUA EXPLORANDO
```

---

# ↪️ Estratégia “Sumô Inverso”

O sistema de navegação é inspirado em um robô sumô, porém com comportamento contrário.

Um robô sumô normalmente procura algo e avança contra ele.

Nesse projeto:

```text
encontrou obstáculo
        ↓
não ataca
        ↓
desvia
```

O robô tende a virar para a direita até encontrar uma nova direção para continuar.

---

# 🧭 Memória de Caminho

Enquanto explora, o robô armazena os movimentos realizados.

Os códigos internos são:

```text
F = Frente
R = Direita
L = Esquerda
```

Um exemplo de trajetória:

```text
F
F
F
R
F
F
R
F
```

é armazenado internamente.

---

# 🔙 Backtracking

Depois de capturar a vítima, o robô lê o histórico ao contrário.

Exemplo:

```text
IDA:

F
F
R
F
R
F
```

No retorno:

```text
F -> RÉ

R -> ESQUERDA

F -> RÉ

R -> ESQUERDA

F -> RÉ

F -> RÉ
```

Assim, ele tenta reconstruir o caminho percorrido.

---

# 🤖 Sequência de Captura

Quando a vítima é confirmada:

```text
Possível vítima
       ↓
Confirmação
       ↓
Abre a garra
       ↓
Abaixa a garra
       ↓
Aproxima
       ↓
Avança um pouco
       ↓
Fecha a garra
       ↓
Levanta a garra
       ↓
Inicia retorno
```

---

# 🏠 Retorno ao Ponto A

Depois de capturar a vítima:

```text
CAPTURA
   ↓
BACKTRACKING
   ↓
RETORNA AO INÍCIO
   ↓
ABAIXA A GARRA
   ↓
ABRE
   ↓
LIBERA A VÍTIMA
   ↓
LEVANTA A GARRA
   ↓
PARA
```

---

# 🛠️ Valores para Calibração

Os principais valores que podem precisar ser ajustados são os ângulos dos servos.

## Garra

```cpp
const int GARRA_ABERTA  = 135;
const int GARRA_FECHADA = 50;
```

Caso o servo force fisicamente, diminua os limites.

Por exemplo:

```cpp
const int GARRA_ABERTA  = 120;
const int GARRA_FECHADA = 65;
```

---

# ↕️ Altura

```cpp
const int BASE_ALTA  = 135;
const int BASE_BAIXA = 45;
```

Esses valores dependem diretamente da posição mecânica do servo.

---

# ↪️ Giro

O giro é controlado por tempo.

```cpp
const unsigned long TEMPO_GIRO_90 = 430;
```

Esse valor deve ser ajustado até o robô girar aproximadamente 90 graus.

Exemplo:

```text
Gira pouco?

430 -> 470 -> 500
```

Se girar demais:

```text
430 -> 400 -> 370
```

---

# 🛣️ Passo de Movimento

```cpp
const unsigned long TEMPO_PASSO_FRENTE = 170;
```

Quanto maior esse número, maior será o deslocamento de cada passo.

---

# 🔎 Distância de Obstáculo

```cpp
const float DISTANCIA_OBSTACULO = 26.0;
```

Isso significa que objetos abaixo de aproximadamente 26 cm serão analisados.

---

# 🤏 Distância de Captura

```cpp
const float DISTANCIA_CAPTURA = 10.0;
```

Quando o robô chega aproximadamente a essa distância, pode iniciar a captura.

---

# 🧪 Testes Recomendados

Antes de testar tudo de uma vez, faça testes separados.

## Teste 1 — Motores

Verifique:

```text
Frente
Direita
Esquerda
Ré
```

---

## Teste 2 — Ultrassônico

Abra o Monitor Serial.

Use:

```text
9600 baud
```

Aproxime a mão do sensor.

Você deve visualizar:

```text
Distancia: 42.3 cm
Distancia: 31.5 cm
Distancia: 20.2 cm
Distancia: 12.8 cm
```

---

## Teste 3 — Garra

Verifique:

```text
Abrir
Fechar
```

sem forçar mecanicamente.

---

## Teste 4 — Servo de Altura

Verifique:

```text
Subir
Descer
```

---

## Teste 5 — Desvio

Coloque uma caixa na frente.

O comportamento esperado é:

```text
ANDA
  ↓
DETECTA CAIXA
  ↓
PARA
  ↓
ANALISA
  ↓
CONSIDERA OBSTÁCULO
  ↓
VIRA PARA DIREITA
```

---

## Teste 6 — Boneco

Coloque um boneco relativamente estreito sozinho na frente.

O comportamento esperado:

```text
DETECTA
  ↓
OLHA ESQUERDA
  ↓
OLHA DIREITA
  ↓
ESPAÇO DOS DOIS LADOS
  ↓
POSSÍVEL VÍTIMA
  ↓
CONFIRMA
  ↓
CAPTURA
```

---

# 🖥️ Monitor Serial

Abra:

```text
Ferramentas
→ Monitor Serial
```

Selecione:

```text
9600 baud
```

Exemplos de mensagens:

```text
[EXPLORANDO] Frente: 67.5 cm

[EXPLORANDO] Frente: 23.1 cm

Objeto detectado na frente.

ANALISANDO OBJETO

Centro: 23.1 cm
Esquerda: 48.2 cm
Direita: 51.0 cm

OBJETO ESTREITO DETECTADO
Possivel vitima.

Confirmacao de vitima: 1/2
```

Depois:

```text
VITIMA ENCONTRADA

Abrindo garra...
Abaixando garra...
Aproximacao final...
Fechando garra...
Levantando garra...

VITIMA CAPTURADA
```

E durante o retorno:

```text
BACKTRACKING PARA O PONTO A

Retorno 1/14 -> RECUAR
Retorno 2/14 -> RECUAR
Retorno 3/14 -> GIRO ESQUERDA
```

---

# ❌ Problemas Comuns

## Motores não funcionam

Verifique:

```text
ENA -> jumper instalado
ENB -> jumper instalado
```

Confira também:

```text
GND Arduino -> GND L298N
```

e a alimentação dos motores.

---

## Motor gira ao contrário

Troque os fios:

```text
OUT1 <-> OUT2
```

ou:

```text
OUT3 <-> OUT4
```

do motor correspondente.

---

## Arduino reinicia quando servo mexe

Normalmente significa falta de corrente.

Use uma fonte 5 V externa para os servos.

Não esqueça:

```text
GND fonte externa
      |
      +-------- GND Arduino
```

---

## Servo fica tremendo

Possíveis causas:

- alimentação fraca;
- fonte compartilhada inadequadamente;
- GND não conectado;
- esforço mecânico excessivo;
- posição fora do limite físico.

---

## HC-SR04 apresenta valores estranhos

Confira:

```text
TRIG -> D9
ECHO -> D8
```

Também confira:

```text
VCC -> 5V
GND -> GND
```

---

# 📊 Pinagem Final

```text
┌─────────────────────────────────────┐
│              ARDUINO UNO            │
├─────────────────────┬───────────────┤
│ D2                  │ Servo Garra   │
│ D3                  │ Servo Base    │
│ D4                  │ L298N IN1     │
│ D5                  │ L298N IN2     │
│ D6                  │ L298N IN3     │
│ D7                  │ L298N IN4     │
│ D8                  │ HC-SR04 ECHO  │
│ D9                  │ HC-SR04 TRIG  │
│ 5V                  │ HC-SR04 VCC   │
│ GND                 │ HC-SR04 GND   │
│ GND                 │ L298N GND     │
└─────────────────────┴───────────────┘
```

---

# 🔌 Diagrama Geral Simplificado

```text
                   ARDUINO UNO
              ┌─────────────────┐
              │                 │
          D2  ├─────────────────┼── Servo Garra
          D3  ├─────────────────┼── Servo Base
              │                 │
          D4  ├─────────────────┼── IN1 L298N
          D5  ├─────────────────┼── IN2 L298N
          D6  ├─────────────────┼── IN3 L298N
          D7  ├─────────────────┼── IN4 L298N
              │                 │
          D8  ├─────────────────┼── ECHO HC-SR04
          D9  ├─────────────────┼── TRIG HC-SR04
              │                 │
          5V  ├─────────────────┼── VCC HC-SR04
              │                 │
         GND  ├────────────┬────┼── GND HC-SR04
              │            │    │
              └────────────┼────┘
                           │
                           ├──────── GND L298N
                           │
                           └──────── GND Fonte dos Servos
```

---

# ⚠️ Regra Mais Importante da Montagem

Todos os circuitos precisam compartilhar o mesmo GND.

```text
GND ARDUINO
    |
    +---------- GND HC-SR04
    |
    +---------- GND L298N
    |
    +---------- GND FONTE DOS SERVOS
    |
    +---------- NEGATIVO DA BATERIA
```

> **O GND DA PONTE H L298N DEVE ESTAR CONECTADO AO GND DO ARDUINO UNO.**

---

# 🚀 Melhorias Futuras

O projeto pode ser evoluído com:

- sensores de linha;
- encoder nas rodas;
- MPU6050;
- sensores laterais;
- múltiplos HC-SR04;
- ESP32;
- câmera;
- reconhecimento de imagem;
- sensor de temperatura;
- localização por mapa;
- controle de velocidade por PWM;
- algoritmos de busca mais avançados.

Com encoders e uma IMU seria possível melhorar bastante a precisão do backtracking.

---

# ✅ Resultado Esperado

Ao final, o robô deverá ser capaz de:

```text
✔ iniciar sozinho
✔ navegar automaticamente
✔ detectar obstáculos
✔ analisar objetos
✔ desviar de barreiras
✔ procurar possíveis vítimas
✔ aproximar automaticamente
✔ abaixar a garra
✔ capturar a vítima
✔ levantar a vítima
✔ memorizar o caminho
✔ executar backtracking
✔ retornar ao ponto inicial
✔ liberar a vítima
✔ finalizar a missão
```

---

# 🏁 Projeto Final

**Robô Autônomo de Resgate com Arduino UNO**

Tecnologias utilizadas:

```text
Arduino UNO
C/C++
HC-SR04
L298N
Motores DC
Servo Motor
Navegação Autônoma
Desvio de Obstáculos
Detecção Geométrica
Memória de Trajetória
Backtracking
Sistema de Garra
```

O projeto demonstra conceitos de:

- robótica móvel;
- sistemas embarcados;
- sensores;
- atuadores;
- algoritmos de navegação;
- automação;
- resgate autônomo;
- tomada de decisão.
