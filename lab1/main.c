#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#define maxErrActiveCode 4
#define maxErrPassword 3
int countErrActiveCode = 0;
int countErrPassword = 0;
char activeCode[32] = "20194717";

typedef enum { blocked, active, idle } STATUS;

typedef struct {
  char username[32];
  char password[32];
  STATUS status;
} User;

struct ListUser {
  User user;
  struct ListUser *next;
};

typedef struct ListUser *pUser;

pUser createUser(User user) {
  pUser temp;
  temp = (pUser)malloc(sizeof(struct ListUser));
  temp->next = NULL;
  temp->user = user;
  return temp;
}

pUser addUser(pUser head, User user) {
  pUser temp, p;
  temp = createUser(user);
  if (head == NULL) {
    head = temp;
  } else {
    p = head;
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = temp;
  }
  return head;
}

int searchUser(pUser head, char username[32]) {
  for (pUser i = head; i != NULL; i = i->next) {
    if (strcmp(i->user.username, username) == 0) {
      return i->user.status;
    }
  }
  return -1;
}

void resetData(pUser head) {
  FILE *inputFile;
  inputFile = fopen("account.txt", "w");
  for (pUser i = head; i != NULL; i = i->next) {
    if (i->next == NULL) {
      fprintf(inputFile, "%s %s %d", i->user.username, i->user.password,
              i->user.status);
    } else {
      fprintf(inputFile, "%s %s %d\n", i->user.username, i->user.password,
              i->user.status);
    }
  }
  fclose(inputFile);
}

int login(pUser head, char username[32], char password[32]) {
  for (pUser i = head; i != NULL; i = i->next) {
    if (strcmp(i->user.username, username) == 0) {
      if (strcmp(i->user.password, password) == 0) {
        printf("hello %s\n", username);
        return 1;
      } else {
        error(ERR_INVALID_PASSWORD);
        countErrPassword++;
        if (countErrPassword == maxErrPassword) {
          printf("Password is incorrect. Account is blocked\n");
          i->user.status = blocked;
          resetData(head);
        }
        return 0;
      }
    }
  }
  error(ERR_USER_NOT_EXIST);
  return 0;
}

void activeUser(pUser head, char username[32], char password[32],
                char activeInput[32]) {
  for (pUser i = head; i != NULL; i = i->next) {
    if (strcmp(i->user.username, username) == 0) {
      if (strcmp(i->user.password, password) == 0) {
        if (strcmp(activeInput, activeCode) == 0) {
          i->user.status = active;
          resetData(head);
          printf("Account is activated \n");
          return;
        } else {
          countErrActiveCode++;
          if (countErrActiveCode == maxErrActiveCode) {
            printf("Activation code is incorrect. Account is blocked\n");
            i->user.status = blocked;
            resetData(head);
            return;
          }
          printf("Account is not activated\n");
          return;
        }
      } else {
        error(ERR_INVALID_PASSWORD);
        return;
      }
    }
  }
  error(ERR_USER_NOT_EXIST);
  return;
}

void changePassword(pUser head, char username[32], char password[32],
                    char newPass[32]) {
  for (pUser i = head; i != NULL; i = i->next) {
    if (strcmp(i->user.username, username) == 0) {
      if (strcmp(i->user.password, password) == 0) {
        printf("Password is changed\n");
        strcpy(i->user.password, newPass);
        resetData(head);
        return;
      } else {
        printf("Current password is incorrect. Please try again\n");
        return;
      }
    }
  }
  error(ERR_USER_NOT_EXIST);
}

void printList(pUser head) {
  printf("\n");
  for (pUser i = head; i != NULL; i = i->next) {
    printf("%s %s %d\n", i->user.username, i->user.password, i->user.status);
  }
}

void main() {
  // initial Values
  int choice = 0;
  int count = 0;
  pUser head = NULL;

  // variable read and store data from file
  char username[32];
  char password[32];
  int status = 0;
  User newUser;

  // variable active user
  char activeInput[32];

  // variables check login
  int isLogin = 0;

  // variables search user
  char userSearch[32];
  int result = -1;

  char newPassword[32];

  FILE *inputFile;
  inputFile = fopen("account.txt", "r");
  if (inputFile == NULL) {
    printf("can not open file\n");
    exit(0);
  }

  while (fscanf(inputFile, "%s %s %d", username, password, &status) != EOF) {
    strcpy(newUser.username, username);
    strcpy(newUser.password, password);
    newUser.status = status;
    head = addUser(head, newUser);
    count++;
  }
  fclose(inputFile);
  while (1) {
    printf("USER MANAGEMENT PROGRAM\n");
    printf("--------------------------------\n");
    printf("1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("Your choice(1-6, other to quit)\n");
    scanf("%d", &choice);
    switch (choice) {
      case 1:
        printf("Enter your username:");
        scanf("%s", username);
        printf("Enter your password:");
        scanf("%s", password);
        result = searchUser(head, username);
        if (result != -1) {
          error(ERR_USER_EXIST);
          break;
        } else {
          strcpy(newUser.username, username);
          strcpy(newUser.password, password);
          newUser.status = idle;
          head = addUser(head, newUser);
          // save user
          inputFile = fopen("account.txt", "a");
          fprintf(inputFile, "\n%s %s %d", newUser.username, newUser.password,
                  newUser.status);
          fclose(inputFile);
          printf("Successful registration. Activation required.\n");
        }
        break;
      case 2:
        printf("Enter your username:");
        scanf("%s", username);
        printf("Enter your password:");
        scanf("%s", password);
        printf("Enter active code:");
        scanf("%s", activeInput);
        activeUser(head, username, password, activeInput);
        break;
      case 3:
        printf("Enter your username:");
        scanf("%s", username);
        printf("Enter your password:");
        scanf("%s", password);
        isLogin = login(head, username, password);
        break;
      case 4:
        if (isLogin == 0) {
          error(ERR_NOT_LOGIN);
          break;
        }
        printf("Enter the user to search: ");
        scanf("%s", userSearch);
        result = searchUser(head, userSearch);
        switch (result) {
          case active:
            printf("Account is active\n");
            break;
          case blocked:
            printf("Account is blocked\n");
            break;
          case idle:
            printf("Account is idle\n");
            break;
          default:
            error(ERR_USER_NOT_EXIST);
            break;
        }
        break;
      case 5:
        if (isLogin == 0) {
          error(ERR_NOT_LOGIN);
          break;
        }
        printf("Enter your username:");
        scanf("%s", username);
        printf("Enter your password:");
        scanf("%s", password);
        printf("Enter your new password:");
        scanf("%s", newPassword);
        changePassword(head, username, password, newPassword);
        break;
      case 6:
        if (isLogin == 0) {
          error(ERR_NOT_LOGIN);
        } else {
          printf("Enter your username: ");
          scanf("%s", userSearch);
          result = searchUser(head, userSearch);
          if (result == -1) {
            error(ERR_USER_NOT_EXIST);
          } else {
            printf("Goodbye %s\n", userSearch);
            isLogin = 0;
          }
        }
        break;
      default:
        exit(0);
        break;
    }
  }
  exit(0);

  return;
}