/*
 * io.h -- switch and button module interface
 *
 */
#pragma once

#include <stdio.h>			/* printf for errors */
#include <stdbool.h>
#include "xparameters.h"  	/* constants used by the hardware */
#include "xil_types.h"		/* types used by xilinx */
#include "led.h"
#include "gic.h"		/* interrupt controller interface */
#include "io.h"

#define CONFIGURE 0
#define PING 1
#define UPDATE 2


//init function
int wifi_init(void);
//close function
void wifi_close(void);
