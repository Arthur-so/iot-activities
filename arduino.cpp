//Arduino 

// -------------------- ESTADOS PRINCIPAIS --------------------
enum State {
    IDLE,      // 00
    CLEANING,  // 01
    DOCKING,   // 10
    CHARGING   // 11
};

const int INPUT1 = 24;
const int INPUT2 = 25;
const int LED1 = 52;
const int LED2 = 53;

// Saídas para mandar estado de distância ao ESP32
const int DIST_OUT1 = 30;  // bit mais significativo
const int DIST_OUT2 = 31;  // bit menos significativo

// Sensor ultrassônico
const int TRIG = 5;
const int ECHO = 6;

unsigned long lastMillis1;
unsigned long lastMillis2;

unsigned long time_50 = 50;
unsigned long time_100 = 100;

int LED1_state = LOW;
int LED2_state = LOW;

// -------------------- ESTADOS DE DISTÂNCIA --------------------
enum DistState {
  NO_OBS = 0,   // 00
  FAR = 1,      // 01
  MID = 2,      // 10
  NEAR = 3      // 11
};

DistState distState = NO_OBS;
DistState lastDistState = NO_OBS;

void setup()
{
  Serial.begin(115200);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Configura saídas para ESP32
  pinMode(DIST_OUT1, OUTPUT);
  pinMode(DIST_OUT2, OUTPUT);
}

void loop()
{
  // ---- Máquina de estados principal ----
  int input1 = digitalRead(INPUT1);
  int input2 = digitalRead(INPUT2);
  int state = (input1 << 1) | input2;
  Serial.print("State: ");
  Serial.println(state);

  switch(state) {
    case IDLE: idle(); break;
    case CLEANING: cleaning(); break;
    case DOCKING: docking(); break;
    case CHARGING: charging(); break;
  }

  // ---- Máquina de estados de distância ----
  checkDistance();
}

// -------------------- ESTADOS PRINCIPAIS --------------------
void idle() {
  Serial.println("idle");
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

void cleaning() {
  Serial.println("cleaning");
  if(millis()-lastMillis1 > time_100) {
    lastMillis1 = millis();
    LED1_state = !LED1_state;
    digitalWrite(LED1, LED1_state);
  }
  digitalWrite(LED2, LOW);
}

void docking() {
  Serial.println("docking");
  if(millis()-lastMillis2 > time_50) {
    lastMillis2 = millis();
    LED2_state = !LED2_state;
    digitalWrite(LED2, LED2_state);
  }
  digitalWrite(LED1, LOW);
}

void charging() {
  Serial.println("charging");
  if(millis()-lastMillis1 > time_100) {
    lastMillis1 = millis();
    LED1_state = !LED1_state;
    digitalWrite(LED1, LED1_state);
  }
  if(millis()-lastMillis2 > time_50) {
    lastMillis2 = millis();
    LED2_state = !LED2_state;
    digitalWrite(LED2, LED2_state);
  }
}

// -------------------- DISTÂNCIA --------------------
void checkDistance() {
  // Mede distância com HC-SR04
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  int distance = duration * 0.034 / 2; // em cm

  // Define limites (ajuste conforme necessidade)
  if (distance > 20 || distance <= 0) {
    distState = NO_OBS;
  } else if (distance > 10) {
    distState = FAR;
  } else if (distance > 5) {
    distState = MID;
  } else {
    distState = NEAR;
  }

  // Só envia se mudou
  if (distState != lastDistState) {
    Serial.print("Dist state: ");
    Serial.println(distState, BIN); // debug

    // ---- Saídas digitais para ESP32 ----
    digitalWrite(DIST_OUT1, (distState >> 1) & 0x01); // bit mais significativo
    digitalWrite(DIST_OUT2, distState & 0x01);        // bit menos significativo

    lastDistState = distState;
  }
}
