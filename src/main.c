
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_drv_timer.h"
#include "app_error.h"


#include "pin_debuger.h"
#include "giikerLog.h"
#include "giikerConfig.h"




#include "nrf_delay.h"






int main(void) {

	nrf_delay_ms(10);

	DebugUartInit();

	print("");
	print("");
	print("");
	print("");
	print("");
	print("");
	print("hello world.");
	print("");
	print("");
	print("");



	spiSdInit();
	print("sdCardInit.ok");
	fileOp();


	for(;;) {
		__WFI();
	}
}

/** @} */
