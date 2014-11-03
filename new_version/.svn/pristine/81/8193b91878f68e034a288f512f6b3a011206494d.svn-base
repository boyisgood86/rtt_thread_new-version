#ifndef __YM_DEV_LIST_H_
#define __YM_DEV_LIST_H_

#include "Interface.h"
#include "ym_json.h"

#define MAX_NUM     (20)
#define CTR_WRITE    (0x01)
#define CTR_READ     (0x02)
#define CTR_FREE     (0x00)

typedef struct ym_device_list ym_dev_list;
/*device list*/
struct ym_device_list
{
    ym_dev_list *next;
    unsigned char type;     /*device type*/
    unsigned char flag;     /*node can use or not: 1 -> can't use, 0: can usr*/
    Send_Recv_CMDA_t data;   /*data*/
};

ym_dev_list * device_list_create(void);
ym_dev_list * device_list_init(ym_dev_list * pHead, int amount);
void device_node_write(const ym_dev_list *data);
void device_node_change(Send_Recv_CMDA_t * msg);
ym_dev_ctrl_t device_list_read(void);


#endif /*__YM_DEV_LIST_H_*/
