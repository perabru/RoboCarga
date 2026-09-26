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

  // Motor esquerdo
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor direito
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// TRÁS
// =====================================================

void tras() {

  // Motor esquerdo
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor direito
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// =====================================================
// ESQUERDA
// =====================================================

void esquerda() {

  // Motor esquerdo para trás
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Motor direito para frente
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// DIREITA
// =====================================================

void direita() {

  // Motor esquerdo para frente
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor direito para trás
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

  if (buffer.charAt(0) == 'G' ||
      buffer.charAt(0) == 'g') {

    buffer.remove(0, 1);
  }


  if (buffer.length() > 0) {

    bool numeroValido = true;

    for (unsigned int i = 0; i < buffer.length(); i++) {

      if (!isDigit(buffer.charAt(i))) {

        numeroValido = false;
      }
    }


    if (numeroValido) {

      int angulo = buffer.toInt();

      if (angulo >= 0 && angulo <= 180) {

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

  // ===================================================
  // HC-05 NOS PINOS 0 E 1
  // ===================================================

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


  // Robô começa parado
  parar();


  // ===================================================
  // SERVO
  // ===================================================

  garra.attach(PINO_SERVO);

  // Posição inicial da garra
  garra.write(90);

  delay(500);
}


// =====================================================
// LOOP
// =====================================================

void loop() {


  // ===================================================
  // RECEBER BLUETOOTH
  // ===================================================

  while (Serial.available() > 0) {

    char comando = Serial.read();

    ultimoCaractere = millis();


    // =================================================
    // MOVIMENTAÇÃO
    // =================================================

    if (comando == 'F' || comando == 'f') {

      frente();

      buffer = "";
    }


    else if (
      comando == 'T' ||
      comando == 't' ||
      comando == 'B' ||
      comando == 'b'
    ) {

      tras();

      buffer = "";
    }


    else if (comando == 'E' || comando == 'e') {

      esquerda();

      buffer = "";
    }


    else if (comando == 'D' || comando == 'd') {

      direita();

      buffer = "";
    }


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
    // VALOR DA GARRA
    // =================================================

    else if (
      isDigit(comando) ||
      comando == 'G' ||
      comando == 'g'
    ) {

      buffer += comando;
    }


    // =================================================
    // ENTER OU FINAL DO COMANDO
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
  // PROCESSAR NÚMERO SEM PRECISAR DE ENTER
  // ===================================================

  if (
    buffer.length() > 0 &&
    millis() - ultimoCaractere > TEMPO_COMANDO
  ) {

    processarBuffer();
  }
}