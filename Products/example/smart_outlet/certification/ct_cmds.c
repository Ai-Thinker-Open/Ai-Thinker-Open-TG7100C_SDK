/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "iot_export.h"
#include "iot_import.h"
#include "aos/kv.h"

#include <k_api.h>

#include "ct_cmds.h"
#include "ct_ut.h"
#include "ct_simulate.h"

#ifdef CONFIG_AOS_CLI
static void handle_sop_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 3 && !strcmp("prop", argv[1]))
    {
        ct_simulate_post_property(argv[2]);
    }
    else if (argc == 4 && !strcmp("event", argv[1]))
    {
        ct_simulate_post_event(argv[2], argv[3]);
    }
    else if (argc == 6 && !strcmp("service", argv[1]))
    {
        //ct_simulate_service_response(int code, char*msg_id, char *service_id, char *service, char*ctx);
        ct_simulate_service_response(atoi(argv[2]), argv[3], argv[4], argv[5], NULL);
    }
    else
    {
        aos_cli_printf("Params Err\r\n");
    }

    return;
}

static struct cli_command sopcmd = {.name = "sop",
                                    .help = "sop prop payload|sop event eid epayload|sop service code msgid serviceid service ctx",
                                    .function = handle_sop_cmd};


int ct_register_cmds(void)
{
    aos_cli_register_command(&sopcmd);

    return 0;
}
#endif
