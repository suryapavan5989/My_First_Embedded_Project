# MINI BATTERY MANAGEMENT SYSTEM (BMS)
### Project Report

---

# 1. Abstract

A Battery Management System (BMS) is an important component of battery-powered applications. It monitors battery parameters, detects abnormal operating conditions and manages battery operation through suitable control mechanisms.

This project presents the development of a Mini Battery Management System using the C programming language. The system is designed as a PC-based simulation of a three-cell series battery pack.

The implementation includes individual cell voltage monitoring, temperature monitoring, State of Charge (SOC) estimation, fault detection, voltage filtering, hysteresis, operating-state management and charging/discharging control logic.

The simulation also provides UART-style diagnostic output and CSV-based fault logging.

The primary objective is to understand the fundamental working principles of a BMS and establish a software foundation for future microcontroller-based implementation.

---

# 2. Introduction

Rechargeable batteries are widely used in electric vehicles, portable electronics and energy storage systems.

However, battery cells must operate within appropriate voltage and temperature limits. Abnormal operating conditions can affect battery performance, reliability and safety.

A Battery Management System helps monitor these conditions and determines suitable control actions.

This project implements a simplified BMS model that monitors three individual battery cells connected in series.

The current implementation uses manually entered parameters to simulate battery operation on a PC.

---

# 3. Problem Statement

Monitoring only the total battery pack voltage may not reveal abnormalities in individual cells.

For example, one cell may have a significantly higher or lower voltage than the other cells.

Similarly, excessive temperature and abnormal operating conditions must be identified before they affect battery operation.

Therefore, this project aims to develop a basic monitoring and control simulation capable of:

- Monitoring individual cell voltages.
- Monitoring battery temperature.
- Identifying abnormal battery conditions.
- Estimating remaining battery charge.
- Determining charging and discharging enable outputs.
- Recording detected fault events.

---

# 4. Project Objectives

The main objectives are:

1. Understand the fundamental operation of a Battery Management System.
2. Monitor individual voltages in a three-cell series battery pack.
3. Implement overvoltage and undervoltage detection.
4. Implement overtemperature and cell imbalance detection.
5. Estimate battery State of Charge.
6. Apply moving average voltage filtering.
7. Implement hysteresis for fault recovery.
8. Develop charging and discharging control logic.
9. Generate diagnostic information.
10. Implement fault event logging.

---

# 5. System Specifications

| Parameter | Description |
|---|---|
| Project Type | Embedded Systems Software Simulation |
| Programming Language | C |
| Battery Configuration | Three-cell series pack |
| Development Platform | Windows PC |
| Compiler | GCC |
| Development Environment | MSYS2 UCRT64 |
| Diagnostic Output | UART-style terminal simulation |
| Fault Storage | CSV file |

## 5.1 Battery Configuration

The simulation uses three cells connected in series.

For a series connection:

- Individual cell voltages add together.
- The same current passes through every cell.
- The capacity in Ah remains that of an individual cell, assuming identical cells.

The nominal reference voltage used for each cell is 3.7 V.

Therefore:

Nominal Pack Voltage = 3 × 3.7 V = 11.1 V.

---

# 6. System Architecture

The system follows this processing sequence:

1. Input Acquisition
2. Input Validation
3. Voltage Filtering
4. SOC Estimation
5. Fault Detection
6. State Management
7. Charging and Discharging Control
8. Diagnostic Reporting
9. Fault Logging

## 6.1 Input Parameters

The simulation accepts:

- Cell 1 voltage.
- Cell 2 voltage.
- Cell 3 voltage.
- Battery temperature.
- Charging command.
- Discharging command.

## 6.2 Output Parameters

The system generates:

- Charging enable.
- Discharging enable.
- Fault indicator.
- Battery status.
- Fault code.
- Maximum cell voltage.
- Minimum cell voltage.
- Cell voltage difference.
- Estimated SOC.
- Operating state.

---

# 7. Implementation Details

## 7.1 Cell Voltage Monitoring

The system monitors all three battery cell voltages individually.

This helps identify abnormalities that may not be visible through total pack voltage monitoring.

## 7.2 Temperature Monitoring

Battery temperature is monitored to identify excessive operating temperatures.

The simulation generates an overtemperature fault when the configured threshold is exceeded.

## 7.3 Fault Detection

Four major battery conditions are monitored:

**Overvoltage:**
Detects when an individual cell voltage exceeds the configured upper limit.

**Undervoltage:**
Detects when an individual cell voltage falls below the configured lower limit.

**Overtemperature:**
Detects when battery temperature exceeds the configured temperature limit.

**Cell Imbalance:**
Calculates the difference between the maximum and minimum cell voltages to identify significant voltage variations.

## 7.4 Hysteresis

Hysteresis uses different activation and recovery thresholds.

This prevents the fault condition from repeatedly switching ON and OFF when measurements fluctuate near a threshold.

The simulation implements hysteresis for overvoltage, undervoltage, overtemperature and cell imbalance.

## 7.5 Voltage Filtering

A five-sample moving average filter is implemented independently for each battery cell.

The filter reduces short-term voltage fluctuations and provides smoother readings for subsequent processing.

## 7.6 State of Charge Estimation

State of Charge represents the estimated remaining battery charge as a percentage.

The current implementation uses a simplified voltage-based estimation method.

It provides an educational approximation rather than an accurate measurement of real battery capacity.

## 7.7 Operating State Management

The system determines the battery's operating condition based on the processed parameters and detected faults.

The simulation includes operating conditions such as:

- IDLE
- CHARGING
- DISCHARGING
- WARNING
- FAULT

## 7.8 Charging and Discharging Control

The system evaluates charging and discharging commands against detected battery conditions.

Based on its implemented control logic, it determines whether the corresponding enable outputs should be activated.

## 7.9 Diagnostic Communication

The simulation displays diagnostic information through UART-style terminal output.

The displayed information includes SOC, operating state and fault codes.

This is a communication simulation and does not represent actual hardware UART transmission.

## 7.10 Fault Logging

Detected fault events are recorded in a local CSV file named `fault_log.csv`.

This provides PC-based fault storage and allows recorded events to remain available after the program terminates.

---

# 8. Monitoring Thresholds

The simulation uses the following educational assumptions:

| Parameter | Activation Threshold |
|---|---|
| Cell Overvoltage | > 4.2 V |
| Cell Undervoltage | < 3.0 V |
| Overtemperature | > 60°C |
| Cell Imbalance | > 0.20 V |

## 8.1 Hysteresis Recovery Thresholds

| Parameter | Recovery Threshold |
|---|---|
| Cell Overvoltage | ≤ 4.10 V |
| Cell Undervoltage | ≥ 3.10 V |
| Overtemperature | ≤ 55°C |
| Cell Imbalance | ≤ 0.15 V |

**Note:** These values are educational assumptions for a conventional lithium-ion cell. Actual battery protection thresholds must be selected according to the manufacturer's specifications.

---

# 9. Testing and Validation

Seven major functional scenarios were performed to evaluate the simulation.

| Test | Scenario | Result |
|---|---|---|
| 01 | Normal Operation | PASS |
| 02 | Overvoltage Detection | PASS |
| 03 | Undervoltage Detection | PASS |
| 04 | Overtemperature Detection | PASS |
| 05 | Cell Imbalance Detection | PASS |
| 06 | Hysteresis and Voltage Filtering | PASS |
| 07 | Multiple Fault Detection | PASS |

## 9.1 Normal Operation

Input:
- Cell voltages: 3.7 V, 3.7 V, 3.7 V.
- Temperature: 25°C.
- Charging: ON.
- Discharging: OFF.

Result:
- Battery status: NORMAL.
- Operating state: CHARGING.
- Charging enabled.
- No faults detected.

## 9.2 Overvoltage

Input:
- Cell voltages: 3.7 V, 4.3 V, 3.7 V.
- Temperature: 25°C.

Result:
- Overvoltage detected.
- Cell imbalance also detected.
- Fault code: 9.
- Charging and discharging disabled.

## 9.3 Undervoltage

Input:
- Cell voltages: 2.8 V, 2.9 V, 2.9 V.
- Temperature: 25°C.

Result:
- Undervoltage detected.
- Fault code: 2.
- Discharging disabled.
- Charging enable remained ON according to the implemented recovery-control logic.

This behaviour requires further review before any hardware implementation.

## 9.4 Overtemperature

Input:
- Cell voltages: 3.7 V, 3.7 V, 3.7 V.
- Temperature: 65°C.

Result:
- Overtemperature detected.
- Fault code: 4.
- Charging and discharging disabled.

## 9.5 Cell Imbalance

Input:
- Cell voltages: 4.0 V, 3.7 V, 3.4 V.
- Temperature: 25°C.

Result:
- Cell imbalance detected.
- Voltage difference: 0.6 V.
- Battery entered WARNING state.
- Fault code: 8.
- Charging and discharging disabled.

## 9.6 Hysteresis and Filtering

Input sequence:
- Initial cell voltages: 4.3 V.
- Subsequent readings: 4.1 V.

Result:
- Overvoltage fault activated.
- Moving average initially retained the elevated voltage.
- Subsequent readings gradually reduced the filtered voltage.
- Fault cleared after the recovery threshold was reached.

## 9.7 Multiple Faults

Input:
- Cell voltages: 4.3 V, 3.7 V, 3.7 V.
- Temperature: 65°C.

Result:
- Overvoltage detected.
- Overtemperature detected.
- Cell imbalance detected.
- Combined fault code: 13.
- Both charging and discharging disabled.

## 9.8 Testing Summary

All seven major functional scenarios produced the expected results in the PC simulation.

Additional validation is still required for invalid inputs, conflicting commands, fault-log persistence and recovery from other fault conditions.

---

# 10. Software Structure

The source code is divided into three main files.

| File | Responsibility |
|---|---|
| main.c | Handles user interaction and simulation execution. |
| bms_logic.c | Implements battery monitoring, fault detection and control logic. |
| bms_logic.h | Defines constants, structures, enumerations and function declarations. |

This modular structure separates user interaction from the core BMS processing logic.

---

# 11. Current Limitations

The current implementation is a PC-based software simulation.

The following limitations apply:

1. Battery readings are entered manually rather than acquired from physical sensors.
2. SOC estimation uses a simplified voltage-based method.
3. UART communication is simulated through terminal output.
4. Fault persistence uses a CSV file instead of embedded non-volatile memory.
5. Physical cell balancing is not implemented.
6. Current monitoring and overcurrent protection are not implemented.
7. Actual battery protection hardware has not been integrated.
8. The simulation has not been validated on a physical battery pack.

---

# 12. Future Scope

The project can be extended through the following improvements:

- Integration with an Arduino or STM32 development board.
- Real-time battery voltage monitoring.
- Temperature sensor integration.
- Current sensing and overcurrent detection.
- Hardware UART communication.
- EEPROM or Flash-based fault storage.
- Cell balancing implementation.
- Improved SOC estimation techniques.
- Integration of a suitable multi-cell battery monitoring and protection circuit.
- Hardware-level testing and validation.

---

# 13. Conclusion

The Mini Battery Management System demonstrates the fundamental working principles of battery monitoring and protection logic through a C-based software simulation.

The implementation combines individual cell voltage monitoring, temperature monitoring, SOC estimation, voltage filtering, hysteresis, fault detection, operating-state management, charging and discharging control, diagnostic output and fault logging.

The seven major functional test scenarios demonstrate the expected behaviour of the implemented software under selected operating conditions.

The project establishes a foundation for further exploration of Embedded Systems and future microcontroller-based BMS development.

---

**End of Report**
