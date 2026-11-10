#include "bms_logic.h"
#include <stddef.h>
#include <math.h>

/* Persistent software state between calls to BMS_Process(). */
static float voltage_history[NUM_CELLS][FILTER_WINDOW];
static uint8_t filter_count;
static uint8_t filter_index;
static uint8_t cell_ov_active[NUM_CELLS];
static uint8_t cell_uv_active[NUM_CELLS];
static uint8_t temperature_active;
static uint8_t imbalance_active;

static float moving_average(const float samples[FILTER_WINDOW])
{
    float sum = 0.0f;
    uint8_t i;
    for (i = 0; i < FILTER_WINDOW; ++i) sum += samples[i];
    return sum / (float)FILTER_WINDOW;
}

void BMS_Init(void)
{
    uint8_t i, j;
    filter_count = 0;
    filter_index = 0;
    temperature_active = 0;
    imbalance_active = 0;

    for (i = 0; i < NUM_CELLS; ++i) {
        cell_ov_active[i] = 0;
        cell_uv_active[i] = 0;
        for (j = 0; j < FILTER_WINDOW; ++j) voltage_history[i][j] = 0.0f;
    }
}

static void set_safe_fault_output(BMS_Output *output)
{
    output->charge_enable = 0;
    output->discharge_enable = 0;
    output->fault_indicator = 1;
    output->battery_status = BMS_STATUS_FAULT;
    output->operating_state = BMS_STATE_FAULT;
    output->fault_code = FAULT_INVALID_INPUT;
    output->state_of_charge = 0.0f;
    output->maximum_voltage = 0.0f;
    output->minimum_voltage = 0.0f;
    output->voltage_difference = 0.0f;
    {
        uint8_t i;
        for (i = 0; i < NUM_CELLS; ++i) output->filtered_cell_voltage[i] = 0.0f;
    }
}

void BMS_Process(const BMS_Input *input, BMS_Output *output)
{
    uint8_t i;
    float minimum = 1000.0f;
    float maximum = -1000.0f;
    uint8_t faults = FAULT_NONE;
    uint8_t critical_faults;

    if (output == NULL) return;

    output->charge_enable = 0;
    output->discharge_enable = 0;
    output->fault_indicator = 0;
    output->battery_status = BMS_STATUS_NORMAL;
    output->operating_state = BMS_STATE_IDLE;
    output->fault_code = FAULT_NONE;
    output->state_of_charge = 0.0f;
    output->maximum_voltage = 0.0f;
    output->minimum_voltage = 0.0f;
    output->voltage_difference = 0.0f;

    if (input == NULL || input->charge_command > 1 || input->discharge_command > 1 ||
        (input->charge_command == 1 && input->discharge_command == 1) ||
        !isfinite(input->temperature) || input->temperature < -40.0f || input->temperature > 125.0f) {
        set_safe_fault_output(output);
        return;
    }

    for (i = 0; i < NUM_CELLS; ++i) {
        if (!isfinite(input->cell_voltage[i]) || input->cell_voltage[i] < 0.0f || input->cell_voltage[i] > 5.0f) {
            set_safe_fault_output(output);
            return;
        }
    }

    /* Seed all five filter slots with the first valid sample to avoid startup bias. */
    if (filter_count == 0) {
        for (i = 0; i < NUM_CELLS; ++i) {
            uint8_t j;
            for (j = 0; j < FILTER_WINDOW; ++j)
                voltage_history[i][j] = input->cell_voltage[i];
        }
        filter_count = 1;
    } else {
        for (i = 0; i < NUM_CELLS; ++i)
            voltage_history[i][filter_index] = input->cell_voltage[i];
        if (filter_count < FILTER_WINDOW) ++filter_count;
        filter_index = (uint8_t)((filter_index + 1u) % FILTER_WINDOW);
    }

    for (i = 0; i < NUM_CELLS; ++i) {
        float v = moving_average(voltage_history[i]);
        output->filtered_cell_voltage[i] = v;
        if (v > maximum) maximum = v;
        if (v < minimum) minimum = v;

        if (v > CELL_OVERVOLTAGE_LIMIT) cell_ov_active[i] = 1;
        else if (v <= CELL_OV_RECOVERY_LIMIT) cell_ov_active[i] = 0;

        if (v < CELL_UNDERVOLTAGE_LIMIT) cell_uv_active[i] = 1;
        else if (v >= CELL_UV_RECOVERY_LIMIT) cell_uv_active[i] = 0;

        if (cell_ov_active[i]) faults |= FAULT_OVERVOLTAGE;
        if (cell_uv_active[i]) faults |= FAULT_UNDERVOLTAGE;
    }

    output->maximum_voltage = maximum;
    output->minimum_voltage = minimum;
    output->voltage_difference = maximum - minimum;

    if (input->temperature > BATTERY_OVERTEMP_LIMIT) temperature_active = 1;
    else if (input->temperature <= BATTERY_TEMP_RECOVERY_LIMIT) temperature_active = 0;
    if (temperature_active) faults |= FAULT_OVERTEMPERATURE;

    if (output->voltage_difference > CELL_IMBALANCE_LIMIT) imbalance_active = 1;
    else if (output->voltage_difference <= CELL_IMBALANCE_RECOVERY_LIMIT) imbalance_active = 0;
    if (imbalance_active) faults |= FAULT_CELL_IMBALANCE;

    /* Conservative educational estimate: use the weakest cell's voltage estimate.
       This linear mapping is not accurate enough for a real battery pack. */
    {
        float soc = ((minimum - CELL_UNDERVOLTAGE_LIMIT) /
                    (CELL_OVERVOLTAGE_LIMIT - CELL_UNDERVOLTAGE_LIMIT)) * 100.0f;
        if (soc < 0.0f) soc = 0.0f;
        if (soc > 100.0f) soc = 100.0f;
        output->state_of_charge = soc;
    }

    output->fault_code = faults;
    output->fault_indicator = (faults != FAULT_NONE) ? 1u : 0u;

    critical_faults = (uint8_t)(FAULT_OVERVOLTAGE | FAULT_UNDERVOLTAGE |
                                FAULT_OVERTEMPERATURE | FAULT_INVALID_INPUT);
    if ((faults & critical_faults) != 0u) {
        output->battery_status = BMS_STATUS_FAULT;
        output->operating_state = BMS_STATE_FAULT;
    } else if ((faults & FAULT_CELL_IMBALANCE) != 0u) {
        output->battery_status = BMS_STATUS_WARNING;
        output->operating_state = BMS_STATE_WARNING;
    } else if (input->charge_command) {
        output->operating_state = BMS_STATE_CHARGING;
    } else if (input->discharge_command) {
        output->operating_state = BMS_STATE_DISCHARGING;
    }

    /* Protection rules: OV/imbalance/overtemperature stop charging;
       UV/overtemperature stop discharging. Critical invalid data stops both. */
    if (input->charge_command && !input->discharge_command &&
        !(faults & (FAULT_OVERVOLTAGE | FAULT_OVERTEMPERATURE |
                    FAULT_CELL_IMBALANCE | FAULT_INVALID_INPUT)))
        output->charge_enable = 1;

    if (input->discharge_command && !input->charge_command &&
        !(faults & (FAULT_UNDERVOLTAGE | FAULT_OVERTEMPERATURE |
                    FAULT_INVALID_INPUT)))
        output->discharge_enable = 1;
}

const char *BMS_GetFaultDescription(uint8_t fault_code)
{
    if (fault_code == FAULT_NONE) return "No Fault";
    if ((fault_code & (uint8_t)~(FAULT_OVERVOLTAGE | FAULT_UNDERVOLTAGE |
        FAULT_OVERTEMPERATURE | FAULT_CELL_IMBALANCE | FAULT_INVALID_INPUT)) != 0u)
        return "Unknown Fault Code";
    if ((fault_code & (FAULT_OVERVOLTAGE | FAULT_UNDERVOLTAGE |
        FAULT_OVERTEMPERATURE | FAULT_INVALID_INPUT)) != 0u &&
        (fault_code & (fault_code - 1u)) != 0u)
        return "Multiple Faults Detected";
    switch (fault_code) {
        case FAULT_OVERVOLTAGE: return "Cell Overvoltage";
        case FAULT_UNDERVOLTAGE: return "Cell Undervoltage";
        case FAULT_OVERTEMPERATURE: return "Battery Overtemperature";
        case FAULT_CELL_IMBALANCE: return "Cell Imbalance";
        case FAULT_INVALID_INPUT: return "Invalid Input";
        default: return "Multiple Faults Detected";
    }
}

const char *BMS_GetStateName(BMS_State state)
{
    switch (state) {
        case BMS_STATE_IDLE: return "IDLE";
        case BMS_STATE_CHARGING: return "CHARGING";
        case BMS_STATE_DISCHARGING: return "DISCHARGING";
        case BMS_STATE_WARNING: return "WARNING";
        case BMS_STATE_FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}
