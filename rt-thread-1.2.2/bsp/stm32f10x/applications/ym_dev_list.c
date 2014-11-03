#include <rtthread.h>
#include "ym_dev_list.h"

extern ym_dev_list *ym_head;

/*
@function: create list head
*/
ym_dev_list * device_list_create(void)
{
 ym_dev_list *head = (ym_dev_list*)rt_malloc(sizeof(ym_dev_list));
 if(!head) {
    MY_DEBUG("%s,%d: create device list faild !\n",__func__,__LINE__);
    goto EXIT;
 }
 rt_memset(head, 0, sizeof(ym_dev_list));
 
 return head;
EXIT:
  return RT_NULL;
}

/*
@function : create  devices node in an list
@param: device amount
@return : devilce list or RT_NULL
*/
ym_dev_list * device_list_init(ym_dev_list * pHead, int amount)
{
    ym_dev_list *head = pHead;
    
    ym_dev_list *tail = pHead;
    ym_dev_list *new_node = RT_NULL;
    
    for(int i = 0; i < amount; i++) {
        new_node = (ym_dev_list*)rt_malloc(sizeof(ym_dev_list));
        if(!new_node) {
          MY_DEBUG("%s,%d: malloc new node faild !\n",__func__,__LINE__);
          goto EXIT;
        }
        rt_memset(new_node, 0, sizeof(ym_dev_list));
        tail->next = new_node;
        new_node->next = RT_NULL;
        tail = new_node;
    }
    
    return head;
EXIT:
  return RT_NULL;
}

/*
@function: save msg of send , to device list, include : mac, short address,and others
@param: msg data<ym_dev_list>
*/
void device_node_write(const ym_dev_list *data)
{
    ym_dev_list * temp = (ym_dev_list*)data;
    ym_dev_list *p = ym_head->next;
    
    while(p != RT_NULL) {
      if(!p->flag) {
         p->flag = CTR_WRITE;
         p->data.SendNode.NwkDest = temp->data.SendNode.NwkDest;
         memcpy(p->data.SendNode.Info_mac.mac, temp->data.SendNode.Info_mac.mac,8);
         p->data.CMD = temp->data.CMD;
         p->data.ClusterID = temp->data.ClusterID;
         break;
      }
      p = p->next;
    }
    
    return ;
}

/*
@function : read an device from list
@return : mac \ device progess.
*/

ym_dev_ctrl_t device_list_read(void)
{
  ym_dev_ctrl_t dev;
  rt_memset(&dev, 0, sizeof(dev));
  ym_dev_list *p = ym_head->next;
    while(RT_NULL != p) {
      if(p->flag == CTR_READ) {
         //mac
        memcpy(dev.device_mac, p->data.SendNode.Info_mac.mac, 8);
        
        //device statu
        dev.devcice_progress = p->data.SendNode.Info_mac.U_info.info.LState.CtrlVal;
        
        dev.flag = 1;
        /*other msg todo...*/
        p->flag = CTR_FREE;
        rt_memset(&(p->data), 0, sizeof(p->data));
        break ;
      }
      p = p->next;
    }
    return dev;
}

/*
@function: recive uart msg , and put into headlist;
@param: msg   --> 12 bytes
*/
void device_node_change(Send_Recv_CMDA_t * msg)
{
   ym_dev_list *p = ym_head->next;
   while(RT_NULL != p) {
       if(p->data.CMD == msg->CMD && 
          p->data.ClusterID == msg->ClusterID && 
          p->data.SendNode.NwkDest == msg->SendNode.NwkDest &&
            p->flag == CTR_WRITE)
       {
            p->data.SendNode.Info_mac.U_info.info.LState = msg->SendNode.Info_mac.U_info.info.LState;
            p->flag = CTR_READ;
            
            break ;
       }
       p = p->next;
   }
   return ;
}


