# ProxioSence

This project contains the source code for an arduino based prototype.

This project uses the Arduino IDE to build and upload the sources to an Arduino Nano 33 BLE Sense.
We use an RPLidar A1 with 8 Grove Vibration Motors.

## Setup
1. Install AruinoIDE
2. Open it, set Sketchbook location under file -> preferences to this folder
3. Code
4. Connect the Arduino
5. Upload
6. ???
7. Profit


## Docs
### Pinout

| Pin Name      | PIN | Type    | PWM | Use                                                    |
|---------------|-----|---------|---|--------------------------------------------------------|
| lidarMotorPin | 2   | Output  | (✅) | Controll the motor of the lidar. Always HIGH           |
| NorthPin      | 3   | Output  | ✅ | Controll the vibration strengh on the north side.      |
| NorthEastPin  | 4   | Output  | ✅ | Controll the vibration strengh on the north east side. |
| EastPin       | 5   | Output  | ✅ | Controll the vibration strengh on the east side.       |
| SouthEastPin  | 6   | Output  | ✅ | Controll the vibration strengh on the south east side. |
| SouthPin      | 7   | Output  | ✅ | Controll the vibration strengh on the south side.      |
| SouthWestPin  | 8   | Output  | ✅ | Controll the vibration strengh on the south west side. |
| WestPin       | 9   | Output  | ✅ | Controll the vibration strengh on the west side.       |
| NorthWestPin  | 10  | Output  | ✅ | Controll the vibration strengh on the north west side. |


### Schematic
The schematic can be found at `schematic.png`. Just keep in mind that this schematic shows an Arduino Uno and not an 
Arduino nano 33 BLE that is required for this project. Building it with an uno and keeping the same pinout as the 
schematic shows will **not** work, simply because the nano 33 BLE has PWM control on different pins than the uno!
![](.\schematic.png)


### Model
A model that is ready to print can be found in `model.zip`. A complete model showing the final assembly is available
in `model_complete.zip`. A combination of both are freely available on 


## License
Copyright 2023 The ProxioSense Team

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

