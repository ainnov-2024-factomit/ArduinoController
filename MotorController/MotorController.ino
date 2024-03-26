// // Define the motor pins
// const int motorPin1 = 12;
// const int motorPin2 = 11;

// void setup() {
//     Serial.begin(115200);

//     // Set the motor pins as outputs
//     pinMode(motorPin1, OUTPUT);
//     pinMode(motorPin2, OUTPUT);
// }

// void loop() {
//     Serial.println("Running the motor pin 1");
//     // Run the motor forward
//     digitalWrite(motorPin1, HIGH);
//     digitalWrite(motorPin2, LOW);

//     // Wait for 2 seconds
//     Serial.println("Wait");
//     delay(2000);

//     Serial.println("Stop the motor pin 1");
//     // Stop the motor
//     digitalWrite(motorPin1, LOW);
//     digitalWrite(motorPin2, LOW);

//     Serial.println("Wait");
//     // Wait for 1 second
//     delay(1000);

//     Serial.println("Running the motor pin 2");
//     // Run the motor backward
//     digitalWrite(motorPin1, LOW);
//     digitalWrite(motorPin2, HIGH);

//     // Wait for 2 seconds
//     Serial.println("Wait");
//     delay(2000);

//     // Stop the motor
//     Serial.println("Stop the motor pin 2");
//     digitalWrite(motorPin1, LOW);
//     digitalWrite(motorPin2, LOW);

//     Serial.println("Wait");
//     // Wait for 1 second
//     delay(1000);
// }

////////////////////////////////////////////////////////////////////



const int input_voltage  = 9; //V
const int nominal_voltage  = 5; ////V
const int MAX_SPEED  = int(nominal_voltage * 255 / input_voltage);

//uncomment if using channel B, and remove above definitions
//int directionPin = 13;
//int pwmPin = 11;
//int brakePin = 8;

//boolean to switch direction
bool directionState;

void setup() {

Serial.begin(115200);
  
//define pins
pinMode(directionPinA, OUTPUT);
pinMode(pwmPinA, OUTPUT);
pinMode(brakePinA, OUTPUT);

directionState = true;

}

void loop() {
Serial.println("Start loop");

//change direction every loop()
directionState = !directionState;

Serial.print("Direction: ");
Serial.println(directionState);

//write a low state to the direction pin (13)
if(directionState == false){
  digitalWrite(directionPinA, LOW);
}

//write a high state to the direction pin (13)
else{
  digitalWrite(directionPinA, HIGH);
}

//release breaks
Serial.println("Release brakes on motor A");
digitalWrite(brakePinA, LOW);

//set work duty for the motor
Serial.print("Move motor A - Speed: ");
Serial.println(MAX_SPEED);
analogWrite(pwmPinA, 255);

delay(2000);

//activate breaks
Serial.println("Apply brakes on motor A");
digitalWrite(brakePinA, HIGH);

//set work duty for the motor to 0 (off)
Serial.println("Set motor A speed to 0");
analogWrite(pwmPinA, 0);

delay(2000);
}