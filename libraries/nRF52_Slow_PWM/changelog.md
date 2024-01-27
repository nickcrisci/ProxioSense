# nRF52_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/nRF52_Slow_PWM.svg?)](https://www.ardu-badge.com/nRF52_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/nRF52_Slow_PWM.svg)](https://github.com/khoih-prog/nRF52_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/nRF52_Slow_PWM/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/nRF52_Slow_PWM.svg)](http://github.com/khoih-prog/nRF52_Slow_PWM/issues)

---
---

## Table of Contents

* [Changelog](#changelog)
  * [Releases v1.2.2](#releases-v122)
  * [Releases v1.2.1](#releases-v121)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.0](#releases-v110)
  * [Initial Releases v1.0.0](#Initial-Releases-v100)

---
---

## Changelog

### Releases v1.2.2

1. Add support to Seeeduino nRF52840-based boards such as **Seeed_XIAO_NRF52840 and Seeed_XIAO_NRF52840_SENSE**, etc. using Seeeduino `nRF52` core
2. Add astyle using `allman` style. Restyle the library
3. Update all examples
4. Update `Packages' Patches` to add Seeeduino `nRF52` core

### Releases v1.2.1

1. Fix `DutyCycle` bug. Check [float precisison of DutyCycle only sometimes working #3](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/3)
2. Fix `New Period` display bug. Check [random dropouts #4](https://github.com/khoih-prog/SAMD_Slow_PWM/issues/4)
3. Display informational warning only when `_PWM_LOGLEVEL_` > 3
4. Add support to `Sparkfun Pro nRF52840 Mini`
5. Update examples
6. Update `Packages' Patches`

### Releases v1.2.0

1. Fix `multiple-definitions` linker error. Drop `src_cpp` and `src_h` directories
2. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`. Check [Change Duty Cycle #1](https://github.com/khoih-prog/ESP8266_PWM/issues/1#issuecomment-1024969658)
3. DutyCycle to be optionally updated at the end current PWM period instead of immediately. Check [DutyCycle to be updated at the end current PWM period #2](https://github.com/khoih-prog/ESP8266_PWM/issues/2)
4. Optimize library code by using `reference-passing` instead of `value-passing`
5. Add example [multiFileProject](examples/multiFileProject) to demo for multiple-file project
6. Update examples accordingly
7. Update `Packages_Patches`

### Releases v1.1.0

1. Add functions to modify PWM settings on-the-fly
2. Add example to demo how to modify PWM settings on-the-fly

### Initial Releases v1.0.0

1. Initial coding to support **nRF52-based boards, such as AdaFruit Itsy-Bitsy nRF52840, Feather nRF52840 Express**, etc. using [`Adafruit nRF52 core`](https://github.com/adafruit/Adafruit_nRF52_Arduino)

2. The hybrid ISR-based PWM channels can generate from very low (much less than 1Hz) to highest PWM frequencies up to 1000Hz with acceptable accuracy.

---
---

## Copyright

Copyright 2021- Khoi Hoang


