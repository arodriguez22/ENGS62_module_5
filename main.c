#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <unistd.h>
#include <stdbool.h>
#include "xuartps.h"
#include "led.h"
#include "gic.h"		/* interrupt controller interface */
#include "io.h"
#include "xuartps_hw.h"
#include "wifi.h"

static bool done = false;



void getLine (char *str);
void callback(u32 led_num){
	led_toggle(led_num);
	if (led_num == CONFIGURE){
		printf("[CONFIGURE]\n\r");
	}

	if (led_num == PING){
		printf("[PING]\n\r");
	}

	if (led_num == UPDATE){
		printf("[UPDATE]\n\r");
	}

	if (led_num == 3){
		done = true;
	}
}


int main()
{
	/* do some initialization */
	init_platform();
	led_init();

	if (gic_init() == XST_SUCCESS){
		io_btn_init(callback);
		io_sw_init(callback);
		wifi_init();
	}

	printf("[hello]\n");
	while (!done){
		sleep(1);
	}
	printf("[done]\n");
	sleep(1);


	io_btn_close();
	io_sw_close();
	wifi_close();
	gic_close();
	cleanup_platform();
	return 0;
}

void getLine(char * str){
	char c;
	int i = 0;

	c = getchar();
	printf("%c", c);
	fflush(stdout);

	while ( c != 13 ){
		str[i] = c;
		i++;
		c = getchar();
		printf("%c", c);
		fflush(stdout);
	}
	str[i] = '\0';
}
