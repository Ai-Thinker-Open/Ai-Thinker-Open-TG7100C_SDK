/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef YTS_H
#define YTS_H

void osupdate_online_test_run(char *bin, int id2);

#define check_cond_wait(cond, seconds) do { \
    unsigned int i; \
    for (i=0;i<(unsigned int)seconds && !(cond);i++) { \
        aos_msleep(1000); \
    } \
    YUNIT_ASSERT(cond); \
} while(0);

#define run_times(func, times) do { \
    int i; \
    for (i=0;i<times;i++, func); \
} while(0);

extern int yts_get_args(const char ***argv);
extern void yts_run(int argc, char **argv);
extern int yts_run_suite_by_name(const char *suite_name, int test_case_times);
extern void ct_yts_run_all(void);

#endif /* YTS_H */

