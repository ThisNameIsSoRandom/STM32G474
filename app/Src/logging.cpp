#include "logging.h"
#include "SEGGER_RTT.h"
#include <stdio.h>

#ifdef USE_UART_CONSOLE_LOGGING
#include <iostream>
#include <cstdio>
#endif

void initLogging(void)
{
#ifdef USE_SEGGER_RTT_LOGGING
    // Configure SEGGER RTT
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    
    // Display startup banner via SEGGER RTT
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
#elif defined(USE_UART_CONSOLE_LOGGING)
    // Display startup banner via UART console
    printf("mmmmm mmmmm mmmmm mmmmm mmmmm mmmmm \n\r");
    printf("MM                               MM \n\r");
    printf("MM     `7MM\"\"\"YMM   .M\"\"\"bgd     MM\tExisting solutions v 01.08.25\n\r");
    printf("MM       MM    `7  ,MI    \"Y     MM \n\r");
    printf("MM       MM   d    `MMb.         MM \n\r");
    printf("MM       MMmmMM      `YMMNq.     MM \n\r");
    printf("MM       MM   Y  , .     `MM     MM \n\r");
    printf("MM       MM     ,M Mb     dM     MM \n\r");
    printf("MM     .JMMmmmmMMM P\"Ybmmd\"      MM \n\r");
    printf("MM                               MM \n\r");
    printf("mmmmm mmmmm mmmmm mmmmm mmmmm mmmmm \n\r");
#endif
}