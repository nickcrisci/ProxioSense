#include "rplidar_driver_impl.h"
#include <math.h>
#include <Arduino.h>

const int lidarMotorPin = 3;
char report[80];
RPLidar rplidar;

// Strategy values
const int averageStrat = 0;
const int minStrat = 1;
const int slidingWindowStrat = 2;

// Distance constants
const int minDistance = 300;
const int maxDistance = 3000;

const int bufferSize = 20;

class VibrationSensor {
private:
  int pin;
  int intensity;
  int distanceBuffer[bufferSize];
  int currentIndex = 0;
  int strategy = 0;

  void processBuffer() {
    float distance;
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

// The Arrays vibrationMotorPins and motorStrats contain configurations for the vibration sensors
// The values are ordered the following way:
// 0: North, 1: North East, 2: East, 3: South East, 4: South, 5: South West, 6: West, 7: North West
int vibrationMotorPins[8] = {10, 1, 2, 3, 4, 5, 6, 7};
int motorStrats[8] = { minStrat, averageStrat, minStrat, averageStrat, averageStrat, averageStrat, averageStrat, averageStrat};
VibrationSensor vibrationSensors[8];

void setup() {
  Serial.begin(2000000);
  pinMode(lidarMotorPin, OUTPUT);
  setupVibrationMotors();
  rplidar.begin();
  delay(1000);
}

// Sets up vibration motors: Each motor is inizialized with its pin and strategy
void setupVibrationMotors() {
  for (int i; i < 8; i++) {
    int pin = vibrationMotorPins[i];
    int strategy = motorStrats[i];
    vibrationSensors[i] = VibrationSensor(pin, strategy);
  }
}

/*void checkForCommand() {
  if (Serial.available() >= 3) {
    char command = Serial.read();
    int sensor = Serial.parseInt();
    int strategy = Serial.parseInt();
    switch (command) {
      case 'S':
        Serial.print(command);
        Serial.print(" : ");
        Serial.print(sensor);
        Serial.print(" : ");
        Serial.println(strategy);
        break;
      default:
        break;
    }
  }
}*/

// Print Lidar info
void printInfo() {
  rplidar_response_device_info_t info;
  rplidar.getDeviceInfo(info);

  snprintf(report, sizeof(report), "model: %d firmware: %d.%d", info.model, info.firmware_version / 256, info.firmware_version % 256);
  Serial.println(report);
  delay(1000);
}

// Print Lidar sample
void printSampleDuration() {
  rplidar_response_sample_rate_t sampleInfo;
  rplidar.getSampleDuration_uS(sampleInfo);

  snprintf(report, sizeof(report), "TStandard: %d[us] TExpress: %d[us]", sampleInfo.std_sample_duration_us, sampleInfo.express_sample_duration_us);
  Serial.println(report);
  delay(1000);
}

void loop() {
  //checkForCommand();
  if (!rplidar.isScanning()) {
    rplidar.startScanNormal(true); // start lidar is normal mode
    digitalWrite(lidarMotorPin, HIGH);  // turn on the motor
    delay(100);
  } else {
    // loopScanData needs to be called every loop
    rplidar.loopScanData();

    // create object to hold received data for processing
    rplidar_response_measurement_node_hq_t nodes[512];
    size_t nodeCount = 512;  // variable will be set to number of received measurement by reference
    u_result ans = rplidar.grabScanData(nodes, nodeCount);
    
    if (IS_OK(ans)) {
      for (size_t i = 0; i < nodeCount; ++i) {
        // convert to standard units
        float angle_in_degrees = nodes[i].angle_z_q14 * 90.f / (1 << 14);
        int distance_in_mm = nodes[i].dist_mm_q2 / (1 << 2);

        if (nodes[i].quality < 60) continue; // if the quality is not sufficient discard the measurement
        processData(angle_in_degrees, distance_in_mm);
      }
    }
  }
}

// calculate cardinal direction (as int) from the angle
int getDirection(float angle) {
  int index = (int)round(angle / 45) % 8;
  return index;
}

// Process incoming angle and distance
void processData(float angle, int distance) {
  int direction = getDirection(angle);
  if (direction != 0) return;
  vibrationSensors[direction].add(distance);
}