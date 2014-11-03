#include <rtthread.h>
#include "cJSON.h"
#include "ym_json.h"
#include "ym_config.h"
#include "ym_user.h"
#include "dev_opt.h"

/*******************************************************************************/
#define MAC_LEN_IN_BYTE 8  

/* do mac */
static unsigned char
a2x(const char c)
{
  if(c >= '0' && c <= '9')
    return (unsigned char)atoi(&c);
  else if(c >= 'a' && c <= 'f')
    return 0xa + (c-'a');
  else if(c >= 'A' && c <= 'F')
    return 0xa + (c-'A');
  else
    return 0;
//    switch(c) {
//    case '0'...'9':
//
//        return (unsigned char)atoi(&c);
//    case 'a'...'f':
//        return 0xa + (c-'a');
//    case 'A'...'F':  
//        return 0xa + (c-'A');
//    default:  
//        goto error;  
//    }   
//error:  
//    return 0;  
} 

/*
** Mac Format Like BC:52:C6:18:ED:FD:12:23
*/
 #define MAC_LEN_IN_BYTE (8)  

void copy_str2mac(char *mac_buff, char* str)
{
        for(int i = 0 ; i < MAC_LEN_IN_BYTE; i++){
            mac_buff[i] = (a2x(str[i*3]) << 4) + a2x(str[i*3 + 1]);
        }
}


/*  mac to string */
static void mac2str(char * mac_buff,char *mac_address ,char count)
{
//  char mac[32] = {0};
  char temp_buff[3] = {0};
  char i = 0;
  
  char mac[30];
  
  rt_memset(mac, 0, sizeof(mac));
  
  for(i = 0; i < count;i++) {
    rt_memset(temp_buff, 0, sizeof(temp_buff));
    sprintf(temp_buff, "%02X", mac_buff[i]);
    strcat(mac, temp_buff);
    if(i < (count-1))
        strcat(mac, ":");
  }
  
//  MY_DEBUG("%s,%d: mac string is %s\n\r",__func__,__LINE__, mac);
  memcpy(mac_address, mac, 25);
  
  return ;
EXIT:
  return;
}

/************************************** device msg *************************************************/

/* contrl device msg */
char * pack_device_ctl_msg(ym_dev_ctl_t op)
{
  cJSON *root, *js_head, *js_body, *js_arry, *js_list, *js_user;
  int i = 0;
  char mac[30] = {0};
  
  /* root */
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* body */
  cJSON_AddItemToObject(root, "body", js_body=cJSON_CreateObject());
  /* list */
   cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
   for(i = 0; i< op.number; i++) {
        cJSON_AddItemToArray(js_list, js_arry = cJSON_CreateObject());
   
        rt_memset(mac, 0, sizeof(mac));
        mac2str((op.dev+i)->device_mac,mac, 8);
        cJSON_AddStringToObject(js_arry, "device_mac",mac); //TODO...
        cJSON_AddNumberToObject(js_arry, "devcice_progress",(op.dev+i)->devcice_progress);

        js_arry = RT_NULL;
   }
   cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
   cJSON_AddStringToObject(js_user, "name",op.js.name);
   cJSON_AddStringToObject(js_user, "password", op.js.passwd);  
  /* head */
  cJSON_AddItemToObject(root, "head", js_head=cJSON_CreateObject());
//  if(op.flag == 1)
    cJSON_AddStringToObject(js_head,"message","控制设备成功");
//  else if( !op.flag )
//    cJSON_AddStringToObject(js_head,"message","正在更新设备列表...");
//  else if(op.flag == -1)
//    cJSON_AddStringToObject(js_head,"message","控制设备失败");
  cJSON_AddNumberToObject(js_head,"method",op.js.method);
  cJSON_AddStringToObject(js_head,"mobileid",op.js.mobileid);
  
//  if(op.flag == 1)
    cJSON_AddNumberToObject(js_head,"status",1);
//  else if(!op.flag)
//    cJSON_AddNumberToObject(js_head,"status",2);
//  else if(op.flag == -1)
//    cJSON_AddNumberToObject(js_head,"status",3);
  
  {
     char *s = cJSON_PrintUnformatted(root);
     if(root)
       cJSON_Delete(root); 
     /* return */
     return s;
  }
EXIT:
  if(root)
    cJSON_Delete(root);    
  return RT_NULL;
}


/* delete device msg */
char * pack_device_delete_msg(ReadDev op)
{
  cJSON *root, *js_head;
  
  /* root */
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* head */
  cJSON_AddItemToObject(root, "head", js_head=cJSON_CreateObject());
  if(op.op)
    cJSON_AddStringToObject(js_head,"message","删除设备成功");
  else
    cJSON_AddStringToObject(js_head,"message","删除设备失败");
  
  cJSON_AddNumberToObject(js_head,"method",op.js.method);
  cJSON_AddStringToObject(js_head,"mobileid",op.js.mobileid);
  
  if(op.op)
     cJSON_AddNumberToObject(js_head,"status",1);
  else
    cJSON_AddNumberToObject(js_head,"status",2);
  
  {
     char *s = cJSON_PrintUnformatted(root);
     if(root)
        cJSON_Delete(root);
     
     return s;
  }
  
EXIT:
  return RT_NULL;
}


/*add device msg*/
char * pack_device_add_msg(ReadDev op)
{
  cJSON *root, *js_head;
  
  /* root */
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* head */
  cJSON_AddItemToObject(root, "head", js_head=cJSON_CreateObject());
  if(op.op == 1)
    cJSON_AddStringToObject(js_head,"message","添加设备成功");
  else if(op.op == 0)
    cJSON_AddStringToObject(js_head,"message","添加设备失败");
  else if(op.op == 2) {
    cJSON_AddStringToObject(js_head,"message","设备已存在");
  }
  
  cJSON_AddNumberToObject(js_head,"method",op.js.method);
  cJSON_AddStringToObject(js_head,"mobileid",op.js.mobileid);
  
  if(op.op)
     cJSON_AddNumberToObject(js_head,"status",1);
  else
    cJSON_AddNumberToObject(js_head,"status",2);
  
  {
     char *s = cJSON_PrintUnformatted(root);

     if(root)
     cJSON_Delete(root);
     
     return s;
  }
  
EXIT:
  if(root)
    cJSON_Delete(root);  
  return RT_NULL;
}

/* do read device msg */
char * pack_device_read_status_msg(ReadDev read_dev)
{
  //TODO.... 
  //rt_uint8_t dev_list(void **dev,rt_uint8_t *cnt);  
  cJSON *root, *js_head, *js_list, *js_user,*js_arry,*js_body;
  int i  = 0, j = 0;
  char mac[30] = {0};
  
  dev_t *all_dev = RT_NULL;
  rt_uint8_t amout = 0;
  
  rt_uint8_t ret = dev_list((void*)&all_dev, &amout);
  if(!ret) {
    MY_DEBUG("%s,%d: read device statu faild !\n",__func__,__LINE__);
    amout = 0;
  }else {
    MY_DEBUG("%s,%d: read device statu success !\n",__func__,__LINE__);
  }
  
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
   cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject());
   cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
   
   for(i = 0; i < amout; i++) {
        cJSON_AddItemToArray(js_list, js_arry = cJSON_CreateObject());
        rt_memset(mac, 0, sizeof(mac));
        mac2str((all_dev+i)->mac,mac, 8);
        cJSON_AddStringToObject(js_arry, "device_mac",mac); //TODO...
        cJSON_AddNumberToObject(js_arry, "devcice_progress", (all_dev+i)->stat);
        js_arry = RT_NULL;
   }
//   cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
//   cJSON_AddStringToObject(js_user, "name", read_dev.js.name);
//   cJSON_AddStringToObject(js_user, "password", read_dev.js.passwd);

   cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
   if(amout) {
     cJSON_AddStringToObject(js_head, "message", "读取状态成功"); 
   }else
     cJSON_AddStringToObject(js_head, "message", "读取状态失败");
   
   cJSON_AddNumberToObject(js_head, "method", 1007);
   cJSON_AddStringToObject(js_head, "mobileid", "13956412369");
   
   if(amout) {
       cJSON_AddNumberToObject(js_head, "status", 1); 
   }else
     cJSON_AddNumberToObject(js_head, "status", 0); 
   
   {
        char *s =  cJSON_PrintUnformatted(root);

        if(root) 
           cJSON_Delete(root);
        if(js_arry)    
           cJSON_Delete(js_arry);
        if(all_dev)
          rt_free(all_dev);
       
        return s;
   }

EXIT:
  if(root)
    cJSON_Delete(root);
  if(all_dev)
    rt_free(all_dev);
  if(js_arry)
     cJSON_Delete(js_arry);
  return RT_NULL;
}

/**************************************** user msg *****************************************************/

/*delete user*/
char * pack_user_delet_msg(deltuser_t user, int value)
{
    cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
    unsigned int count = 0;
    white_list_t * whitelist= RT_NULL;
    char i = 0;

    /* create json string root */
    root = cJSON_CreateObject();
    if(!root) { 
      MY_DEBUG("%s,%d: get root faild\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    
    /* get whitelist */
    white_list((void*)&whitelist, &count);
    
    /* packing a json string now... */     

//{"body":{"list":["admin","admin11","admin22"],"other":{"isAuthority":true},"user":{"name":"admin","password":"admin"}},"head":{"method":1000,"mobileid":"353714986786243","status":0}}        
        
        /* body */
      cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject());
        /*list*/
      if(whitelist != RT_NULL) {
        cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
        for(i = 0; i < count; i++) 
          cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
      }       
       /* other */
       cJSON_AddItemToObject(js_body, "other", js_other = cJSON_CreateObject());
       cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);
//       cJSON_AddNumberToObject(js_other,"isAuthority",1);
       
       /* user */
       cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
       cJSON_AddStringToObject(js_user,"name",user.js.name);
       cJSON_AddStringToObject(js_user,"password",user.js.passwd);
       
       /* head */
       cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
       if(value == 1) {
           cJSON_AddStringToObject(js_head,"message","删除用户成功"); 
       }else if(value == 0){
            cJSON_AddStringToObject(js_head,"message","删除用户失败"); 
       }
       
       cJSON_AddNumberToObject(js_head,"method",user.js.method);
       cJSON_AddStringToObject(js_head,"mobileid",user.js.mobileid);
       
       if(value == 1){
         cJSON_AddNumberToObject(js_head,"status",1);
       }else if(value == 0){
            cJSON_AddNumberToObject(js_head,"status",0);
       }
       
       cJSON_AddStringToObject(js_head,"boxid","827312897");
       
       {
            char *s = cJSON_PrintUnformatted(root);

            if(root)
              cJSON_Delete(root);
            if(whitelist)
              rt_free(whitelist);
            /* return */
            return s;
       }
EXIT:
  if(root)    
    cJSON_Delete(root);
  if(whitelist)
    rt_free(whitelist);
  return RT_NULL;     
}


/* new passwd */
char * pack_new_passwd_msg(Passwd_t * ps, int value)
{
   cJSON *root, *head;
   /* create root */
   root = cJSON_CreateObject(); 
   if(!root) {
     MY_DEBUG("%s,%d: create root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   cJSON_AddItemToObject(root, "head", head=cJSON_CreateObject());
   
   if(value == 1)
    cJSON_AddStringToObject(head,"message","修改密码成功");
   else if(value == 0)
    cJSON_AddStringToObject(head,"message","修改密码失败");
   
   cJSON_AddNumberToObject(head,"method",ps->js.method);
   cJSON_AddStringToObject(head,"mobileid",ps->js.mobileid);
   
   if(value == 1) 
     cJSON_AddNumberToObject(head,"status",1);
   if(value == 0)
     cJSON_AddNumberToObject(head,"status",2);
   
   cJSON_AddStringToObject(head,"boxid",ps->js.mobileid);
   {
        char *s = cJSON_PrintUnformatted(root);
//        if(!s) {
//            goto EXIT;
//        }
//        
//        char *out = (char*)rt_malloc(strlen(s)+2);
//        if(!out) {
//          MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
//          goto EXIT;
//        }
//        rt_memset(out, 0, (strlen(s)+2));
//        memcpy(out, s, strlen(s));
//        
//        /*free*/
//        rt_free(s);
        if(root)
          cJSON_Delete(root);
        
        /*return*/
        return s;
        
   }
EXIT:
  if(root)
    cJSON_Delete(root);
  return RT_NULL;    
}


/* on or off whitelist */
char * pack_whitelist_msg(WhiteList *wl, char value)
{
/* enable or disable whitelist */
  cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
  white_list_t * whitelist= RT_NULL;
  unsigned int count = 0;
  char i = 0;

  /* get whitelist */
  is_enable_list();
  if(white_list_enable == ENABLE) {
       white_list((void*)&whitelist, &count); 
  }
  
  /*root*/
   root = cJSON_CreateObject();
   if(!root) { 
     MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__); 
     goto EXIT;
   }
   /* body */
   cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject()); 
    /*list*/
   if(whitelist != RT_NULL) {
      cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
      for(i = 0; i < count; i++) 
        cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
   }
    /* other */
    cJSON_AddItemToObject(js_body, "other", js_other = cJSON_CreateObject());
    cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);   
       /* user */
    cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
    cJSON_AddStringToObject(js_user,"name",wl->js.name);
    cJSON_AddStringToObject(js_user,"password",wl->js.passwd);   
       /* head */
    cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
    if(white_list_enable == ENABLE) 
      cJSON_AddStringToObject(js_head,"message","白名单开启");
    if(white_list_enable != ENABLE) 
      cJSON_AddStringToObject(js_head,"message","白名单关闭");
    
    cJSON_AddNumberToObject(js_head,"method",wl->js.method);
    cJSON_AddStringToObject(js_head,"mobileid",wl->js.mobileid);
    
    if(wl->isAuthority == enable) {
      if(value == 1) {
        cJSON_AddNumberToObject(js_head,"status",1);
      }else if(value < 0){
        cJSON_AddNumberToObject(js_head,"status",2);
      }
    }else if(wl->isAuthority == disable) {
      if(value == 0) {
        cJSON_AddNumberToObject(js_head,"status",1);
      }else if(value < 0) {
        cJSON_AddNumberToObject(js_head,"status",2);
      }
    }else
      cJSON_AddNumberToObject(js_head,"status",3);
    
//    cJSON_AddNumberToObject(js_head,"status",1);
    cJSON_AddStringToObject(js_head,"boxid","827312897");
       
    {
          char *s = cJSON_PrintUnformatted(root);
//          if(!s) {
//            goto EXIT;
//          }
//          char *out = (char*)rt_malloc(strlen(s)+2);
//          if(!out) {
//            MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
//            goto EXIT;
//          }
//          
//          rt_memset(out, 0, (strlen(s)+2));
//          memcpy(out, s, strlen(s));
//          
//          /*delete heap buff*/
//          rt_free(s);
          cJSON_Delete(root);
          rt_free(whitelist);
          
          return s;
     }    
    
EXIT:
  if(root)  
    cJSON_Delete(root);
  if(whitelist) 
    rt_free(whitelist);
  return RT_NULL;
}



/* add user msg packing */
char * pack_sign_msg(int value,white_list_t * whitelist, unsigned int count, JSON * js)
{
    cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
    char i = 0;
    
    /* create json string root */
    root = cJSON_CreateObject();
    if(!root) { 
      MY_DEBUG("%s,%d: get root faild\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    
    /* packing a json string now... */     

//{"body":{"list":["admin","admin11","admin22"],"other":{"isAuthority":true},"user":{"name":"admin","password":"admin"}},"head":{"method":1000,"mobileid":"353714986786243","status":0}}        
        
        /* body */
      cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject());
        /*list*/
      if(whitelist != RT_NULL) {
        cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
        for(i = 0; i < count; i++) 
          cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
      }       
       /* other */
       cJSON_AddItemToObject(js_body, "other", js_other = cJSON_CreateObject());     
       cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);
       
       
       /* user */
       cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
       cJSON_AddStringToObject(js_user,"name",js->name);
       cJSON_AddStringToObject(js_user,"password",js->passwd);     
       
       /* head */
       cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
       if(value == FAIL)
        cJSON_AddStringToObject(js_head,"message","注册失败");
       else if(value == SUCCESS)
         cJSON_AddStringToObject(js_head,"message","注册成功");
       else if(value == EXIST)
         cJSON_AddStringToObject(js_head,"message","用户已存在");
       else
         cJSON_AddStringToObject(js_head,"message","未知错误");
       
       cJSON_AddNumberToObject(js_head,"method",js->method);
       cJSON_AddStringToObject(js_head,"mobileid",js->mobileid);
       cJSON_AddNumberToObject(js_head,"status",1);
       cJSON_AddStringToObject(js_head,"boxid","827312897");
   
       
       {
            char *s = cJSON_PrintUnformatted(root);
//            if(!s) {
//                goto EXIT;
//            }
//            char *out = (char*)rt_malloc(strlen(s)+2);
//            if(!out) {
//              MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
//              goto EXIT;
//            }
//            rt_memset(out, 0, (strlen(s)+2));
//            memcpy(out, s, strlen(s));
//            rt_free(s);
            cJSON_Delete(root);
            return s;
       }

EXIT:
  if(root)    
    cJSON_Delete(root);
  return RT_NULL;
}


/* do login error msg */
char* pack_error_msg(void)
{
  cJSON *root, *js_head;

  // {"head":{"message":ERR_MSG,"status":0}}
      
  /* create json string root */
  root = cJSON_CreateObject();
  if(!root) { MY_DEBUG("%s,%d:  get root is faild !\n\r",__func__,__LINE__); cJSON_GetErrorPtr(); goto EXIT; }
  
  cJSON_AddItemToObject(root,"head",js_head=cJSON_CreateObject());
  cJSON_AddNumberToObject(js_head,"status",0);
  cJSON_AddStringToObject(js_head,"message","登陆失败");
  
  {
    char *s = cJSON_PrintUnformatted(root);
//    if(!s) { 
//      MY_DEBUG("%s,%d: js print to string faild!\n\r",__func__,__LINE__); 
//      goto EXIT; 
//    }
//    char *out = (char*)rt_malloc(strlen(s) + 1);
//    if(!out) {
//      MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
//      goto EXIT;
//    }
//    rt_memset(out, 0, (strlen(s)+1));
//    memcpy(out, s, strlen(s));
//    rt_free(s);
    cJSON_Delete(root);
    return s;
  }
  
EXIT:
    if(root)    
      cJSON_Delete(root);
    return RT_NULL;
}

/* Normal user login msg packing */
 char * pack_normal_msg(void)
{
//TODO...
  cJSON *root, *js_head;
    /* create json string root */
  root = cJSON_CreateObject();
  if(!root) { 
    MY_DEBUG("%s,%d: get js root faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  cJSON_AddItemToObject(root,"head",js_head=cJSON_CreateObject());
  cJSON_AddNumberToObject(js_head,"status",1);
  cJSON_AddStringToObject(js_head,"message","登陆成功");
  
  {
    char *s = cJSON_PrintUnformatted(root);
//    if(!s) { 
//      MY_DEBUG("%s,%d: js print to string faild!\n\r",__func__,__LINE__); 
//      goto EXIT; 
//    }
//    char *out = (char*)rt_malloc(strlen(s) + 1);
//        if(!out) {
//          MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
//          goto EXIT;
//        }
//        rt_memset(out, 0, (strlen(s)+1));
//        memcpy(out, s, strlen(s));
//        rt_free(s);
        cJSON_Delete(root); 
        return s;
  }
    
EXIT:
  if(root) 
    cJSON_Delete(root);
  return RT_NULL;
}


char * pack_admin_msg(white_list_t * whitelist, unsigned int count, JSON *js)
{
    cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
    char i = 0;
    /* create json string root */
    root = cJSON_CreateObject();
    if(!root) { 
      MY_DEBUG("%s,%d: get root faild\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    
    /* packing a json string now... */     

//{"body":{"list":["admin","admin11","admin22"],"other":{"isAuthority":true},"user":{"name":"admin","password":"admin"}},"head":{"method":1000,"mobileid":"353714986786243","status":0}}        
        
        /* body */
      cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject());
        /*list*/
      if(whitelist != RT_NULL) {
        cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
        for(i = 0; i < count; i++) 
          cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
      }       
       /* other */
       cJSON_AddItemToObject(js_body, "other", js_other = cJSON_CreateObject());
       cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);
//       cJSON_AddNumberToObject(js_other,"isAuthority",1);
       
       /* user */
       cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
       cJSON_AddStringToObject(js_user,"name",js->name);
       cJSON_AddStringToObject(js_user,"password",js->passwd);
       
       /* head */
       cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
       cJSON_AddStringToObject(js_head,"message","登陆成功");
       cJSON_AddNumberToObject(js_head,"method",js->method);
       cJSON_AddStringToObject(js_head,"mobileid",js->mobileid);
       cJSON_AddNumberToObject(js_head,"status",1);
       cJSON_AddStringToObject(js_head,"boxid","827312897");
       
       {
            char *s = cJSON_PrintUnformatted(root);
//            if(s) {
//                char *out = (char*)rt_malloc(strlen(s) + 1 );
//                if(!out) {
//                  MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
//                  goto EXIT;
//                }
//                rt_memset(out, 0, (strlen(s)+1));
//                memcpy(out, s, strlen(s));
//                rt_free(s);
                cJSON_Delete(root);
//                MY_DEBUG("%s,%d: msg is %s\n",__func__,__LINE__,out);
                return s;
//            }else 
//              goto EXIT;
       }

EXIT:
  if(root)    
    cJSON_Delete(root);
  return RT_NULL;
}

/**********************************  js handle **********************************************/

/* passwd js handle*/
int passwd_js_handle(Passwd_t *ps, const char * const string)
{
  cJSON * root, *js_body,*js_user,*js_passwd;
  
  if(!ps || !string) {
    MY_DEBUG("%s,%d: string or ps is NULL..\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* get basic json */
  ps->js = js_handel(string);
  if(!ps->js.flag) {
    MY_DEBUG("%s,%d: get basic json faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
     /* create json root */
    root = cJSON_Parse(string);
    if(!root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }   
   /* get body */
    /*  get body */
    js_body = cJSON_GetObjectItem(root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /* get user */
    js_user = cJSON_GetObjectItem(js_body, "user") ;
    if(!js_user) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    } 
   /* get new passwd */
   js_passwd =  cJSON_GetObjectItem(js_user,"newPassword");
   if(!js_passwd) {
     MY_DEBUG("%s,%d: no new passwd !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   strcpy(ps->new_passwd, js_passwd->valuestring);
   
   /* free space */
   cJSON_Delete(root);
  return ym_ok;
EXIT:
  return ym_err;
}

/* get device parmer */
ym_dev_ctl_t get_device_param(const char *  string)
{
    ym_dev_ctl_t dev_param;
    rt_memset(&dev_param, 0, sizeof(dev_param));
    
    /* get basic json item */
    dev_param.js = js_handel(string);
    if(!dev_param.js.flag) {
      MY_DEBUG("%s,%d: deal with basic json faild!\n",__func__,__LINE__);
      goto EXIT;
    }
    
    /* get root */
    cJSON * js_root = cJSON_Parse(string);
    if(!js_root){
      MY_DEBUG("%s,%d: get root faild !\n",__func__,__LINE__);
      goto EXIT;
    }
    /* get body */
    cJSON * js_body = cJSON_GetObjectItem(js_root, "body");
    if(!js_body) {
      MY_DEBUG("%s,%d: get body faild !\n",__func__,__LINE__);
      goto EXIT;
    }
    /* get list */
    cJSON * js_list = cJSON_GetObjectItem(js_body, "list");
    if(!js_list) {
      MY_DEBUG("%s,%d: get list faild !\n",__func__,__LINE__);
      goto EXIT;
    }
    /* get array number */
    int size = cJSON_GetArraySize(js_list);
    if(size <= 0) {
      MY_DEBUG("%s,%d: no array...\n",__func__,__LINE__);
      goto EXIT;
    }
    if(size > YM_DEVICE_MAX_NUMBER) {
      MY_DEBUG("%s,%d: more device...\n",__func__,__LINE__);
      goto EXIT;
    }
    /* set device number */
    dev_param.number = size;
    /* malloc device number space */
    dev_param.dev = (ym_dev_ctrl_t*)rt_malloc(sizeof(ym_dev_ctrl_t)*size);
    if(!dev_param.dev) {
      MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
      goto EXIT;
    }
    rt_memset(dev_param.dev, 0, sizeof(ym_dev_ctrl_t)*size);
    /* now get parm */
    int i = 0;
    cJSON * item = RT_NULL;
    for(i = 0; i < size; i++){
      if(item) {
        cJSON_Delete(item);
        item = RT_NULL;
      }
      
      cJSON * js_item = cJSON_GetArrayItem(js_list,i);
      if(js_item) {
         char *  p = cJSON_PrintUnformatted(js_item);
         item = cJSON_Parse(p);
         if(p)
           rt_free(p);
         if(!item)
            continue ;
          /*device_style*/
          cJSON *device_style = cJSON_GetObjectItem(item, "device_style");
          if(device_style){
             (dev_param.dev+i)->device_style =  device_style->valueint; 
           }
           /* devcice_progress*/
          cJSON * devcice_progress = cJSON_GetObjectItem(item, "devcice_progress");
          if(devcice_progress){
            (dev_param.dev+i)->devcice_progress = devcice_progress->valueint;  
          }
          /* device_mac*/
          rt_memset((dev_param.dev+i)->device_mac, 0, LONG_ADDR);
          cJSON * device_mac = cJSON_GetObjectItem(item, "device_mac");
          if(device_mac) {
                char mac_buff[8];
                rt_memset(mac_buff, 0, sizeof(mac_buff));
                copy_str2mac(mac_buff, device_mac->valuestring);
                memcpy((dev_param.dev+i)->device_mac , mac_buff,MAC_LEN_IN_BYTE);
           } 
         /* get device type */
          cJSON * device_type = cJSON_GetObjectItem(item, "device_type");
            if(device_type){
            (dev_param.dev+i)->device_type = device_type->valueint;          
              switch(device_type->valueint){
                /*normal light*/
                  case YM_LIGHT_NORMALI:
                /*socket*/
                case YM_SOCKET:
                  break ;
                /*switch*/
                  case YM_SWITCH:
                      cJSON * switch_select = cJSON_GetObjectItem(item, "light_style");
                      if(switch_select)
                        (dev_param.dev+i)->switch_type = switch_select->valueint;
                        break;
                  
                  case YM_LIGHT_COLD: 
                         cJSON * light_style_cold = cJSON_GetObjectItem(item, "light_style");
                         if(light_style_cold)
                           (dev_param.dev+i)->light_style = light_style_cold->valueint;
                    
                    break;
                  case YM_LIGHT_COLOR:
                    /* light_style*/
                   cJSON * light_style_color =  cJSON_GetObjectItem(item, "light_style");
                   if(light_style_color){
                        (dev_param.dev+i)->light_style = light_style_color->valueint;
                   }
                   /*LED color*/
                   cJSON * device_B = cJSON_GetObjectItem(item, "device_B");
                   if(device_B){
                      (dev_param.dev+i)->device_B = device_B->valueint;  
                   }
                   cJSON * device_G = cJSON_GetObjectItem(item, "device_G");
                   if(device_G){
                       (dev_param.dev+i)->device_G = device_G->valueint; 
                   }
                   cJSON * device_R = cJSON_GetObjectItem(item, "device_R");
                   if(device_R){
                       (dev_param.dev+i)->device_R = device_R->valueint; 
                   }
                    break;
                             
                  default:
                    break;
              };
            }
      
      }
    
    
    }
    if(item) 
     cJSON_Delete(item);
   cJSON_Delete(js_root);   
    /* return */
    return dev_param;
EXIT:
  if(dev_param.dev)
    rt_free(dev_param.dev);
  rt_memset(&dev_param, 0, sizeof(dev_param));
  return dev_param;
}


/* read */
ReadDev  read_js_handel(const char * const string)
{
//   ReadDev * read_dev = RT_NULL;
   const char *s = string;
   char mac_buff[MAC_LEN_IN_BYTE];
//   char * p = RT_NULL;
    ReadDev  read_dev ;
     
   cJSON *dev_mac = RT_NULL;
   cJSON *dev_progress = RT_NULL;
   cJSON *js = RT_NULL;
   rt_memset(&read_dev, 0, sizeof(read_dev));
   
   if(!s) {
     MY_DEBUG("%s,%d: tcp string is NULL..\n\r",__func__,__LINE__);
     goto EXIT;
   }
   /* deal with baisc string */
   read_dev.js = js_handel(s);
   if( !read_dev.js.flag) {
     MY_DEBUG("%s,%d: deal basic string is faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
    /* create json root */
    cJSON * js_root = cJSON_Parse(s);
    if(!js_root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }   
   /* get body */
    /*  get body */
    cJSON * js_body = cJSON_GetObjectItem(js_root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
//      goto EXIT;
    }
    /* get list */
   cJSON *js_item = RT_NULL;
   cJSON *item = RT_NULL;
    if(js_body) {
    cJSON * js_list = cJSON_GetObjectItem(js_body, "list") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }    

    
   int size = cJSON_GetArraySize(js_list);
   MY_DEBUG("%s,%d: size is %d\n\r",__func__,__LINE__, size);
   

   int j  = 0;

   /* device count */
   read_dev.count = 0;
   read_dev.op = 100;  
   /* get mac and progress */
      int i = 0;
       for(i = 0; i < size; i++) {
         if(item) {
            cJSON_Delete(item);
            item = RT_NULL;
         }
           js_item = cJSON_GetArrayItem(js_list,i);
           if(js_item) {
              char*  p = cJSON_PrintUnformatted(js_item);
                
//                MY_DEBUG("%s,%d: p is %s\n",__func__,__LINE__, p);
                item = cJSON_Parse(p);
                if(p) {
                    rt_free(p);
                }
                if(!item) 
                  continue ;
                
                /* dev_progress */
                dev_progress =  cJSON_GetObjectItem(item, "devcice_progress");
                if(dev_progress) {
                    read_dev.dev[i].devcice_progress = dev_progress->valueint;
                } 
                
                /* device_mac */
                dev_mac = cJSON_GetObjectItem(item, "device_mac");            
                if(dev_mac) {
                    /* copy str to mac */
                    rt_memset(mac_buff, 0, sizeof(mac_buff));
                    copy_str2mac(mac_buff, dev_mac->valuestring);
                    
                    /* for test */
                    for(j = 0; j < 8; j++) {
                        read_dev.dev[i].device_mac[j] = mac_buff[j];
                    }
                    (read_dev.count)++;
    //                read_dev.dev++;
                }
    //            cJSON_Delete(item);
           }
       }
    } /*js_body*/
   if(item) 
     cJSON_Delete(item);
   cJSON_Delete(js_root);
   read_dev.js.flag = 1;
   return read_dev;
EXIT:
  if(js_root) 
    cJSON_Delete(js_root);
  read_dev.js.flag = 0;
  return read_dev;
}

/* whitelist tcp data */
WhiteList white_js_handel(const char * const string)
{
   WhiteList wl;
   const char *s = string;
   cJSON * js_root, *js_body, *js_head, *js_user, *js_other,*js;    
   rt_memset(&wl, 0, sizeof(WhiteList));
   wl.js = js_handel(string);
   if(!wl.js.flag) {
     MY_DEBUG("%s,%d: do tcp data faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   
   js_root = cJSON_Parse(string);
   if(!js_root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
    /*  get body */
    js_body = cJSON_GetObjectItem(js_root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /* get other */
    js_other = cJSON_GetObjectItem(js_body, "other");
      if( !js_other) { 
          MY_DEBUG("%s,%d: no isAuthority\n\r",__func__,__LINE__);
          goto EXIT;
      }else {
            js = cJSON_GetObjectItem(js_other, "isAuthority");
            if(!js) { 
              MY_DEBUG("%s,%d: isAuthority error..\n\r",__func__,__LINE__);
            }else{
              if(js->type == cJSON_Number) {
                wl.isAuthority = js->valueint;
              }
            }
        }
        cJSON_Delete(js_root);
        return wl;
EXIT:
   if(js_root) 
     cJSON_Delete(js_root);
   wl.js.flag = 0;
   return wl;
}



/* parsel json string */
JSON js_handel(const char * const string)
{
//TODO...
  const char *s = string;
  JSON cjson;
  cJSON * js_root, *js_body, *js_head, *js_user, *js_other,*js;  

  rt_memset(&cjson, 0, sizeof(JSON));
  
  if(!s) {
    MY_DEBUG("%s,%d: tcp string is NULL...\n\r",__func__,__LINE__);
    goto EXIT;
  }
    /* create json root */
    js_root = cJSON_Parse(string);
    if(!js_root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
    /*  get body */
    js_body = cJSON_GetObjectItem(js_root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /*get user*/
    js_user = cJSON_GetObjectItem(js_body, "user");
    if(!js_user) { 
      MY_DEBUG("%s,%d: no user !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    
    /* get name and passwd */
    js = cJSON_GetObjectItem(js_user, "name");
    if(!js) { 
      MY_DEBUG("%s,%d: no name..!\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    if(js->type == cJSON_String) 
      strcpy(cjson.name , js->valuestring);
    
    /* get passwd */
    js = cJSON_GetObjectItem(js_user, "password");
    if(js) { 
      MY_DEBUG("%s,%d: no passwd\n\r",__func__,__LINE__); 
      if(js->type == cJSON_String) 
        strcpy(cjson.passwd, js->valuestring);
    }
    /* parsel head */
    js_head = cJSON_GetObjectItem(js_root, "head") ;
    if(!js_head) { 
      MY_DEBUG("%s,%d: no head..\n\r",__func__,__LINE__); 
      goto EXIT; 
    };
    /* get method */
    js = cJSON_GetObjectItem(js_head, "method");
    if(!js) { 
      MY_DEBUG("%s,%d: no method\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    if(js->type == cJSON_Number)  
      cjson.method = js->valueint;
    /* get mobile ID */
    js = cJSON_GetObjectItem(js_head, "mobileid");
    if(!js) { 
      MY_DEBUG("%s,%d: no mobileid\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    if(js->type == cJSON_String)  
      strcpy(cjson.mobileid , js->valuestring);
    /* get status */
    js = cJSON_GetObjectItem(js_head, "status");
    if(!js) {
      MY_DEBUG("%s,%d: no status \n",__func__,__LINE__);
      cjson.status = 0;
    }else
      cjson.status = js->valueint;
    
    cJSON_Delete(js_root);
  
  cjson.flag = 1;
  return cjson;
EXIT:
  cjson.flag = 0;
  if(js_root)  
    cJSON_Delete(js_root);
  return cjson;
}


