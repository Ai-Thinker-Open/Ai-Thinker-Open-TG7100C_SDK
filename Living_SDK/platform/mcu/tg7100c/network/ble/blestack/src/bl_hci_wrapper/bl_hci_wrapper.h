#ifndef __BL_HCI_WRAPPER_H__
#define __BL_HCI_WRAPPER_H__

#include "net/buf.h"
#include "bluetooth.h"

typedef enum {
  DATA_TYPE_COMMAND = 1,
  DATA_TYPE_ACL     = 2,
  DATA_TYPE_SCO     = 3,
  DATA_TYPE_EVENT   = 4
} serial_data_type_t;

uint8_t bl_onchiphci_interface_init(void);

int bl_onchiphci_send_2_controller(struct net_buf *buf);

#endif //__BL_CONTROLLER_H__