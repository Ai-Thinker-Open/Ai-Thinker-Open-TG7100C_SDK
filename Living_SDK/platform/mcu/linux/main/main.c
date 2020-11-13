/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <arg_options.h>

#ifndef CONFIG_VCALL_POSIX
#include <k_api.h>
#endif

#define TAG "main"

#ifdef TFS_EMULATE
extern int tfs_emulate_id2_index;
#endif

extern void krhino_lwip_init(int enable_tapif);
/* check in gcc sources gcc/gcov-io.h for the prototype */
extern void __gcov_flush(void);
extern void rl_free_line_state(void);
extern void rl_cleanup_after_signal(void);
extern void hw_start_hal(options_t *poptions);
extern void trace_start();
extern void netmgr_init(void);
extern int  aos_framework_init(void);
extern int  aos_cli_init(void);
extern void cpu_tmr_sync(void);

static options_t options = { 0 };
static kinit_t kinit = { 0 };

static void aos_features_init(void);
static void signal_handler(int signo);
static int  setrlimit_for_vfs(void);

static void exit_clean(void)
{
    char fn[64] = {0};
    snprintf(fn, sizeof(fn), "rm -f ./aos_partition_%d_*", getpid());
    system(fn);
}

static void app_entry(void *arg)
{
    int i = 0;

    kinit.argc        = options.argc;
    kinit.argv        = options.argv;
    kinit.cli_enable  = options.cli.enable;

#ifndef CONFIG_VCALL_POSIX
    cpu_tmr_sync();
#endif

    aos_features_init();

#ifdef CONFIG_AOS_MESHYTS
    options.flash.per_pid = true;
#else
    options.flash.per_pid = false;
#endif
    hw_start_hal(&options);

    aos_kernel_init(&kinit);
}

static void start_app(void)
{
#ifndef CONFIG_VCALL_POSIX
#if (RHINO_CONFIG_CPU_NUM > 1)
    ktask_t     *app_task;
    krhino_task_cpu_dyn_create(&app_task, "app_task", 0, 20, 0, 2048, app_entry, 0, 1);
#else
    aos_task_new("app", app_entry, NULL, 8192);
#endif
#else
    aos_task_new("app", app_entry, NULL, 8192);
#endif
}

int csp_get_args(const char ***pargv)
{
    *pargv = (const char **)options.argv;
    return options.argc;
}

void aos_features_init(void)
{
#ifdef CONFIG_NET_LWIP
    if (options.lwip.enable) {
        krhino_lwip_init(options.lwip.tapif);
    }
#endif
}

void signal_handler(int signo)
{
    LOGD(TAG, "Received signal %d\n", signo);

#ifdef ENABLE_GCOV
    __gcov_flush();
#endif

#ifdef CONFIG_AOS_DDA
    rl_free_line_state ();
    rl_cleanup_after_signal ();
#endif

    exit(0);
}

#define ARCH_MAX_NOFILE 64
int setrlimit_for_vfs(void)
{
    int           ret;
    struct rlimit rlmt;

    getrlimit(RLIMIT_NOFILE, &rlmt);
    if (rlmt.rlim_cur > ARCH_MAX_NOFILE) {
        rlmt.rlim_cur = ARCH_MAX_NOFILE;
        ret = setrlimit(RLIMIT_NOFILE, &rlmt);
        if (ret != 0) {
            LOGE(TAG, "setrlimit error: %s", strerror(errno));
            return ret;
        }
    }
    LOGD(TAG, "Limit max open files to %d", (int)rlmt.rlim_cur);

    return 0;
}

int main(int argc, char **argv)
{
    int ret;

    setvbuf(stdout, NULL, _IONBF, 0);

    options.argc        = argc;
    options.argv        = argv;
    options.lwip.enable = true;
#if defined(TAPIF_DEFAULT_OFF) || !defined(WITH_LWIP)
    options.lwip.tapif  = false;
#else
    options.lwip.tapif  = true;
#endif
    options.log_level   = AOS_LL_WARN;
    options.cli.enable  = true;

#if defined(CONFIG_AOS_DDA) || defined(ENABLE_GCOV)
    signal(SIGINT, signal_handler);
#endif
#ifdef CONFIG_AOS_UT
    signal(SIGPIPE, SIG_IGN);
#endif

    atexit(exit_clean);

    aos_init();

    ret = setrlimit_for_vfs();
    if (ret != 0) {
        return ret;
    }

    parse_options(&options);

    aos_set_log_level(options.log_level);

#ifdef TFS_EMULATE
    tfs_emulate_id2_index = options.id2_index;
#endif

    start_app();

    aos_start();

    return ret;
}

int board_cli_init(void)
{
    return 0;
}

