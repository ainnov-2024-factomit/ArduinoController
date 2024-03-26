// Define the pins for the devices
// Ultrasound depth
const int depthUsTrigPin = 7;
const int depthUsEchoPin = 10;

// Ultrasound basket height
const int heightUsTrigPin = 2;
const int heightUsEchoPin = 4;

// Potentiometer measurement
const int rotationPotentiometerPinA = 5;
const int rotationPotentiometerPinB = 6;

// Depth motor
const int depthMotorDirectionPin = 12;
const int depthMotorPwmPin = 3;
const int depthMotorBrakePin = 9;

// Rotation motor
const int rotationMotorDirectionPin = 13;
const int rotationMotorPwmPin = 11;
const int rotationMotorBrakePin = 8;

// Basket motor
const int basketMotorControlPin = 0;

// Define the current commands
int currentCommands[3][2];

// Data storage
int rotationMotorLastRawState = 0;
int rotationMotorAngle = 0;
int rotationAngleStep = 0;
int data[3];

void setup()
{
    // Initialize the serial communication
    Serial.begin(115200);

    ////////////////////////////////////////////////

    // Set pins modes
    // Depth sensor
    pinMode(depthUsTrigPin, OUTPUT);
    pinMode(depthUsEchoPin, INPUT);

    // Height sensor
    pinMode(heightUsTrigPin, OUTPUT);
    pinMode(heightUsEchoPin, INPUT);

    // Rotation potentiometer
    pinMode(rotationPotentiometerPinA, INPUT);
    pinMode(rotationPotentiometerPinB, INPUT);

    // Set Motors pins modes
    // Depth motor
    pinMode(depthMotorDirectionPin, OUTPUT);
    pinMode(depthMotorPwmPin, OUTPUT);
    pinMode(depthMotorBrakePin, OUTPUT);

    // Rotation motor
    pinMode(rotationMotorDirectionPin, OUTPUT);
    pinMode(rotationMotorPwmPin, OUTPUT);
    pinMode(rotationMotorBrakePin, OUTPUT);

    pinMode(basketMotorControlPin, OUTPUT);

    ////////////////////////////////////////////////

    // Init values
    rotationMotorLastRawState = digitalRead(rotationPotentiometerPinA);
}

void loop() {
    UpdateFromSerial();

    // Get and store data
    data[0] = GetUltrasoundData(heightUsTrigPin, heightUsEchoPin);
    data[1] = GetUltrasoundData(depthUsTrigPin, depthUsEchoPin);
    data[2] = GetPotentiometerData(rotationMotorDirectionPin, rotationMotorPwmPin);

    MoveCrane();

    WriteDataToSerial();
}

void UpdateFromSerial()
{ 
    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n');
        
        if (command[0] != 'c')
        {
            Serial.flush();
            return;
        }

        String strs[6];
        int stringCount = 0;
        while (command.length() > 0)
        {
            int index = command.indexOf(',');
            if (index == -1) // No space found
            {
                strs[stringCount++] = command;
                break;
            }
            else
            {
                strs[stringCount++] = command.substring(0, index);
                command = command.substring(index+1);
            }
        }

        currentCommands[0][0] = strs[1].toInt();
        currentCommands[0][1] = strs[2].toInt();
        currentCommands[1][0] = strs[3].toInt();
        currentCommands[1][1] = strs[4].toInt();
        currentCommands[2][0] = strs[5].toInt();
        currentCommands[2][1] = strs[6].toInt();

        Serial.flush();
    }
}

void WriteDataToSerial()
{
    String command = "";

    command += "d,";
    command += data[0];
    command += ",";
    command += data[1];
    command += ",";
    command += data[2];

    Serial.println(command);
}

int GetUltrasoundData(int trigPin, int echoPin)
{
    // Send a pulse to the ultrasound device
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measure the duration of the echo pulse
    long duration = pulseIn(echoPin, HIGH);

    // Calculate the distance in centimeters
    float distance = duration * 0.034 / 2;

    return distance;
}

int GetPotentiometerData(int pinA, int pinB) {
    int potentiometerState = digitalRead(pinA);    
   
   if (potentiometerState != rotationMotorLastRawState){     
     // outputB != outputA state, encoder is rotating clockwise
     if (digitalRead(pinB) != potentiometerState) { 
      movePotentiometerClockwise();
     } 
     else {
       movePotentiometerCounterClockwise();
     }
   }

    rotationMotorLastRawState = potentiometerState;
}

void movePotentiometerClockwise() {
    rotationMotorAngle = rotationMotorAngle + rotationAngleStep;
}

void movePotentiometerCounterClockwise() {
    rotationMotorAngle = rotationMotorAngle - rotationAngleStep;
}

void MoveCrane() {
    // TODO Fix basket handling
    // Move basket motor
    if (currentCommands[0][0] == 1)
        analogWrite(basketMotorControlPin, HIGH);
    else
        analogWrite(basketMotorControlPin, LOW);

    // Move depth motor
    if (currentCommands[1][0] == 1)
        digitalWrite(depthMotorDirectionPin, HIGH);
    else
        digitalWrite(depthMotorDirectionPin, LOW);

    int depthMotorSpeed = GetMotorSpeedFromPercentage(currentCommands[1][1]);
    if(depthMotorSpeed > 0)
        digitalWrite(depthMotorBrakePin, LOW);
    else
        digitalWrite(depthMotorBrakePin, HIGH);

    analogWrite(depthMotorPwmPin, depthMotorSpeed);

    // Move rotation motor
    if (currentCommands[2][0] == 1)
        digitalWrite(rotationMotorDirectionPin, HIGH);
    else
        digitalWrite(rotationMotorDirectionPin, LOW);

    int rotationMotorSpeed = GetMotorSpeedFromPercentage(currentCommands[2][1]);
    if(rotationMotorSpeed > 0)
        digitalWrite(rotationMotorBrakePin, LOW);
    else
        digitalWrite(rotationMotorBrakePin, HIGH);

    analogWrite(rotationMotorPwmPin, rotationMotorSpeed);
}

int GetMotorSpeedFromPercentage(int percentage)
{
    const int input_voltage  = 9; //V
    const int nominal_voltage  = 5; ////V
    const int MAX_SPEED  = int(nominal_voltage * 255 / input_voltage);
    
    return percentage * MAX_SPEED / 100;
}