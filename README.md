# Proximity-Activated Cooling System

Embedded system that intelligently controls a fan using temperature sensing and user presence detection to improve energy efficiency and responsiveness.

---

## Overview

This project implements a smart cooling system using an Arduino Nano that activates a fan only when both environmental and user conditions are met. Temperature is measured using a DHT11 sensor, and user presence is detected using an HC-SR04 ultrasonic sensor.

The system dynamically adjusts fan speed using PWM based on temperature levels, demonstrating real-time embedded control and sensor fusion.

---

## System Architecture

The system integrates multiple subsystems:

- **Sensing:** Temperature (DHT11) + Distance (HC-SR04)
- **Control:** Arduino Nano (decision logic + PWM control)
- **Actuation:** TIP120 transistor driving DC motor (fan)
- **Feedback:** LCD display + LED indicators

```
[Temp Sensor + Distance Sensor] → [Control Logic] → [PWM Motor Driver] → [Fan]
```

---

## Control Strategy

### Dual-Condition Logic

The fan activates only when:

- Temperature exceeds threshold  
- User is within proximity (≤ 80 cm)  

### Temperature-Based PWM Control

| Temperature (°C) | Fan Speed | PWM |
|----------------|----------|-----|
| < 26           | OFF      | 0   |
| 26–29          | LOW      | 120 |
| 29–32          | MEDIUM   | 180 |
| ≥ 32           | HIGH     | 255 |

### Hysteresis

- Fan turns OFF when temperature drops below 25°C  
- Prevents rapid toggling  

---

## State Machine

- **Idle:** Monitor sensors  
- **Evaluate:** Check thresholds  
- **Cooling Active:** Fan ON (PWM controlled)  
- **Standby:** Conditions not met  

This ensures stable and energy-efficient operation.

---

## Hardware Design

### Components

- Arduino Nano  
- DHT11 Temperature Sensor  
- HC-SR04 Ultrasonic Sensor  
- TIP120 Transistor  
- DC Motor with fan blades  
- LCD Display (I2C)  
- LEDs (status indicators)  
- Flyback Diode  
- External 5V power supply  

### Key Design Decisions

- **Sensor Fusion:** Combined temperature and proximity prevents unnecessary operation  
- **External Power Supply:** Required to avoid Arduino voltage instability  
- **TIP120 Driver:** Ensures reliable switching for motor current  
- **PWM Control:** Enables efficient and scalable fan speed control  

---

## Circuit Configuration

| Component | Connection |
|----------|-----------|
| DHT11 Data | D2 |
| Motor Control | D3 (PWM → TIP120 base) |
| Ultrasonic Trigger | D9 |
| Ultrasonic Echo | D10 |
| LCD SDA | A4 |
| LCD SCL | A5 |
| LEDs | D6, D7 |

All grounds are shared across Arduino and external supply.

---

## Software Implementation

Core firmware features:

- Sensor reading (temperature + distance)
- Distance calculation using time-of-flight
- Threshold-based decision logic
- PWM fan control
- LCD and LED feedback
- Modular function-based architecture

```cpp
if (personNearby) {
    if (temperatureC >= TEMP_ON_HIGH) fanPWM = 255;
    else if (temperatureC >= TEMP_ON_MED) fanPWM = 180;
    else if (temperatureC >= TEMP_ON_LOW) fanPWM = 120;
}
```

---

## System Performance

Testing confirmed:

- Fan activates only when both conditions are satisfied  
- Smooth PWM transitions between speed levels  
- Real-time response (~500 ms update rate)  
- Accurate sensor readings displayed on LCD  

Example behavior:

| Temp (°C) | Distance (cm) | Output |
|----------|--------------|--------|
| 24       | 100          | OFF    |
| 27       | 60           | LOW    |
| 30       | 50           | MEDIUM |
| 33       | 40           | HIGH   |
| 31       | 120          | OFF    |

---

## Engineering Challenges & Solutions

### Issue: Power Instability  
- **Fix:** Separate 5V supply for motor and LCD  

### Issue: LCD Visibility  
- **Fix:** Adjusted contrast potentiometer  

### Issue: Sensor Noise  
- **Fix:** Minor filtering and threshold tuning  

---

## Results & Validation

- Verified correct system response under varying conditions  
- Confirmed energy-efficient operation (fan only runs when needed)  
- Demonstrated stable integration of multiple sensors  
- Validated PWM-based control effectiveness  

---

## Future Improvements

- Higher accuracy temperature sensor  
- MOSFET-based driver for efficiency  
- Advanced filtering for ultrasonic readings  
- Wireless monitoring (IoT integration)  
- Enclosure and real-world deployment  

---

## Skills Demonstrated

- Embedded systems design  
- Sensor fusion and real-time control  
- PWM motor control  
- Transistor-based switching  
- Hardware/software integration  
- System debugging and validation  
