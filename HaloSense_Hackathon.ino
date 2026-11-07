const int NUM_SENSORS = 10;

const int trigPins[NUM_SENSORS]  = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40};
const int echoPins[NUM_SENSORS]  = {23, 25, 27, 29, 31, 33, 35, 37, 39, 41};
const int motorPins[NUM_SENSORS] = { 2,  3,  4,  5,  6,  7,  8,  9, 10, 11};

const int maxDistanceCm = 100;  // vibration starts here
const int minDistanceCm = 5;    // HC-SR04 can't read reliably much closer
const int pwmMin = 0;          // lowest value that reliably spins the motor
const int pwmMax = 150;         // ~3V average on a 5V supply

// ~100cm round trip is about 5.8ms, so 6ms timeout keeps the loop fast.
// Anything beyond 100cm times out and counts as "no reading".
const unsigned long echoTimeoutUs = 6000;

// Gap between sensors so one sensor's echo fades before the next fires
const int interSensorDelayMs = 10;

long readDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, echoTimeoutUs);
  if (duration == 0) return -1;  // no reading

  return duration * 0.0343 / 2;
}

void setup() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    pinMode(motorPins[i], OUTPUT);
    analogWrite(motorPins[i], 0);
  }
  Serial.begin(9600);
}

void loop() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    long distance = readDistanceCm(trigPins[i], echoPins[i]);

    int pwm = 0;
    if (distance > 0 && distance < maxDistanceCm) {
      long d = constrain(distance, minDistanceCm, maxDistanceCm);
      // Closer = stronger: invert the mapping
      pwm = map(d, maxDistanceCm, minDistanceCm, pwmMin, pwmMax);
    }

    analogWrite(motorPins[i], pwm);

    Serial.print("S");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(distance);
    Serial.print(" cm  PWM: ");
    Serial.println(pwm);

    delay(interSensorDelayMs);
  }
  Serial.println("---");
}
