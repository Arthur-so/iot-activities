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


unsigned long lastMillis;
unsigned long time = 500;

void setup()
{
  Serial.begin(115200);
  pinMode(OUTPUT1, OUTPUT);
  pinMode(OUTPUT2, OUTPUT);

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
            }
      		break;
      
    	case CLEANING:
      		cleaning();      		
      		if(input == 'b' || millis()-lastMillis > time) {
        		state = DOCKING;
              	lastMillis = millis();
            }
      		break;
      	case DOCKING:
      		docking();
      		if(millis()-lastMillis > time) {
              	state = CHARGING;
              	lastMillis = millis();
            }
      		break;
      	case CHARGING:
			charging();
      		if(input == 'a') {
        		state = CLEANING;
            }
      		if(millis()-lastMillis > time) {
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
}

void cleaning() {
    Serial.println("cleaning");
  	digitalWrite(OUTPUT1, LOW);
	digitalWrite(OUTPUT2, HIGH);
}

void docking() {
    Serial.println("docking");
  	digitalWrite(OUTPUT1, HIGH);
	digitalWrite(OUTPUT2, LOW);	
}

void charging() {
	Serial.println("charging");
  	digitalWrite(OUTPUT1, HIGH);
	digitalWrite(OUTPUT2, HIGH);
}
  
