#include "lib.h"

//  socket
#define BUFF_SIZE 32
int listenfd, rcvBytes, sendBytes;
socklen_t clilen;

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

  // create server socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  //  preparation of the socket address struct
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  //  bind the socket to the port in address
  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
    perror("Error:");
    exit(0);
  }
  if (listen(listenfd, 5) != 0) {
    perror("Error:");
    exit(0);
  }
  printf("%s\n", "Server running...waiting for connections");

  //  if accept out while true => will recive only 1 connect => handle allow:
  fd_set master;  // set save connections to the server
  FD_ZERO(&master);
  FD_SET(listenfd, &master);
  int max_socket = listenfd;

  for (;;) {
    clilen = sizeof(cliaddr);
    while (1) {
      fd_set read_fds = master;
      if (select(max_socket + 1, &read_fds, NULL, NULL, NULL) == -1) {
        perror("select");
        exit(4);
      }
      // loop into all socket connected
      for (int i = 1; i <= max_socket; i++) {
        if (FD_ISSET(i, &read_fds)) {  // if socket ready used to read
          if (i == listenfd) {
            int sock_client =
                accept(i, (struct sockaddr *)&cliaddr,
                       &clilen);  // check current connect = socket on use
            if (sock_client == -1) {
              perror("accept");
              continue;  // skip this error connection
            }

            FD_SET(sock_client, &master);  // add new connection into fdset
            if (sock_client > max_socket) {
              max_socket = sock_client;
            }
            printf("New connection from %s on socket %d\n",
                   inet_ntoa(cliaddr.sin_addr), sock_client);
          } else {
            if (!isLogin) {
              // read username
              rcvBytes = recv(i, usernameRcv, BUFF_SIZE, 0);
              if (rcvBytes < 1) {
                perror("connection to the client is closed ");
                close(i);
                FD_CLR(i, &master);
                exit(0);
              }
              printf("%s", "String received from the client: ");
              puts(usernameRcv);
              // printf("send to client: ");
              strcpy(mesg, "Insert password: ");
              sendBytes = send(i, mesg, rcvBytes, 0);
              if (sendBytes < 0) {
                perror("Error: ");
                exit(0);
              }

              // read password
              rcvBytes = recv(i, passwordRcv, BUFF_SIZE, 0);
              if (rcvBytes < 1) {
                perror("connection to the client is closed ");
                close(i);
                FD_CLR(i, &master);
                exit(0);
              }
              // passwordRcv[rcvBytes - 1] = '\0';  // clear \n in string input
              // from
              printf("%s", "String received from the client: ");
              puts(passwordRcv);
              isLogin = login(head, usernameRcv, passwordRcv);
              rcvBytes = strlen(mesg);
              sendBytes = send(i, mesg, rcvBytes, 0);
              if (sendBytes < 0) {
                perror("Error: ");
                exit(0);
              }
            } else {
              // read new password
              rcvBytes = recv(i, newPasswordRcv, BUFF_SIZE, 0);
              if (rcvBytes < 1) {
                perror("connection to the client is closed ");
                close(i);
                FD_CLR(i, &master);
                exit(0);
              }
              // newPasswordRcv[rcvBytes - 1] = '\0';
              printf("%s", "String received from the client: ");
              puts(newPasswordRcv);
              changePassword(head, newPasswordRcv);
              rcvBytes = strlen(mesg);
              sendBytes = send(i, mesg, rcvBytes, 0);
              if (sendBytes < 0) {
                perror("Error: ");
                exit(0);
              }
            }
          }
        }
      }
    }
    // close(connfd);  // close the file descriptor
  }
  close(listenfd);  // close listening socket

  return;
}