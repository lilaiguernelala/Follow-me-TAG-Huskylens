#include "HUSKYLENS.h"
#include <Motor.h>
#include "SoftwareSerial.h"

HUSKYLENS huskylens;
SoftwareSerial mySerial(20, 21);
int ledPin = 13;

Motor motor1(6, 7);
Motor motor2(9, 8);
bool continueForward = false;

void printResult(HUSKYLENSResult result);
void stop();
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void Asservissement(HUSKYLENSResult result);

#define DIR_G 7
#define PWM_G 6
#define DIR_D 8
#define PWM_D 9

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);
    Wire.begin();

    while (!huskylens.begin(Wire)) {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1. Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings >> Protocol Type >> I2C)"));
        Serial.println(F("2. Please recheck the connection."));
        delay(100);
    }

    pinMode(ledPin, OUTPUT);
    pinMode(DIR_G, OUTPUT);
    pinMode(DIR_D, OUTPUT);
}

void loop() {
    if (!huskylens.request()) {
        Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    } else if (!huskylens.isLearned()) {
        Serial.println(F("Nothing learned, press the learn button on HUSKYLENS to learn one!"));
    } else if (!huskylens.available()) {
        Serial.println(F("No block or arrow appears on the screen!"));
        stop();
    } else {
        Serial.println(F("###########"));
        while (huskylens.available()) {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
            driveBot(result);
        }
    }
}

void printResult(HUSKYLENSResult result) {
    Serial.println(String() + F("height: ") + result.height + F("\txCenter: ") + result.xCenter);
}

void driveBot(HUSKYLENSResult result) {
    if (result.ID == 1) {
        continueForward = true;
    } else {
        continueForward = false;
    }

    if (continueForward) {
        moveForward();
    } else {
        stop();
    }
}

void stop() {
    motor1.setU(0);
    motor2.setU(0);
    digitalWrite(ledPin, LOW);
    Serial.println("Stop");
}

void moveForward() {
    // Decrease the speed by 25%
    int speed = -200;
    int reducedSpeed = int(speed * 0.75); 
    motor1.setU(reducedSpeed);
    motor2.setU(reducedSpeed);
    digitalWrite(ledPin, HIGH);
    Serial.println("Move Forward");
}

void moveBackward() {
    // Decrease the speed by 25%
    int speed = 200;
    int reducedSpeed = int(speed * 0.75);  
    motor1.setU(reducedSpeed);
    motor2.setU(reducedSpeed);
    digitalWrite(ledPin, HIGH);
    Serial.println("Move Backward");
}

void moveLeft() {
    // Decrease the speed by 25%
    int speed = -200;
    int reducedSpeed = int(speed * 0.75); 
    motor1.setU(-reducedSpeed);
    motor2.setU(reducedSpeed);
    digitalWrite(ledPin, HIGH);
    Serial.println("Move Left");
}

void moveRight() {
    // Decrease the speed by 25%
    int speed = -200;
    int reducedSpeed = int(speed * 0.75); 
    motor1.setU(reducedSpeed);
    motor2.setU(-reducedSpeed);
    digitalWrite(ledPin, HIGH);
    Serial.println("Move Right");
}

void Asservissement(HUSKYLENSResult result) {
    int consigneAngulaire = 0;
    int consigneLineaire = 100;
    int tagX = result.xCenter - 160;
    int tagH = result.height;
    float a = tagX * (0.5);
    float l = consigneLineaire;
    cmd_robot(l, a);
    Serial.println(String() + F("l: ") + l + String() + F("\ta: ") + a);
}

void cmd_robot(int lineaire, int angulaire) {
    int nG = (lineaire + angulaire);
    int nD = (lineaire - angulaire);
    cmd_moteur(nG, nD);
}

void cmd_moteur(int nG, int nD) {
    motor1.setU(-nG);
    motor2.setU(-nD);
}
