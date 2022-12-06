#include "error.h"

#include <stdio.h>

void error(ErrorCode err) {
  switch (err) {
    case ERR_USER_EXIST:
      printf("%s\n", ERM_USER_EXIST);
      break;
    case ERR_ACTIVE_CODE:
      printf("%s\n", ERM_ACTIVE_CODE);
      break;
    case ERR_USER_NOT_EXIST:
      printf("%s\n", ERM_USER_NOT_EXIST);
      break;
    case ERR_INVALID_PASSWORD:
      printf("%s\n", ERM_INVALID_PASSWORD);
      break;
    case ERR_NOT_LOGIN:
      printf("%s\n", ERM_NOT_LOGIN);
      break;
  }
}