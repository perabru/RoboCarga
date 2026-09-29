#include <Servo.h>

// ==========================================================
// ROBÔ AUTÔNOMO DE RESGATE
//
// Arduino UNO
// L298N
// HC-SR04
// 2 Servos
//
// FUNÇÕES:
//
// - Navega sozinho
// - Detecta obstáculos
// - Desvia sempre para direita
// - Faz varredura para identificar objeto estreito
// - Trata objeto estreito como vítima
// - Aproxima automaticamente
// - Abaixa a garra
// - Abre a garra
// - Captura a vítima
// - Levanta a vítima
// - Memoriza o caminho
// - Faz backtracking físico
// - Retorna ao ponto inicial
// - Solta a vítima
//
// ==========================================================


// ==========================================================
// PINOS
// ==========================================================

// Servos
#define SERVO_GARRA_PIN 2
#define SERVO_BASE_PIN  3

// Ponte H
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

// Ultrassônico
#define ECHO 8
#define TRIG 9


// ==========================================================
// SERVOS
// ==========================================================

Servo servoGarra;
Servo servoBase;


// ==========================================================
// AJUSTE DA GARRA
//
// CALIBRE ESTES VALORES PARA SUA MECÂNICA
// ==========================================================

const int GARRA_ABERTA  = 135;
const int GARRA_FECHADA = 50;

// Servo D3
const int BASE_ALTA  = 135;
const int BASE_BAIXA = 45;


// ==========================================================
// DISTÂNCIAS
// ==========================================================

// Começa a considerar algo como obstáculo
const float DISTANCIA_OBSTACULO = 26.0;

// Distância máxima para analisar se pode ser vítima
const float DISTANCIA_ANALISE_VITIMA = 24.0;

// Espaço lateral necessário para considerar objeto estreito
const float DISTANCIA_LATERAL_LIVRE = 32.0;

// Diferença entre centro e lateral
const float DIFERENCA_OBJETO_ESTREITO = 12.0;

// Distância desejada antes da captura
const float DISTANCIA_CAPTURA = 10.0;

// Segurança mínima
const float DISTANCIA_MINIMA = 3.0;


// ==========================================================
// TEMPOS DOS MOTORES
//
// NECESSÁRIO CALIBRAR NO SEU ROBÔ
// ==========================================================

// Um pequeno passo para frente
const unsigned long TEMPO_PASSO_FRENTE = 170;

// Pequeno movimento durante aproximação da vítima
const unsigned long TEMPO_APROXIMACAO = 55;

// Giro aproximado de 90 graus
const unsigned long TEMPO_GIRO_90 = 430;

// Pequeno giro usado somente para "olhar" para o lado
const unsigned long TEMPO_SCAN = 105;

// Avanço final para colocar a vítima dentro da garra
const unsigned long TEMPO_AVANCO_FINAL = 130;


// ==========================================================
// MEMÓRIA DO CAMINHO
// ==========================================================

#define MAX_CAMINHO 220

char caminho[MAX_CAMINHO];

int tamanhoCaminho = 0;


// ==========================================================
// ESTADOS
// ==========================================================

enum Estado {

  PROCURANDO,
  RESGATANDO,
  RETORNANDO,
  FINALIZADO

};

Estado estado = PROCURANDO;


// ==========================================================
// CONTROLE
// ==========================================================

int confirmacaoVitima = 0;

const int CONFIRMACOES_NECESSARIAS = 2;


// ==========================================================
// MEMÓRIA DA APROXIMAÇÃO FINAL
// ==========================================================

unsigned long tempoAproximacaoFinal = 0;


// ==========================================================
// PARAR
// ==========================================================

void parar() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}


// ==========================================================
// FRENTE
// ==========================================================

void frente() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// ==========================================================
// TRÁS
// ==========================================================

void tras() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// ==========================================================
// DIREITA
// ==========================================================

void direita() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// ==========================================================
// ESQUERDA
// ==========================================================

void esquerda() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// ==========================================================
// MOVIMENTOS
// ==========================================================

void andarFrente(unsigned long tempo) {

  frente();

  delay(tempo);

  parar();

  delay(60);
}


void andarTras(unsigned long tempo) {

  tras();

  delay(tempo);

  parar();

  delay(60);
}


void girarDireitaPor(unsigned long tempo) {

  direita();

  delay(tempo);

  parar();

  delay(100);
}


void girarEsquerdaPor(unsigned long tempo) {

  esquerda();

  delay(tempo);

  parar();

  delay(100);
}


// ==========================================================
// MEMORIZAR MOVIMENTO
//
// F = frente
// R = giro direita
// L = giro esquerda
// ==========================================================

void registrarMovimento(char movimento) {

  if (tamanhoCaminho >= MAX_CAMINHO) {

    Serial.println(
      "ATENCAO: memoria do caminho cheia!"
    );

    return;
  }

  caminho[tamanhoCaminho] = movimento;

  tamanhoCaminho++;
}


// ==========================================================
// LEITURA DO HC-SR04
// ==========================================================

float lerDistanciaSimples() {

  digitalWrite(TRIG, LOW);

  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);

  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);


  unsigned long duracao =
    pulseIn(ECHO, HIGH, 25000);


  if (duracao == 0) {

    return 999.0;
  }


  float distancia =
    (duracao * 0.0343) / 2.0;


  if (distancia < DISTANCIA_MINIMA) {

    return 999.0;
  }


  return distancia;
}


// ==========================================================
// FILTRO DO ULTRASSÔNICO
//
// 5 leituras + mediana
// ==========================================================

float lerDistancia() {

  float valores[5];


  for (int i = 0; i < 5; i++) {

    valores[i] =
      lerDistanciaSimples();

    delay(7);
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


// ==========================================================
// PRINT DISTÂNCIA
// ==========================================================

void mostrarDistancia(
  const char* nome,
  float distancia
) {

  Serial.print(nome);

  Serial.print(": ");


  if (distancia >= 900) {

    Serial.println("LIVRE");

  } else {

    Serial.print(distancia, 1);

    Serial.println(" cm");
  }
}


// ==========================================================
// MOVIMENTAR SERVO SUAVEMENTE
// ==========================================================

void moverServoSuave(
  Servo &servo,
  int destino
) {

  destino = constrain(destino, 0, 180);

  int atual = servo.read();


  if (atual < destino) {

    for (
      int angulo = atual;
      angulo <= destino;
      angulo++
    ) {

      servo.write(angulo);

      delay(12);
    }

  } else {

    for (
      int angulo = atual;
      angulo >= destino;
      angulo--
    ) {

      servo.write(angulo);

      delay(12);
    }
  }
}


// ==========================================================
// ABRIR GARRA
// ==========================================================

void abrirGarra() {

  Serial.println("Abrindo garra...");

  moverServoSuave(
    servoGarra,
    GARRA_ABERTA
  );
}


// ==========================================================
// FECHAR GARRA
// ==========================================================

void fecharGarra() {

  Serial.println("Fechando garra...");

  moverServoSuave(
    servoGarra,
    GARRA_FECHADA
  );
}


// ==========================================================
// ABAIXAR GARRA
// ==========================================================

void abaixarGarra() {

  Serial.println("Abaixando garra...");

  moverServoSuave(
    servoBase,
    BASE_BAIXA
  );
}


// ==========================================================
// LEVANTAR GARRA
// ==========================================================

void levantarGarra() {

  Serial.println("Levantando garra...");

  moverServoSuave(
    servoBase,
    BASE_ALTA
  );
}


// ==========================================================
// TESTE GEOMÉTRICO PARA IDENTIFICAR VÍTIMA
//
// O robô olha:
//
//   ESQUERDA
//      \
//       X <- objeto
//      /
//   DIREITA
//
// Se centro está perto, mas os dois lados ficam muito mais
// livres, provavelmente é um objeto estreito (boneco).
//
// Se centro, esquerda e direita continuam próximos,
// provavelmente é parede/escombro.
// ==========================================================

bool pareceVitima(float centro) {

  Serial.println();

  Serial.println("==========================");

  Serial.println("ANALISANDO OBJETO");

  Serial.println("==========================");


  mostrarDistancia(
    "Centro",
    centro
  );


  // ========================================================
  // OLHAR PARA ESQUERDA
  // ========================================================

  girarEsquerdaPor(
    TEMPO_SCAN
  );


  float esquerdaDist =
    lerDistancia();


  mostrarDistancia(
    "Esquerda",
    esquerdaDist
  );


  // ========================================================
  // IR DA ESQUERDA ATÉ A DIREITA
  // ========================================================

  girarDireitaPor(
    TEMPO_SCAN * 2
  );


  float direitaDist =
    lerDistancia();


  mostrarDistancia(
    "Direita",
    direitaDist
  );


  // ========================================================
  // VOLTAR AO CENTRO
  // ========================================================

  girarEsquerdaPor(
    TEMPO_SCAN
  );


  delay(100);


  // ========================================================
  // ANALISAR
  // ========================================================

  bool esquerdaLivre =
    esquerdaDist >= DISTANCIA_LATERAL_LIVRE ||
    esquerdaDist >= centro + DIFERENCA_OBJETO_ESTREITO;


  bool direitaLivre =
    direitaDist >= DISTANCIA_LATERAL_LIVRE ||
    direitaDist >= centro + DIFERENCA_OBJETO_ESTREITO;


  if (
    esquerdaLivre &&
    direitaLivre
  ) {

    Serial.println();

    Serial.println(
      "*** OBJETO ESTREITO DETECTADO ***"
    );

    Serial.println(
      "Possivel vitima."
    );

    return true;
  }


  Serial.println();

  Serial.println(
    "Objeto largo."
  );

  Serial.println(
    "Tratando como obstaculo."
  );


  return false;
}


// ==========================================================
// GIRAR 90° DIREITA E MEMORIZAR
// ==========================================================

void virarDireita90() {

  Serial.println(
    "Virando 90 graus para direita."
  );


  girarDireitaPor(
    TEMPO_GIRO_90
  );


  registrarMovimento('R');
}


// ==========================================================
// PASSO DE EXPLORAÇÃO
// ==========================================================

void passoFrente() {

  andarFrente(
    TEMPO_PASSO_FRENTE
  );


  registrarMovimento('F');
}


// ==========================================================
// PROCURAR CAMINHO
// ==========================================================

void procurar() {

  float distancia =
    lerDistancia();


  Serial.print(
    "[EXPLORANDO] "
  );


  mostrarDistancia(
    "Frente",
    distancia
  );


  // ========================================================
  // CAMINHO LIVRE
  // ========================================================

  if (
    distancia >= 900 ||
    distancia > DISTANCIA_OBSTACULO
  ) {

    confirmacaoVitima = 0;

    passoFrente();

    return;
  }


  // ========================================================
  // OBJETO PRÓXIMO
  // ========================================================

  parar();


  Serial.println();

  Serial.println(
    "Objeto detectado na frente."
  );


  // ========================================================
  // ANALISAR SE PODE SER VÍTIMA
  // ========================================================

  if (
    distancia <=
    DISTANCIA_ANALISE_VITIMA
  ) {

    bool candidato =
      pareceVitima(distancia);


    if (candidato) {

      confirmacaoVitima++;


      Serial.print(
        "Confirmacao de vitima: "
      );

      Serial.print(
        confirmacaoVitima
      );

      Serial.print("/");

      Serial.println(
        CONFIRMACOES_NECESSARIAS
      );


      if (
        confirmacaoVitima >=
        CONFIRMACOES_NECESSARIAS
      ) {

        estado = RESGATANDO;

        return;
      }


      return;
    }
  }


  // ========================================================
  // É OBSTÁCULO
  //
  // SUMÔ INVERSO:
  // vira sempre para direita
  // ========================================================

  confirmacaoVitima = 0;


  Serial.println(
    "OBSTACULO -> desviando para direita."
  );


  virarDireita90();
}


// ==========================================================
// APROXIMAÇÃO FINAL DA VÍTIMA
// ==========================================================

bool aproximarVitima() {

  tempoAproximacaoFinal = 0;


  Serial.println();

  Serial.println(
    "Aproximacao final..."
  );


  while (true) {

    float distancia =
      lerDistancia();


    mostrarDistancia(
      "Vitima",
      distancia
    );


    // Já está perto
    if (
      distancia != 999 &&
      distancia <= DISTANCIA_CAPTURA
    ) {

      parar();

      return true;
    }


    // Perdeu a vítima
    if (
      distancia >= 900 ||
      distancia > 35
    ) {

      parar();


      Serial.println(
        "Vitima perdida durante aproximacao."
      );


      return false;
    }


    andarFrente(
      TEMPO_APROXIMACAO
    );


    tempoAproximacaoFinal +=
      TEMPO_APROXIMACAO;


    // Proteção
    if (
      tempoAproximacaoFinal > 1800
    ) {

      parar();

      return false;
    }
  }
}


// ==========================================================
// EXECUTAR RESGATE
// ==========================================================

void executarResgate() {

  parar();


  Serial.println();

  Serial.println(
    "=================================="
  );

  Serial.println(
    "       VITIMA ENCONTRADA"
  );

  Serial.println(
    "=================================="
  );


  // Abre antes de aproximar
  abrirGarra();


  // Abaixa
  abaixarGarra();


  delay(300);


  // Aproxima
  if (!aproximarVitima()) {

    Serial.println(
      "Abortando captura."
    );


    levantarGarra();

    estado = PROCURANDO;

    confirmacaoVitima = 0;

    return;
  }


  // Pequeno avanço final
  Serial.println(
    "Avanco final da garra..."
  );


  andarFrente(
    TEMPO_AVANCO_FINAL
  );


  delay(250);


  // Fecha
  fecharGarra();


  delay(600);


  // Levanta
  levantarGarra();


  delay(600);


  Serial.println();

  Serial.println(
    "*** VITIMA CAPTURADA ***"
  );


  // ========================================================
  // VOLTAR AO PONTO ANTES DA APROXIMAÇÃO
  // ========================================================

  Serial.println(
    "Recuando da area de resgate..."
  );


  andarTras(
    tempoAproximacaoFinal +
    TEMPO_AVANCO_FINAL
  );


  estado = RETORNANDO;
}


// ==========================================================
// RETORNO POR BACKTRACKING
//
// Movimento original:
// F -> volta usando TRÁS
// R -> desfaz girando ESQUERDA
// L -> desfaz girando DIREITA
// ==========================================================

void retornarAoInicio() {

  Serial.println();

  Serial.println(
    "=================================="
  );

  Serial.println(
    " BACKTRACKING PARA O PONTO A"
  );

  Serial.println(
    "=================================="
  );


  Serial.print(
    "Movimentos armazenados: "
  );

  Serial.println(
    tamanhoCaminho
  );


  // ========================================================
  // LER CAMINHO AO CONTRÁRIO
  // ========================================================

  for (
    int i = tamanhoCaminho - 1;
    i >= 0;
    i--
  ) {

    char movimento =
      caminho[i];


    Serial.print(
      "Retorno "
    );

    Serial.print(
      tamanhoCaminho - i
    );

    Serial.print("/");

    Serial.print(
      tamanhoCaminho
    );

    Serial.print(
      " -> "
    );


    // ======================================================
    // DESFAZER AVANÇO
    // ======================================================

    if (movimento == 'F') {

      Serial.println(
        "RECUAR"
      );


      andarTras(
        TEMPO_PASSO_FRENTE
      );
    }


    // ======================================================
    // DESFAZER GIRO DIREITA
    // ======================================================

    else if (
      movimento == 'R'
    ) {

      Serial.println(
        "GIRO ESQUERDA"
      );


      girarEsquerdaPor(
        TEMPO_GIRO_90
      );
    }


    // ======================================================
    // DESFAZER GIRO ESQUERDA
    // ======================================================

    else if (
      movimento == 'L'
    ) {

      Serial.println(
        "GIRO DIREITA"
      );


      girarDireitaPor(
        TEMPO_GIRO_90
      );
    }
  }


  parar();


  Serial.println();

  Serial.println(
    "*** PONTO A ALCANCADO ***"
  );


  // ========================================================
  // SOLTAR VÍTIMA
  // ========================================================

  Serial.println(
    "Depositando vitima..."
  );


  abaixarGarra();


  delay(500);


  abrirGarra();


  delay(700);


  levantarGarra();


  delay(500);


  parar();


  estado = FINALIZADO;
}


// ==========================================================
// SETUP
// ==========================================================

void setup() {

  Serial.begin(9600);


  // ========================================================
  // PONTE H
  // ========================================================

  pinMode(IN1, OUTPUT);

  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);

  pinMode(IN4, OUTPUT);


  parar();


  // ========================================================
  // HC-SR04
  // ========================================================

  pinMode(TRIG, OUTPUT);

  pinMode(ECHO, INPUT);


  digitalWrite(
    TRIG,
    LOW
  );


  // ========================================================
  // SERVOS
  // ========================================================

  servoGarra.attach(
    SERVO_GARRA_PIN
  );


  servoBase.attach(
    SERVO_BASE_PIN
  );


  servoGarra.write(
    GARRA_ABERTA
  );


  servoBase.write(
    BASE_ALTA
  );


  delay(1000);


  // ========================================================
  // APRESENTAÇÃO
  // ========================================================

  Serial.println();

  Serial.println(
    "=================================="
  );

  Serial.println(
    " ROBO AUTONOMO DE RESGATE"
  );

  Serial.println(
    "=================================="
  );


  Serial.println();

  Serial.println(
    "PINAGEM:"
  );

  Serial.println(
    "Garra       = D2"
  );

  Serial.println(
    "Base        = D3"
  );

  Serial.println(
    "L298N IN1   = D4"
  );

  Serial.println(
    "L298N IN2   = D5"
  );

  Serial.println(
    "L298N IN3   = D6"
  );

  Serial.println(
    "L298N IN4   = D7"
  );

  Serial.println(
    "HC-SR04 ECHO = D8"
  );

  Serial.println(
    "HC-SR04 TRIG = D9"
  );


  Serial.println();

  Serial.println(
    "Modo FULL AUTONOMO"
  );

  Serial.println(
    "Iniciando em 3 segundos..."
  );


  delay(3000);
}


// ==========================================================
// LOOP
// ==========================================================

void loop() {

  switch (estado) {


    // ======================================================
    // PROCURANDO
    // ======================================================

    case PROCURANDO:

      procurar();

      break;


    // ======================================================
    // RESGATANDO
    // ======================================================

    case RESGATANDO:

      executarResgate();

      break;


    // ======================================================
    // RETORNANDO
    // ======================================================

    case RETORNANDO:

      retornarAoInicio();

      break;


    // ======================================================
    // FINAL
    // ======================================================

    case FINALIZADO:

      parar();


      Serial.println(
        "MISSAO CONCLUIDA."
      );


      while (true) {

        parar();

        delay(1000);
      }

      break;
  }
}
