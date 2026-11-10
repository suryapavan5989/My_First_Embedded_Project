#ifndef BMS_LOGIC_H
#define BMS_LOGIC_H

#include <stdint.h>

#define NUM_CELLS 3
#define FILTER_WINDOW 5

/* Educational simulation limits for a conventional Li-ion cell.
   Replace these with the selected cell manufacturer's specifications
   before connecting real hardware. */
#define CELL_OVERVOLTAGE_LIMIT       4.20f
#define CELL_UNDERVOLTAGE_LIMIT      3.00f
#define BATTERY_OVERTEMP_LIMIT      60.0f
#define CELL_IMBALANCE_LIMIT         0.20f

#define CELL_OV_RECOVERY_LIMIT       4.10f
#define CELL_UV_RECOVERY_LIMIT       3.10f
#define BATTERY_TEMP_RECOVERY_LIMIT 55.0f
#define CELL_IMBALANCE_RECOVERY_LIMIT 0.15f

typedef enum {
    BMS_STATUS_NORMAL = 0,
    BMS_STATUS_WARNING,
    BMS_STATUS_FAULT
} BMS_Status;

typedef enum {
    BMS_STATE_IDLE = 0,
    BMS_STATE_CHARGING,
    BMS_STATE_DISCHARGING,
    BMS_STATE_WARNING,
    BMS_STATE_FAULT
} BMS_State;

typedef enum {
    FAULT_NONE            = 0,
    FAULT_OVERVOLTAGE     = 1u << 0,
    FAULT_UNDERVOLTAGE    = 1u << 1,
    FAULT_OVERTEMPERATURE = 1u << 2,
    FAULT_CELL_IMBALANCE  = 1u << 3,
    FAULT_INVALID_INPUT   = 1u << 4
} BMS_Fault;

typedef struct {
    float cell_voltage[NUM_CELLS];
    float temperature;
    uint8_t charge_command;     /* 0 or 1 */
    uint8_t discharge_command;  /* 0 or 1 */
} BMS_Input;

typedef struct {
    float filtered_cell_voltage[NUM_CELLS];
    float maximum_voltage;
    float minimum_voltage;
    float voltage_difference;
    float state_of_charge;      /* rough voltage-based estimate */
    uint8_t charge_enable;
    uint8_t discharge_enable;
    uint8_t fault_indicator;
    BMS_Status battery_status;
    BMS_State operating_state;
    uint8_t fault_code;         /* bitmask; multiple faults can be combined */
} BMS_Output;

void BMS_Init(void);
void BMS_Process(const BMS_Input *input, BMS_Output *output);
const char *BMS_GetFaultDescription(uint8_t fault_code);
const char *BMS_GetStateName(BMS_State state);

#endif
