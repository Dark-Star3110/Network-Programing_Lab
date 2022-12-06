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
  int rcvBytes;
  // Step 1: Construct socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Error: ");
    exit(0);
  }
  // Step 2: Define the address of the server
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_aton(SERV_ADDR, &servaddr.sin_addr);
  servaddr.sin_port = htons(SERV_PORT);

  while (1) {
    len = sizeof(servaddr);
    if (!isLogin) {
      // input username
      printf("Enter your username: ");
      fgets(username, BUFF_SIZE, stdin);
      // scanf("%s", username);
      if (strcmp(username, "\n") == 0) exit(0);
      sendBytes = sendto(sockfd, username, strlen(username), 0,
                         (struct sockaddr *)&servaddr, len);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      rcvBytes = recvfrom(sockfd, mesg, BUFF_SIZE, 0,
                          (struct sockaddr *)&servaddr, &len);
      if (rcvBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      mesg[rcvBytes] = '\0';
      printf("%s", mesg);
      // input password
      fgets(password, BUFF_SIZE, stdin);
      // scanf("%s", password);
      if (strcmp(password, "\n") == 0) exit(0);
      sendBytes = sendto(sockfd, password, strlen(password), 0,
                         (struct sockaddr *)&servaddr, len);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      rcvBytes = recvfrom(sockfd, mesg, BUFF_SIZE, 0,
                          (struct sockaddr *)&servaddr, &len);
      if (rcvBytes < 0) {
        perror("Error: ");
        exit(0);
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
      // scanf("%s", newPassword);
      if (strcmp(newPassword, "\n") == 0) exit(0);
      if (strcmp(newPassword, "bye\n") == 0) isLogin = 0;
      sendBytes = sendto(sockfd, newPassword, strlen(newPassword), 0,
                         (struct sockaddr *)&servaddr, len);
      if (sendBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      rcvBytes = recvfrom(sockfd, mesg, BUFF_SIZE, 0,
                          (struct sockaddr *)&servaddr, &len);
      if (rcvBytes < 0) {
        perror("Error: ");
        exit(0);
      }
      mesg[rcvBytes] = '\0';
      printf("%s", mesg);
    }
  }

  close(sockfd);
  return;
}
