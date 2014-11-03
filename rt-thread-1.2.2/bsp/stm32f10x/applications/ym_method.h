#ifndef __YM_METHOD_H_
#define __YM_METHOD_H_


/* method */
#define YM_USER_LOGIN_METHOD        (1000)  /* method is login */
#define YM_USER_SIGN_METHOD         (1004)   /* user sign in */ 
#define YM_USER_NEW_PASSWD_METHOD   (10004) /* change passwd */
#define YM_USER_DELETE_METHOD       (10007) /* delete user */
#define YM_USER_WHITELIST_METHOD    (10005) /* enable or disable whitelist */

#define YM_DEVICE_CTRL_METHOD       (1002)  /*device ctrol*/
#define YM_DEVICE_READ_METHOD       (1007)  /* read device status */
#define YM_DEVICE_ADD_METHOD        (11001) /* add device*/
#define YM_DEVICE_DELETE_METHOD     (11002) /* rm device */

#define YM_UDP_SEARCH_METHOD        (10001) /* udp  broadcast ip and port*/  

#define YM_WARM_CLOD_LIGHT_METHOD  (11003) /* warm and clod light control */
#define YM_WARM_CLOD_SELECT_METHOD     YM_DEVICE_CTRL_METHOD /* select warm or clod light */    

int get_method(char * const string);
char * ym_deal_data_handle(char * const string);


#endif /* __YM_METHOD_H_ */



