# nRF52_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/nRF52_Slow_PWM.svg?)](https://www.ardu-badge.com/nRF52_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/nRF52_Slow_PWM.svg)](https://github.com/khoih-prog/nRF52_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/nRF52_Slow_PWM/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/nRF52_Slow_PWM.svg)](http://github.com/khoih-prog/nRF52_Slow_PWM/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Donate to my libraries using BuyMeACoffee" style="height: 50px !important;width: 181px !important;" ></a>
<a href="https://www.buymeacoffee.com/khoihprog6" title="Donate to my libraries using BuyMeACoffee"><img src="https://img.shields.io/badge/buy%20me%20a%20coffee-donate-orange.svg?logo=buy-me-a-coffee&logoColor=FFDD00" style="height: 20px !important;width: 200px !important;" ></a>


---
---

## Table of Contents

* [Important Change from v1.2.0](#Important-Change-from-v120)
* [Why do we need this nRF52_Slow_PWM library](#why-do-we-need-this-nRF52_Slow_PWM-library)
  * [Features](#features)
  * [Why using ISR-based PWM is better](#why-using-isr-based-pwm-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [Packages' Patches](#packages-patches)
  * [1. For Adafruit nRF52840 and nRF52832 boards](#1-for-adafruit-nRF52840-and-nRF52832-boards)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [Usage](#usage)
  * [1. Init Hardware Timer](#1-init-hardware-timer)
  * [2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions](#2-Set-PWM-Frequency-dutycycle-attach-irqCallbackStartFunc-and-irqCallbackStopFunc-functions)
* [Examples](#examples)
  * [ 1. ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
  * [ 2. ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
  * [ 3. ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
  * [ 4. ISR_Changing_PWM](examples/ISR_Changing_PWM)
  * [ 5. ISR_Modify_PWM](examples/ISR_Modify_PWM)
  * [ 6. multiFileProject](examples/multiFileProject). **New**
* [Example ISR_16_PWMs_Array_Complex](#Example-ISR_16_PWMs_Array_Complex)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_16_PWMs_Array_Complex on NRF52840_ITSYBITSY](#1-ISR_16_PWMs_Array_Complex-on-NRF52840_ITSYBITSY)
  * [2. ISR_16_PWMs_Array on NRF52840_ITSYBITSY](#2-ISR_16_PWMs_Array-on-NRF52840_ITSYBITSY)
  * [3. ISR_16_PWMs_Array_Simple on NRF52840_ITSYBITSY](#3-ISR_16_PWMs_Array_Simple-on-NRF52840_ITSYBITSY)
  * [4. ISR_Modify_PWM on NRF52840_ITSYBITSY](#4-ISR_Modify_PWM-on-NRF52840_ITSYBITSY)
  * [5. ISR_Changing_PWM on NRF52840_ITSYBITSY](#5-ISR_Changing_PWM-on-NRF52840_ITSYBITSY)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.2.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

As more complex calculation and check **inside ISR** are introduced from v1.2.0, there is possibly some crash depending on use-case.

You can modify to use larger `HW_TIMER_INTERVAL_US`, (from current 10uS), according to your board and use-case if crash happens.


```cpp
// Don't change these numbers to make higher Timer freq. System can hang
#define HW_TIMER_INTERVAL_US      10L
```

---
---

### Why do we need this [nRF52_Slow_PWM library](https://github.com/khoih-prog/nRF52_Slow_PWM)

### Features

This library enables you to use ISR-based PWM channels on an nRF52-based board, such as `Itsy-Bitsy nRF52840`, using [`Adafruit nRF52 core`](https://github.com/adafruit/Adafruit_nRF52_Arduino) core  to create and output PWM any GPIO pin. Because this library doesn't use the powerful purely hardware-controlled PWM with many limitations, the maximum PWM frequency is currently limited at **1000Hz**, which is still suitable for many real-life applications. Now you can also modify PWM settings on-the-fly.

---

This library enables you to use Interrupt from Hardware Timers on nRF52-based boards to create and output PWM to pins. It now supports 16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer. PWM interval can be very long (uint64_t microsecs / millisecs). The most important feature is they're ISR-based PWM channels. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software PWM using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software PWM channels. 

The most important feature is they're ISR-based PWM channels. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**ISR_16_PWMs_Array_Complex**](examples/ISR_16_PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy, compared to software PWM, by printing the actual period / duty-cycle in `microsecs` of each of PWM-channels.

Being ISR-based PWM, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet or Blynk services. You can also have many `(up to 16)` PWM channels to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see `software-based` SimpleTimer is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

#### Why using ISR-based PWM is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software PWM channels using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software PWM channels, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

1. **AdaFruit Feather nRF52832, nRF52840 Express, BlueFruit Sense, Itsy-Bitsy nRF52840 Express, Metro nRF52840 Express, NINA_B302_ublox, NINA_B112_ublox etc.**
2. **Sparkfun Pro nRF52840 Mini**
3. **Seeeduino nRF52840-based boards such as SEEED_XIAO_NRF52840 and SEEED_XIAO_NRF52840_SENSE**, etc. using Seeeduino `nRF%2` core

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [`Adafruit nRF52 v1.3.0+`](https://github.com/adafruit/Adafruit_nRF52_Arduino) for nRF52 boards such as Adafruit NRF52840_FEATHER, NRF52832_FEATHER, NRF52840_FEATHER_SENSE, NRF52840_ITSYBITSY, NRF52840_CIRCUITPLAY, NRF52840_CLUE, NRF52840_METRO, NRF52840_PCA10056, PARTICLE_XENON, **NINA_B302_ublox**, etc. [![GitHub release](https://img.shields.io/github/release/adafruit/Adafruit_nRF52_Arduino.svg)](https://github.com/adafruit/Adafruit_nRF52_Arduino/releases/latest)
 3. [`Seeeduino nRF52 core 1.0.0+`](https://github.com/Seeed-Studio/Adafruit_nRF52_Arduino) for Seeeduino nRF52840-based boards such as **Seeed_XIAO_NRF52840 and Seeed_XIAO_NRF52840_SENSE**. [![GitHub release](https://img.shields.io/github/release/Seeed-Studio/Adafruit_nRF52_Arduino.svg)](https://github.com/Seeed-Studio/Adafruit_nRF52_Arduino/releases/latest)
 
 4. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) for [ISR_16_PWMs_Array_Complex example](examples/ISR_16_PWMs_Array_Complex).


---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**nRF52_Slow_PWM**](https://github.com/khoih-prog/nRF52_Slow_PWM), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/nRF52_Slow_PWM.svg?)](https://www.ardu-badge.com/nRF52_Slow_PWM) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**nRF52_Slow_PWM**](https://github.com/khoih-prog/nRF52_Slow_PWM) page.
2. Download the latest release `nRF52_Slow_PWM-main.zip`.
3. Extract the zip file to `nRF52_Slow_PWM-main` directory 
4. Copy whole `nRF52_Slow_PWM-main` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**nRF52_Slow_PWM** library](https://registry.platformio.org/libraries/khoih-prog/nRF52_Slow_PWM) by using [Library Manager](https://registry.platformio.org/libraries/khoih-prog/nRF52_Slow_PWM/installation). Search for **nRF52_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---

### Packages' Patches

#### 1. For Adafruit nRF52840 and nRF52832 boards

**To be able to compile, run and automatically detect and display BOARD_NAME on nRF52840/nRF52832 boards**, you have to copy the whole [nRF52 1.3.0](Packages_Patches/adafruit/hardware/nrf52/1.3.0) directory into Adafruit nRF52 directory (~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0). 

Supposing the Adafruit nRF52 version is 1.3.0. These files must be copied into the directory:
- `~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/platform.txt`
- `~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/boards.txt`
- `~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/variants/NINA_B302_ublox/variant.h`
- `~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/variants/NINA_B302_ublox/variant.cpp`
- `~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/variants/NINA_B112_ublox/variant.h`
- `~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/variants/NINA_B112_ublox/variant.cpp`
- **`~/.arduino15/packages/adafruit/hardware/nrf52/1.3.0/cores/nRF5/Udp.h`**

Whenever a new version is installed, remember to copy these files into the new version directory. For example, new version is x.yy.z
These files must be copied into the directory:

- `~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/platform.txt`
- `~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/boards.txt`
- `~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/variants/NINA_B302_ublox/variant.h`
- `~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/variants/NINA_B302_ublox/variant.cpp`
- `~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/variants/NINA_B112_ublox/variant.h`
- `~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/variants/NINA_B112_ublox/variant.cpp`
- **`~/.arduino15/packages/adafruit/hardware/nrf52/x.yy.z/cores/nRF5/Udp.h`**

---

#### 2. For Seeeduino nRF52840 boards

**To be able to compile and run on Xiao nRF52840 boards**, you have to copy the whole [nRF52 1.0.0](Packages_Patches/Seeeduino/hardware/nrf52/1.0.0) directory into Seeeduino nRF52 directory (~/.arduino15/packages/Seeeduino/hardware/nrf52/1.0.0). 

Supposing the Seeeduino nRF52 version is 1.0.0. These files must be copied into the directory:

- **`~/.arduino15/packages/Seeeduino/hardware/nrf52/1.0.0/cores/nRF5/Print.h`**
- **`~/.arduino15/packages/Seeeduino/hardware/nrf52/1.0.0/cores/nRF5/Print.cpp`**
- **`~/.arduino15/packages/Seeeduino/hardware/nrf52/1.0.0/cores/nRF5/Udp.h`**

Whenever a new version is installed, remember to copy these files into the new version directory. For example, new version is x.yy.z
These files must be copied into the directory:

- **`~/.arduino15/packages/Seeeduino/hardware/nrf52/x.yy.z/cores/nRF5/Print.h`**
- **`~/.arduino15/packages/Seeeduino/hardware/nrf52/x.yy.z/cores/nRF5/Print.cpp`**
- **`~/.arduino15/packages/Seeeduino/hardware/nrf52/x.yy.z/cores/nRF5/Udp.h`**


---

To use `Sparkfun Pro nRF52840 Mini`, you must install `Packages_Patches` and use Adafruit nrf52 core v1.0.0+

---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```cpp
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "nRF52_Slow_PWM.hpp"    //https://github.com/khoih-prog/nRF52_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```cpp
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_Slow_PWM.h"      //https://github.com/khoih-prog/nRF52_Slow_PWM
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)

---
---


## Usage


Before using any Timer, you have to make sure the Timer has not been used by any other purpose. Depending on the board, you can select NRF52 Hardware Timer from NRF_TIMER_1-NRF_TIMER_4 (1 to 4)


#### 1. Init Hardware Timer

```cpp
// Depending on the board, you can select NRF52 Hardware Timer from NRF_TIMER_1-NRF_TIMER_4 (1 to 4)
// If you select the already-used NRF_TIMER_0, it'll be auto modified to use NRF_TIMER_1

// Init NRF52 timer NRF_TIMER2
NRF52Timer ITimer(NRF_TIMER_2);

// Init nRF52_Slow_PWM, each can service 16 different ISR-based PWM channels
NRF52_Slow_PWM ISR_PWM;
```

#### 2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions

```cpp
void irqCallbackStartFunc()
{

}

void irqCallbackStopFunc()
{

}

void setup()
{
  ....
  
  // You can use this with PWM_Freq in Hz
  ISR_PWM.setPWM(PWM_Pin, PWM_Freq, PWM_DutyCycle, irqCallbackStartFunc, irqCallbackStopFunc);
                   
  ....                 
}  
```

---
---

### Examples: 

 1. [ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
 2. [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
 3. [ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
 4. [ISR_Changing_PWM](examples/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/ISR_Modify_PWM)
 6. [**multiFileProject**](examples/multiFileProject) **New**

 
---
---

### Example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)

https://github.com/khoih-prog/nRF52_Slow_PWM/blob/2c86fbfde1ea38be2f4ff90c2ca818ca225e5315/examples/ISR_16_PWMs_Array_Complex/ISR_16_PWMs_Array_Complex.ino#L16-L595


---
---

### Debug Terminal Output Samples

### 1. ISR_16_PWMs_Array_Complex on NRF52840_ITSYBITSY

The following is the sample terminal output when running example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex) to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_16_PWMs_Array_Complex on NRF52840_ITSYBITSY
NRF52_Slow_PWM v1.2.2
[PWM] NRF52TimerInterrupt: Timer =  NRF_TIMER3 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  200000.00 , _count =  80
Starting ITimer OK, micros() = 
Channel : 0	    Period : 1000000		OnTime : 49999	Start_Time : 2647460
Channel : 1	    Period : 500000		OnTime : 49999	Start_Time : 2648437
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2649414
Channel : 3	    Period : 250000		OnTime : 74999	Start_Time : 2650390
Channel : 4	    Period : 200000		OnTime : 79999	Start_Time : 2651367
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2652343
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2653320
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2654296
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2655273
Channel : 9	    Period : 100000		OnTime : 64999	Start_Time : 2656250
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2657226
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2658203
Channel : 12	    Period : 40000		OnTime : 31999	Start_Time : 2659179
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2660156
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2661132
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2663085
SimpleTimer (ms): 2000, us : 12805664, Dus : 10159180
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.18
PWM Channel : 1, programmed Period (us): 500000.00, actual : 496094, programmed DutyCycle : 10.00, actual : 10.24
PWM Channel : 2, programmed Period (us): 333333.34, actual : 340820, programmed DutyCycle : 20.00, actual : 19.20
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 30.00, actual : 31.25
PWM Channel : 4, programmed Period (us): 200000.00, actual : 205078, programmed DutyCycle : 40.00, actual : 37.14
PWM Channel : 5, programmed Period (us): 166666.67, actual : 171875, programmed DutyCycle : 45.00, actual : 42.05
PWM Channel : 6, programmed Period (us): 142857.14, actual : 147461, programmed DutyCycle : 50.00, actual : 47.02
PWM Channel : 7, programmed Period (us): 125000.00, actual : 122070, programmed DutyCycle : 55.00, actual : 56.80
PWM Channel : 8, programmed Period (us): 111111.11, actual : 107422, programmed DutyCycle : 60.00, actual : 60.91
PWM Channel : 9, programmed Period (us): 100000.00, actual : 106446, programmed DutyCycle : 65.00, actual : 58.72
PWM Channel : 10, programmed Period (us): 66666.66, actual : 64453, programmed DutyCycle : 70.00, actual : 69.70
PWM Channel : 11, programmed Period (us): 50000.00, actual : 47851, programmed DutyCycle : 75.00, actual : 73.47
PWM Channel : 12, programmed Period (us): 40000.00, actual : 37110, programmed DutyCycle : 80.00, actual : 78.95
PWM Channel : 13, programmed Period (us): 33333.33, actual : 30273, programmed DutyCycle : 85.00, actual : 96.78
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25390, programmed DutyCycle : 90.00, actual : 92.31
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20508, programmed DutyCycle : 95.00, actual : 95.24
SimpleTimer (ms): 2000, us : 22888671, Dus : 10083007
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1002929, programmed DutyCycle : 5.00, actual : 4.77
PWM Channel : 1, programmed Period (us): 500000.00, actual : 505859, programmed DutyCycle : 10.00, actual : 9.46
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333985, programmed DutyCycle : 20.00, actual : 20.18
PWM Channel : 3, programmed Period (us): 250000.00, actual : 254883, programmed DutyCycle : 30.00, actual : 28.35
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200195, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166992, programmed DutyCycle : 45.00, actual : 45.03
PWM Channel : 6, programmed Period (us): 142857.14, actual : 140625, programmed DutyCycle : 50.00, actual : 52.08
PWM Channel : 7, programmed Period (us): 125000.00, actual : 130859, programmed DutyCycle : 55.00, actual : 52.99
PWM Channel : 8, programmed Period (us): 111111.11, actual : 116211, programmed DutyCycle : 60.00, actual : 57.98
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100586, programmed DutyCycle : 65.00, actual : 65.05
PWM Channel : 10, programmed Period (us): 66666.66, actual : 71289, programmed DutyCycle : 70.00, actual : 60.27
PWM Channel : 11, programmed Period (us): 50000.00, actual : 47851, programmed DutyCycle : 75.00, actual : 77.55
PWM Channel : 12, programmed Period (us): 40000.00, actual : 36133, programmed DutyCycle : 80.00, actual : 81.08
PWM Channel : 13, programmed Period (us): 33333.33, actual : 31250, programmed DutyCycle : 85.00, actual : 84.37
PWM Channel : 14, programmed Period (us): 25000.00, actual : 76172, programmed DutyCycle : 90.00, actual : 97.43
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20508, programmed DutyCycle : 95.00, actual : 95.24
SimpleTimer (ms): 2000, us : 32927734, Dus : 10039063
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.08
PWM Channel : 1, programmed Period (us): 500000.00, actual : 496094, programmed DutyCycle : 10.00, actual : 10.24
PWM Channel : 2, programmed Period (us): 333333.34, actual : 337891, programmed DutyCycle : 20.00, actual : 18.79
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250000, programmed DutyCycle : 30.00, actual : 30.47
PWM Channel : 4, programmed Period (us): 200000.00, actual : 196289, programmed DutyCycle : 40.00, actual : 39.80
PWM Channel : 5, programmed Period (us): 166666.67, actual : 171875, programmed DutyCycle : 45.00, actual : 42.05
PWM Channel : 6, programmed Period (us): 142857.14, actual : 148438, programmed DutyCycle : 50.00, actual : 46.71
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125000, programmed DutyCycle : 55.00, actual : 53.12
PWM Channel : 8, programmed Period (us): 111111.11, actual : 107422, programmed DutyCycle : 60.00, actual : 59.09
PWM Channel : 9, programmed Period (us): 100000.00, actual : 97656, programmed DutyCycle : 65.00, actual : 64.00
PWM Channel : 10, programmed Period (us): 66666.66, actual : 63477, programmed DutyCycle : 70.00, actual : 69.23
PWM Channel : 11, programmed Period (us): 50000.00, actual : 46875, programmed DutyCycle : 75.00, actual : 72.92
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40039, programmed DutyCycle : 80.00, actual : 80.49
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33204, programmed DutyCycle : 85.00, actual : 88.23
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25391, programmed DutyCycle : 90.00, actual : 92.31
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20507, programmed DutyCycle : 95.00, actual : 95.24
```

---

### 2. ISR_16_PWMs_Array on NRF52840_ITSYBITSY

The following is the sample terminal output when running example [**ISR_16_PWMs_Array**](examples/ISR_16_PWMs_Array) on **nRF52-based NRF52840_ITSYBITSY** to demonstrate how to use multiple PWM channels with simple callback functions.

```
Starting ISR_16_PWMs_Array on NRF52840_ITSYBITSY
NRF52_Slow_PWM v1.2.2
[PWM] NRF52TimerInterrupt: Timer =  NRF_TIMER2 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  50000.00 , _count =  320
Starting ITimer OK, micros() = 2889648
Channel : 0	    Period : 1000000		OnTime : 49999	Start_Time : 2890625
Channel : 1	    Period : 500000		OnTime : 49999	Start_Time : 2890625
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2891601
Channel : 3	    Period : 250000		OnTime : 74999	Start_Time : 2891601
Channel : 4	    Period : 200000		OnTime : 79999	Start_Time : 2892578
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2893554
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2893554
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2894531
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2894531
Channel : 9	    Period : 100000		OnTime : 64999	Start_Time : 2895507
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2895507
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2896484
Channel : 12	    Period : 40000		OnTime : 31999	Start_Time : 2897460
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2897460
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2898437
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2898437
```

---

### 3. ISR_16_PWMs_Array_Simple on NRF52840_ITSYBITSY

The following is the sample terminal output when running example [**ISR_16_PWMs_Array_Simple**](examples/ISR_16_PWMs_Array_Simple) on **nRF52-based NRF52840_ITSYBITSY** to demonstrate how to use multiple PWM channels.

```
Starting ISR_16_PWMs_Array_Simple on NRF52840_ITSYBITSY
NRF52_Slow_PWM v1.2.2
[PWM] NRF52TimerInterrupt: Timer =  NRF_TIMER2 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  50000.00 , _count =  320
Starting ITimer OK, micros() = 2924804
Channel : 0	    Period : 1000000		OnTime : 49999	Start_Time : 2925781
Channel : 1	    Period : 500000		OnTime : 49999	Start_Time : 2925781
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2926757
Channel : 3	    Period : 250000		OnTime : 74999	Start_Time : 2927734
Channel : 4	    Period : 200000		OnTime : 79999	Start_Time : 2927734
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2928710
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2928710
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2929687
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2929687
Channel : 9	    Period : 100000		OnTime : 64999	Start_Time : 2930664
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2930664
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2931640
Channel : 12	    Period : 40000		OnTime : 31999	Start_Time : 2931640
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2932617
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2933593
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2933593
```

---

### 4. ISR_Modify_PWM on NRF52840_ITSYBITSY

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/ISR_Modify_PWM) on **nRF52-based NRF52840_ITSYBITSY** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on NRF52840_ITSYBITSY
NRF52_Slow_PWM v1.2.2
[PWM] NRF52TimerInterrupt: Timer =  NRF_TIMER2 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  50000.00 , _count =  320
Starting ITimer OK, micros() = 3310546
Using PWM Freq = 200.00, PWM DutyCycle = 1.00
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 3311523
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 13313476
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 23309570
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 33310546
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 43306640
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 53313476
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 63309570
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 73316406
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 83312500
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 93319335
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 103315429
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 113322265
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 123318359
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 133325195
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 143326171
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 153322265
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 163318359
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 173325195
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 183321289
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 193328125
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 203324218
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 213331054
Channel : 0	    Period : 5000		OnTime : 50	Start_Time : 223327148
Channel : 0	    Period : 10000		OnTime : 555	Start_Time : 233333984

```

---

### 5. ISR_Changing_PWM on NRF52840_ITSYBITSY

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/ISR_Changing_PWM) on **nRF52-based NRF52840_ITSYBITSY** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on NRF52840_ITSYBITSY
NRF52_Slow_PWM v1.2.2
[PWM] NRF52TimerInterrupt: Timer =  NRF_TIMER2 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  50000.00 , _count =  320
Starting ITimer OK, micros() = 2925781
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2926757
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 18140625
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 33396484
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 48607421
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 63864257
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 79079101
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 94331054
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 109539062
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level `_PWM_LOGLEVEL_` from 0 to 4

```cpp
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [nRF52_Slow_PWM issues](https://github.com/khoih-prog/nRF52_Slow_PWM/issues)

---

## TO DO

1. Search for bug and improvement.
2. Similar features for remaining Arduino boards

---

## DONE

 1. Basic hardware multi-channel PWM for **nRF52-based AdaFruit Itsy-Bitsy nRF52840, Feather nRF52840 Express, etc.** using [`Adafruit nRF52 core`](https://github.com/adafruit/Adafruit_nRF52_Arduino)
 2. Add Table of Contents
 3. Add functions to modify PWM settings on-the-fly
 4. Fix `multiple-definitions` linker error
 5. Optimize library code by using `reference-passing` instead of `value-passing`
 6. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
 7. DutyCycle to be optionally updated at the end current PWM period instead of immediately.
 8. Display informational warning only when `_PWM_LOGLEVEL_` > 3
 9. Add support to `Sparkfun Pro nRF52840 Mini`
10. Add support to Seeeduino nRF52840-based boards such as **SEEED_XIAO_NRF52840 and SEEED_XIAO_NRF52840_SENSE**, etc. using Seeeduino `nRF%2` core
11. Add astyle using `allman` style. Restyle the library
12. Add `Packages' Patches` to add Seeeduino `nRF52` core


---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/nRF52_Slow_PWM/blob/main/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


