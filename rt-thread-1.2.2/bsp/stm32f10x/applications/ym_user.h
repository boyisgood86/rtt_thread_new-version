#ifndef __YM_USER_H_
#define __YM_USER_H_


char *ym_user_login(const char * string);
char *ym_user_sign(const char *string);
char *ym_user_whitelist(const char *string);
char *ym_user_new_passwd(const char *string);
char *ym_user_delete(const char *string);

#endif /* __YM_USER_H_ */

