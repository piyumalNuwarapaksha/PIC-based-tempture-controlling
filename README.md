# PIC-Based Temperature Controlled Fan

## Overview

The **PIC-Based Temperature Controlled Fan** is an embedded system designed to automatically control the speed of a DC fan based on the surrounding temperature.

A temperature sensor continuously measures the ambient temperature and provides the corresponding signal to the **PIC microcontroller**. The microcontroller processes the temperature data and generates a **PWM (Pulse Width Modulation)** signal to control the fan speed.

As the temperature increases, the PWM duty cycle is increased, causing the fan to operate at a higher speed. When the temperature decreases, the fan speed is reduced accordingly.

This project demonstrates the integration of **temperature sensing, microcontroller programming, PWM-based motor control, and embedded system design**.

## Key Features

- PIC microcontroller-based control
- Real-time temperature sensing
- Automatic fan-speed adjustment
- PWM-based DC fan speed control
- Temperature-dependent fan operation
- Reduced unnecessary power consumption
- Embedded hardware and firmware development

## System Operation

```text
Temperature Sensor
        ↓
   PIC Microcontroller
        ↓
   Temperature Processing
        ↓
    PWM Generation
        ↓
    Fan Driver Circuit
        ↓
       DC Fan
```

The system continuously monitors the temperature and adjusts the fan speed accordingly, providing an automatic and efficient cooling solution.

## Technologies Used

- **Microcontroller:** PIC
- **Programming:** Embedded C
- **Temperature Sensor:** Temperature sensing module
- **Motor Control:** PWM
- **Actuator:** DC Fan
- **Development:** Embedded Hardware & Firmware

## Applications

This system can be used as a basic thermal management solution for:

- Electronic equipment cooling
- Small enclosures
- Embedded systems
- Computer and power-supply cooling
- Temperature-controlled ventilation systems

## Learning Outcomes

Through this project, the following concepts were explored:

- PIC microcontroller programming
- Sensor interfacing
- ADC/temperature measurement
- PWM generation
- DC motor/fan speed control
- Embedded system hardware design
- Firmware development and testing
