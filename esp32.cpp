// esp32
enum State {
    IDLE,      // 00
    CLEANING,  // 01
    DOCKING,   // 10
    CHARGING   // 11
};

State state = IDLE;

const int OUTPUT1 = 2;
const int OUTPUT2 = 4;
const int LED3 = 25;

unsigned long lastMillis;
unsigned long tempo = 2000;

void setup() {
  Serial.begin(115200);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  dacWrite(LED3, 255);
}

void loop() {
  char input;
  if (Serial.available() > 0) {
    input = Serial.read();
  } else {
    input = '\0';
  }

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

void idle() {
  Serial.println("idle");
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, LOW);
  // LED sempre ligado
  dacWrite(LED3, 255);
}

void cleaning() {
  Serial.println("cleaning");
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, HIGH);

  // tempo decorrido
  unsigned long elapsed = millis() - lastMillis;
  if (elapsed > tempo) elapsed = tempo;

  // intensidade decresce de 255 -> 0
  int brightness = map(elapsed, 0, tempo, 255, 0);
  dacWrite(LED3, brightness);
}

void docking() {
  Serial.println("docking");
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, LOW);
  // LED sempre apagado
  dacWrite(LED3, 0);
}

void charging() {
  Serial.println("charging");
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, HIGH);

  // tempo decorrido
  unsigned long elapsed = millis() - lastMillis;
  if (elapsed > tempo) elapsed = tempo;

  // intensidade cresce de 0 -> 255
  int brightness = map(elapsed, 0, tempo, 0, 255);
  dacWrite(LED3, brightness);
}
