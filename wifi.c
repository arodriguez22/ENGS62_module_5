/*
 * io.h -- switch and button module interface
 *
 */

#include <stdio.h>			/* printf for errors */
#include <stdbool.h>
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */
#include "led.h"
#include "gic.h"		/* interrupt controller interface */
#include "io.h"
#include "wifi.h"

static XUartPs uart1;
static XUartPs uart0;

static int state;


void set_state(int state){
	if (state == CONFIGURE){

	}else if (PING){

	}else if(UPDATE){

	}
}




// uart 0 handler funtion
static void uart0_handler(void *callBackRef, u32 event, unsigned int eventData){ // Wi-fly
	if(event == XUARTPS_EVENT_RECV_DATA){
		u8 buffer;
		u32 numBytes_requested = 1;

		XUartPs *uart0 = (XUartPs*)callBackRef;

		XUartPs_Recv(uart0, &buffer, numBytes_requested);

		if (state == CONFIGURE){
			XUartPs_Send(&uart1, &buffer, numBytes_requested);
		}

	}
}

// uart 1 handler function

static void uart1_handler(void *callBackRef, u32 event, unsigned int eventData){ // Terminal
	if(event == XUARTPS_EVENT_RECV_DATA){
		u8 buffer;
		u32 numBytes_requested = 1;


		XUartPs *uart1 = (XUartPs*)callBackRef;

		// information recieved by UART 1
		XUartPs_Recv(uart1, &buffer, numBytes_requested);
		if (state == CONFIGURE){
			XUartPs_Send(&uart0, &buffer, numBytes_requested);
			if (buffer == (u8) '\r'){
				buffer = (u8) '\n';
				XUartPs_Send(uart1, &buffer, numBytes_requested);
			}
		}
	}
}

//init function
int wifi_init(void){

			XUartPs_Config * ConfigPtr0;
			ConfigPtr0 = XUartPs_LookupConfig(XPAR_PS7_UART_0_DEVICE_ID);
			if (XUartPs_CfgInitialize(&uart0,  ConfigPtr0, ConfigPtr0->BaseAddress) != XST_SUCCESS){
				return 3;
				printf("init of uart 0 failed\n\r");
			}else{
				u8 triggerLevel0 = 1;
				XUartPs_SetFifoThreshold(&uart0, triggerLevel0);
				XUartPs_SetBaudRate(&uart0, 9600);

				XUartPs_SetInterruptMask(&uart0, XUARTPS_IXR_RXOVR);
				XUartPs_SetHandler(&uart0, (XUartPs_Handler) uart0_handler, &uart0);
				gic_connect(XPAR_XUARTPS_0_INTR, (Xil_InterruptHandler) XUartPs_InterruptHandler, &uart0);
			}


			XUartPs_Config * ConfigPtr1;
			ConfigPtr1 = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
			if (XUartPs_CfgInitialize(&uart1,  ConfigPtr1, ConfigPtr1->BaseAddress) != XST_SUCCESS){
				return 4;
				printf("init of uart 1 failed\n\r");

			}else{
				u8 triggerLevel1 = 1;
				XUartPs_SetFifoThreshold(&uart1, triggerLevel1);


				XUartPs_SetInterruptMask(&uart1, XUARTPS_IXR_RXOVR);
				XUartPs_SetHandler(&uart1, (XUartPs_Handler) uart1_handler, &uart1);
				gic_connect(XPAR_XUARTPS_1_INTR, (Xil_InterruptHandler) XUartPs_InterruptHandler, &uart1);
			}
			return 0;
		}
//close function
void wifi_close(void){

	XUartPs_DisableUart(&uart0);
	XUartPs_DisableUart(&uart1);
	gic_disconnect(XPAR_XUARTPS_0_INTR);
	gic_disconnect(XPAR_XUARTPS_1_INTR);

}
