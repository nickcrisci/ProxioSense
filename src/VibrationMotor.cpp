#include <math.h>
#include <Arduino.h>

// Strategy values
const int averageStrat = 0;
const int minStrat = 1;
const int slidingWindowStrat = 2;

const int bufferSize = 10;

// Distance constants
const int minDistance = 300;
const int maxDistance = 3000;

class VibrationMotor {
private:
  int distanceBuffer[bufferSize];
  int currentIndex = 0;
  int strategy = 0;

  void processBuffer() {
    float distance = 0;
    if (this->strategy == averageStrat || this->strategy == slidingWindowStrat) {
      distance = this->calcAverage();
    } else if (this->strategy == minStrat) {
      distance = this->calcMin();
    }

    this->setIntensity(this->calculateIntensity(distance));
    if (this->currentIndex == bufferSize) {
      this->currentIndex = 0;
    }
  }

  int calculateIntensity(float distance) {
    if (distance > maxDistance) return -1; // This leads to analogWrite switching off the motor
    if (distance == maxDistance) return 255; // distance == maxDistance would lead to a zero division, therefor we return early

    // Calculate the intensity based on the distance
    int intensity = static_cast<int>((maxDistance - distance) / (maxDistance - minDistance) * 255.0);
    intensity = constrain(intensity, 0, 255);
    if (intensity == 0) return -1; // This leads to analogWrite switching off the motor
    return intensity;
  }

  // Calculate min strategy
  float calcMin() {
    int minNum = 9999;
    for (int i = 0; i < this->currentIndex; i++) {
      if (this->distanceBuffer[i] < minDistance) continue;
      minNum = min(minNum, this->distanceBuffer[i]);
    }

    return (float) minNum;
  }

  // Calculate average strategy, can be reused for sliding window strategy
  float calcAverage() {
    float sum = 0.0;

    for (int i = 0; i < this->currentIndex; i++) {
      sum += this->distanceBuffer[i];
    }
    float average = sum / this->currentIndex;
    return average;
  }

  void setIntensity(int intensity) {
    if (intensity <= 0)
      this->deactivateMotor();
    else
      this->intensity = intensity;
    Serial.print(this->pin);
    Serial.print(" : ");
    Serial.println(intensity);
  }  

public:
  VibrationMotor(): pin(-1), intensity(999) {}  // Default constructor

  int pin;
  int intensity;

  VibrationMotor(int pin, int strategy) {
    this->pin = pin;
    this->intensity = -1;
    this->strategy = strategy;
    pinMode(this->pin, OUTPUT);
  }

  void add(float distance) {
    this->distanceBuffer[this->currentIndex] = distance;
    this->currentIndex++;

    if (this->strategy == slidingWindowStrat || this->currentIndex == bufferSize) {
      this->processBuffer();
    }
  }

  void startMotor() {
    analogWrite(this->pin, this->intensity);
  }

  void deactivateMotor() {
    Serial.print("Clearing Motor on pin ");
    Serial.println(this->pin);
    this->intensity = -1;
    analogWrite(this->pin, this->intensity);
  }
};