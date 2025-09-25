// C++ code
enum State {
    IDLE,      // 00
    CLEANING,  // 01
    DOCKING,   // 10
    CHARGING   // 11
};

State state = IDLE;

const int OUTPUT1 = 2;
const int OUTPUT2 = 4;
const int LED_PIN = 25;

unsigned long lastMillis;
unsigned long time = 500;

int ledValue = 255;
int fadeStep = 5;
unsigned long lastFadeMillis;
unsigned long fadeInterval = 30;

void setup()
{
  Serial.begin(115200);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);
  dacWrite(LED_PIN, 255);
}

void loop()
{
  char input;
  if (Serial.available() > 0) {
    input = Serial.read();
  } else {
    input = '\0';
  }

  switch(state) {
    case IDLE:
      idle();
      if(input == 'a') {
        state = CLEANING;
        lastMillis = millis();
        ledValue = 255;  // brilho máximo ao entrar
      }
      break;

    case CLEANING:
      cleaning();      		
      if(input == 'b' || millis()-lastMillis > time) {
        state = DOCKING;
        lastMillis = millis();
        ledValue = 0;   // brilho mínimo ao sair
        dacWrite(LED_PIN, ledValue);
      }
      break;

    case DOCKING:
      docking();
      if(millis()-lastMillis > time) {
        state = CHARGING;
        lastMillis = millis();
        ledValue = 0;   // inicia apagado ao entrar no charging
      }
      break;

    case CHARGING:
      charging();
      if(input == 'a') {
        state = CLEANING;
        ledValue = 255; // começa no máximo de novo
      }
      if(millis()-lastMillis > time) {
        state = IDLE;
        lastMillis = millis();
      }
      break;
  }
}

// ----- Funções dos estados -----

void idle() {
  Serial.println("idle");
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, LOW);

  // LED sempre ligado no máximo
  dacWrite(LED_PIN, 255);
}

void cleaning() {
  Serial.println("cleaning");
  digitalWrite(OUTPUT1, LOW);
  digitalWrite(OUTPUT2, HIGH);

  // LED reduzindo intensidade gradualmente
  if (millis() - lastFadeMillis > fadeInterval) {
    lastFadeMillis = millis();
    if (ledValue > 0) {
      ledValue -= fadeStep;
      if (ledValue < 0) ledValue = 0;
      dacWrite(LED_PIN, ledValue);
    }
  }
}

void docking() {
  Serial.println("docking");
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, LOW);	

  // LED apagado
  dacWrite(LED_PIN, 0);
}

void charging() {
  Serial.println("charging");
  digitalWrite(OUTPUT1, HIGH);
  digitalWrite(OUTPUT2, HIGH);

  // LED aumentando intensidade gradualmente
  if (millis() - lastFadeMillis > fadeInterval) {
    lastFadeMillis = millis();
    if (ledValue < 255) {
      ledValue += fadeStep;
      if (ledValue > 255) ledValue = 255;
      dacWrite(LED_PIN, ledValue);
    }
  }
}

