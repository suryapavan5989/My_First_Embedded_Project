# Mini BMS – Test Results Report

## 1. Introduction

This document presents the functional testing performed on the Mini Battery Management System software simulation.

The objective of testing is to verify battery monitoring, fault detection, hysteresis, SOC estimation, and charging/discharging control behaviour under different operating conditions.

**Testing Environment:** PC-based C simulation  
**Battery Configuration:** 3-cell series pack  
**Total Major Test Scenarios:** 7

---

## 2. Test Cases

### Test Case 01 – Normal Operation

**Input:**
- Cell 1: 3.7 V
- Cell 2: 3.7 V
- Cell 3: 3.7 V
- Temperature: 25°C
- Charge Command: ON
- Discharge Command: OFF

**Expected Behaviour:**
- No faults detected.
- Battery status remains NORMAL.
- Charging is enabled.
- Discharging remains disabled.

**Observed Result:** Normal operation, SOC estimation and charging control worked as expected.

**Status: PASS**

---

### Test Case 02 – Overvoltage Detection

**Input:**
- Cell 1: 3.7 V
- Cell 2: 4.3 V
- Cell 3: 3.7 V
- Temperature: 25°C
- Charge Command: ON
- Discharge Command: OFF

**Expected Behaviour:**
- Overvoltage detected.
- Charging and discharging disabled.
- Fault indicator activated.

**Observed Result:** Overvoltage and cell imbalance were detected. Fault code: 9.

**Status: PASS**

---

### Test Case 03 – Undervoltage Detection

**Input:**
- Cell 1: 2.8 V
- Cell 2: 2.9 V
- Cell 3: 2.9 V
- Temperature: 25°C
- Charge Command: ON
- Discharge Command: OFF

**Expected Behaviour:**
- Undervoltage detected.
- Discharging disabled.
- Fault indicator activated.

**Observed Result:** Undervoltage detected successfully. Fault code: 2. Charging enable remained ON according to the implemented recovery-control logic.

**Status: PASS**

---

### Test Case 04 – Overtemperature Detection

**Input:**
- Cell 1: 3.7 V
- Cell 2: 3.7 V
- Cell 3: 3.7 V
- Temperature: 65°C
- Charge Command: ON
- Discharge Command: OFF

**Expected Behaviour:**
- Overtemperature detected.
- Charging and discharging disabled.
- Fault indicator activated.

**Observed Result:** Overtemperature detected successfully. Fault code: 4.

**Status: PASS**

---

### Test Case 05 – Cell Imbalance Detection

**Input:**
- Cell 1: 4.0 V
- Cell 2: 3.7 V
- Cell 3: 3.4 V
- Temperature: 25°C
- Charge Command: ON
- Discharge Command: OFF

**Expected Behaviour:**
- Cell voltage difference identified.
- Imbalance warning generated.
- Charging and discharging disabled according to the implemented control logic.

**Observed Result:** Cell imbalance detected. Battery entered WARNING state. Fault code: 8.

**Status: PASS**

---

### Test Case 06 – Hysteresis and Voltage Filtering

**Input Sequence:**
- Initial cell voltages: 4.3 V, 4.3 V, 4.3 V.
- Subsequent readings: 4.1 V, 4.1 V, 4.1 V.
- Temperature: 25°C.

**Expected Behaviour:**
- Overvoltage fault activates when the filtered voltage exceeds the upper threshold.
- Fault remains active while the recovery threshold has not been reached.
- Fault clears after sufficient readings bring the filtered voltage to the recovery threshold.

**Observed Result:** The five-sample moving average initially retained the elevated voltage reading. After subsequent readings, the filtered voltage reached 4.1 V and the fault cleared.

**Status: PASS**

---

### Test Case 07 – Multiple Fault Detection

**Input:**
- Cell 1: 4.3 V
- Cell 2: 3.7 V
- Cell 3: 3.7 V
- Temperature: 65°C
- Charge Command: ON
- Discharge Command: OFF

**Expected Behaviour:**
- Multiple faults detected simultaneously.
- Charging and discharging disabled.
- Fault indicator activated.
- Combined fault code generated.

**Observed Result:** Overvoltage, overtemperature and cell imbalance were detected together. Combined fault code: 13.

**Status: PASS**

---

## 3. Testing Summary

| Test No. | Test Scenario | Result |
|---|---|---|
| 01 | Normal Operation | PASS |
| 02 | Overvoltage | PASS |
| 03 | Undervoltage | PASS |
| 04 | Overtemperature | PASS |
| 05 | Cell Imbalance | PASS |
| 06 | Hysteresis and Filtering | PASS |
| 07 | Multiple Fault Detection | PASS |

**Overall Result: 7/7 major functional scenarios passed.**

---

## 4. Conclusion

The functional tests demonstrate that the Mini BMS simulation can identify the selected battery fault conditions, estimate SOC, apply voltage filtering, maintain hysteresis behaviour, and generate appropriate control outputs.

Further testing is required for invalid inputs, command conflicts, fault-log persistence, and additional recovery conditions.

These results apply to the current PC-based software simulation and do not represent physical battery hardware validation.
