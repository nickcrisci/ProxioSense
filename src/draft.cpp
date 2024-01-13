#include "rplidar_driver_impl.h"
#include <math.h>
const int lidarMotorPin = 3;
char report[80];
RPLidar rplidar;

#include <Arduino.h>

const float minDistance = 0.01;
const float maxDistance = 3.0;

class VibrationSensor {
private:
  int pin;
  int intensity;
  float distanceBuffer[512];
  int currentIndex = 0;

  void processBuffer() {
    float average = this->calcAverage();
  }

  int calculateIntensity(float distance) {
    // Ensure the distance is within the valid range
    if (distance > 3.0) {
      return 0;
    }

    // Calculate the intensity based on the distance
    int intensity = static_cast<int>((maxDistance - distance) / (maxDistance - minDistance) * 255.0);

    // Ensure that the intensity stays within the valid range (0 to 255)
    intensity = constrain(intensity, 0, 255);
    return intensity;
  }

public:
  VibrationSensor()
    : pin(0), intensity(0) {}  // Default constructor

  VibrationSensor(int pin) {
    this->pin = pin;
    this->intensity = 0;
    pinMode(this->pin, OUTPUT);
  }

  void add(float distance) {
    this->distanceBuffer[this->currentIndex] = distance;
    this->currentIndex++;
    // If the buffer is full, reset the index to 0
  }

  void changeSens() {
    float average = this->calcAverage();
    int intensity = this->calculateIntensity(average);
    Serial.println(intensity);
    this->setIntensity(intensity);
    this->currentIndex = 0;
  }

  float calcAverage() {
    float sum = 0.0;

    for (int i = 0; i < this->currentIndex; i++) {
      sum += this->distanceBuffer[i];
    }

    return sum / this->currentIndex;
  }

  void setIntensity(int intensity) {
    intensity = constrain(intensity, 0, 255);
    this->intensity = intensity;
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
    vibrationSensors[sensorPin[0]] = VibrationSensor(sensorPin[1]);
  }
}

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

void loop() {
  if (!rplidar.isScanning()) {
    rplidar.startScanNormal(true);
    digitalWrite(lidarMotorPin, HIGH);  // turn on the motor
    delay(10);
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
        float distance_in_meters = nodes[i].dist_mm_q2 / 1000.f / (1 << 2);
        //float distance_in_meters = nodes[i].dist_mm_q2 / (1<<2);
        processData(angle_in_degrees, distance_in_meters, nodes[i].quality);

        //snprintf(report, sizeof(report), "%.2f:%.2f:%d", distance_in_meters, angle_in_degrees, nodes[i].quality);
        //Serial.println(report);
      }

      for (int i = 0; i < 8; i++) {
        vibrationSensors[i].changeSens();
      }
    }
  }
}

void processData(float angle, float distance, int quality) {
  int direction = getDirection(angle);
  if (direction != 0) return;
  int intensity = calculateIntensity(distance);
  vibrationSensors[direction].add(distance);
  //vibrationSensors[direction].setIntensity(intensity);
}

int getDirection(float angle) {
  int index = (int)round(angle / 45) % 8;
  return index;
}