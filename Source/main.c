#include "bms_logic.h"
#include <stdio.h>
#include <time.h>

#define FAULT_LOG_FILE "fault_log.csv"

static int read_float(const char *prompt, float *value)
{
    printf("%s", prompt);
    return scanf("%f", value) == 1;
}

static int read_command(const char *prompt, uint8_t *value)
{
    unsigned int temp;
    printf("%s", prompt);
    if (scanf("%u", &temp) != 1 || temp > 1u) return 0;
    *value = (uint8_t)temp;
    return 1;
}

/* PC simulation of persistent fault logging. This is file storage, not MCU Flash. */
static void log_fault_event(uint8_t fault_code, const BMS_Input *input,
                            const BMS_Output *output)
{
    FILE *file = fopen(FAULT_LOG_FILE, "a");
    time_t now;
    struct tm *local_time;
    char timestamp[32] = "unknown-time";

    if (file == NULL) {
        printf("[LOG WARNING] Could not open %s for writing.\n", FAULT_LOG_FILE);
        return;
    }

    now = time(NULL);
    local_time = localtime(&now);
    if (local_time != NULL)
        (void)strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

    fprintf(file, "%s,%u,%.3f,%.3f,%.3f,%.2f,%.2f\n",
            timestamp, fault_code,
            output->filtered_cell_voltage[0],
            output->filtered_cell_voltage[1],
            output->filtered_cell_voltage[2],
            input->temperature, output->state_of_charge);
    fclose(file);
    printf("[FAULT LOG] New fault event saved to %s\n", FAULT_LOG_FILE);
}

int main(void)
{
    BMS_Input input;
    BMS_Output output;
    char again = 'y';
    uint8_t previous_fault_code = FAULT_NONE;

    BMS_Init();
    printf("========== MINI BMS SIMULATION ==========\n");
    printf("Educational PC simulation | 3-cell series pack\n");
    printf("Fault log: %s\n", FAULT_LOG_FILE);

    while (again == 'y' || again == 'Y') {
        unsigned int i;

        printf("\nEnter cell voltages (V):\n");
        for (i = 0; i < NUM_CELLS; ++i) {
            char prompt[40];
            (void)snprintf(prompt, sizeof(prompt), "Cell %u voltage: ", i + 1u);
            if (!read_float(prompt, &input.cell_voltage[i])) {
                printf("Invalid numeric input. Restart the program and try again.\n");
                return 1;
            }
        }

        if (!read_float("Battery temperature (C): ", &input.temperature) ||
            !read_command("Charging command (1=ON, 0=OFF): ", &input.charge_command) ||
            !read_command("Discharging command (1=ON, 0=OFF): ", &input.discharge_command)) {
            printf("Invalid input. Use numbers; commands must be 0 or 1.\n");
            return 1;
        }

        BMS_Process(&input, &output);

        printf("\n--------------- BMS REPORT ---------------\n");
        for (i = 0; i < NUM_CELLS; ++i)
            printf("Cell %u: raw %.3f V | filtered %.3f V\n", i + 1u,
                   input.cell_voltage[i], output.filtered_cell_voltage[i]);
        printf("Temperature       : %.2f C\n", input.temperature);
        printf("Maximum cell      : %.3f V\n", output.maximum_voltage);
        printf("Minimum cell      : %.3f V\n", output.minimum_voltage);
        printf("Cell difference   : %.3f V\n", output.voltage_difference);
        printf("Estimated SOC     : %.1f%% (rough voltage estimate)\n", output.state_of_charge);
        printf("Battery status    : %s\n",
               output.battery_status == BMS_STATUS_NORMAL ? "NORMAL" :
               output.battery_status == BMS_STATUS_WARNING ? "WARNING" : "FAULT");
        printf("Operating state   : %s\n", BMS_GetStateName(output.operating_state));
        printf("Charge enable     : %s\n", output.charge_enable ? "ON" : "OFF");
        printf("Discharge enable  : %s\n", output.discharge_enable ? "ON" : "OFF");
        printf("Fault indicator   : %s\n", output.fault_indicator ? "ON" : "OFF");
        printf("Fault code        : %u\n", output.fault_code);
        printf("Fault description : %s\n", BMS_GetFaultDescription(output.fault_code));
        printf("------------------------------------------\n");

        /* Terminal output represents UART diagnostics in this PC-only prototype.
           Real UART requires a selected MCU and its UART peripheral. */
        printf("[UART SIMULATION] SOC=%.1f%% State=%s FaultCode=%u\n",
               output.state_of_charge, BMS_GetStateName(output.operating_state),
               output.fault_code);

        if (output.fault_code != FAULT_NONE && output.fault_code != previous_fault_code)
            log_fault_event(output.fault_code, &input, &output);
        previous_fault_code = output.fault_code;

        printf("\nEnter another reading? (y/n): ");
        if (scanf(" %c", &again) != 1) break;
    }

    printf("\nBMS simulation ended.\n");
    return 0;
}
