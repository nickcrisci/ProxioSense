#include "rplidar_driver_impl.h"
#include "VibrationMotor.cpp"

const int lidarMotorPin = 2;
char report[80];
RPLidar rplidar;

// The Arrays vibrationMotorPins and motorStrats contain configurations for the vibration sensors
// The values are ordered the following way:
// 3: North, 4: North East, 5: East, 6: South East, 7: South, 8: South West, 9: West, 10: North West
const int NorthPin = 3;
const int North = 0;
const int NorthEastPin = 4;
const int NorthEast = 1;
const int EastPin = 5;
const int East = 2;
const int SouthEastPin = 6;
const int SouthEast = 3;
const int SouthPin = 7;
const int South = 4;
const int SouthWestPin = 8;
const int SouthWest = 5;
const int WestPin = 9;
const int West = 6;
const int NorthWestPin = 10;
const int NorthWest = 7;

int vibrationMotorPins[8] = {NorthPin, NorthEastPin, EastPin, SouthEastPin, SouthPin, SouthWestPin, WestPin, NorthWestPin};
int motorStrats[8] = {minStrat, minStrat, minStrat, minStrat, minStrat, minStrat, minStrat, minStrat};
VibrationSensor vibrationSensors[8];

void setup() {
  Serial.begin(2000000);
  pinMode(lidarMotorPin, OUTPUT);
  setupVibrationMotors();
  digitalWrite(lidarMotorPin, HIGH);  // turn on the motor
  rplidar.begin();
  delay(1000);
}

// Sets up vibration motors: Each motor is inizialized with its pin and strategy
void setupVibrationMotors() {
  for (int i = 0; i < 8; i++) {
    int pin = vibrationMotorPins[i];
    int strategy = motorStrats[i];
    vibrationSensors[i] = VibrationSensor(pin, strategy);
  }
}

void loop() {
  while (!rplidar.isScanning()) {
    Serial.println("Not scanning, starting");
    rplidar.startScanNormal(true); // start lidar in normal mode
    delay(100); // Wait to scan
  } 

  rplidar.loopScanData();

  rplidar_response_measurement_node_hq_t nodes[512];
  size_t nodeCount = 512;  // variable will be set to number of received measurement by reference. Probably often just 1
  u_result ans = rplidar.grabScanData(nodes, nodeCount);

  if (IS_FAIL(ans)) {
    Serial.println("Fail");
    printError(ans);
    return;
  }

  if (IS_OK(ans)) {
    for (size_t i = 0; i < nodeCount; i++) {
        // convert to standard units
        float angle_in_degrees = nodes[i].angle_z_q14 * 90.f / (1 << 14);
        int distance_in_mm = nodes[i].dist_mm_q2 / (1 << 2);

        if (nodes[i].quality < 60) continue; // if the quality is not sufficient discard the measurement
        processData(angle_in_degrees, distance_in_mm);
      }
  }
}


// Process incoming angle and distance
void processData(float angle, int distance) {
  int direction = getDirection(angle);

  if (direction == North || direction == East || direction == South || direction == West) return;
  
  vibrationSensors[direction].add(distance);
}

// calculate cardinal direction (as int) from the angle
int getDirection(float angle) {
  int index = (int)round(angle / 45) % 8;
  return index;
}

void printError (u_result result) {
    uint rawCode = result & ~RESULT_FAIL_BIT;

    Serial.print("Error:");
    if (rawCode == 32) Serial.println("RESULT_ALREADY_DONE");
    else if (rawCode == 32768) Serial.println("RESULT_INVALID_DATA");
    else if (rawCode == 32769) Serial.println("RESULT_OPERATION_FAIL");
    else if (rawCode == 32770) Serial.println("RESULT_OPERATION_TIMEOUT");
    else if (rawCode == 32771) Serial.println("RESULT_OPERATION_STOP");
    else if (rawCode == 32772) Serial.println("RESULT_OPERATION_NOT_SUPPORT");
    else if (rawCode == 32773) Serial.println("RESULT_FORMAT_NOT_SUPPORT");
    else if (rawCode == 32774) Serial.println("RESULT_INSUFFICIENT_MEMORY");
    else {
      Serial.print("Unknown error: ");
      Serial.println(result);
    }
}