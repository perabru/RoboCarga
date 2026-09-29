# 🤖 Robô de Resgate com ESP32

Projeto de um robô controlado por **Bluetooth interno da ESP32**, utilizando:

- ESP32
- Ponte H L298N
- 2 motores DC
- 2 servos
- Sensor ultrassônico HC-SR04
- Aplicativo Android em Kotlin
- Comunicação Bluetooth Classic

O módulo HC-05/HC-06 não é necessário, pois a própria ESP32 realiza a comunicação Bluetooth.

---

# 📌 Mapa completo de pinos

## ESP32

| Componente | Função | GPIO ESP32 |
|---|---|---:|
| Servo Garra | Abrir/fechar garra | GPIO 18 |
| Servo Base | Subir/descer garra | GPIO 19 |
| L298N IN1 | Motor esquerdo | GPIO 25 |
| L298N IN2 | Motor esquerdo | GPIO 26 |
| L298N IN3 | Motor direito | GPIO 27 |
| L298N IN4 | Motor direito | GPIO 14 |
| HC-SR04 TRIG | Disparo ultrassônico | GPIO 32 |
| HC-SR04 ECHO | Retorno ultrassônico | GPIO 33 |
| GND | Terra comum | GND |

---

# 🗺️ Mapa geral

```text
                     ┌──────────────────────┐
                     │        ESP32         │
                     │                      │
         Servo Garra │ GPIO 18              │
                     │                      │
          Servo Base │ GPIO 19              │
                     │                      │
          L298N IN1  │ GPIO 25              │
          L298N IN2  │ GPIO 26              │
          L298N IN3  │ GPIO 27              │
          L298N IN4  │ GPIO 14              │
                     │                      │
       HC-SR04 TRIG  │ GPIO 32              │
       HC-SR04 ECHO  │ GPIO 33              │
                     │                      │
                GND  │ GND                  │
                     └──────────────────────┘
```

---

# ⚙️ Ponte H L298N

## Ligações de controle

```text
ESP32                 L298N

GPIO 25  -----------> IN1
GPIO 26  -----------> IN2
GPIO 27  -----------> IN3
GPIO 14  -----------> IN4
GND      -----------> GND
```

---

# 🚗 Motores

## Motor esquerdo

```text
L298N

OUT1 -------- Motor esquerdo
OUT2 -------- Motor esquerdo
```

## Motor direito

```text
L298N

OUT3 -------- Motor direito
OUT4 -------- Motor direito
```

---

# 🔋 Alimentação dos motores

Os motores devem ser alimentados pela ponte H.

```text
Bateria +

   |
   +---------- +12V / VIN L298N


Bateria -

   |
   +---------- GND L298N
```

O negativo da bateria também deve compartilhar o GND com a ESP32.

```text
Bateria GND
    |
    +-------- GND L298N
    |
    +-------- GND ESP32
```

---

# ⚠️ GND comum

Essa é uma das partes mais importantes da montagem.

Todos os módulos precisam compartilhar a mesma referência de GND.

```text
                 GND ESP32
                     |
        ┌────────────┼────────────┐
        │            │            │
        │            │            │
    GND L298N   GND HC-SR04   GND Servos
        │
        │
Negativo bateria
```

Portanto:

```text
GND ESP32
   |
   +------ GND L298N
   |
   +------ GND HC-SR04
   |
   +------ GND fonte dos servos
   |
   +------ negativo da bateria
```

> O GND da ponte H deve estar ligado ao GND da ESP32.

Sem isso, os motores podem não responder corretamente aos comandos.

---

# ⚡ ENA e ENB da L298N

Se a velocidade não estiver sendo controlada por PWM, deixe os jumpers:

```text
ENA
ENB
```

instalados na L298N.

Exemplo:

```text
ENA -> jumper colocado

ENB -> jumper colocado
```

Se os jumpers forem removidos e os pinos ENA e ENB não forem ligados, os motores podem não funcionar.

---

# 🤏 Servo da Garra

O servo responsável por abrir e fechar a garra utiliza:

```text
Servo Garra          ESP32

SINAL  ------------> GPIO 18
VCC    ------------> 5V externo
GND    ------------> GND comum
```

No código:

```cpp
#define SERVO_GARRA_PIN 18
```

---

# ↕️ Servo da Base

O servo da base é responsável por subir e descer a garra.

```text
Servo Base           ESP32

SINAL  ------------> GPIO 19
VCC    ------------> 5V externo
GND    ------------> GND comum
```

No código:

```cpp
#define SERVO_BASE_PIN 19
```

---

# 🔌 Alimentação dos servos

É recomendado utilizar uma fonte externa de 5 V para os dois servos.

Exemplo:

```text
Fonte externa 5V
      |
      +-------- VCC Servo Garra
      |
      +-------- VCC Servo Base


GND da fonte
      |
      +-------- GND Servo Garra
      |
      +-------- GND Servo Base
      |
      +-------- GND ESP32
```

Evite alimentar dois servos fortes diretamente pela ESP32.

Isso pode causar:

- servo lento;
- servo tremendo;
- perda de torque;
- resets da ESP32;
- desconexão Bluetooth.

---

# 📡 HC-SR04

O sensor ultrassônico utiliza:

```text
HC-SR04              ESP32

VCC   -------------- 5V
GND   -------------- GND
TRIG  -------------- GPIO 32
ECHO  -------------- GPIO 33
```

No código:

```cpp
#define TRIG 32
#define ECHO 33
```

---

# ⚠️ Atenção ao ECHO do HC-SR04

O HC-SR04 pode fornecer aproximadamente 5 V no pino ECHO.

A ESP32 trabalha com lógica de 3,3 V.

Por isso, é recomendado utilizar um divisor de tensão.

Exemplo:

```text
ECHO HC-SR04
     |
     |
    1kΩ
     |
     +------------ GPIO 33 ESP32
     |
    2kΩ
     |
    GND
```

Assim o sinal cai de aproximadamente:

```text
5V
↓
3,3V
```

protegendo a entrada da ESP32.

---

# 🗺️ Diagrama completo

```text
                          ESP32
                  ┌──────────────────┐
                  │                  │
Servo Garra ------│ GPIO 18          │
Servo Base  ------│ GPIO 19          │
                  │                  │
L298N IN1  -------│ GPIO 25          │
L298N IN2  -------│ GPIO 26          │
L298N IN3  -------│ GPIO 27          │
L298N IN4  -------│ GPIO 14          │
                  │                  │
HC-SR04 TRIG -----│ GPIO 32          │
HC-SR04 ECHO -----│ GPIO 33          │
                  │                  │
GND --------------│ GND              │
                  └──────────────────┘
                           |
                           |
              ┌────────────┼─────────────┐
              │            │             │
              ↓            ↓             ↓
          GND L298N   GND HC-SR04   GND Servos
              │
              ↓
        Negativo bateria
```

---

# 🔌 Diagrama da L298N

```text
ESP32                       L298N

GPIO 25 ------------------> IN1
GPIO 26 ------------------> IN2
GPIO 27 ------------------> IN3
GPIO 14 ------------------> IN4

GND ----------------------> GND


Motor esquerdo:

OUT1 --------------------- Motor
OUT2 --------------------- Motor


Motor direito:

OUT3 --------------------- Motor
OUT4 --------------------- Motor
```

---

# 🔋 Alimentação geral

Uma montagem típica pode utilizar:

```text
BATERIA DOS MOTORES
       |
       +-------- L298N
                  |
                  +-------- Motores


FONTE 5V SERVOS
       |
       +-------- Servo Garra
       |
       +-------- Servo Base


ESP32
       |
       +-------- USB / fonte apropriada
```

Mas os GNDs devem permanecer unidos.

```text
GND bateria
    |
GND L298N
    |
GND ESP32
    |
GND HC-SR04
    |
GND fonte dos servos
```

---

# 📱 Bluetooth

A ESP32 utiliza o Bluetooth interno.

O nome configurado no código é:

```text
RoboResgateESP32
```

No Android, procure esse dispositivo e faça a conexão através do aplicativo Kotlin.

Não é mais necessário utilizar:

```text
HC-05
HC-06
```

---

# 🎮 Comandos Bluetooth

O aplicativo pode enviar os seguintes comandos.

## Movimentação

```text
F = Frente

T = Trás

E = Esquerda

D = Direita

P = Parar

S = Parar
```

---

# 🤏 Garra

Os comandos começam com:

```text
G
```

Exemplos:

```text
G0
G45
G90
G135
G180
```

Exemplo:

```text
G140
```

envia a garra diretamente para 140 graus.

---

# ↕️ Base

Os comandos começam com:

```text
B
```

Exemplos:

```text
B0
B45
B90
B135
B180
```

Exemplo:

```text
B40
```

move o mecanismo de subida/descida para 40 graus.

---

# 📡 Solicitar ultrassônico

O comando:

```text
U
```

solicita uma medição do HC-SR04.

A ESP32 pode responder:

```text
DISTANCIA:25.4
```

---

# 📲 Dados enviados pela ESP32

Além de receber comandos, a ESP32 também pode enviar informações para o aplicativo.

Exemplos:

```text
STATUS:FRENTE

STATUS:TRAS

STATUS:ESQUERDA

STATUS:DIREITA

STATUS:PARADO
```

Servos:

```text
GARRA:140

BASE:90
```

Ultrassônico:

```text
DISTANCIA:27.8
```

---

# 🧠 Funcionamento do sistema

O fluxo básico fica:

```text
APLICATIVO KOTLIN
       |
       | Bluetooth
       ↓
      ESP32
       |
       ├──── Motores
       |
       ├──── Servo Garra
       |
       ├──── Servo Base
       |
       └──── HC-SR04
```

---

# 🚗 Controle dos motores

## Frente

```text
IN1 = HIGH
IN2 = LOW

IN3 = HIGH
IN4 = LOW
```

---

## Trás

```text
IN1 = LOW
IN2 = HIGH

IN3 = LOW
IN4 = HIGH
```

---

## Direita

```text
Motor esquerdo -> frente
Motor direito  -> trás
```

---

## Esquerda

```text
Motor esquerdo -> trás
Motor direito  -> frente
```

---

# 🧪 Ordem recomendada de testes

Faça os testes nesta ordem:

## 1. ESP32

Confirme que o código é enviado corretamente.

---

## 2. Bluetooth

Procure:

```text
RoboResgateESP32
```

no Android.

---

## 3. Motores

Teste:

```text
F
T
E
D
P
```

---

## 4. Servo da garra

Teste:

```text
G50

G90

G140
```

---

## 5. Servo da base

Teste:

```text
B40

B90

B140
```

---

## 6. Ultrassônico

Envie:

```text
U
```

e verifique se recebe algo como:

```text
DISTANCIA:31.5
```

---

# ❌ Problemas comuns

## Motores não giram

Verifique:

```text
ENA -> jumper instalado
ENB -> jumper instalado
```

Confira também:

```text
GND ESP32 -> GND L298N
```

---

## Um motor gira ao contrário

Troque:

```text
OUT1 <-> OUT2
```

ou:

```text
OUT3 <-> OUT4
```

dependendo do motor invertido.

---

## Servo está lento

Verifique a alimentação.

Uma alimentação fraca pode causar perda de velocidade e torque.

Prefira:

```text
Fonte externa 5V
```

com corrente suficiente.

---

## Servo treme

Possíveis causas:

- alimentação insuficiente;
- GND não compartilhado;
- esforço mecânico;
- ângulo além do limite da garra.

---

## Ultrassônico não funciona

Confira:

```text
TRIG -> GPIO 32

ECHO -> GPIO 33
```

Também verifique o divisor de tensão do ECHO.

---

## Bluetooth não aparece

Confirme se a placa ESP32 utilizada suporta Bluetooth Classic.

No código:

```cpp
SerialBT.begin("RoboResgateESP32");
```

Depois procure no celular por:

```text
RoboResgateESP32
```

---

# 📊 Resumo final

```text
┌──────────────────────────────────────┐
│               ESP32                  │
├─────────────────────┬────────────────┤
│ GPIO 18             │ Servo Garra    │
│ GPIO 19             │ Servo Base     │
│ GPIO 25             │ L298N IN1      │
│ GPIO 26             │ L298N IN2      │
│ GPIO 27             │ L298N IN3      │
│ GPIO 14             │ L298N IN4      │
│ GPIO 32             │ HC-SR04 TRIG   │
│ GPIO 33             │ HC-SR04 ECHO   │
│ GND                 │ GND comum      │
└─────────────────────┴────────────────┘
```

---

# ⚠️ Regra principal

```text
GND ESP32
   |
   +------ GND L298N
   |
   +------ GND HC-SR04
   |
   +------ GND fonte dos servos
   |
   +------ GND bateria
```

> **Todos os módulos precisam compartilhar o mesmo GND.**

---

# ✅ Configuração final

```text
ESP32

GPIO 18 -> Servo Garra
GPIO 19 -> Servo Base

GPIO 25 -> L298N IN1
GPIO 26 -> L298N IN2
GPIO 27 -> L298N IN3
GPIO 14 -> L298N IN4

GPIO 32 -> HC-SR04 TRIG
GPIO 33 -> HC-SR04 ECHO

Bluetooth -> interno da ESP32
```

O resultado é um robô com:

- controle Bluetooth;
- movimentação para frente e trás;
- giro para esquerda e direita;
- controle da garra;
- controle de subida/descida;
- leitura de distância;
- comunicação bidirecional com aplicativo Android;
- possibilidade de evolução para modo autônomo.
