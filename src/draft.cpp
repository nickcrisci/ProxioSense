#include "rplidar_driver_impl.h"
#include <math.h>
const int lidarMotorPin = 3;
char report[80];
RPLidar rplidar;

#include <Arduino.h>

const int averageStrat = 0;
const int minStrat = 1;
const int slidingWindowStrat = 2;

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
    if (distance == maxDistance) return 255;
    // Calculate the intensity based on the distance
    int intensity = static_cast<int>((maxDistance - distance) / (maxDistance - minDistance) * 255.0);
    intensity = constrain(intensity, 0, 255);
    return intensity;
  }

  float calcMin() {
    int minNum = 9999;
    for (int i = 0; i < this->currentIndex; i++) {
      if (this->distanceBuffer[i] < minDistance) continue;
      minNum = min(minNum, this->distanceBuffer[i]);
    }
    return (float) minNum;
  }

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
    //Serial.println(this->strategy);
    this->distanceBuffer[this->currentIndex] = distance;
    this->currentIndex++;

    if (this->strategy == slidingWindowStrat || this->currentIndex == bufferSize) {
      this->processBuffer();
    }
  }

  int getIntensity() {
    return this->intensity;
  }
};

const int NORTH = 0;
const int NORTH_EAST = 1;
const int EAST = 2;
const int SOUTH_EAST = 3;
const int SOUTH = 4;
const int SOUTH_WEST = 5;
const int WEST = 6;
const int NORTH_WEST = 7;

int vibrationSensorPins[8][2] = { { NORTH, 10 }, { NORTH_EAST, 1 }, { EAST, 2 }, { SOUTH_EAST, 3 }, { SOUTH, 4 }, { SOUTH_WEST, 5 }, { WEST, 6 }, { NORTH_WEST, 7 } };
int sensorStrats[8] = {minStrat, averageStrat, averageStrat, averageStrat, averageStrat, averageStrat, averageStrat, averageStrat};
VibrationSensor vibrationSensors[8];

void setup() {
  Serial.begin(2000000);
  pinMode(lidarMotorPin, OUTPUT);
  setupVibrationMotors();
  rplidar.begin();
  delay(1000);
}

void setupVibrationMotors() {
  for (int* sensorPin : vibrationSensorPins) {
    int pin = sensorPin[1];
    int strategy = sensorStrats[sensorPin[0]];
    vibrationSensors[sensorPin[0]] = VibrationSensor(pin, strategy);
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

void printInfo() {
  rplidar_response_device_info_t info;
  rplidar.getDeviceInfo(info);

  snprintf(report, sizeof(report), "model: %d firmware: %d.%d", info.model, info.firmware_version / 256, info.firmware_version % 256);
  Serial.println(report);
  delay(1000);
}

void printSampleDuration() {
  rplidar_response_sample_rate_t sampleInfo;
  rplidar.getSampleDuration_uS(sampleInfo);

  snprintf(report, sizeof(report), "TStandard: %d[us] TExpress: %d[us]", sampleInfo.std_sample_duration_us, sampleInfo.express_sample_duration_us);
  Serial.println(report);
  delay(1000);
}

#define SCAN_TYPE_STD
//#define SCAN_TYPE_EXPRESS
int counter = 0;
void loop() {
  //checkForCommand();
  if (!rplidar.isScanning()) {
    rplidar.startScanNormal(true);
    digitalWrite(lidarMotorPin, HIGH);  // turn on the motor
    delay(100);
  } else {
    // loop needs to be send called every loop
    rplidar.loopScanData();

    // create object to hold received data for processing
    rplidar_response_measurement_node_hq_t nodes[512];
    size_t nodeCount = 512;  // variable will be set to number of received measurement by reference
    u_result ans = rplidar.grabScanData(nodes, nodeCount);
    
    if (IS_OK(ans)) {
      for (size_t i = 0; i < nodeCount; ++i) {
        // convert to standard units
        float angle_in_degrees = nodes[i].angle_z_q14 * 90.f / (1 << 14);
        //float distance_in_meters = nodes[i].dist_mm_q2 / 1000.f / (1 << 2);
        int distance_in_mm = nodes[i].dist_mm_q2;// / (1 << 2);
        //float distance_in_meters = nodes[i].dist_mm_q2 / (1<<2);
        if (nodes[i].quality < 60) continue;
        processData(angle_in_degrees, distance_in_mm);

        snprintf(report, sizeof(report), "%d:%.2f:%d", distance_in_mm, angle_in_degrees, nodes[i].quality);
        //Serial.println(report);
      }
    }
  }
}

void processData(float angle, int distance) {
  int direction = getDirection(angle);
  if (direction != 0) return;
  vibrationSensors[direction].add(distance);
}

int getDirection(float angle) {
  int index = (int)round(angle / 45) % 8;
  return index;
}