# Mini BMS – System Architecture

## 1. Introduction

The Mini Battery Management System follows a modular processing architecture to monitor battery parameters, identify abnormal conditions, and determine appropriate control actions.

The current implementation is a PC-based software simulation developed using C.

## 2. Input Parameters

The system accepts the following inputs:

- Cell 1 voltage
- Cell 2 voltage
- Cell 3 voltage
- Battery temperature
- Charging command
- Discharging command

## 3. Processing Architecture

### 3.1 Input Validation
Checks whether the received battery parameters and control commands contain valid values.

### 3.2 Voltage Filtering
Applies a five-sample moving average filter independently to each cell voltage to reduce measurement fluctuations.

### 3.3 SOC Estimation
Calculates an approximate State of Charge using a simplified voltage-based estimation method.

### 3.4 Fault Detection
Monitors battery parameters and identifies:
- Overvoltage
- Undervoltage
- Overtemperature
- Cell imbalance

Hysteresis is used to provide separate fault activation and recovery thresholds.

### 3.5 State Management
Determines the battery operating condition based on the processed inputs and detected faults.

### 3.6 Control Decisions
Evaluates charging and discharging commands against the detected battery conditions and determines the corresponding enable outputs.

## 4. Output and Diagnostics

The system provides:

- Charging enable status
- Discharging enable status
- Battery status
- Operating state
- Fault indicator
- Fault code
- Estimated SOC
- Cell voltage information

Diagnostic information is displayed through simulated UART-style terminal output.

Detected fault events are recorded in a local CSV file.

## 5. Processing Sequence

The overall processing sequence is:

1. Input acquisition
2. Input validation
3. Voltage filtering
4. SOC estimation
5. Fault detection
6. State management
7. Charging and discharging control
8. Diagnostic reporting
9. Fault logging

## 6. Implementation Scope

The current architecture represents a software simulation.

Actual battery sensors, microcontroller peripherals, hardware UART communication, and physical protection circuits are planned for future hardware integration.

## 7. Conclusion

The architecture demonstrates the fundamental workflow of a Battery Management System, combining battery monitoring, filtering, SOC estimation, fault detection, operating-state management, control decisions, and diagnostic reporting.
