#include "lib.h"

typedef enum { blocked, active } STATUS;

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

// point to head of User List
pUser head = NULL;

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

void printList(pUser head) {
  for (pUser i = head; i != NULL; i = i->next) {
    printf("%s %s %d\n", i->user.username, i->user.password, i->user.status);
  }
}

void *client_handler(void *arg) {
  int clientfd;
  int sendBytes, rcvBytes;
  char usernameRcv[BUFF_SIZE + 1];
  char passwordRcv[BUFF_SIZE + 1];
  pthread_detach(pthread_self());
  clientfd = *(int *)arg;
  while (1) {
    // read username
    rcvBytes = recv(clientfd, usernameRcv, BUFF_SIZE, 0);
    if (rcvBytes < 1) {
      perror("connection to the client is closed ");
      close(clientfd);
      break;
    }
    printf("%s", "String received from the client: ");
    puts(usernameRcv);
    // printf("send to client: ");
    strcpy(mesg, "Insert password: ");
    sendBytes = send(clientfd, mesg, rcvBytes, 0);
    if (sendBytes < 0) {
      perror("Error: ");
      break;
    }

    // read password
    rcvBytes = recv(clientfd, passwordRcv, BUFF_SIZE, 0);
    if (rcvBytes < 1) {
      perror("connection to the client is closed ");
      close(clientfd);
      break;
    }
    // passwordRcv[rcvBytes - 1] = '\0';  // clear \n in string input
    // from
    printf("%s", "String received from the client: ");
    puts(passwordRcv);
    isLogin = login(head, usernameRcv, passwordRcv);
    rcvBytes = strlen(mesg);
    sendBytes = send(clientfd, mesg, rcvBytes, 0);
    if (sendBytes < 0) {
      perror("Error: ");
      break;
    }
  }
  close(clientfd);

  return NULL;
}

void main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("INVALID PORT SERVER PARAMETTER\n");
    exit(0);
  }

  //  process id
  pthread_t tid;

  // socket variables
  int SERV_PORT = atoi(argv[1]);

  // initial Values
  int count = 0;

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

  while (1) {
    clilen = sizeof(cliaddr);
    int connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    pthread_create(&tid, NULL, &client_handler, &connfd);
  }

  close(listenfd);  // close listening socket

  return;
}