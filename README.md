# Carrinho Bluetooth com Arduino Uno, HC-05, L298N e Servo

## Descrição

Este projeto utiliza um **Arduino Uno** para controlar um carrinho via Bluetooth.

O sistema possui:

- 2 motores DC
- Ponte H L298N
- Módulo Bluetooth HC-05
- Servo motor para controle de uma garra
- Arduino Uno

O celular envia comandos Bluetooth para o módulo HC-05. O Arduino recebe esses comandos pelos pinos seriais `0` e `1` e controla os motores e o servo.

---

## Componentes utilizados

- Arduino Uno
- HC-05
- L298N
- 2 motores DC
- 1 servo motor
- Bateria para os motores
- Fonte adequada para o servo, se necessário
- Jumpers
- Resistores para divisor de tensão no RX do HC-05

---

## Pinagem

### Ponte H L298N

| L298N | Arduino Uno |
|---|---|
| IN1 | D2 |
| IN2 | D3 |
| IN3 | D4 |
| IN4 | D5 |
| GND | GND |

Ligação dos motores:

```text
Motor esquerdo -> OUT1 e OUT2
Motor direito  -> OUT3 e OUT4
```

Se os jumpers `ENA` e `ENB` estiverem instalados na L298N, os motores ficarão habilitados em velocidade máxima.

---

## Bluetooth HC-05

| HC-05 | Arduino Uno |
|---|---|
| TXD | D0 / RX |
| RXD | D1 / TX |
| GND | GND |
| VCC | 5V |

A comunicação é feita em:

```text
9600 baud
```

No código:

```cpp
Serial.begin(9600);
```

---

## Atenção aos pinos 0 e 1

Os pinos:

```text
D0 = RX
D1 = TX
```

também são utilizados pelo Arduino Uno durante o envio do programa pelo USB.

Se ocorrer erro durante o upload:

1. Desconecte o HC-05 dos pinos `0` e `1`.
2. Faça o upload do código.
3. Aguarde terminar.
4. Reconecte o HC-05.

---

## Atenção ao RX do HC-05

A saída TX do Arduino trabalha com aproximadamente `5 V`.

A entrada RX do HC-05 trabalha com nível lógico menor.

Por isso, recomenda-se usar um divisor resistivo entre:

```text
Arduino D1 / TX
      |
   divisor
      |
HC-05 RX
```

O TX do HC-05 pode ser conectado diretamente ao pino `D0 / RX` do Arduino.

---

## Servo da garra

| Servo | Arduino Uno |
|---|---|
| Sinal | D11 |
| VCC | 5V ou fonte externa |
| GND | GND |

O servo é controlado pelo pino:

```cpp
#define PINO_SERVO 11
```

É utilizada a biblioteca:

```cpp
#include <Servo.h>
```

O servo é inicializado com:

```cpp
Servo garra;
```

e depois:

```cpp
garra.attach(PINO_SERVO);
```

A posição inicial é:

```cpp
garra.write(90);
```

---

## Alimentação do servo

Se estiver utilizando um servo de maior corrente, como:

```text
MG995
MG996R
```

é recomendado utilizar uma fonte externa de `5 V` a `6 V`.

Evite alimentar servos de alta corrente diretamente pelo pino `5V` do Arduino.

---

## GND comum

Todos os módulos precisam compartilhar o mesmo GND.

Exemplo:

```text
Arduino GND
   |
   +---- L298N GND
   |
   +---- HC-05 GND
   |
   +---- Servo GND
   |
   +---- Fonte/Bateria GND
```

Isso é muito importante para o funcionamento correto do sistema.

---

# Comandos Bluetooth

O Arduino recebe comandos simples pelo HC-05.

## Movimento

| Comando | Ação |
|---|---|
| F | Frente |
| T | Trás |
| B | Trás |
| E | Esquerda |
| D | Direita |
| P | Parar |
| S | Parar |

Exemplo:

```text
F
```

faz o carrinho andar para frente.

Para parar:

```text
P
```

---

# Controle da garra

A garra utiliza valores entre:

```text
0° e 180°
```

Os comandos podem ser enviados assim:

```text
G0
G45
G90
G120
G180
```

Exemplo:

```text
G90
```

posiciona o servo em aproximadamente `90°`.

Também é possível enviar apenas o valor numérico:

```text
0
45
90
135
180
```

---

# Funcionamento dos motores

## Frente

```cpp
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);

digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
```

Os dois motores giram no sentido configurado como frente.

---

## Trás

```cpp
digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);

digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
```

Os motores giram no sentido contrário.

---

## Esquerda

```cpp
digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);

digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
```

O motor esquerdo gira para trás e o direito gira para frente.

Com isso, o carrinho gira para a esquerda.

---

## Direita

```cpp
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);

digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
```

O motor esquerdo gira para frente e o direito para trás.

---

## Parar

```cpp
digitalWrite(IN1, LOW);
digitalWrite(IN2, LOW);

digitalWrite(IN3, LOW);
digitalWrite(IN4, LOW);
```

Os dois motores são desligados.

---

# Funcionamento do servo

A posição do servo é alterada com:

```cpp
garra.write(angulo);
```

Exemplo:

```cpp
garra.write(0);
```

posição inicial.

```cpp
garra.write(90);
```

posição central.

```cpp
garra.write(180);
```

posição final.

O código limita o valor recebido usando:

```cpp
angulo = constrain(angulo, 0, 180);
```

Assim, o servo nunca recebe um valor menor que `0` ou maior que `180`.

---

# Código completo

```cpp
#include <Servo.h>

// =====================================================
// ROBÔ BLUETOOTH
// Arduino UNO + HC-05 nos pinos 0 e 1
// L298N + Servo da Garra
// =====================================================


// =====================================================
// PONTE H L298N
// =====================================================

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5


// =====================================================
// SERVO
// =====================================================

#define PINO_SERVO 11

Servo garra;


// =====================================================
// VARIÁVEIS
// =====================================================

String buffer = "";

unsigned long ultimoCaractere = 0;

const unsigned long TEMPO_COMANDO = 80;


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
// TRÁS
// =====================================================

void tras() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// =====================================================
// ESQUERDA
// =====================================================

void esquerda() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

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
// CONTROLAR GARRA
// =====================================================

void moverGarra(int angulo) {

  angulo = constrain(angulo, 0, 180);

  garra.write(angulo);
}


// =====================================================
// PROCESSAR VALOR DA GARRA
// =====================================================

void processarBuffer() {

  if (buffer.length() == 0) {
    return;
  }


  // Permite enviar:
  //
  // 90
  //
  // ou:
  //
  // G90


  if (
    buffer.charAt(0) == 'G' ||
    buffer.charAt(0) == 'g'
  ) {

    buffer.remove(0, 1);
  }


  if (buffer.length() > 0) {

    bool numeroValido = true;


    for (
      unsigned int i = 0;
      i < buffer.length();
      i++
    ) {

      if (!isDigit(buffer.charAt(i))) {

        numeroValido = false;
      }
    }


    if (numeroValido) {

      int angulo = buffer.toInt();


      if (
        angulo >= 0 &&
        angulo <= 180
      ) {

        moverGarra(angulo);
      }
    }
  }


  buffer = "";
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  // HC-05:
  //
  // D0 = RX
  // D1 = TX

  Serial.begin(9600);


  // ===================================================
  // MOTORES
  // ===================================================

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  // Começa parado

  parar();


  // ===================================================
  // SERVO
  // ===================================================

  garra.attach(PINO_SERVO);


  // Posição inicial

  garra.write(90);


  delay(500);
}


// =====================================================
// LOOP
// =====================================================

void loop() {


  while (Serial.available() > 0) {


    char comando = Serial.read();


    ultimoCaractere = millis();


    // =================================================
    // FRENTE
    // =================================================

    if (
      comando == 'F' ||
      comando == 'f'
    ) {

      frente();

      buffer = "";
    }


    // =================================================
    // TRÁS
    // =================================================

    else if (
      comando == 'T' ||
      comando == 't' ||
      comando == 'B' ||
      comando == 'b'
    ) {

      tras();

      buffer = "";
    }


    // =================================================
    // ESQUERDA
    // =================================================

    else if (
      comando == 'E' ||
      comando == 'e'
    ) {

      esquerda();

      buffer = "";
    }


    // =================================================
    // DIREITA
    // =================================================

    else if (
      comando == 'D' ||
      comando == 'd'
    ) {

      direita();

      buffer = "";
    }


    // =================================================
    // PARAR
    // =================================================

    else if (
      comando == 'P' ||
      comando == 'p' ||
      comando == 'S' ||
      comando == 's'
    ) {

      parar();

      buffer = "";
    }


    // =================================================
    // COMANDO DA GARRA
    // =================================================

    else if (
      isDigit(comando) ||
      comando == 'G' ||
      comando == 'g'
    ) {

      buffer += comando;
    }


    // =================================================
    // FINAL DO COMANDO
    // =================================================

    else if (
      comando == '\n' ||
      comando == '\r' ||
      comando == ';' ||
      comando == '#'
    ) {

      processarBuffer();
    }
  }


  // ===================================================
  // PROCESSAR SEM ENTER
  // ===================================================

  if (
    buffer.length() > 0 &&
    millis() - ultimoCaractere > TEMPO_COMANDO
  ) {

    processarBuffer();
  }
}
```

# Fluxo do sistema

```text
CELULAR
   |
   | Bluetooth
   ↓
HC-05
   |
   | Serial
   ↓
ARDUINO UNO
   |
   +--------------------+
   |                    |
   ↓                    ↓
 L298N                 SERVO
   |                    |
   ↓                    ↓
2 MOTORES              GARRA
```

# Exemplo de funcionamento

O celular envia:

```text
F
```

O Arduino executa:

```cpp
frente();
```

A L298N aciona os dois motores e o carrinho anda para frente.

Depois o celular envia:

```text
P
```

O Arduino executa:

```cpp
parar();
```

e os motores param.

Se o celular enviar:

```text
G120
```

o Arduino executa:

```cpp
garra.write(120);
```

e a garra muda de posição.

# Teste recomendado

Antes de colocar o carrinho no chão, faça o primeiro teste com as rodas suspensas.

Teste nesta ordem:

```text
F
P

T
P

E
P

D
P
```

Depois teste a garra:

```text
G0
G45
G90
G135
G180
```

Observe se o servo consegue movimentar a garra sem forçar mecanicamente o mecanismo.

# Se uma roda girar ao contrário

É possível que os dois motores estejam fisicamente montados em sentidos opostos.

Se o comando:

```text
F
```

fizer uma roda andar para frente e a outra para trás, existem duas soluções.

Você pode inverter os dois fios desse motor na saída da L298N.

Ou alterar o código desse motor.

Por exemplo, trocar:

```cpp
digitalWrite(IN3, HIGH);
digitalWrite(IN4, LOW);
```

por:

```cpp
digitalWrite(IN3, LOW);
digitalWrite(IN4, HIGH);
```

O objetivo é que, ao receber `F`, as duas rodas empurrem fisicamente o carrinho para frente.

# Resumo dos comandos

```text
F       = Frente

T       = Trás
B       = Trás

E       = Esquerda

D       = Direita

P       = Parar
S       = Parar

G0      = Garra em 0°

G45     = Garra em 45°

G90     = Garra em 90°

G135    = Garra em 135°

G180    = Garra em 180°
```

# Cuidados importantes

- Não ligue os motores diretamente aos pinos do Arduino.
- Utilize a ponte H L298N.
- Use uma bateria adequada para os motores.
- Não alimente servo de alta corrente diretamente pelo Arduino.
- Mantenha todos os GNDs em comum.
- Utilize divisor de tensão no RX do HC-05.
- Se o upload falhar, desconecte o HC-05 dos pinos `0` e `1`.
- Teste os motores com as rodas levantadas antes de colocar o carrinho no chão.
- Evite forçar o servo além dos limites físicos da garra.

# Resultado

Com este projeto, o Arduino Uno consegue receber comandos Bluetooth pelo HC-05 e controlar:

```text
✓ Frente
✓ Trás
✓ Esquerda
✓ Direita
✓ Parada
✓ Garra de 0° até 180°
```

O HC-05 funciona como a comunicação entre o celular e o Arduino, enquanto a L298N controla os motores DC e o servo controla a abertura e o fechamento da garra.
