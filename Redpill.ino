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
int nextRotaionTime = -1;

int prevSec = -1;
int currSec = 0;

boolean readyToServe = false;

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
    pillTimes[0] = 5;
    pillTimes[1] = 10;
    pillTimes[2] = 15;
    pillTimes[3] = 20;

    switchValues[0] = HIGH;
    switchValues[1] = HIGH;
    switchValues[2] = HIGH;
    switchValues[3] = HIGH;

    startTime = now(); // Debug

    digitalWrite(3, LOW);
    digitalWrite(2, LOW);
    digitalWrite(1, LOW);
}

void loop() {

    // Only calculate stuff once a second
    currSec = second();
    if (currSec != prevSec) {

        updateSwitchValues();
        if (currSec % 5 == 0) printSwitchValues(); // DEBUG

        // Check if there is time for the next chamber.
        // If so, rotate, turn on green LED,
        // set ready to serve

        // If all switches are off, nothing will occur.
        int nextState = getNextState();

        if (nextState != -1) {
            if (timeForNextState(nextState)) {
                digitalWrite(ledPinB, LOW);
                setState(nextState);
                rotateToNext();
                readyToServe = true;
                digitalWrite(ledPinG, HIGH);
                print("Ringing alarm.. Push the button to dispence!");
            }
        }

        prevSec = currSec;
        print("");

        // Reset at 45 seconds beyond midnight for testing purposes.
        if (currSec >= 25) {
            setTime(startTime);
            readyToServe = false;
        }
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


// Returns true if there is a pill to serve
// Input: int (0-3)
boolean timeForNextState(int nextState) {

    // Special code if only one switch is ON.
    if (currentState == nextState)
	return false;
	
    int nextRotationTime = pillTimes[nextState];

    // Converting the current time to an int that increases
    // each second through the the day
    String timeString = getTimeString(); // "hhmmss"
    int timeInt = timeString.toInt();

    // Check if we are in state EVENING (last state) and time is less than midnight
    // Need special code for this situation, since nextRotationTime is less than
    // current time.
    
    if (currentState == 3 && timeInt <= 235959 && timeInt > pillTimes[3]) {
        return false;
    }

    int nextNextState = nextState + 1;
    if (nextNextState > 3)
	nextNextState -= 4;

    int nextNextRotationTime = pillTimes[nextNextState];

    // Check if have entered the next time state
    if (timeInt >= nextRotationTime && timeInt < nextNextRotationTime)
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

// Returns the next state by checking current state and switch values
// Return -1 if all switches are off.
int getNextState() {

    // Loop trough the states {0, 1, 2, 3} as a ring, from current state.
    for (int i = 0; i < 4; i++) {
        int tmpState = currentState + 1 + i;

        if (tmpState > 3)
            tmpState -= 4;

        if (switchValues[tmpState] == HIGH)
            return tmpState;
    }
    return -1;
}

// Changes state to next valid state.
void setState(int state) {

    switch(state) {
    case 0:
        print("State MORNING");
        break;
    case 1:
        print("State NOON");
        break;
    case 2:
        print("State AFTERNOON");
        break;
    case 3:
        print("State EVENING");
        break;
    }

    currentState = state;
}

// Calculates the correct number of steps and rotates.
void rotateToNext() {
    int steps = 1;
    int debugCnt = 0;

    String tmp = "Rotating " + String(steps) + " steps..";
    print(tmp);

    rotateSteps(steps);

    print("Rotation complete. Ready to dispence pills.");
}

// Rotates the wheel a number of steps
void rotateSteps(int steps) {
    steps = steps * -29;

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
    //pinMode(ledPinR, OUTPUT); // Deaktivert
    pinMode(hatchPin, OUTPUT);

    pinMode(stepper1Pin, OUTPUT);
    pinMode(stepper2Pin, OUTPUT);
    pinMode(stepper3Pin, OUTPUT);
    pinMode(stepper4Pin, OUTPUT);

    //pinMode(hatchPin, OUTPUT);
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
