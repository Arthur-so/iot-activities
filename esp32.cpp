//esp32

enum State {
    IDLE,      
    CLEANING, 
    DOCKING,  
    CHARGING   
};

State state = IDLE;

const int OUTPUT1 = 2;
const int OUTPUT2 = 4;
const int LED3   = 25;  

const int DIST_IN1 = 22; 
const int DIST_IN2 = 23;  

unsigned long lastMillis;
unsigned long tempo = 10000;

unsigned long lastPrint = 0;
unsigned long printInterval = 1000;

void setup() {
  Serial.begin(115200);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);

  pinMode(DIST_IN1, INPUT);
  pinMode(DIST_IN2, INPUT);

  dacWrite(LED3, 255); 
}

void loop() {
  char input;
  if (Serial.available() > 0) {
    input = Serial.read();
  } else {
    input = '\0';
  }

  // ---------- Lê estado de distância do Arduino ----------
  static int lastDistState = -1; 
  int d1 = digitalRead(DIST_IN1);
  int d2 = digitalRead(DIST_IN2);
  int distState = (d1 << 1) | d2; 

  // imprime apenas quando muda OU a cada printInterval
  if (distState != lastDistState || millis() - lastPrint >= printInterval) {
    lastPrint = millis();
    lastDistState = distState;

    switch (distState) {
      case 0: Serial.println("Sem obstáculos"); break;
      case 1: Serial.println("Longe"); break;
      case 2: Serial.println("Meia distância"); break;
      case 3: Serial.println("Perto"); break;
    }
  }

  // ---------- Máquina de estados principal ----------
  switch (state) {
    case IDLE:
      idle();
      if (input == 'a') {
        state = CLEANING;
        lastMillis = millis();
      }
      break;

    case CLEANING:
      cleaning();
      if (input == 'b' || millis() - lastMillis > tempo) {
        state = DOCKING;
        lastMillis = millis();
      }
      break;

    case DOCKING:
      docking();
      if (millis() - lastMillis > tempo) {
        state = CHARGING;
        lastMillis = millis();
      }
      break;

    case CHARGING:
      charging();
      if (input == 'a') {
        state = CLEANING;
        lastMillis = millis();
      }
      if (millis() - lastMillis > tempo) {
        state = IDLE;
        lastMillis = millis();
      }
      break;
  }
}

// ---------------- FUNÇÕES DOS ESTADOS ----------------
void idle() {
  //Serial.println("idle");
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, LOW);
  dacWrite(LED3, 255); 
}

void cleaning() {
  //Serial.println("cleaning");
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, HIGH);

  unsigned long elapsed = millis() - lastMillis;
  if (elapsed > tempo) elapsed = tempo;

  int brightness = map(elapsed, 0, tempo, 255, 0);
  dacWrite(LED3, brightness);
}

void docking() {
  //Serial.println("docking");
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, LOW);
  dacWrite(LED3, 0); 
}

void charging() {
  //Serial.println("charging");
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, HIGH);

  unsigned long elapsed = millis() - lastMillis;
  if (elapsed > tempo) elapsed = tempo;

  int brightness = map(elapsed, 0, tempo, 0, 255);
  dacWrite(LED3, brightness);
}
