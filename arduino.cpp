// C++ code
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

unsigned long lastMillis1;
unsigned long lastMillis2;

unsigned long time_50 = 50;
unsigned long time_100 = 100;

int LED1_state = LOW;
int LED2_state = LOW;

void setup()
{
  Serial.begin(115200);
  pinMode(INPUT1, INPUT);
  pinMode(INPUT2, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}

void loop()
{
 	int input1 = digitalRead(INPUT1);
  	int input2 = digitalRead(INPUT2);
  	int state = (input1 << 1) | input2;
  	Serial.println(state);
	switch(state) {
    	case IDLE:
      		idle();
      		break;
    	case CLEANING:
      		cleaning();      		
      		break;
      	case DOCKING:
      		docking();
      		break;
      	case CHARGING:
			charging();
      		break;
  	}
  	
  
}

void idle() {
  	Serial.println("idle");
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
}

void cleaning() {
    Serial.println("cleaning");
  	if(millis()-lastMillis1 > time_100) {
      	lastMillis1 = millis();
      	LED1_state = (LED1_state == LOW) ? HIGH : LOW;
      	digitalWrite(LED1, LED1_state);
    }
  	digitalWrite(LED2, LOW);
}

void docking() {
    Serial.println("docking");
  	if(millis()-lastMillis2 > time_50) {
      	lastMillis2 = millis();
      	LED2_state = (LED2_state == LOW) ? HIGH : LOW;
      	digitalWrite(LED2, LED2_state);
    }
	digitalWrite(LED1, LOW);
}

void charging() {
	Serial.println("charging");
  	if(millis()-lastMillis1 > time_100) {
      	lastMillis1 = millis();
      	LED1_state = (LED1_state == LOW) ? HIGH : LOW;
      	digitalWrite(LED1, LED1_state);
    }
  	if(millis()-lastMillis2 > time_50) {
      	lastMillis2 = millis();
      	LED2_state = (LED2_state == LOW) ? HIGH : LOW;
      	digitalWrite(LED2, LED2_state);
    }
}
  
