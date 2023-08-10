#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

//Servo SG90 cắm vao giắc 3 chân: SERVO1 (TRÊN L293D)

#define ECHO_PIN A0  //noi chan A0 (Uno)
#define TRIG_PIN A1  //noi chan A1 (Uno)

#define MAX_DISTANCE 200
#define MAX_SPEED 200  // cài đặt tốc độ động cơ DC motors
#define MAX_SPEED_OFFSET 50

AF_DCMotor motor1(2);  //motor - phai- noi pin 2 của L293D
AF_DCMotor motor2(3);  //motor - trai- noi pin 3 của L293D

Servo myservo;

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
boolean goesForward = false;
int distance = 100;
int speedSet = 0;
char command;
int dugme = 0;

void setup() {
  myservo.attach(9);  //tương ứng SERVO2;
  myservo.write(85);  //Chỉnh lại góc quay SG90 sao cho cảm biến siêu âm thẳng với xe
  delay(1000);
  distance = readPing();
  delay(100);

  Serial.begin(9600);  //Set the baud rate to your Bluetooth module.
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.println(command);
  }
  
  if (command == '1' && dugme == 0) {
    forward(200, 200);
  } else if (command == '2' && dugme == 0) {
    back(200, 200);
  } else if (command == '3' && dugme == 0) {
    aheadLeft(150);
  } else if (command == '4' && dugme == 0) {
    aheadRight(150);
  } else if (command == '6' && dugme == 0) {
    moveStop();
  } else if (command == '7') {
    dugme = 1;
  } else if (command == '8') {
    stopAvoiding();
  }
  delay(5);

  if (dugme == 1) {
    int distanceR = 0;
    int distanceL = 0;
    if (distance <= 20) {
      moveStop();
      delay(200);
      distanceR = lookRight();
      delay(200);
      distanceL = lookLeft();
      delay(200);
      moveBackward();
      delay(200);
      moveStop();
      delay(100);
      if (distanceR <= 40 && distanceL <= 40) {
        left(200, 200);
        delay(500);
        moveStop();
        delay(200);
      } else if (distanceR >= distanceL) {
        right(200, 200);
        delay(200);
        moveStop();
        delay(100);
      } else {
        left(200, 200);
        delay(200);
        moveStop();
        delay(100);
      }
    } else {
      forward(200, 200);
    }
    distance = readPing();
  }
}
void forward(int a, int b) {
  motor1.setSpeed(a);                     //Define maximum velocity
  motor1.run(FORWARD);                    //rotate the motor clockwise
  motor2.setSpeed(b + MAX_SPEED_OFFSET);  //Define maximum velocity
  motor2.run(FORWARD);                    //rotate the motor clockwise
}
void back(int a, int b) {
  motor1.setSpeed(a);
  motor1.run(BACKWARD);
  motor2.setSpeed(b + MAX_SPEED_OFFSET);
  motor2.run(BACKWARD);
}
void left(int a, int b) {
  motor1.setSpeed(a);
  motor1.run(FORWARD);
  motor2.setSpeed(b + MAX_SPEED_OFFSET);
  motor2.run(BACKWARD);
}
void right(int a, int b) {
  motor1.setSpeed(a);
  motor1.run(BACKWARD);
  motor2.setSpeed(b + MAX_SPEED_OFFSET);
  motor2.run(FORWARD);
}
void aheadLeft(int a) {
  motor1.setSpeed(a);
  motor1.run(FORWARD);
  motor2.setSpeed((a + MAX_SPEED_OFFSET) / 2);
  motor2.run(FORWARD);
}
void aheadRight(int a) {
  motor1.setSpeed(a / 2);
  motor1.run(FORWARD);
  motor2.setSpeed(a + MAX_SPEED_OFFSET);
  motor2.run(FORWARD);
}
void stopAvoiding() {
  dugme = 0;
  motor1.setSpeed(0);
  motor2.run(RELEASE);
  motor2.setSpeed(0);
  motor2.run(RELEASE);
}

int lookRight() {
  myservo.write(0);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(85);
  return distance;
}

int lookLeft() {
  myservo.write(170);
  delay(500);
  int distance = readPing();
  delay(100);
  myservo.write(85);
  return distance;
}

int readPing() {
  delay(70);
  int cm = sonar.ping_cm();
  if (cm <= 0) {
    cm = 250;
  }
  return cm;
}

void moveStop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void moveForward() {
  if (!goesForward) {
    goesForward = true;
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    // slowly bring the speed up to avoid loading down the batteries too quickly
    for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
      motor1.setSpeed(speedSet);
      motor2.setSpeed(speedSet + MAX_SPEED_OFFSET);
      delay(5);
    }
  }
}

void moveBackward() {
  goesForward = false;
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  // slowly bring the speed up to avoid loading down the batteries too quickly
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet += 2) {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet + MAX_SPEED_OFFSET);
    delay(5);
  }
}
