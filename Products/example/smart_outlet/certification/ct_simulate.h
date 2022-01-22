#ifndef __CT_SIMULATE_H__
#define __CT_SIMULATE_H__

int32_t ct_simulate_post_property(char *prop);
int32_t ct_simulate_post_event(char *event_id, char *event_payload);
void ct_simulate_deviceinfo_update(char *device_info_update);
void ct_simulate_deviceinfo_delete(char *device_info_delete);
void ct_simulate_service_response(int code, char*msg_id, char *service_id, char *service, char*ctx);

#endif
