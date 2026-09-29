# 🤖 Robô Sumô Inverso com Arduino UNO

Projeto de um robô autônomo utilizando **Arduino UNO**, **ponte H L298N** e **sensor ultrassônico HC-SR04**.

A lógica do robô funciona como um **“sumô inverso”**: em vez de procurar e atacar um obstáculo, ele tenta **evitar obstáculos automaticamente**.

O robô anda continuamente para frente. Quando detecta algum objeto próximo, ele para e começa a girar para a direita até encontrar novamente um caminho livre. Assim que encontra espaço suficiente, volta a andar para frente.

---

## 🎯 Objetivo do projeto

O robô deve funcionar de maneira totalmente autônoma, sem Bluetooth, controle remoto ou intervenção do usuário.

O comportamento esperado é:

```text
LIGA
  ↓
ANDA PARA FRENTE
  ↓
MEDE A DISTÂNCIA
  ↓
TEM OBSTÁCULO?
  ↓
NÃO ───────────────→ CONTINUA PARA FRENTE
  ↓
SIM
  ↓
PARA
  ↓
GIRA PARA DIREITA
  ↓
MEDE NOVAMENTE
  ↓
AINDA TEM OBSTÁCULO?
  ↓
SIM → CONTINUA GIRANDO
  ↓
NÃO
  ↓
VOLTA A ANDAR PARA FRENTE
```

---

# 🧰 Componentes utilizados

- 1 Arduino UNO
- 1 Ponte H L298N
- 1 Sensor ultrassônico HC-SR04
- 2 Motores DC
- 2 Rodas
- Bateria ou fonte adequada para os motores
- Jumpers
- Chassi do robô

---

# 📌 Mapa completo de pinos

## Arduino UNO

| Componente | Pino |
|---|---:|
| L298N IN1 | D4 |
| L298N IN2 | D5 |
| L298N IN3 | D6 |
| L298N IN4 | D7 |
| HC-SR04 ECHO | D8 |
| HC-SR04 TRIG | D9 |

---

# 🔊 Ligação do HC-SR04

| HC-SR04 | Arduino UNO |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | D9 |
| ECHO | D8 |

Representação:

```text
HC-SR04             Arduino UNO

VCC   -------------- 5V
GND   -------------- GND
TRIG  -------------- D9
ECHO  -------------- D8
```

---

# ⚙️ Ligação da ponte H L298N

## Entradas de controle

```text
L298N                 Arduino UNO

IN1 -----------------> D4
IN2 -----------------> D5
IN3 -----------------> D6
IN4 -----------------> D7
```

---

## Motores

O primeiro motor deve ser conectado em:

```text
OUT1
OUT2
```

O segundo motor deve ser conectado em:

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

O **GND da ponte H L298N obrigatoriamente deve estar conectado ao GND do Arduino UNO**.

Essa conexão é extremamente importante.

Faça:

```text
Arduino GND
     |
     |
     +-------------------- GND L298N
```

Ou seja:

```text
GND Arduino -------- GND Ponte H
```

Mesmo que o Arduino e a ponte H estejam sendo alimentados por fontes diferentes, eles precisam compartilhar o mesmo **GND de referência**.

Sem essa ligação, os sinais enviados pelos pinos:

```text
D4
D5
D6
D7
```

podem não ser interpretados corretamente pela L298N.

O resultado pode ser:

- motores não girarem;
- motores girarem aleatoriamente;
- somente um motor funcionar;
- comportamento instável;
- ponte H aparentemente não responder.

Portanto:

> **SEMPRE CONECTE O GND DA L298N AO GND DO ARDUINO.**

---

# 🔋 Alimentação da ponte H

Os motores **não devem ser alimentados diretamente pelo pino 5V do Arduino**.

A bateria dos motores deve ser conectada diretamente na L298N.

Exemplo:

```text
BATERIA +
    |
    +---------- VIN / +12V L298N


BATERIA -
    |
    +---------- GND L298N
                     |
                     |
                     +---------- GND Arduino
```

Portanto:

```text
Bateria +  -------- +12V/VIN L298N

Bateria -  -------- GND L298N

Arduino GND -------- GND L298N
```

---

# 🔗 Esquema completo

```text
                  ARDUINO UNO
                 ┌────────────┐
                 │            │
             D4  ├────────────┼──── IN1 L298N
             D5  ├────────────┼──── IN2 L298N
             D6  ├────────────┼──── IN3 L298N
             D7  ├────────────┼──── IN4 L298N
                 │            │
             D8  ├────────────┼──── ECHO HC-SR04
             D9  ├────────────┼──── TRIG HC-SR04
                 │            │
             5V  ├────────────┼──── VCC HC-SR04
                 │            │
            GND  ├───────┬────┼──── GND HC-SR04
                 │       │    │
                 └───────┼────┘
                         │
                         │
                         ↓
                     GND L298N
```

A ligação do GND comum é:

```text
GND Arduino
    |
    +---------------- GND HC-SR04
    |
    +---------------- GND L298N
    |
    +---------------- Negativo da bateria
```

---

# ⚡ ENA e ENB da L298N

Normalmente a placa L298N possui:

```text
ENA
IN1
IN2
IN3
IN4
ENB
```

Se você **não estiver controlando velocidade por PWM**, mantenha os jumpers:

```text
ENA
ENB
```

colocados na placa.

Representação:

```text
ENA -> JUMPER INSTALADO

ENB -> JUMPER INSTALADO
```

Se os jumpers forem retirados e ENA/ENB não estiverem conectados a nenhum sinal, os motores podem não funcionar.

---

# 🧠 Funcionamento do algoritmo

O robô trabalha com dois limites principais.

## Distância de obstáculo

```cpp
const float DISTANCIA_OBSTACULO = 20.0;
```

Isso significa que qualquer objeto detectado a **20 cm ou menos** será considerado um obstáculo.

Exemplo:

```text
50 cm → livre

35 cm → livre

25 cm → livre

20 cm → obstáculo

15 cm → obstáculo

10 cm → obstáculo
```

---

# 🛣️ Distância considerada livre

No código:

```cpp
const float DISTANCIA_LIVRE = 28.0;
```

Depois que começa a girar, o robô somente considera que encontrou uma nova direção segura quando o ultrassônico medir pelo menos:

```text
28 cm
```

Isso evita um problema comum:

```text
obstáculo = 20 cm
vira um pouco
mede 21 cm
anda
detecta novamente
vira
anda
vira
anda
```

Utilizando dois limites diferentes:

```text
20 cm → começa a desviar

28 cm → considera realmente livre
```

o comportamento fica muito mais estável.

---

# ↪️ Movimento para direita

Quando encontra um obstáculo, o robô faz:

```cpp
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);

digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
```

Isso faz:

```text
Motor esquerdo → frente

Motor direito → trás
```

fazendo o robô girar no próprio eixo para a direita.

---

# 🔄 Busca pelo caminho livre

O robô gira pequenos intervalos:

```cpp
const unsigned long TEMPO_PASSO_GIRO = 90;
```

Ou seja:

```text
gira um pouco
      ↓
para
      ↓
mede
      ↓
continua bloqueado?
      ↓
gira novamente
```

Assim ele não executa simplesmente um giro fixo de 90 graus.

Ele realmente usa o sensor para descobrir quando o caminho está livre.

---

# 📡 Filtro do sensor ultrassônico

O código não utiliza somente uma leitura do HC-SR04.

Ele realiza:

```text
5 leituras
```

Depois organiza os valores e utiliza a:

```text
MEDIANA
```

Isso ajuda a eliminar leituras incorretas.

Exemplo:

```text
Leituras:

19.8 cm
20.1 cm
98.0 cm
20.0 cm
19.9 cm
```

A leitura de:

```text
98.0 cm
```

provavelmente é um erro.

Ao utilizar a mediana, o resultado fica próximo de:

```text
20.0 cm
```

tornando a navegação mais confiável.

---

# 💻 Código completo

```cpp
// =====================================================
// ROBÔ "SUMÔ INVERSO"
// Arduino UNO + L298N + HC-SR04
//
// COMPORTAMENTO:
//
// 1. Anda para frente
// 2. Encontrou obstáculo
// 3. Para
// 4. Gira para a DIREITA
// 5. Continua girando até achar caminho livre
// 6. Volta a andar para frente
//
// HC-SR04:
// ECHO -> D8
// TRIG -> D9
//
// L298N:
// IN1 -> D4
// IN2 -> D5
// IN3 -> D6
// IN4 -> D7
// =====================================================

#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

#define ECHO 8
#define TRIG 9

const float DISTANCIA_OBSTACULO = 20.0;

const float DISTANCIA_LIVRE = 28.0;

const unsigned long TEMPO_PASSO_GIRO = 90;

const unsigned long PAUSA = 40;


// =====================================================
// PARAR
// =====================================================

void parar() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


// =====================================================
// FRENTE
// =====================================================

void frente() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// DIREITA
// =====================================================

void direita() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// =====================================================
// LEITURA HC-SR04
// =====================================================

float lerDistanciaSimples() {

  digitalWrite(TRIG, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  unsigned long duracao =
    pulseIn(ECHO, HIGH, 25000);

  if (duracao == 0) {

    return 999;
  }

  float distancia =
    (duracao * 0.0343) / 2.0;

  return distancia;
}


// =====================================================
// FILTRO
// =====================================================

float lerDistancia() {

  float valores[5];

  for (int i = 0; i < 5; i++) {

    valores[i] =
      lerDistanciaSimples();

    delay(5);
  }

  for (int i = 0; i < 4; i++) {

    for (int j = i + 1; j < 5; j++) {

      if (valores[j] < valores[i]) {

        float temp = valores[i];

        valores[i] = valores[j];

        valores[j] = temp;
      }
    }
  }

  return valores[2];
}


// =====================================================
// PROCURAR SAÍDA PARA DIREITA
// =====================================================

void procurarSaidaDireita() {

  Serial.println();

  Serial.println("==============================");

  Serial.println("OBSTACULO DETECTADO");

  Serial.println("Girando para DIREITA...");

  Serial.println("==============================");

  parar();

  delay(150);

  while (true) {

    direita();

    delay(TEMPO_PASSO_GIRO);

    parar();

    delay(80);

    float distancia =
      lerDistancia();

    Serial.print(
      "Procurando saida | Distancia: "
    );

    if (distancia == 999) {

      Serial.println("LIVRE");

      break;
    }

    Serial.print(distancia, 1);

    Serial.println(" cm");

    if (
      distancia >= DISTANCIA_LIVRE
    ) {

      Serial.println();

      Serial.println(
        "*** CAMINHO LIVRE ENCONTRADO ***"
      );

      break;
    }
  }

  parar();

  delay(150);

  Serial.println(
    "Voltando a andar para frente."
  );

  Serial.println();
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  parar();

  pinMode(TRIG, OUTPUT);

  pinMode(ECHO, INPUT);

  digitalWrite(TRIG, LOW);

  Serial.println();

  Serial.println("==============================");

  Serial.println(" ROBO SUMO INVERSO");

  Serial.println("==============================");

  Serial.println("ECHO = D8");

  Serial.println("TRIG = D9");

  Serial.println();

  Serial.println("Iniciando...");

  Serial.println();

  delay(1000);
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  float distancia =
    lerDistancia();

  Serial.print("Distancia: ");

  if (distancia == 999) {

    Serial.println("sem obstaculo");

  }

  else {

    Serial.print(distancia, 1);

    Serial.println(" cm");
  }

  if (
    distancia != 999 &&
    distancia <= DISTANCIA_OBSTACULO
  ) {

    parar();

    Serial.println(
      "OBSTACULO NA FRENTE!"
    );

    procurarSaidaDireita();
  }

  else {

    frente();
  }

  delay(PAUSA);
}
```

---

# 🧪 Teste recomendado

Antes de colocar o robô no chão, faça o teste com as rodas levantadas.

Abra o:

```text
Monitor Serial
```

em:

```text
9600 baud
```

Você deve visualizar:

```text
ROBO SUMO INVERSO

Distancia: 85.2 cm
Distancia: 72.5 cm
Distancia: 43.1 cm

Distancia: 18.9 cm

OBSTACULO NA FRENTE!

Girando para DIREITA...

Procurando saida | Distancia: 17.5 cm
Procurando saida | Distancia: 19.3 cm
Procurando saida | Distancia: 24.8 cm
Procurando saida | Distancia: 31.2 cm

*** CAMINHO LIVRE ENCONTRADO ***

Voltando a andar para frente.
```

---

# ❌ Se os motores não girarem

Confira primeiro:

```text
ENA → jumper colocado
ENB → jumper colocado
```

Depois confira:

```text
Arduino D4 → IN1
Arduino D5 → IN2
Arduino D6 → IN3
Arduino D7 → IN4
```

E principalmente:

```text
GND Arduino → GND L298N
```

Essa conexão é obrigatória.

Confira também a alimentação dos motores.

---

# 🔄 Se uma roda girar ao contrário

Se o motor conectado a:

```text
OUT3
OUT4
```

estiver girando ao contrário durante o movimento para frente, você pode trocar fisicamente os dois fios do motor:

```text
OUT3 ↔ OUT4
```

ou alterar no código:

De:

```cpp
digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
```

Para:

```cpp
digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
```

Faça isso somente para o lado que estiver invertido.

---

# 📊 Resumo da pinagem

```text
┌───────────────────────────────────┐
│          ARDUINO UNO              │
├────────────────┬──────────────────┤
│ D4             │ L298N IN1        │
│ D5             │ L298N IN2        │
│ D6             │ L298N IN3        │
│ D7             │ L298N IN4        │
│ D8             │ HC-SR04 ECHO     │
│ D9             │ HC-SR04 TRIG     │
│ 5V             │ HC-SR04 VCC      │
│ GND            │ HC-SR04 GND      │
│ GND            │ L298N GND        │
└────────────────┴──────────────────┘
```

---

# ⚠️ Regra principal da montagem

```text
ARDUINO GND
     |
     +-------- GND HC-SR04
     |
     +-------- GND L298N
     |
     +-------- NEGATIVO DA FONTE/BATERIA
```

Todos precisam possuir a mesma referência elétrica.

## ✅ Nunca esqueça:

> **O GND DA PONTE H DEVE ESTAR CONECTADO AO GND DO ARDUINO UNO.**

---

# 🚗 Resultado esperado

Depois de ligado, o robô funciona sozinho:

```text
ANDA
 ↓
ANDA
 ↓
ANDA
 ↓
OBSTÁCULO
 ↓
PARA
 ↓
VIRA PARA DIREITA
 ↓
PROCURA
 ↓
PROCURA
 ↓
CAMINHO LIVRE
 ↓
ANDA
 ↓
ANDA
 ↓
...
```

O processo continua enquanto o Arduino estiver alimentado.

---

## Projeto

**Robô autônomo com desvio de obstáculos**

Tecnologias:

- Arduino UNO
- C/C++
- L298N
- HC-SR04
- Motores DC
- Navegação autônoma por ultrassom
