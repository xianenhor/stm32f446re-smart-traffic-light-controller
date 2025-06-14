#include "main.h"
#include <stdbool.h>

void RunTrafficSequence(int i, int delay);
void ResetOtherCountsExcept(uint8_t currentIndex);  // helper function declaration

uint32_t u32LONGBLINKING = 3000;   // 3s
uint32_t u32SHORTBLINKING = 1500;  // 1.5s
uint8_t count [4] = {0,0,0,0};

uint16_t red_pins[] = {
    (1 << 0),   // PC0 - TL_Red_1
    (1 << 3),   // PC3 - TL_Red_2
    (1 << 6),   // PC6 - TL_Red_3
    (1 << 9)    // PC9 - TL_Red_4
};
uint16_t yellow_pins[] = {
    (1 << 1),   // PC1 - TL_Yellow_1
    (1 << 4),   // PC4 - TL_Yellow_2
    (1 << 7),   // PC7 - TL_Yellow_3
    (1 << 10)   // PC10 - TL_Yellow_4
};
uint16_t green_pins[] = {
    (1 << 2),   // PC2 - TL_Green_1
    (1 << 5),   // PC5 - TL_Green_2
    (1 << 8),   // PC8 - TL_Green_3
    (1 << 11)   // PC11 - TL_Green_4
};

uint8_t index = 0;  // TL index

int main(void)
{
    HAL_Init();

    /* Traffic Light (Port C)
     * TL_Red_1 = pin 0
     * TL_Red_2 = pin 3
     * TL_Red_3 = pin 6
     * TL_Red_4 = pin 9
     * TL_Yellow_1 = pin 1
     * TL_Yellow_2 = pin 4
     * TL_Yellow_3 = pin 7
     * TL_Yellow_4 = pin 10
     * TL_Green_1 = pin 2
     * TL_Green_2 = pin 5
     * TL_Green_3 = pin 8
     * TL_Green_4 = pin 11
     */

    /* IR Sensor (Port B)
     * sensor1_pin_1 = pin 0
     * sensor1_pin_2 = pin 1
     * sensor1_pin_3 = pin 2
     * sensor1_pin_4 = pin 3
     * sensor2_pin_1 = pin 4
     * sensor2_pin_2 = pin 5
     * sensor2_pin_3 = pin 6
     * sensor2_pin_4 = pin 7
     */

    RCC->AHB1ENR |= 0x06;
    GPIOB->MODER = 0;
    GPIOC->MODER |= 0x555555;

    while (1)
    {
        uint8_t ir_status = GPIOB->IDR & 0xFF;

        bool sensor1_active = ((ir_status & 0x0F) == 0);     // All sensor1 active
        bool sensor2_active = ((ir_status & 0xF0) == 0);     // All sensor2 active
        bool all_inactive   = (ir_status == 0xFF);           // None active (all high)

        bool sensor1_pin1   = ((ir_status & (1 << 0)) == 0); // sensor1_pin_1 detects car
        bool sensor2_pin1   = ((ir_status & (1 << 4)) == 0); // sensor2_pin_1 detects car

        bool sensor1_pin2   = ((ir_status & (1 << 1)) == 0); // sensor1_pin_2 detects car
        bool sensor2_pin2   = ((ir_status & (1 << 5)) == 0); // sensor2_pin_2 detects car

        bool sensor1_pin3   = ((ir_status & (1 << 2)) == 0); // sensor1_pin_3 detects car
        bool sensor2_pin3   = ((ir_status & (1 << 6)) == 0); // sensor2_pin_3 detects car

        bool sensor1_pin4   = ((ir_status & (1 << 3)) == 0); // sensor1_pin_4 detects car
        bool sensor2_pin4   = ((ir_status & (1 << 7)) == 0); // sensor2_pin_4 detects car

        bool any_sensor2_detect = (ir_status & 0xF0) != 0xF0; // At least one sensor2 detects car

        // Turn on all TL red light
        for (int j = 0; j < 4; j++) {
            GPIOC->ODR |= red_pins[j];
        }

        // Scenario 1: when no car at all junction and no sensor hit, green light on 1.5s, TL run by sequence
        if (all_inactive) {
            RunTrafficSequence(index, u32SHORTBLINKING);
        }

        // Scenario 2: when all car arrive at all of the junction and hit the sensor, run TL by sequence with the sensor hit
        // only first sensor hit - 15s
        // first sensor and second sensor hit - 30s
        else if (sensor2_pin1 && sensor2_pin2 && sensor2_pin3 && sensor2_pin4) {
            RunTrafficSequence(index, u32LONGBLINKING );
        }

        else if ( (count[0] < 3 ) && (count[1] < 3 ) && (count[2] < 3 ) && (count[3] < 3 ) && sensor1_active) {
            if (sensor2_pin1) index = 0;
            else if (sensor2_pin2) index = 1;
            else if (sensor2_pin3) index = 2;
            else if (sensor2_pin4) index = 3;
            RunTrafficSequence(index, any_sensor2_detect ? u32LONGBLINKING : u32SHORTBLINKING);
            count[index] +=1;
            ResetOtherCountsExcept(index);  // Reset other lanes' counts except the current active one
        }

        // Scenario 3: rewrite sequence if any car detected
        else if ((count[0] < 3 ) && sensor1_pin1) {
            index = 0;
            RunTrafficSequence(index, sensor2_pin1 ? u32LONGBLINKING : u32SHORTBLINKING);

            // Scenario 4: Each lane only gets to run 3 times max before the reserve is disabled
            count[index] +=1;
            ResetOtherCountsExcept(index);  // Reset other lanes' counts except the current active one
        }

        else if ( (count[1] < 3 ) && sensor1_pin2) {
            index = 1;
            RunTrafficSequence(index, sensor2_pin2 ? u32LONGBLINKING : u32SHORTBLINKING);
            count[index] +=1;
            ResetOtherCountsExcept(index);  // Reset other lanes' counts except the current active one
        }

        else if ( (count[2] < 3 ) && sensor1_pin3) {
            index = 2;
            RunTrafficSequence(index, sensor2_pin3 ? u32LONGBLINKING : u32SHORTBLINKING);
            count[index] +=1;
            ResetOtherCountsExcept(index);  // Reset other lanes' counts except the current active one
        }

        else if ( (count[3] < 3 ) && sensor1_pin4) {
            index = 3;
            RunTrafficSequence(index, sensor2_pin4 ? u32LONGBLINKING : u32SHORTBLINKING);
            count[index] +=1;
            ResetOtherCountsExcept(index);  // Reset other lanes' counts except the current active one
        }

        else {
            RunTrafficSequence(index, u32SHORTBLINKING);  // fallback/default
            ResetOtherCountsExcept(index);  // Reset other lanes' counts except the current active one
        }

        // Move to next in round-robin
        index = (index + 1) % 4;

        // Optional: delay for debounce or smooth loop timing
        HAL_Delay(100);
    }
}

void RunTrafficSequence(int i, int delay)
{
    // Red off
    HAL_Delay(500);
    GPIOC->ODR &= ~red_pins[i];

    // Green on
    GPIOC->ODR |= green_pins[i];
    HAL_Delay(delay);
    GPIOC->ODR &= ~green_pins[i];

    // Yellow on
    GPIOC->ODR |= yellow_pins[i];
    HAL_Delay(u32SHORTBLINKING);
    GPIOC->ODR &= ~yellow_pins[i];

    // Red on
    GPIOC->ODR |= red_pins[i];
}

// Function to reset all other traffic lane counts except the currently running one
void ResetOtherCountsExcept(uint8_t currentIndex)
{
    for (int j = 0; j < 4; j++) {
        if (j != currentIndex) count[j] = 0;
    }
}
