#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define RCCLEDPORT (RCC_GPIOC)
#define LEDPORT (GPIOC)
#define LEDPIN (GPIO13)
#define GPIO_MODE_OUTPUT GPIO_MODE_OUTPUT_2_MHZ
#define GPIO_PUPD_NONE GPIO_CNF_OUTPUT_PUSHPULL

// Called when stack overflow!
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) { 
	for (;;);
}

// LED blinker Task
static void blinkLedTask(void *args) {
    for (;;) {
		gpio_toggle(LEDPORT, LEDPIN);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

int main(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	rcc_periph_clock_enable(RCCLEDPORT);
    gpio_set_mode(LEDPORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LEDPIN);
	gpio_set(LEDPORT, LEDPIN); // turn led off at start

	xTaskCreate(blinkLedTask, "blinkLed", 100, NULL, 2, NULL);

	vTaskStartScheduler();

	for (;;);

	return 0;
}
