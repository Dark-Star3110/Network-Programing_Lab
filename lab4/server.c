#include "lib.h"

//  socket
#define BUFF_SIZE 32
int sockfd, rcvBytes, sendBytes;
socklen_t len;

struct sockaddr_in servaddr, cliaddr;
char mesg[BUFF_SIZE + 1];
char userlogined[BUFF_SIZE + 1];

// account
#define maxErrPassword 3
int countErrPassword = 0;
// variables check login
int isLogin = 0;

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

int encodePassword(char password[32], char alphaPass[32], char digitPass[32]) {
  int len = strlen(password);
  int i;
  int j = 0;
  int k = 0;
  for (i = 0; i < len; i++) {
    if (isalpha(password[i])) {
      alphaPass[j] = password[i];
      j++;
    } else if (isdigit(password[i])) {
      digitPass[k] = password[i];
      k++;
    } else {
      return 0;
    }
  }
  alphaPass[j] = '\0';
  digitPass[k] = '\0';
  return 1;
}

int login(pUser head, char username[32], char password[32]) {
  for (pUser i = head; i != NULL; i = i->next) {
    if (strcmp(i->user.username, username) == 0) {
      if (strcmp(i->user.password, password) == 0) {
        if (i->user.status == blocked) {
          strcpy(mesg, "Account is blocked\n");
          return 0;
        }
        printf("hello %s\n", username);
        strcpy(mesg, "OK\n");
        strcpy(userlogined, i->user.username);
        return 1;
      } else {
        countErrPassword++;
        printf("%d", countErrPassword);
        if (countErrPassword == maxErrPassword) {
          i->user.status = blocked;
          resetData(head);
          strcpy(mesg, "Account is blocked\n");
          return 0;
        }
        strcpy(mesg, "Not OK!\n");
        return 0;
      }
    }
  }
  strcpy(mesg, "User do not exist!\n");
  return 0;
}

void changePassword(pUser head, char newPass[32]) {
  if (strcmp(newPass, "bye") == 0) {
    strcpy(mesg, "bye ");
    strcat(mesg, userlogined);
    strcat(mesg, "\n");
    isLogin = 0;
    return;
  }
  char passEncode[BUFF_SIZE + 1];
  strcpy(passEncode, newPass);
  // variables encode password
  char alphaPass[BUFF_SIZE + 1];
  char digitPass[BUFF_SIZE + 1];
  int checkNewPass = encodePassword(passEncode, alphaPass, digitPass);
  if (checkNewPass) {
    for (pUser i = head; i != NULL; i = i->next) {
      if (strcmp(i->user.username, userlogined) == 0) {
        strcpy(i->user.password, newPass);
        resetData(head);
        // hash
        strcpy(mesg, digitPass);
        strcat(mesg, "\n");
        strcat(mesg, alphaPass);
        strcat(mesg, "\n");
        return;
      }
    }
  } else {
    strcpy(mesg, "Error\n");
    return;
  }
}

void printList(pUser head) {
  for (pUser i = head; i != NULL; i = i->next) {
    printf("%s %s %d\n", i->user.username, i->user.password, i->user.status);
  }
}

void main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("INVALID PORT SERVER PARAMETTER\n");
    exit(0);
  }
  // socket variables
  int SERV_PORT = atoi(argv[1]);
  char usernameRcv[BUFF_SIZE + 1];
  char passwordRcv[BUFF_SIZE + 1];
  char newPasswordRcv[BUFF_SIZE + 1];
  int rcvBytes;

  // initial Values
  int count = 0;
  pUser head = NULL;

  // variable read and store data from file
  char username[32];
  char password[32];
  int status = 0;
  User newUser;

  // read and store data
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

  // socket initial
  /// Step 1: Construct socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Error: ");
    exit(0);
  }
  // Step 2: Bind address to socket
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) {
    perror("Error: ");
    exit(0);
  }
  printf("Server started.\n");

  for (;;) {
    if (strcmp(newPasswordRcv, "bye") == 0) break;
    len = sizeof(cliaddr);
    if (!isLogin) {
      // read username
      rcvBytes = recvfrom(sockfd, usernameRcv, BUFF_SIZE, 0,
                          (struct sockaddr *)&cliaddr, &len);
      if (rcvBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      usernameRcv[rcvBytes - 1] = '\0';  // clear \n in string input from fgets
      printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr),
             ntohs(cliaddr.sin_port), usernameRcv);
      // printf("send to client: ");
      strcpy(mesg, "Insert password: ");
      rcvBytes = strlen(mesg);
      sendBytes =
          sendto(sockfd, mesg, rcvBytes, 0, (struct sockaddr *)&cliaddr, len);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }

      // read password
      rcvBytes = recvfrom(sockfd, passwordRcv, BUFF_SIZE, 0,
                          (struct sockaddr *)&cliaddr, &len);
      if (rcvBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      passwordRcv[rcvBytes - 1] = '\0';  // clear \n in string input from fgets
      printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr),
             ntohs(cliaddr.sin_port), passwordRcv);
      // printf("send to client: ");
      isLogin = login(head, usernameRcv, passwordRcv);
      rcvBytes = strlen(mesg);
      sendBytes =
          sendto(sockfd, mesg, rcvBytes, 0, (struct sockaddr *)&cliaddr, len);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
    } else {
      // read new password
      rcvBytes = recvfrom(sockfd, newPasswordRcv, BUFF_SIZE, 0,
                          (struct sockaddr *)&cliaddr, &len);
      if (rcvBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      newPasswordRcv[rcvBytes - 1] = '\0';  // clear \n
      printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr),
             ntohs(cliaddr.sin_port), newPasswordRcv);
      changePassword(head, newPasswordRcv);
      rcvBytes = strlen(mesg);
      sendBytes =
          sendto(sockfd, mesg, rcvBytes, 0, (struct sockaddr *)&cliaddr, len);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
    }
  }
  return;
}