#ifndef __ERROR_H__
#define __ERROR_H__

typedef enum {
  ERR_USER_EXIST,
  ERR_ACTIVE_CODE,
  ERR_USER_NOT_EXIST,
  ERR_INVALID_PASSWORD,
  ERR_NOT_LOGIN
} ErrorCode;

#define ERM_USER_EXIST "Account existed"
#define ERM_ACTIVE_CODE "Activation code is incorrect."
#define ERM_USER_NOT_EXIST "Cannot find account."
#define ERM_INVALID_PASSWORD "Password is incorrect"
#define ERM_NOT_LOGIN "Account is not sign in"

void error(ErrorCode err);

#endif