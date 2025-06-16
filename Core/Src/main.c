#include "main.h"
#include <stdbool.h>

void RunTrafficSequence(int i, int delay);

uint32_t u32LONGBLINKING = 3000;   // 3s
uint32_t u32SHORTBLINKING = 1500;  // 1.5s

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

    RCC->AHB1ENR |= 0x06; // Enable the clock of Port B and C
    GPIOB->MODER = 0; // Set Port B as input
    GPIOC->MODER |= 0x555555;  // Set Port C as output

    while (1)
    {
        uint8_t ir_status = GPIOB->IDR & 0xFF;


        bool all_inactive = (ir_status == 0xFF); // None active (all high)

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

		// Scenario 3: overwrite the TL sequence if any car detected; only first sensor hit - 15s first sensor and second sensor hit - 30s

		else {
			bool handled = false;

			// Highest priority: sensor2 hit
			for (int j = 0; j < 4; j++) {
				int z = (j + index) % 4;
				bool sensor2 = ((ir_status & (1 << (z + 4))) == 0); // PB4–PB7
				if (sensor2) {
					index = z;
					RunTrafficSequence(index, u32LONGBLINKING);
					handled = true;
					break;
				}
			}

			if (!handled) {
				// Next priority: sensor1 hit
				for (int j = 0; j < 4; j++) {
					int z = (j + index) % 4;
					bool sensor1 = ((ir_status & (1 << z)) == 0); // PB0–PB3
					if (sensor1) {
						index = z;
						RunTrafficSequence(index, u32SHORTBLINKING);
						handled = true;
						break;
					}
				}
			}

			if (!handled) {
				// Default fallback: all inactive or unrecognized
				RunTrafficSequence(index, u32SHORTBLINKING);
			}
		}
		index = (index + 1) % 4;
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
