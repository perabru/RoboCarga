#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ESP32Servo.h>

// =====================================================
// ROBÔ BLUETOOTH ESP32 - RESPOSTA RÁPIDA
// =====================================================

// ----------------------
// Bluetooth
// ----------------------

BluetoothSerial SerialBT;

const char* NOME_BLUETOOTH = "RoboResgateESP32";


// ----------------------
// Servos
// ----------------------

#define SERVO_GARRA_PIN 18
#define SERVO_BASE_PIN  19

Servo servoGarra;
Servo servoBase;

int anguloGarra = 90;
int anguloBase  = 90;


// ----------------------
// Ponte H L298N
// ----------------------

#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14


// ----------------------
// HC-SR04
// ----------------------

#define TRIG 32
#define ECHO 33


// =====================================================
// BLUETOOTH
// =====================================================

String comandoRecebido = "";

unsigned long ultimoCaractere = 0;

// Antes estava 80 ms.
// Agora responde muito mais rápido.
const unsigned long TEMPO_COMANDO = 15;


// =====================================================
// TELEMETRIA
// =====================================================

unsigned long ultimaTelemetria = 0;

// Menos leituras automáticas para não interromper
// os comandos do controle.
const unsigned long INTERVALO_TELEMETRIA = 1500;


// =====================================================
// MOTORES
// =====================================================

void parar() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  Serial.println("PARADO");
}


void frente() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  Serial.println("FRENTE");
}


void tras() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  Serial.println("TRAS");
}


void direita() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  Serial.println("DIREITA");
}


void esquerda() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  Serial.println("ESQUERDA");
}


// =====================================================
// ULTRASSÔNICO
// =====================================================

float lerDistancia() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);


  // Timeout reduzido para não travar o controle
  unsigned long duracao =
    pulseIn(ECHO, HIGH, 20000);


  if (duracao == 0) {

    return -1;
  }


  return (duracao * 0.0343) / 2.0;
}


// =====================================================
// ENVIAR DISTÂNCIA
// =====================================================

void enviarDistancia() {

  float distancia = lerDistancia();


  if (distancia < 0) {

    SerialBT.println("DISTANCIA:-1");

    return;
  }


  SerialBT.print("DISTANCIA:");

  SerialBT.println(distancia, 1);
}


// =====================================================
// GARRA - MOVIMENTO RÁPIDO
// =====================================================

void moverGarra(int angulo) {

  angulo = constrain(angulo, 0, 180);

  anguloGarra = angulo;


  // Movimento DIRETO
  servoGarra.write(anguloGarra);


  Serial.print("GARRA -> ");

  Serial.print(anguloGarra);

  Serial.println(" graus");


  SerialBT.print("GARRA:");

  SerialBT.println(anguloGarra);
}


// =====================================================
// BASE - MOVIMENTO RÁPIDO
// =====================================================

void moverBase(int angulo) {

  angulo = constrain(angulo, 0, 180);

  anguloBase = angulo;


  servoBase.write(anguloBase);


  Serial.print("BASE -> ");

  Serial.print(anguloBase);

  Serial.println(" graus");


  SerialBT.print("BASE:");

  SerialBT.println(anguloBase);
}


// =====================================================
// VERIFICAR NÚMERO
// =====================================================

bool numeroValido(String texto) {

  if (texto.length() == 0) {

    return false;
  }


  for (
    unsigned int i = 0;
    i < texto.length();
    i++
  ) {

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


  Serial.print("Recebido: ");

  Serial.println(comando);


  char tipo =
    toupper(comando.charAt(0));


  // ---------------------
  // Frente
  // ---------------------

  if (
    tipo == 'F' &&
    comando.length() == 1
  ) {

    frente();

    return;
  }


  // ---------------------
  // Trás
  // ---------------------

  if (
    tipo == 'T' &&
    comando.length() == 1
  ) {

    tras();

    return;
  }


  // ---------------------
  // Esquerda
  // ---------------------

  if (
    tipo == 'E' &&
    comando.length() == 1
  ) {

    esquerda();

    return;
  }


  // ---------------------
  // Direita
  // ---------------------

  if (
    tipo == 'D' &&
    comando.length() == 1
  ) {

    direita();

    return;
  }


  // ---------------------
  // Parar
  // ---------------------

  if (
    (
      tipo == 'P' ||
      tipo == 'S'
    ) &&
    comando.length() == 1
  ) {

    parar();

    return;
  }


  // ---------------------
  // Ultrassônico
  // ---------------------

  if (
    tipo == 'U' &&
    comando.length() == 1
  ) {

    enviarDistancia();

    return;
  }


  // ===================================================
  // GARRA
  //
  // G0
  // G45
  // G90
  // G140
  // G180
  // ===================================================

  if (tipo == 'G') {

    String numero =
      comando.substring(1);


    numero.trim();


    if (numeroValido(numero)) {

      int angulo =
        numero.toInt();


      moverGarra(angulo);
    }


    return;
  }


  // ===================================================
  // BASE
  //
  // B0
  // B45
  // B90
  // B140
  // B180
  // ===================================================

  if (tipo == 'B') {

    String numero =
      comando.substring(1);


    numero.trim();


    if (numeroValido(numero)) {

      int angulo =
        numero.toInt();


      moverBase(angulo);
    }


    return;
  }
}


// =====================================================
// RECEBER BLUETOOTH
// =====================================================

void receberBluetooth() {

  while (SerialBT.available()) {

    char caractere =
      SerialBT.read();


    ultimoCaractere =
      millis();


    // Final do comando
    if (
      caractere == '\n' ||
      caractere == '\r' ||
      caractere == ';' ||
      caractere == '#'
    ) {

      if (
        comandoRecebido.length() > 0
      ) {

        processarComando(
          comandoRecebido
        );


        comandoRecebido = "";
      }
    }

    else {

      if (
        comandoRecebido.length() < 20
      ) {

        comandoRecebido +=
          caractere;

      }

      else {

        comandoRecebido = "";
      }
    }
  }


  // ===================================================
  // NÃO RECEBEU ENTER
  // ===================================================

  if (
    comandoRecebido.length() > 0 &&
    millis() - ultimoCaractere >
    TEMPO_COMANDO
  ) {

    processarComando(
      comandoRecebido
    );


    comandoRecebido = "";
  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);


  // ---------------------
  // Motores
  // ---------------------

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  parar();


  // ---------------------
  // Ultrassônico
  // ---------------------

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);


  digitalWrite(TRIG, LOW);


  // ---------------------
  // Servos
  // ---------------------

  servoGarra.setPeriodHertz(50);

  servoBase.setPeriodHertz(50);


  servoGarra.attach(
    SERVO_GARRA_PIN,
    500,
    2400
  );


  servoBase.attach(
    SERVO_BASE_PIN,
    500,
    2400
  );


  servoGarra.write(anguloGarra);

  servoBase.write(anguloBase);


  // ---------------------
  // Bluetooth
  // ---------------------

  SerialBT.begin(
    NOME_BLUETOOTH
  );


  Serial.println();
  Serial.println("==========================");
  Serial.println(" ROBO ESP32 - MODO RAPIDO");
  Serial.println("==========================");

  Serial.println();

  Serial.println(
    "Bluetooth: RoboResgateESP32"
  );

  Serial.println();

  Serial.println("Garra GPIO 18");
  Serial.println("Base  GPIO 19");

  Serial.println();

  Serial.println("Pronto.");
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // Prioridade máxima ao Bluetooth
  receberBluetooth();


  // Telemetria com prioridade menor
  if (
    millis() - ultimaTelemetria >=
    INTERVALO_TELEMETRIA
  ) {

    ultimaTelemetria =
      millis();


    enviarDistancia();
  }


  // Delay mínimo
  delay(1);
}
