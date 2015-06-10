// The dipsenser will dispense at 0800, 1200, 1600, 2000.
// (While testing at 10, 20, 30, 40 secs).
// If the dispence button is not pressed within the next time, the
// the chamber rotates past.


// When the button is pressed and there is a dose ready,
// the hatch will open for HATCH_TIME millis.

#include <Time.h>
#include <Stepper.h>
#include <Servo.h>


// Pins-------------------
const int hatchPin = 13;

const int stepper1Pin = 12;
const int stepper2Pin = 11;
const int stepper3Pin = 10;
const int stepper4Pin = 9;

const int switch1Pin = 8;
const int switch2Pin = 5;
const int switch3Pin = 6;
const int switch4Pin = 7;

const int buttonPin = 4;

const int ledPinB = 3;
const int ledPinG = 2;


// Constants---------------
const int HATCH_TIME = 1000;
const int ROTATION_TIME = 2000;

// At which times pills are to be dispenced.
int pillTimes[4];

// Wich of the times to use.
int switchValues[4];

// Initialization, set current state to last state
int currentState = 3;

int prevSec = -1;
int currSec = 0;
int resetTime = 32; // DEBUG

boolean readyToServe = false;
boolean longRotation = true;

time_t startTime; // Debug (To reset time)

// Servo object
Servo servo;

// Stepper object
Stepper stepperMotor = Stepper(512, stepper1Pin, stepper2Pin, stepper3Pin, stepper4Pin);
const int STEPPER_SPEED = 10;

void setup() {
    setPinModes();

    // Attaches the servo to a servo object
    servo.attach(hatchPin);
    stepperMotor.setSpeed(STEPPER_SPEED);
    servo.write(0); // Set servo to start pos.

    Serial.begin(9600);
    delay(200);

    // Normally the values will be
    // 80000, 120000, 160000, 200000
    // However for testing purposes, they are set
    // within the first minute after midnight.
    pillTimes[0] = 2;
    pillTimes[1] = 10;
    pillTimes[2] = 18;
    pillTimes[3] = 26;

    switchValues[0] = HIGH;
    switchValues[1] = HIGH;
    switchValues[2] = HIGH;
    switchValues[3] = HIGH;

    startTime = now(); // Debug
}

void loop() {

    // Only calculate stuff once a second
    currSec = second();
    if (currSec != prevSec) {

        updateSwitchValues();
        if (currSec % 5 == 0) printSwitchValues(); // DEBUG
	
	// Check if there is time for next state
	// If so, we rotate
        if (timeForNextState()){
            goToNextState();
	    rotateSteps(1);
            delay(500);

            // If the switch is ON for this state we rotate.
	    if (switchValues[currentState] == HIGH) {
		readyToServe = true;
                digitalWrite(ledPinB, LOW);
                digitalWrite(ledPinG, HIGH);
                print("Ringing alarm.. Push the button to dispence!");
            }
	    else {
		readyToServe = false;
		digitalWrite(ledPinG, LOW);
		digitalWrite(ledPinB, HIGH);
	    }
	}

        String status = "State: " + String(currentState);
        print(status);

        // Reset at resetTime seconds beyond midnight for testing purposes.
        if (currSec >= resetTime) {
            setTime(startTime);
        }
        prevSec = currSec;
    }

    // Check if the button is pressed and we are ready to serve
    // a pill, if so we dispence it.
    if (digitalRead(buttonPin) == HIGH) {
        if (readyToServe) {
            digitalWrite(ledPinG, LOW);
            openHatch(); // Blocking
            digitalWrite(ledPinB, HIGH);
            readyToServe = false;
        }
    }
}


// Returns true if we have entered next state
// Input: int (0-3)
boolean timeForNextState() {
    int nextState = (currentState == 3) ? 0 : currentState + 1;
    int nextStateTime = pillTimes[nextState];

    // Converting the current time to an int that increases
    // each second through the the day
    String timeString = getTimeString(); // "hhmmss"
    int timeInt = timeString.toInt();

    // Check if we are in state EVENING (last state) and time is less than midnight
    // Need special code for this situation, since nextStateTime is less than
    // current time.

    if (currentState == 3 && timeInt <= 235959 && timeInt > pillTimes[3]) {
        return false;
    }

    // Check if have entered the next time state
    if (timeInt >= nextStateTime)// && timeInt < nextNextRotationTime)
        return true;

    return false;
}

// Creates time string in format "hhmmss"
String getTimeString() {
    String hr = toString2Digits(hour());
    String min = toString2Digits(minute());
    String sec = toString2Digits(second());
    String s = String(hour()) + String(minute()) + String(second());

    return s;
}

// Changes state to next state.
void goToNextState() {
    if (currentState == 3)
        currentState = 0;
    else
        currentState++;

    switch(currentState) {
    case 0:
        print("Entered state MORNING");
        break;
    case 1:
        print("Entered state NOON");
        break;
    case 2:
        print("Entered state AFTERNOON");
        break;
    case 3:
        print("Entered state EVENING");
        break;
    }
}


// Rotates the wheel a number of steps
void rotateSteps(int steps) {
    int scale = longRotation ? -29 : -28;
    longRotation = !longRotation;

    print("Rotating");
    
    steps = steps * scale;

    // Blocking line, waits until rotation is complete
    stepperMotor.step(steps);
}

// Opens the hatch
void openHatch() {
    print("Button pressed, dispensing pills...");

    servo.write(90);
    delay(HATCH_TIME);
    servo.write(0);
    delay(HATCH_TIME);

    print("Pills dispenced.");
}


// Returns a two-digit string
String toString2Digits(int number) {
    if (number < 10) {
        return String("0" + String(number));
    }
    return String(number);
}


// Set pin modes
void setPinModes() {
    pinMode(switch1Pin, INPUT);
    pinMode(switch2Pin, INPUT);
    pinMode(switch3Pin, INPUT);
    pinMode(switch4Pin, INPUT);

    pinMode(buttonPin, INPUT);

    pinMode(ledPinB, OUTPUT);
    pinMode(ledPinG, OUTPUT);

    pinMode(stepper1Pin, OUTPUT);
    pinMode(stepper2Pin, OUTPUT);
    pinMode(stepper3Pin, OUTPUT);
    pinMode(stepper4Pin, OUTPUT);

    pinMode(hatchPin, OUTPUT);
}

void updateSwitchValues() {
    switchValues[0] = !digitalRead(switch1Pin);
    switchValues[1] = !digitalRead(switch2Pin);
    switchValues[2] = !digitalRead(switch3Pin);
    switchValues[3] = !digitalRead(switch4Pin);
}

void printSwitchValues() {
    Serial.print("Switch values: ");
    for (int i = 0; i < 4; i++) {
        Serial.print(switchValues[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void print(String s) {
    Serial.print("[");
    printTime();
    Serial.print("]: ");
    Serial.println(s);

}

void printTime() {
    printDigits(hour());
    Serial.print(":");

    printDigits(minute());
    Serial.print(":");

    printDigits(second());
}

// Prints preceding colon and leading 0
void printDigits(int digits) {
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
