# Mini Battery Management System (BMS)

A C-based educational simulation of a Battery Management System (BMS) designed to monitor a three-cell series battery pack, detect abnormal operating conditions, and manage charging and discharging operations.

---

## 1. Project Overview

A Battery Management System plays an important role in monitoring battery conditions and helping maintain safe and reliable battery operation.

This project implements a software-based BMS simulation using the C programming language. It monitors individual cell voltages and battery temperature, estimates the State of Charge (SOC), detects battery-related faults, and determines appropriate charging and discharging control outputs.

The project is developed as an introductory Embedded Systems application, with a focus on battery monitoring, fault detection, control logic, and diagnostic reporting.

| Project Detail|                                                   | Description |
|---|---|
| Project Name |                                                    | Mini Battery Management System |
| Project Type |                                                    |  Embedded Systems Software Simulation |
| Programming Language |                                            |C |
| Battery Configuration |                                           | 3-Cell Series Pack |
| Compiler |                                                        |GCC |
| Development Environment|                                          | MSYS2 UCRT64 |
| Platform |                                                        | Windows PC Simulation |

---

## 2. Key Features

- **Cell Voltage Monitoring:** Monitors the voltage of three individual battery cells.
- **Temperature Monitoring:** Checks battery temperature for abnormal conditions.
- **Overvoltage Protection Logic:** Detects cell voltage exceeding the defined upper limit.
- **Undervoltage Protection Logic:** Detects cell voltage falling below the defined lower limit.
- **Overtemperature Detection:** Identifies excessive battery temperature.
- **Cell Imbalance Detection:** Identifies significant voltage differences between cells.
- **Charging and Discharging Control:** Determines whether charging and discharging operations should be enabled.
- **Hysteresis:** Uses separate activation and recovery thresholds to prevent unstable fault-state switching.
- **Voltage Filtering:** Applies a five-sample moving average filter to reduce measurement fluctuations.
- **State of Charge Estimation:** Estimates remaining battery charge using a simplified voltage-based method.
- **Operating State Management:** Determines the battery's operating condition.
- **UART-Style Diagnostics:** Displays diagnostic information through simulated serial output.
- **Fault Logging:** Records detected fault events in a CSV file.
- **Input Validation:** Checks input values before processing them.

---

## 3. System Workflow

The BMS follows a sequential monitoring and control process:

1. **Input Acquisition** – Receives three cell voltages, battery temperature, and charging/discharging commands.
2. **Input Validation** – Checks whether the entered values are within acceptable input ranges.
3. **Voltage Filtering** – Applies moving average filtering to individual cell voltage readings.
4. **SOC Estimation** – Calculates an approximate battery State of Charge.
5. **Fault Detection** – Checks for overvoltage, undervoltage, overtemperature, and cell imbalance.
6. **State Management** – Determines the battery's current operating state.
7. **Control Decision** – Calculates charging and discharging enable outputs.
8. **Diagnostic Reporting** – Displays battery parameters, operating status, and fault information.
9. **Fault Logging** – Records detected fault events in a local CSV file.

---

## 4. Monitoring Parameters

The simulation uses the following assumed thresholds for a conventional lithium-ion cell:

| Parameter |                                                | Detection Condition |
|---|---|
| Cell Overvoltage |                                            Cell voltage > 4.2 V |
| Cell Undervoltage |                                           Cell voltage < 3.0 V |
| Overtemperature |                                             Temperature > 60°C |
| Cell Imbalance |                                              Maximum cell voltage − Minimum cell voltage > 0.20 V |

### Hysteresis Recovery Thresholds

| Parameter |                                                | Recovery Condition |
|---|---|
| Cell Overvoltage                                           | Cell voltage ≤ 4.10 V |
| Cell Undervoltage                                          |Cell voltage ≥ 3.10 V |
| Overtemperature |                                          | Temperature ≤ 55°C |
| Cell Imbalance |                                           | Voltage difference ≤ 0.15 V |

**Note:** These values are educational simulation assumptions, not universal battery protection limits. Actual thresholds must be selected according to the specific battery cell manufacturer's datasheet.

---

## 5. Project Structure

```text
My_First_Embedded_Project/
│
├── Source/
│   ├── main.c
│   ├── bms_logic.c
│   └── bms_logic.h
│
├── Simulation/
│
├── Documentation/
│
├── Test_Results/
│
├── .gitignore
└── README.md
```

| File / Directory | Purpose |
|---|---|
| `main.c` | Handles user inputs and displays simulation results. |
| `bms_logic.c` | Implements BMS processing and control logic. |
| `bms_logic.h` | Contains definitions, data structures, and function declarations. |
| `Simulation/` | Reserved for simulation-related materials. |
| `Documentation/` | Reserved for project documentation. |
| `Test_Results/` | Reserved for test cases and validation results. |
| `.gitignore` | Excludes generated files from Git tracking. |

---

## 6. Technologies Used

- C Programming Language
- GCC Compiler
- MSYS2 UCRT64
- Git
- GitHub
- CSV File Handling

---

## 7. Installation and Execution

### Prerequisites

- Windows operating system.
- MSYS2 UCRT64 environment.
- GCC compiler installed and configured.

### Step 1: Clone the Repository [use git bash software ]

```bash
git clone https://github.com/suryapavan5989/My_First_Embedded_Project.git
```

### Step 2: Navigate to the Project Directory

```bash
cd My_First_Embedded_Project
```

### Step 3: Compile the Source Files

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 Source/main.c Source/bms_logic.c -o bms.exe
```

### Step 4: Execute the Simulation

```bash
./bms.exe
```

The program prompts the user to enter battery parameters and displays the corresponding monitoring and control results.

---

## 8. Testing and Validation

The simulation has undergone the following major functional tests:

| Test Case | Input Condition | Observed Result |
|---|---|---|
| Normal Operation | All cells at normal voltage and temperature | Normal battery operation |
| Overvoltage | One cell exceeds 4.2 V | Overvoltage fault detected |
| Undervoltage | Cell voltage falls below 3.0 V | Undervoltage fault detected |
| Overtemperature | Temperature exceeds 60°C | Overtemperature fault detected |
| Cell Imbalance | Cell voltage difference exceeds 0.20 V | Imbalance warning detected |
| Hysteresis Recovery | Voltage returns below the recovery threshold | Fault clears after filtered readings recover |
| Multiple Faults | Overvoltage, overtemperature, and imbalance occur together | Combined fault detection |

**Testing Summary:** All seven major functional scenarios produced the expected results during the PC-based simulation tests.

---

## 9. Diagnostic Output and Fault Logging

The simulation provides diagnostic information, including:

- Individual raw and filtered cell voltages.
- Maximum and minimum cell voltages.
- Cell voltage difference.
- Estimated State of Charge.
- Battery operating state.
- Charging and discharging enable status.
- Fault indicator and fault code.

UART-style diagnostic messages are displayed through the PC terminal.

Detected fault events are recorded in `fault_log.csv`, allowing fault information to be retained between simulation sessions.

**Implementation Note:** The current implementation simulates UART-style communication using terminal output. Fault persistence is implemented through PC-based CSV file storage, not physical EEPROM or Flash memory.

---

## 10. Current Limitations

The current version is a software simulation and has not yet been deployed on a physical microcontroller.

- Battery voltage and temperature values are entered manually.
- SOC estimation uses a simplified voltage-based approach.
- UART communication is simulated rather than implemented through a hardware UART peripheral.
- Fault storage uses a CSV file rather than embedded non-volatile memory.
- Physical battery sensing, cell balancing, and protection circuitry are not implemented.

---

## 11. Future Enhancements

- Integration with an STM32 or Arduino development board.
- Real-time battery voltage and temperature sensing.
- Hardware UART communication.
- Hardware-based fault storage using EEPROM or Flash.
- Improved SOC estimation techniques.
- Integration of a suitable battery monitoring and protection circuit.
- Further testing under hardware-based operating conditions.

---

## 12. Project Objective

The primary objective of this project is to understand the fundamental working principles of a Battery Management System through practical C programming and simulation.

It provides a foundation for exploring embedded monitoring systems, battery protection logic, fault handling, and hardware integration.
