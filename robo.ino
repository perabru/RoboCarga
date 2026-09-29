#include <Servo.h>

// =====================================================
// ROBÔ BLUETOOTH COM GARRA DE 2 SERVOS
// Arduino UNO + HC-05
//
// HC-05:
// TX -> Arduino D0 (RX)
// RX -> Arduino D1 (TX)
//
// L298N:
// IN1 -> D2
// IN2 -> D3
// IN3 -> D4
// IN4 -> D5
//
// BASE DA GARRA -> D10
// GARRA          -> D11
// =====================================================


// =====================================================
// PONTE H L298N
// =====================================================

#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5


// =====================================================
// SERVOS
// =====================================================

// Servo responsável por girar a base
#define PINO_SERVO_BASE 10

// Servo responsável por abrir/fechar a garra
#define PINO_SERVO_GARRA 11

Servo servoBase;
Servo servoGarra;


// =====================================================
// POSIÇÕES INICIAIS
// =====================================================

int anguloBase = 90;
int anguloGarra = 90;


// =====================================================
// RECEPÇÃO BLUETOOTH
// =====================================================

String comandoRecebido = "";

unsigned long ultimoCaractere = 0;

// Caso o aplicativo não envie \n,
// processa o comando depois desse tempo.
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
// CONTROLAR BASE DA GARRA
// =====================================================

void moverBase(int angulo) {

  angulo = constrain(angulo, 0, 180);

  anguloBase = angulo;

  servoBase.write(anguloBase);
}


// =====================================================
// CONTROLAR GARRA
// =====================================================

void moverGarra(int angulo) {

  angulo = constrain(angulo, 0, 180);

  anguloGarra = angulo;

  servoGarra.write(anguloGarra);
}


// =====================================================
// VERIFICAR SE STRING É NÚMERO
// =====================================================

bool numeroValido(String texto) {

  if (texto.length() == 0) {
    return false;
  }

  for (unsigned int i = 0; i < texto.length(); i++) {

    if (!isDigit(texto.charAt(i))) {
      return false;
    }
  }

  return true;
}


// =====================================================
// PROCESSAR COMANDO
// =====================================================

void processarComando(String comando) {

  comando.trim();

  if (comando.length() == 0) {
    return;
  }


  // ===================================================
  // CONVERTER PRIMEIRA LETRA PARA MAIÚSCULA
  // ===================================================

  char tipo = toupper(comando.charAt(0));


  // ===================================================
  // FRENTE
  // Aplicativo envia:
  //
  // F
  // ===================================================

  if (tipo == 'F' && comando.length() == 1) {

    frente();

    return;
  }


  // ===================================================
  // TRÁS
  // Aplicativo envia:
  //
  // T
  // ===================================================

  if (tipo == 'T' && comando.length() == 1) {

    tras();

    return;
  }


  // ===================================================
  // ESQUERDA
  //
  // E
  // ===================================================

  if (tipo == 'E' && comando.length() == 1) {

    esquerda();

    return;
  }


  // ===================================================
  // DIREITA
  //
  // D
  // ===================================================

  if (tipo == 'D' && comando.length() == 1) {

    direita();

    return;
  }


  // ===================================================
  // PARAR
  //
  // P
  //
  // Também aceita S
  // ===================================================

  if (
    (tipo == 'P' || tipo == 'S') &&
    comando.length() == 1
  ) {

    parar();

    return;
  }


  // ===================================================
  // GARRA
  //
  // G0
  // G45
  // G90
  // G135
  // G180
  //
  // Compatível com o aplicativo atual.
  // ===================================================

  if (tipo == 'G') {

    String numero = comando.substring(1);

    numero.trim();

    if (numeroValido(numero)) {

      int angulo = numero.toInt();

      if (angulo >= 0 && angulo <= 180) {

        moverGarra(angulo);
      }
    }

    return;
  }


  // ===================================================
  // BASE DA GARRA
  //
  // B0
  // B45
  // B90
  // B135
  // B180
  //
  // NOVO COMANDO
  // ===================================================

  if (tipo == 'B') {

    String numero = comando.substring(1);

    numero.trim();

    if (numeroValido(numero)) {

      int angulo = numero.toInt();

      if (angulo >= 0 && angulo <= 180) {

        moverBase(angulo);
      }
    }

    return;
  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  // ===================================================
  // BLUETOOTH HC-05
  // ===================================================

  // Arduino UNO:
  //
  // D0 = RX
  // D1 = TX
  //
  // HC-05 normalmente trabalha em 9600 baud.

  Serial.begin(9600);


  // ===================================================
  // MOTORES
  // ===================================================

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  // Robô inicia parado
  parar();


  // ===================================================
  // SERVO DA BASE
  // ===================================================

  servoBase.attach(PINO_SERVO_BASE);

  servoBase.write(anguloBase);


  // ===================================================
  // SERVO DA GARRA
  // ===================================================

  servoGarra.attach(PINO_SERVO_GARRA);

  servoGarra.write(anguloGarra);


  // Aguarda servos alcançarem posição inicial
  delay(700);
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // ===================================================
  // RECEBER DADOS DO HC-05
  // ===================================================

  while (Serial.available() > 0) {

    char caractere = Serial.read();

    ultimoCaractere = millis();


    // =================================================
    // FINAL DO COMANDO
    // =================================================

    if (
      caractere == '\n' ||
      caractere == '\r' ||
      caractere == ';' ||
      caractere == '#'
    ) {

      if (comandoRecebido.length() > 0) {

        processarComando(comandoRecebido);

        comandoRecebido = "";
      }
    }

    else {

      // Proteção para evitar uma String gigante
      if (comandoRecebido.length() < 20) {

        comandoRecebido += caractere;
      }

      else {

        comandoRecebido = "";
      }
    }
  }


  // ===================================================
  // PROCESSAR CASO NÃO RECEBA ENTER
  // ===================================================

  if (
    comandoRecebido.length() > 0 &&
    millis() - ultimoCaractere > TEMPO_COMANDO
  ) {

    processarComando(comandoRecebido);

    comandoRecebido = "";
  }
}
