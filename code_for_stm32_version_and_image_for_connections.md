// ============================================
// MINI BMS - STM32 NUCLEO-C031C6
// Compact firmware for Wokwi
// ============================================

// ANALOG INPUTS
const uint8_t cellPins[3] = {A0, A1, A2};
const uint8_t tempPin = A3;

// LED OUTPUTS
const uint8_t chargeLED = D8;
const uint8_t dischargeLED = D9;
const uint8_t faultLED = D10;

// CELL VOLTAGE THRESHOLDS (mV)
const uint16_t OV_LIMIT = 4200;
const uint16_t UV_LIMIT = 3000;

const uint16_t OV_RECOVERY = 4100;
const uint16_t UV_RECOVERY = 3100;

// OTHER THRESHOLDS
const int16_t TEMP_LIMIT = 600;       // 60.0 C
const int16_t TEMP_RECOVERY = 550;    // 55.0 C

const uint16_t IMBALANCE_LIMIT = 200;
const uint16_t IMBALANCE_RECOVERY = 150;

// FAULT CODES
#define FAULT_NONE       0
#define FAULT_OV         1
#define FAULT_UV         2
#define FAULT_TEMP       4
#define FAULT_IMBALANCE  8
#define FAULT_SENSOR     16

// MEASUREMENTS
uint16_t cellMv[3];

uint16_t maxVoltage;
uint16_t minVoltage;
uint16_t voltageDifference;

int16_t temperature10 = 0;

uint8_t soc = 0;
uint8_t faultCode = 0;

// FAULT MEMORY
bool ovActive[3] = {false, false, false};
bool uvActive[3] = {false, false, false};

bool tempActive = false;
bool imbalanceActive = false;

bool chargeAllowed = false;
bool dischargeAllowed = false;


// ============================================
// NTC LOOKUP TABLE
// Wokwi 10K NTC, Beta 3950
// Temperature stored in tenths of degree C
// ============================================

struct NtcPoint
{
    uint16_t adc;
    int16_t temp10;
};

const NtcPoint ntcTable[] =
{
    {600, -100},
    {939,    0},
    {1357, 100},
    {1817, 200},
    {2048, 250},
    {2270, 300},
    {2676, 400},
    {3014, 500},
    {3280, 600},
    {3482, 700},
    {3633, 800},
    {3745, 900},
    {3828, 1000},
    {3889, 1100},
    {3935, 1200}
};

const uint8_t NTC_COUNT =
    sizeof(ntcTable) / sizeof(ntcTable[0]);


// ============================================
// ADC AVERAGING
// ============================================

uint16_t readAverage(uint8_t pin)
{
    uint32_t total = 0;

    for (uint8_t i = 0; i < 5; i++)
    {
        total += analogRead(pin);
        delayMicroseconds(150);
    }

    return total / 5;
}


// ============================================
// CELL VOLTAGE MONITORING
// ============================================

void readCellVoltages()
{
    for (uint8_t i = 0; i < 3; i++)
    {
        uint16_t raw = readAverage(cellPins[i]);

        cellMv[i] =
            ((uint32_t)raw * 4200) / 4095;
    }
}


// ============================================
// TEMPERATURE MONITORING
// ============================================

bool readTemperature()
{
    uint16_t raw = readAverage(tempPin);

    if (raw < ntcTable[0].adc ||
        raw > ntcTable[NTC_COUNT - 1].adc)
    {
        return false;
    }

    for (uint8_t i = 0; i < NTC_COUNT - 1; i++)
    {
        uint16_t a0 = ntcTable[i].adc;
        uint16_t a1 = ntcTable[i + 1].adc;

        if (raw <= a1)
        {
            int16_t t0 = ntcTable[i].temp10;
            int16_t t1 = ntcTable[i + 1].temp10;

            temperature10 =
                t0 + ((int32_t)(raw - a0) *
                (t1 - t0)) / (a1 - a0);

            return true;
        }
    }

    temperature10 =
        ntcTable[NTC_COUNT - 1].temp10;

    return true;
}


// ============================================
// BATTERY CALCULATIONS
// ============================================

void calculateBattery()
{
    maxVoltage = cellMv[0];
    minVoltage = cellMv[0];

    for (uint8_t i = 1; i < 3; i++)
    {
        if (cellMv[i] > maxVoltage)
            maxVoltage = cellMv[i];

        if (cellMv[i] < minVoltage)
            minVoltage = cellMv[i];
    }

    voltageDifference =
        maxVoltage - minVoltage;

    // Rough SOC estimate using minimum cell voltage

    if (minVoltage <= 3000)
        soc = 0;

    else if (minVoltage >= 4200)
        soc = 100;

    else
        soc = ((minVoltage - 3000) * 100) / 1200;
}


// ============================================
// FAULT DETECTION WITH HYSTERESIS
// ============================================

void detectFaults(bool sensorValid)
{
    faultCode = FAULT_NONE;

    for (uint8_t i = 0; i < 3; i++)
    {
        // Overvoltage
        if (cellMv[i] > OV_LIMIT)
            ovActive[i] = true;

        else if (cellMv[i] <= OV_RECOVERY)
            ovActive[i] = false;

        // Undervoltage
        if (cellMv[i] < UV_LIMIT)
            uvActive[i] = true;

        else if (cellMv[i] >= UV_RECOVERY)
            uvActive[i] = false;

        if (ovActive[i])
            faultCode |= FAULT_OV;

        if (uvActive[i])
            faultCode |= FAULT_UV;
    }

    // Temperature
    if (sensorValid)
    {
        if (temperature10 > TEMP_LIMIT)
            tempActive = true;

        else if (temperature10 <= TEMP_RECOVERY)
            tempActive = false;
    }
    else
    {
        faultCode |= FAULT_SENSOR;
    }

    if (tempActive)
        faultCode |= FAULT_TEMP;

    // Cell imbalance
    if (voltageDifference > IMBALANCE_LIMIT)
        imbalanceActive = true;

    else if (voltageDifference <= IMBALANCE_RECOVERY)
        imbalanceActive = false;

    if (imbalanceActive)
        faultCode |= FAULT_IMBALANCE;
}


// ============================================
// CHARGE / DISCHARGE PERMISSION
// ============================================

void controlBattery()
{
    chargeAllowed =
        !(faultCode &
        (FAULT_OV | FAULT_TEMP |
         FAULT_IMBALANCE | FAULT_SENSOR));

    dischargeAllowed =
        !(faultCode &
        (FAULT_UV | FAULT_TEMP |
         FAULT_SENSOR));
}


// ============================================
// LED CONTROL
// ============================================

void updateLEDs()
{
    digitalWrite(chargeLED, chargeAllowed ? HIGH : LOW);

    digitalWrite(dischargeLED, dischargeAllowed ? HIGH : LOW);

    bool criticalFault =
        faultCode & (FAULT_OV |
                     FAULT_UV |
                     FAULT_TEMP |
                     FAULT_SENSOR);

    digitalWrite(faultLED,
                 criticalFault ? HIGH : LOW);
}


// ============================================
// SERIAL OUTPUT
// ============================================

void printVoltage(uint16_t mv)
{
    Serial.print(mv / 1000);
    Serial.print(".");
    
    uint16_t decimal = mv % 1000;

    if (decimal < 100) Serial.print("0");
    if (decimal < 10) Serial.print("0");

    Serial.print(decimal);
    Serial.println(" V");
}

void printTemperature()
{
    int16_t t = temperature10;

    if (t < 0)
    {
        Serial.print("-");
        t = -t;
    }

    Serial.print(t / 10);
    Serial.print(".");
    Serial.print(t % 10);
    Serial.println(" C");
}

void displayBMS()
{
    Serial.println();
    Serial.println("======= MINI BMS =======");

    for (uint8_t i = 0; i < 3; i++)
    {
        Serial.print("Cell ");
        Serial.print(i + 1);
        Serial.print(": ");

        printVoltage(cellMv[i]);
    }

    Serial.print("Temperature: ");

    if (faultCode & FAULT_SENSOR)
        Serial.println("INVALID");

    else
        printTemperature();

    Serial.print("Maximum: ");
    printVoltage(maxVoltage);

    Serial.print("Minimum: ");
    printVoltage(minVoltage);

    Serial.print("Difference: ");
    printVoltage(voltageDifference);

    Serial.print("Estimated SOC: ");
    Serial.print(soc);
    Serial.println("%");

    Serial.print("Fault Code: ");
    Serial.println(faultCode);

    Serial.print("Charge Permission: ");
    Serial.println(chargeAllowed ? "ON" : "OFF");

    Serial.print("Discharge Permission: ");
    Serial.println(dischargeAllowed ? "ON" : "OFF");

    if (faultCode &
        (FAULT_OV | FAULT_UV |
         FAULT_TEMP | FAULT_SENSOR))
    {
        Serial.println("Status: FAULT");
    }
    else if (faultCode & FAULT_IMBALANCE)
    {
        Serial.println("Status: WARNING");
    }
    else
    {
        Serial.println("Status: NORMAL");
    }

    Serial.println("========================");
}


// ============================================
// SETUP
// ============================================

void setup()
{
    Serial.begin(115200);

    analogReadResolution(12);

    pinMode(chargeLED, OUTPUT);
    pinMode(dischargeLED, OUTPUT);
    pinMode(faultLED, OUTPUT);

    digitalWrite(chargeLED, LOW);
    digitalWrite(dischargeLED, LOW);
    digitalWrite(faultLED, LOW);

    Serial.println("STM32 MINI BMS");
    Serial.println("NUCLEO-C031C6");
    Serial.println("System Initialized");
}


// ============================================
// MAIN LOOP
// ============================================

void loop()
{
    readCellVoltages();

    bool sensorValid = readTemperature();

    calculateBattery();

    detectFaults(sensorValid);

    controlBattery();

    updateLEDs();

    displayBMS();

    delay(1000);
}
<img width="791" height="595" alt="Screenshot 2026-11-13 120738" src="https://github.com/user-attachments/assets/645da882-0cb9-4266-af10-cfa405e61f76" />
