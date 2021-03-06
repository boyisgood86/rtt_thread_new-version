#include <rtthread.h>
#include "cJSON.h"
#include "ym_config.h"
#include "ym_method.h"
#include "ym_user.h"
#include "ym_device.h"



/* get method from js string */
int get_method(char * const string)
{
   cJSON * js_root,*js_head,*js;
    int method;
    if(!string) {
      MY_DEBUG("%s,%d: string is NULL..\n\r",__func__,__LINE__);
      goto EXIT;
    }
        
    /* create root */
    js_root = cJSON_Parse(string);
    if(!js_root) {
      MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* get head */
    js_head = cJSON_GetObjectItem(js_root, "head");
    if(!js_head) {
      MY_DEBUG("%s,%d: The string no have \"head\" key word !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* get method */
    js = cJSON_GetObjectItem(js_head, "method");
    if(!js) {
      MY_DEBUG("%s,%d: The string no have \"method\" key word !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    if(js->type == cJSON_Number) {
       method = js->valueint;
    }else {
      MY_DEBUG("%s,%d: method value type is error !\n\r",__func__,__LINE__);
      goto EXIT;
    } 
    
    if(js_root) 
      cJSON_Delete(js_root);
    return method;
EXIT:
    if(js_root) 
      cJSON_Delete(js_root);  
    return -1;
}



char * ym_deal_data_handle(char * const string)
{
    int method = get_method(string);
    
    MY_DEBUG("\n\n%s,%d: \n%s \n\n", __func__,__LINE__, string);
    
    switch(method) {
    /*user login*/
        case YM_USER_LOGIN_METHOD:         
          return ym_user_login(string);
          
    /*add user accout or sign user*/
        case YM_USER_SIGN_METHOD:       
          return ym_user_sign(string);   
          
    /*enable or disable WhiteList/user_List */     
        case YM_USER_WHITELIST_METHOD: 
         return ym_user_whitelist(string); 
         
    /* change passwrod */      
        case YM_USER_NEW_PASSWD_METHOD:     
          return ym_user_new_passwd(string);
          
    /*delete user accout*/      
        case YM_USER_DELETE_METHOD:       
          return ym_user_delete(string);
    /* read device status from devic status file */  
          
        case YM_DEVICE_READ_METHOD:     
          return ym_device_read_status(string);
//          return RT_NULL;
          
    /* add device to device list file */      
        case YM_DEVICE_ADD_METHOD:    
          return ym_device_add(string);  
          
    /* delete device from device list file */      
        case YM_DEVICE_DELETE_METHOD:     
          return ym_device_delete(string); 
          
    /* war or clod light crontl */ 
        case YM_WARM_CLOD_LIGHT_METHOD:
    /* control device --LED or on/off Curtains */     
        case YM_DEVICE_CTRL_METHOD:    
          return ym_device_contrl(string); 
    /*other*/      
        default :
            goto EXIT;
  };
   return RT_NULL;
   
EXIT:
  return RT_NULL;
}

