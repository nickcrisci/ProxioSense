/* ############ Daniel Kasinki's Library Example ############ 



#include "rplidar_driver_impl.h"

const int lidarMotorPin = 3;
char report[80];
RPLidar rplidar;

void setup(){
  Serial.begin(2000000);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lidarMotorPin, OUTPUT);
  // INFO Serial port used by RPLidar is hardcoded in begin() function! Please modify it
  rplidar.begin();
  delay(1000);
}

void printInfo(){
  rplidar_response_device_info_t info;
  rplidar.getDeviceInfo(info);

  snprintf(report, sizeof(report), "model: %d firmware: %d.%d", info.model, info.firmware_version/256, info.firmware_version%256);
  Serial.println(report);
  delay(1000);
}

void printSampleDuration(){
  rplidar_response_sample_rate_t sampleInfo;
  rplidar.getSampleDuration_uS(sampleInfo);

  snprintf(report, sizeof(report), "TStandard: %d[us] TExpress: %d[us]", sampleInfo.std_sample_duration_us, sampleInfo.express_sample_duration_us);
  Serial.println(report);
  delay(1000);
}

#define SCAN_TYPE_STD
// #define SCAN_TYPE_EXPRESS

void loop(){
#ifdef SCAN_TYPE_STD 
  if (!rplidar.isScanning()){
    rplidar.startScanNormal(true);
    digitalWrite(lidarMotorPin, HIGH); // turn on the motor
    delay(10);
  }
  else{
    // loop needs to be send called every loop
    rplidar.loopScanData();
  
    // create object to hold received data for processing
    rplidar_response_measurement_node_hq_t nodes[512];
    size_t nodeCount = 512; // variable will be set to number of received measurement by reference
    u_result ans = rplidar.grabScanData(nodes, nodeCount);
    if (IS_OK(ans)){
        
      for (size_t i = 0; i < nodeCount; ++i){
        // convert to standard units
        float angle_in_degrees = nodes[i].angle_z_q14 * 90.f / (1<<14);
        float distance_in_meters = nodes[i].dist_mm_q2 / 1000.f / (1<<2);
        snprintf(report, sizeof(report), "%.2f %.2f %d", distance_in_meters, angle_in_degrees, nodes[i].quality);
        Serial.println(report);
        digitalWrite(lidarMotorPin, LOW);
        exit(0);
      }
    }
  }
#endif
#ifdef SCAN_TYPE_EXPRESS
  if (!rplidar.isScanning()){
    rplidar.startScanExpress(true, RPLIDAR_CONF_SCAN_COMMAND_EXPRESS);
    digitalWrite(lidarMotorPin, HIGH); // turn on the motor
    delay(10);
  }
  else{
    // loop needs to be send called every loop
    rplidar.loopScanExpressData();
  
    // create object to hold received data for processing
    rplidar_response_measurement_node_hq_t nodes[512];
    size_t nodeCount = 512; // variable will be set to number of received measurement by reference
    u_result ans = rplidar.grabScanExpressData(nodes, nodeCount);
    if (IS_OK(ans)){
      for (size_t i = 0; i < nodeCount; ++i){
        // convert to standard units
        float angle_in_degrees = nodes[i].angle_z_q14 * 90.f / (1<<14);
        float distance_in_meters = nodes[i].dist_mm_q2 / 1000.f / (1<<2);
        snprintf(report, sizeof(report), "%.2f %.2f %d", distance_in_meters, angle_in_degrees, nodes[i].quality);
        Serial.println(report);
      }
    }
  }
#endif
}

*/

/* ############ RoboPeak's Library Example ############
*/
#include <RPLidar.h>

// You need to create an driver instance 
RPLidar lidar;

#define RPLIDAR_MOTOR 3 // The PWM pin for control the speed of RPLIDAR's motor.
                        // This pin should connected with the RPLIDAR's MOTOCTRL signal 
                       
int counter = 0;
                        
void setup() {
  // bind the RPLIDAR driver to the arduino hardware serial
  lidar.begin(Serial1);
  
  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
}

void loop() {
  if (IS_OK(lidar.waitPoint())) {
    float distance = lidar.getCurrentPoint().distance; //distance value in mm unit
    float angle    = lidar.getCurrentPoint().angle; //anglue value in degree
    bool  startBit = lidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
    byte  quality  = lidar.getCurrentPoint().quality; //quality of the current measurement
    
    if (startBit == 1) {
      counter++;

      if (counter == 1000) {
        lidar.stop();
        analogWrite(RPLIDAR_MOTOR, 0);
        exit(0);
      }
    }

    Serial.println((String)"Measured: " + distance + " : " + angle + " : " + startBit + " : " + quality);
    //perform data processing here... 
    
    
  } else {
    analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor
    
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
       // detected...
      lidar.startScan();
       
       // start motor rotating at max allowed speed
      analogWrite(RPLIDAR_MOTOR, 255);
      delay(1000);
    }
  }
}
