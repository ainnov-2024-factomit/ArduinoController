int pinA = 2;
int pinB = 4;
int rawValue = 0;
int angleStep = 9;
int angle = 0;
int aState;
int aLastState;  

void setup() { 
   pinMode (pinA,INPUT);
   pinMode (pinB,INPUT);
   Serial.begin (115200);
   
   // Starts output
   aLastState = digitalRead(pinA);
} 

void loop() { 
   aState = digitalRead(pinA);    
   
   if (aState != aLastState){     
     // outputB != outputA state, encoder is rotating clockwise
     if (digitalRead(pinB) != aState) { 
      movePotentiometerClockwise();
     } 
     else {
       movedCounterClockwise();
     }
   }

    Serial.print("Angle: "); Serial.println(angle);
    aLastState = aState;
}

void movePotentiometerClockwise() {
    angle = angle + angleStep;
}

void movedCounterClockwise() {
    angle = angle - angleStep;
}