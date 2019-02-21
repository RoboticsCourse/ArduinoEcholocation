#include <Wire.h>
#include <Adafruit_MotorShield.h>

// defines pin numbers and thresholds
#define SIDE_TRIG_PIN 5
#define SIDE_ECHO_PIN 6
#define FRONT_TRIG_PIN 4
#define FRONT_ECHO_PIN 3
#define SIDE_THRESH_HIGH 100
#define SIDE_THRESH_LOW 50
#define FRONT_THRESH 50
#define SPEED 255

// setup motors
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *frontMotor = AFMS.getMotor(4);
Adafruit_DCMotor *rearMotor = AFMS.getMotor(2);
Adafruit_DCMotor *turnMotor = AFMS.getMotor(3);

// defines distance variables
int sideDist;
int frontDist;


// self-explanitory helper functions

void stopMotors() {
    frontMotor->run(RELEASE);
    rearMotor->run(RELEASE);
    turnMotor->run(RELEASE);
}

void goForward() {
    frontMotor->setSpeed(SPEED);
    frontMotor->run(FORWARD);
    rearMotor->setSpeed(SPEED);
    rearMotor->run(FORWARD);
}

void goBackward() {
    frontMotor->setSpeed(SPEED);
    frontMotor->run(BACKWARD);
    rearMotor->setSpeed(SPEED);
    rearMotor->run(BACKWARD);   
}

void straighten() {
	turnMotor->setSpeed(0);
	turnMotor->run(RELEASE);
}

void turnLeft() {
    turnMotor->setSpeed(255);
    turnMotor->run(FORWARD);
}

void turnRight() {
    turnMotor->setSpeed(255);
    turnMotor->run(BACKWARD);
}

void readSensors() {

	// clear the trig pins
	digitalWrite(SIDE_TRIG_PIN, LOW);
	digitalWrite(FRONT_TRIG_PIN, LOW);
	delayMicroseconds(2);

	// set the trig pins on HIGH state for 10 micro seconds
	digitalWrite(SIDE_TRIG_PIN, HIGH);
	digitalWrite(FRONT_TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(SIDE_TRIG_PIN, LOW);
	digitalWrite(FRONT_TRIG_PIN, LOW);

	// read the echo pins, get the sound wave travel time in microseconds, calculate the distance
	sideDist = pulseIn(SIDE_ECHO_PIN, HIGH) * 0.034/2;
	frontDist = pulseIn(FRONT_ECHO_PIN, HIGH) * 0.034/2;
}


void setup() {
	AFMS.begin();
	pinMode(SIDE_TRIG_PIN, OUTPUT);
	pinMode(SIDE_ECHO_PIN, INPUT);
	pinMode(FRONT_TRIG_PIN, OUTPUT);
	pinMode(FRONT_ECHO_PIN, INPUT);
}

void loop() {
	readSensors();
	if (frontDist < FRONT_THRESH) {
		if (sideDist < SIDE_THRESH_HIGH) { 	// when encountering front and side walls, backup to the right
			turnRight();
			goBackward();
			delay(1500);
		} else {							// when encountering only the front wall, back up to the left
			turnLeft();
			goBackward();
			delay(1500);
		}
	} else {	// when in a hallway, try to keep an equal distance between the walls
		if (sideDist < SIDE_THRESH_LOW) {
			turnLeft();
			goForward();
		} else if (sideDist < SIDE_THRESH_HIGH){
			straighten();
			goForward();
		} else {
			turnRight();
			goForward();
		}
	}
}