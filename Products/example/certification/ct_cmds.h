#ifndef __CT_CMDS_H__
#define __CT_CMDS_H__

#define MAX_DEVICES_META_NUM (0)

extern int ct_register_cmds(void);
extern void ct_start_netmgr(void *p);
extern void ct_open_dev_ap(void *p);
extern void ct_awss_reset(void);
extern void ct_do_awss_active(void);

#endif
