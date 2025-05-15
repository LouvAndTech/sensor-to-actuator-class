# Embedded System Project with STM32 and Raspberry Pi

## Introduction

### Context

As part of our academic training, we developed an embedded system using an STM32F411RE microcontroller and a Raspberry Pi 3B+.

### Objective

The main goal was to develop a functional system integrating both an STM32 and a Raspberry Pi.

The system is capable of:

- Displaying the distance measured by the HC-SR04 ultrasonic sensor on a control interface running on the Raspberry Pi.
- Controlling a servo motor connected to the STM32 from the Raspberry Pi's control interface.
- Automatically controlling two servo motors based on the distance measured by the ultrasonic sensor, with calculations performed on the Raspberry Pi.

To achieve this, the **STM32** is configured to:

- Measure distance using the HC-SR04 sensor every 250 ms.
- Transmit distance information via the serial interface every 250 ms.
- Receive a position command for the servo motor via the serial interface and apply it to the servo motor.

Meanwhile, the **Raspberry Pi** is configured to:

- Receive distance measurements from the STM32 via the serial interface.
- Send position commands for the servo motor to the STM32 via the serial interface.
- Compute the position of its own servo motor based on the measured distance.
- Send the calculated position to the STM32's servo motor via the serial interface.

## Full documentation 

The full documentation is available in french [HERE](https://github.com/LouvAndTech/sensor-to-actuator-class/releases/tag/v1)
