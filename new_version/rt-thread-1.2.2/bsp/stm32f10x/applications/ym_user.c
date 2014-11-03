#include <rtthread.h>
#include "cJSON.h"
#include "ym_json.h"
#include "ym_user.h"
#include "login.h"
#include "white_list.h"


/*
@Function : user login
@string : json string  <tcp client --> tcp server>
@return : json string <char*> or RT_NULL, back to client < tcp server ---> tcp client >
*/
char *ym_user_login(const char * string)
{
    CLOGIN js_logig;
    rt_memset(&js_logig, 0, sizeof(js_logig));
    /* get json */
    js_logig.js = js_handel(string);
    
    /* authen name and passwd */
    int ret = login_authentication(&js_logig.js);
    
    /* if admin login--> read whitelist */
    if(ret == (IS_ADMIN|LOGIN_OK)) {
      MY_DEBUG("%s,%d: admin login!\n",__func__,__LINE__);
      /* read whitelist */
      unsigned int count = 0;
      white_list_t * whitelist= RT_NULL;
      white_list((void*)&whitelist, &count);
      
      /* packing back data */
      char *admin = pack_admin_msg(whitelist, count, &js_logig.js);
      if(admin)
        MY_DEBUG("%s,%d: admin string is :\n%s\n",__func__,__LINE__,admin);
      else
        MY_DEBUG("packing admin msg faild !\n");
      
      if(whitelist)
        rt_free(whitelist);
      return admin;
    }
    /* Normal user login */
    else if(ret == LOGIN_OK){
        char *normal = pack_normal_msg();
        if(normal) 
          MY_DEBUG("%s,%d: Normal string is : \n",__func__,__LINE__,normal);
        else
          MY_DEBUG("packing normal msg faild 1\n");
        
        return normal;
    }
    /* err msg */
    else if(ret == LOGIN_ERR) {
        char *err = pack_error_msg();
        if(err) {
          MY_DEBUG("%s,%d: err msg is %s\n",__func__,__LINE__, err);
        }else
          MY_DEBUG("%s,%d: err msg pack faild !\n",__func__,__LINE__);
        return err;
    }
}

/*
@Function : add new user Account
@string : json string <tcp client --> tcp server>
@return : json string <char*> or RT_NULL, back to client < tcp server ---> tcp client >
*/

char *ym_user_sign(const char *string)
{
  CSIGN_IN js_sign;
  char ret;
  white_list_t user ;
  white_list_t * whitelist= RT_NULL;
  unsigned int count = 0;
  
  rt_memset(&js_sign, 0, sizeof(js_sign));
  
  /*get json*/
  js_sign.js = js_handel(string);

 /* add user and passwd to whitelist */
  is_enable_list();
  rt_memset(&user, 0, sizeof(white_list_t));
  strcpy(user.name, js_sign.js.name);
  strcpy(user.psd, js_sign.js.passwd);
  ret = add_white_list(&user);
  
  /* read whitelist */
   white_list((void*)&whitelist, &count);
   
  /* pack sign msg */
   char *user_sign = pack_sign_msg(ret,whitelist,count, &js_sign.js);
   if(user_sign) {
     MY_DEBUG("%s,%d: string is :\n%s\n",__func__,__LINE__,user_sign);
   }else {
     MY_DEBUG("%s,%d: packing faild !\n",__func__,__LINE__);
   }
  
   /* delete whitelist buff */
  if(whitelist)
    rt_free(whitelist);
   
   /* return */
   return user_sign;
  
EXIT:

  if(whitelist) 
    rt_free(whitelist);
  return RT_NULL;  
}


/*
@Function : on or off user whitelist
@string : json string <tcp client --> tcp server>
@return : json string <char*> or RT_NULL, back to client < tcp server ---> tcp client >
*/
char *ym_user_whitelist(const char *string)
{
  WhiteList wl;
  char value;

  rt_memset(&wl, 0, sizeof(wl));
  /* paresel tcp data */
  wl = white_js_handel(string);
  if( !wl.js.flag) {
    MY_DEBUG("%s,%d: do tcp data faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
  /* on or off whitelist */
  if(wl.isAuthority == disable) {
    value =  disable_white_list();
  }
  else if(wl.isAuthority == enable) {
    value =  enable_white_list();
  }
  
  /* packing whitelist json string */
  char *whitelist = pack_whitelist_msg(&wl, value);
  if(whitelist) {
    MY_DEBUG("%s,%d: strig is \n%s\n",__func__,__LINE__, whitelist);
  }else {
    MY_DEBUG("%s,%d: packing faild !\n",__func__,__LINE__);
  }
   
  return whitelist;
  
EXIT:

  return RT_NULL; 
}


/*
@Function : replace accout passwd
@string : json string <tcp client --> tcp server>
@return : json string <char*> or RT_NULL, back to client < tcp server ---> tcp client >
*/
char *ym_user_new_passwd(const char *string)
{
  
  /*malloc*/
  Passwd_t * ym_passwd = (Passwd_t *)rt_malloc(sizeof(Passwd_t));
  if(!ym_passwd) {
    MY_DEBUG("%s,%d: malloc faild !\n",__func__,__LINE__);
    goto EXIT;
  }
  rt_memset(ym_passwd, 0, sizeof(Passwd_t));
  
  /* get json  */
  if(passwd_js_handle(ym_passwd, string) < 0) {
    MY_DEBUG("%s,%d:get js faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }

  /* change passwd */
  int value = change_admin(ym_passwd->js.name, ym_passwd->new_passwd);

  /* packing send data */
  char *new_passwd = pack_new_passwd_msg(ym_passwd, value);
  if(new_passwd) {
    MY_DEBUG("%s,%d: string is : \n%s\n",__func__,__LINE__,new_passwd);
  }else {
    MY_DEBUG("%s,%d: packing faild !\n",__func__,__LINE__);
  }
  
  /* free */
  if(ym_passwd)
    rt_free(ym_passwd);
  
  /* return */
  return new_passwd;
  
EXIT:
  if(ym_passwd)
    rt_free(ym_passwd);
  return RT_NULL;
}


/*
@Function : delete user accout
@string : json string <tcp client --> tcp server>
@return : json string <char*> or RT_NULL, back to client < tcp server ---> tcp client >
*/
char *ym_user_delete(const char *string)
{
    deltuser_t user;
    white_list_t del_user;
    
    /* get json */
    rt_memset(&user, 0, sizeof(user));
    user.js = js_handel(string);
    if(!user.js.flag) {
      MY_DEBUG("%s,%d: get user json string faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* delete user */
    rt_memset(&del_user, 0, sizeof(del_user));
    memcpy(del_user.name, user.js.name, strlen(user.js.name));
    memcpy(del_user.psd, user.js.passwd, strlen(user.js.passwd));
    
    int ret = del_white_list(&del_user);
    
    /* packing data */
    char *delet_user = pack_user_delet_msg(user, ret);
//        char *delet_user =RT_NULL;

    if(delet_user) {
      MY_DEBUG("%s,%d: string is : \n%s\n",__func__,__LINE__, delet_user);
    }else {
      MY_DEBUG("%s,%d: packing faild !\n",__func__,__LINE__);
    }
    
    /* return */
    return delet_user;
EXIT:
    return RT_NULL;
}

