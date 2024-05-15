#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "usbserial.h"
#include "FreeRTOS.h"
#include "task.h"

static void usbTask(void* args) {
	usbserial_init();
	gpio_clear(GPIOC, GPIO13);
	for (;;) {
		usbserial_pool();

		if (command != '\0') {
			switch (command) {
				case '0':
					usbserial_writeline("turning led off");
					usbserial_writeline("write '1' to turn led on");
					gpio_set(GPIOC, GPIO13);
					break;

				case '1':
					usbserial_writeline("turning led on");
					usbserial_writeline("write '0' to turn led off");
					gpio_clear(GPIOC, GPIO13);
				break;

				case 't':
					usbserial_writeline("toggling led state");
					gpio_toggle(GPIOC, GPIO13);
				break;

				case 'g':
					if (1) {
						int port_a_status = gpio_get(GPIOA, 0xFF);
						int port_b_status = gpio_get(GPIOB, 0xFF);
						int port_c_status = gpio_get(GPIOC, GPIO13);
						usbserial_writeline("GPIO PORT STATUSES");
						usbserial_write("A: ");
						usbserial_write_int(port_a_status, 2);
						usbserial_writeline("");
						usbserial_write("B: ");
						usbserial_write_int(port_b_status, 2);
						usbserial_writeline("");
						usbserial_write("C: ");
						usbserial_write_int(port_c_status, 2);
						usbserial_writeline("");
					}
					break;

				default:
					
					usbserial_writeline("unknown command");
					break;
			}

			command = '\0';
		}
	}
}

/*
** Entry Point
*/
int main(void)
{
	// Blue Pill
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	// Initialize Builtin LED
	rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set(GPIOC, GPIO13); // turn led off at start

	xTaskCreate(usbTask, "usb", 200, NULL, configMAX_PRIORITIES - 1, NULL);

	// Start FreeRTOS
	vTaskStartScheduler();

	for (;;);

	return 0;
}

/*
** Stack Overflow Hook
*/
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName) {
	for (;;) {
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}