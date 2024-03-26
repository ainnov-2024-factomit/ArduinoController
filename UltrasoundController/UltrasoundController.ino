// Define the pins for the ultrasound device
const int trigPin = 2;
const int echoPin = 3;

void setup() {
    // Initialize the serial communication
    Serial.begin(115200);

    // Set the trigPin as an output and echoPin as an input
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop() {
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

    // Print the distance to the serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Wait for a short delay before taking the next measurement
    delay(1000);
}