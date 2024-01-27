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


class VibrationSensor {
private:
  int pin;
  int intensity;
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

    int intensity = this->calculateIntensity(distance);
    this->setIntensity(intensity);
    if (this->currentIndex == bufferSize) {
      this->currentIndex = 0;
    }
  }

  int calculateIntensity(float distance) {
    if (distance > maxDistance) return 0; 
    if (distance == maxDistance) return 255; // distance == maxDistance would lead to a zero division, therefor we return early
    // Calculate the intensity based on the distance
    int intensity = static_cast<int>((maxDistance - distance) / (maxDistance - minDistance) * 255.0);
    intensity = constrain(intensity, 0, 255);
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
    this->intensity = intensity;
    Serial.print(this->pin);
    Serial.print(" : ");
    Serial.println(intensity);
    analogWrite(this->pin, this->intensity);
  }  

public:
  VibrationSensor(): pin(0), intensity(0) {}  // Default constructor

  VibrationSensor(int pin, int strategy) {
    this->pin = pin;
    this->intensity = 0;
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
};