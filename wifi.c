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

static u8 long_buffer[200];
static int buffer_counter = 0;

typedef struct ping{
	int type; // assigned to ping
	int id; // assigned to our id on the class roaster
} ping_t;

typedef struct update_request{

	int type;
	int id;
	int value;

} update_request_t;

typedef struct update_response{
	int type;
	int id;
	int average;
	int values[30];
} update_response_t;


/*
 * press button 2
 * go into mode that reads characters
 * characters are packed into message
 * mode switches to update
 * update sends
 */


void send_ping(){

	ping_t p;
	p.type = PING;
	p.id = ID;

	XUartPs_Send(&uart0, (u8*) &p, sizeof(ping_t));
}

void send_update(int value){

	update_request_t u;
	u.type = UPDATE;
	u.id = ID;
	//u.values[30] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	u.value = value;
	XUartPs_Send(&uart0, (u8*) &u, sizeof(update_request_t));
	printf("sending update\n\r");
}

void set_state(int s){
	state = s;
}


// uart 0 handler funtion
static void uart0_handler(void *callBackRef, u32 event, unsigned int eventData){ // Wi-fly
	if(event == XUARTPS_EVENT_RECV_DATA){
		u8 buffer;
		u32 numBytes_requested = 1;


		XUartPs *uart0 = (XUartPs*)callBackRef;

		XUartPs_Recv(uart0, &buffer, numBytes_requested);

		print(":(\n\r");
		if (state == CONFIGURE){
			XUartPs_Send(&uart1, &buffer, numBytes_requested);
		}

		if (state == PING){
			XUartPs_Recv(uart0, &buffer, numBytes_requested);
			long_buffer[buffer_counter] = buffer;
			buffer_counter += 1;
			if (buffer_counter == 8){
				XUartPs_Send(&uart1, long_buffer, 8);
				buffer_counter = 0;
				ping_t *new_p = (ping_t*)long_buffer;
				printf("[PING id:%d, type: %d]\n\r",new_p->id, new_p->type);

			}
		}

		if (state == UPDATE){
			printf("recieving update \n\r");
			XUartPs_Recv(uart0, &buffer, numBytes_requested);
			long_buffer[buffer_counter] = buffer;
			buffer_counter += 1;
			printf("counter: %d\n\r", buffer_counter);
			if (buffer_counter >= sizeof(update_response_t)){
				//XUartPs_Send(&uart1, long_buffer, sizeof(update_response_t));
				buffer_counter = 0;
				update_response_t *new_u = (update_response_t*)long_buffer;
				printf("[update id:%d, average: %d]\n\r",new_u->id, new_u->average);

			}
		}


	}
}

// uart 1 handler function

static void uart1_handler(void *callBackRef, u32 event, unsigned int eventData){ // Terminal
	if(event == XUARTPS_EVENT_RECV_DATA){
		u8 buffer;
		u32 numBytes_requested = 1;
		XUartPs *uart1 = (XUartPs*)callBackRef;

		if (state == READ){
			printf("in read mode\n\r");
			XUartPs_Recv(uart1, &buffer, numBytes_requested);
			set_state(UPDATE);
			if (buffer != 13 && buffer != 2){
				send_update(buffer);
			}
		}


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
