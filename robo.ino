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


// =====================================================
// PINAGEM
// =====================================================

#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

#define ECHO 8
#define TRIG 9


// =====================================================
// CONFIGURAÇÕES
// =====================================================

// Distância para considerar obstáculo
const float DISTANCIA_OBSTACULO = 20.0;

// Para evitar que fique indeciso perto do limite,
// só considera caminho realmente livre acima disso.
const float DISTANCIA_LIVRE = 28.0;

// Tempo de cada pequeno giro para direita
const unsigned long TEMPO_PASSO_GIRO = 90;

// Pausa pequena entre leituras
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

  // Motor esquerdo
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Motor direito
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// GIRAR PARA DIREITA
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
// LEITURA SIMPLES DO HC-SR04
// =====================================================

float lerDistanciaSimples() {

  digitalWrite(TRIG, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);


  unsigned long duracao =
    pulseIn(ECHO, HIGH, 25000);


  // Se não recebeu retorno,
  // consideramos caminho muito distante/livre
  if (duracao == 0) {

    return 999;
  }


  float distancia =
    (duracao * 0.0343) / 2.0;


  return distancia;
}


// =====================================================
// FILTRO DA DISTÂNCIA
//
// Faz 5 leituras e pega a mediana.
// Ajuda a evitar leitura falsa.
// =====================================================

float lerDistancia() {

  float valores[5];


  for (int i = 0; i < 5; i++) {

    valores[i] =
      lerDistanciaSimples();

    delay(5);
  }


  // Ordenação
  for (int i = 0; i < 4; i++) {

    for (int j = i + 1; j < 5; j++) {

      if (valores[j] < valores[i]) {

        float temp = valores[i];

        valores[i] = valores[j];

        valores[j] = temp;
      }
    }
  }


  // Mediana
  return valores[2];
}


// =====================================================
// PROCURAR CAMINHO LIVRE PARA DIREITA
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

    // Pequeno giro para direita
    direita();

    delay(TEMPO_PASSO_GIRO);

    parar();

    delay(80);


    // Mede novamente
    float distancia =
      lerDistancia();


    Serial.print("Procurando saida | Distancia: ");


    if (distancia == 999) {

      Serial.println("LIVRE");

      break;
    }


    Serial.print(distancia, 1);

    Serial.println(" cm");


    // Encontrou caminho livre
    if (distancia >= DISTANCIA_LIVRE) {

      Serial.println();
      Serial.println("*** CAMINHO LIVRE ENCONTRADO ***");

      break;
    }
  }


  parar();

  delay(150);


  Serial.println("Voltando a andar para frente.");
  Serial.println();
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);


  // ===================================================
  // MOTORES
  // ===================================================

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  parar();


  // ===================================================
  // ULTRASSÔNICO
  // ===================================================

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

  } else {

    Serial.print(distancia, 1);

    Serial.println(" cm");
  }


  // ===================================================
  // OBSTÁCULO
  // ===================================================

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


  // ===================================================
  // CAMINHO LIVRE
  // ===================================================

  else {

    frente();
  }


  delay(PAUSA);
}
