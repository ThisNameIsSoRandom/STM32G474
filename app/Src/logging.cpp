#include "logging.h"
#include "SEGGER_RTT.h"

void initLogging(void)
{
    // Configure SEGGER RTT
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    
    // Display startup banner
    SEGGER_RTT_printf(0, "mmmmm mmmmm mmmmm mmmmm mmmmm mmmmm \n\r");
    SEGGER_RTT_printf(0, "MM                               MM \n\r");
    SEGGER_RTT_printf(0, "MM     `7MM\"\"\"YMM   .M\"\"\"bgd     MM\tExisting solutions v 01.08.25\n\r");
    SEGGER_RTT_printf(0, "MM       MM    `7  ,MI    \"Y     MM \n\r");
    SEGGER_RTT_printf(0, "MM       MM   d    `MMb.         MM \n\r");
    SEGGER_RTT_printf(0, "MM       MMmmMM      `YMMNq.     MM \n\r");
    SEGGER_RTT_printf(0, "MM       MM   Y  , .     `MM     MM \n\r");
    SEGGER_RTT_printf(0, "MM       MM     ,M Mb     dM     MM \n\r");
    SEGGER_RTT_printf(0, "MM     .JMMmmmmMMM P\"Ybmmd\"      MM \n\r");
    SEGGER_RTT_printf(0, "MM                               MM \n\r");
    SEGGER_RTT_printf(0, "mmmmm mmmmm mmmmm mmmmm mmmmm mmmmm \n\r");
}