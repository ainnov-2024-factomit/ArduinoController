// Define the pins for the devices
// Ultrasound depth
const int depthUsTrigPin = 7;
const int depthUsEchoPin = 10;

// Ultrasound basket height
// const int heightUsTrigPin = 2;
// const int heightUsEchoPin = 4;

// Potentiometer measurement
const int rotationPotentiometerPinA = 4;
const int rotationPotentiometerPinB = 2;

// Depth motor
const int depthMotorDirectionPin = 13;
const int depthMotorPwmPin = 11;
const int depthMotorBrakePin = 8;

// Rotation motor
const int rotationMotorDirectionPin = 12;
const int rotationMotorPwmPin = 3;
const int rotationMotorBrakePin = 9;

// Basket motor
const int basketMotorControlPinUp = 5;
const int basketMotorControlPinDown = 6;

// Define the current commands
int currentCommands[3][2];

// Data storage
int rotationPotentiometerLastRawState = 0;
int rotationPotentiometerAngle = 0;
const int rotationAngleStep = 9;
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
    // pinMode(heightUsTrigPin, OUTPUT);
    // pinMode(heightUsEchoPin, INPUT);

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

    pinMode(basketMotorControlPinUp, OUTPUT);
    pinMode(basketMotorControlPinDown, OUTPUT);

    ////////////////////////////////////////////////

    // Init values
    rotationPotentiometerLastRawState = digitalRead(rotationPotentiometerPinA);
}

void loop() {
    UpdateFromSerial();

    // Get and store data
    // data[0] = GetUltrasoundData(heightUsTrigPin, heightUsEchoPin);
    data[0] = 0;
    data[1] = GetUltrasoundData(depthUsTrigPin, depthUsEchoPin);
    data[2] = GetPotentiometerData(rotationPotentiometerPinA, rotationPotentiometerPinB);

    MoveCrane();

    WriteDataToSerial();
}

void UpdateFromSerial()
{ 
    if (Serial.available() > 0)
    {
        String commands = Serial.readString();
        
        if (commands[0] != 'c')
            return;

        commands = commands.substring(1); // Remove the first character

        String strs[6];
        int stringCount = 0;

        if (commands.length() < 6)
            return;

        while (commands.length() > 0)
        {
            if (stringCount >= 6)
                break;

            int index = commands.indexOf(',');
            if (index == -1) // No comma found
            {
                strs[stringCount++] = commands;
                break;
            }
            else
            {
                strs[stringCount++] = commands.substring(0, index);
                commands = commands.substring(index+1);
            }
        }

        currentCommands[0][0] = strs[0].toInt();
        currentCommands[0][1] = strs[1].toInt();
        currentCommands[1][0] = strs[2].toInt();
        currentCommands[1][1] = strs[3].toInt();
        currentCommands[2][0] = strs[4].toInt();
        currentCommands[2][1] = strs[5].toInt();

        Serial.flush();
    }
}

void WriteDataToSerial()
{
    String command = "";
    Serial.flush();
    
    command += "d";
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
    long duration = pulseIn(echoPin, HIGH, 10000);

    // Calculate the distance in centimeters
    float distance = duration * 0.034 / 2;

    return distance;
}

int GetPotentiometerData(int pinA, int pinB) {
    int potentiometerState = digitalRead(pinA);

   if (potentiometerState != rotationPotentiometerLastRawState)
   {
        int potentiometerStateB = digitalRead(pinB);

     // outputB != outputA state, encoder is rotating clockwise
     if (potentiometerStateB != potentiometerState) { 
        movePotentiometerClockwise();
     } 
     else {
        movePotentiometerCounterClockwise();
     }
   }

    rotationPotentiometerLastRawState = potentiometerState;

    return rotationPotentiometerAngle;
}

void movePotentiometerClockwise() {
    rotationPotentiometerAngle = rotationPotentiometerAngle + rotationAngleStep;
}

void movePotentiometerCounterClockwise() {
    rotationPotentiometerAngle = rotationPotentiometerAngle - rotationAngleStep;
}

void MoveCrane() {
    // Move basket motor
    if (currentCommands[0][1] == 100) {
        if (currentCommands[0][0] == 1)
        {
            digitalWrite(basketMotorControlPinUp, HIGH);
            digitalWrite(basketMotorControlPinDown, LOW);

            delay(4000);

            digitalWrite(basketMotorControlPinUp, LOW);
            digitalWrite(basketMotorControlPinDown, LOW);
        }
        else
        {
            digitalWrite(basketMotorControlPinUp, LOW);
            digitalWrite(basketMotorControlPinDown, HIGH);

            delay(4000);

            digitalWrite(basketMotorControlPinUp, HIGH);
            digitalWrite(basketMotorControlPinDown, HIGH);
        }  
    }
    else {
        digitalWrite(basketMotorControlPinUp, LOW);
        digitalWrite(basketMotorControlPinDown, LOW);
    }

    // Move depth motor
    if (currentCommands[1][0] == 1)
        digitalWrite(depthMotorDirectionPin, HIGH);
    else
        digitalWrite(depthMotorDirectionPin, LOW);

    // int depthMotorSpeed = GetMotorSpeedFromPercentage(currentCommands[1][1]);
    if (currentCommands[1][1] == 100 && data[1] < 52 && data[1] > 20)
    {
        digitalWrite(depthMotorBrakePin, LOW);
        analogWrite(depthMotorPwmPin, 255);
    }
    else
    {
        digitalWrite(depthMotorBrakePin, HIGH);
        analogWrite(depthMotorPwmPin, 0);
    }

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
    return percentage * 255 / 100;
}