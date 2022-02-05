#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <stdbool.h>
#include "led.h"
#include "gic.h"
#include "io.h"
#include <unistd.h>

XUartPs uart;

bool done = false;

void btn_callback(u32 led_num){
	led_toggle(led_num);
}

void sw_callback(u32 led_num){
	led_toggle(led_num);
}

void uart_handler(void *CallBackRef, u32 Event, unsigned int EventData){

}

int main()
{

	init_platform();
	if (gic_init() != 0){
		// gic didn't initialize
		return 1;
	}else{
		led_init();
		io_btn_init(btn_callback);
		io_sw_init(sw_callback);
		gic_connect(XPAR_UARTPS_1_INTR);

		XUartPs_Config *config = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
		if (XUartPs_CfgInitialize(&uart, config, config->BaseAddress)!=0){
			// config didn't initialize
			return 2;
		}
		u32 baudRate = 9600;
		u8 trigger = 0;
		XUartPs_SetBaudRate(&uart, baudRate);
		XUartPs_SetFifoThreshold(&uart, trigger);
		XUartPs_SetInterruptMask(&uart, XPAR_XUARTPS_1_INTR);
		XUartPs_SetHandler(&uart, XUartPs_InterruptHandler, );


	}

	led_set(4, true);

    // do some initialiation here
	printf("Hello World!\n\r");
	while(!done)
		sleep(1);
	printf("[done]\n");
	sleep(1);
	// do some clean up here

	io_btn_close();
	io_sw_close();
	gic_close();

	cleanup_platform();
	return 0;
}
