/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _ATCMD_DEFINES_MK3060_H_
#define _ATCMD_DEFINES_MK3060_H_

#include <hal/soc/soc.h>

/**
 * AT related platform-dependent things are here, including:
 *   1. AT command;
 *   2. AT response code;
 *   3. AT delimiter;
 *   4. AT event;
 *   5. Uart port used by AT;
 *   6. ...
 */

// AT command
#define AT_CMD_ENET_SEND "AT+ENETRAWSEND"
#define AT_CMD_ENTER_ENET_MODE "AT+ENETRAWMODE=ON"
#define AT_CMD_EHCO_OFF "AT+UARTE=OFF"
#define AT_CMD_TEST "AT"

// Delimiter
#define AT_RECV_DELIMITER "\r\n"
#define AT_SEND_DELIMITER "\r"

// AT event
#define AT_EVENT_ENET_DATA "+ENETEVENT:"

// AT uart
#define AT_UART_PORT 1

#endif
