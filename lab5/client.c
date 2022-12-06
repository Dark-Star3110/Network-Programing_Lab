#include "lib.h"
#define BUFF_SIZE 32

int sockfd, rcvBytes, sendBytes;
socklen_t len;
struct sockaddr_in servaddr;

void main(int argc, char const *argv[]) {
  if (argc < 3) {
    printf("INVALID PARAMETER\n");
    exit(0);
  }

  int isLogin = 0;
  char username[BUFF_SIZE + 1];
  char password[BUFF_SIZE + 1];
  char newPassword[BUFF_SIZE + 1];
  char mesg[BUFF_SIZE + 1];

  char SERV_ADDR[BUFF_SIZE + 1];
  strcpy(SERV_ADDR, argv[1]);
  int SERV_PORT = atoi(argv[2]);

  // Create a socket for the client
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Problem in creating the socket");
    exit(2);
  }
  // Create of the remote server socket information structure
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
  servaddr.sin_port = htons(SERV_PORT);

  //  connect the client to the server socket
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("Problem in connecting to the server");
    exit(3);
  }

  while (1) {
    len = sizeof(servaddr);
    if (!isLogin) {
      // input username
      printf("Enter your username: ");
      fgets(username, BUFF_SIZE, stdin);
      // scanf("%s", username);
      if (strcmp(username, "\n") == 0) exit(0);
      username[strlen(username) - 1] = '\0';
      sendBytes = send(sockfd, username, BUFF_SIZE, 0);
      if (sendBytes < 0) {
        perror("Error");
        exit(0);
      }
      rcvBytes = recv(sockfd, mesg, BUFF_SIZE, 0);
      if (rcvBytes < 0) {
        perror("The server terminated prematurely");
        exit(4);
      }
      mesg[rcvBytes] = '\0';
      printf("%s", mesg);
      // input password
      fgets(password, BUFF_SIZE, stdin);
      // scanf("%s", password);
      if (strcmp(password, "\n") == 0) exit(0);
      password[strlen(password) - 1] = '\0';
      sendBytes = send(sockfd, password, BUFF_SIZE, 0);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      rcvBytes = recv(sockfd, mesg, BUFF_SIZE, 0);
      if (rcvBytes < 0) {
        perror("The server terminated prematurely");
        exit(4);
      }
      mesg[rcvBytes] = '\0';
      printf("%s\n", mesg);
      if (strcmp(mesg, "OK\n") == 0) {
        isLogin = 1;
        // printf("logined\n");
      }
    } else {
      // input newpassword
      printf("Enter new password: ");
      fgets(newPassword, BUFF_SIZE, stdin);
      if (strcmp(newPassword, "\n") == 0) exit(0);
      if (strcmp(newPassword, "bye\n") == 0) isLogin = 0;
      newPassword[strlen(newPassword) - 1] = '\0';
      sendBytes = send(sockfd, newPassword, BUFF_SIZE, 0);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      rcvBytes = recv(sockfd, mesg, BUFF_SIZE, 0);
      if (rcvBytes < 0) {
        perror("The server terminated prematurely");
        exit(4);
      }
      mesg[rcvBytes] = '\0';
      printf("%s", mesg);
    }
  }

  close(sockfd);
  return;
}
