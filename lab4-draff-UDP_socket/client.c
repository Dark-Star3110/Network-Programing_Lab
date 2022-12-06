#include "lib.h"
#define BUFF_SIZE 100
#define PORT 8080
#define MAXLINE 1000
#define SERV_ADDR "0.0.0.0"

int sockfd, rcvBytes, sendBytes;
socklen_t len;
char buff[BUFF_SIZE + 1];
struct sockaddr_in servaddr;

void main() {
  int SERV_PORT = PORT;
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
    printf("Send to server: ");
    fgets(buff, BUFF_SIZE, stdin);
    len = sizeof(servaddr);
    sendBytes = sendto(sockfd, buff, strlen(buff), 0,
                       (struct sockaddr *)&servaddr, len);
    if (sendBytes < 0) {
      perror("Error: ");
      exit(0);
    }
    rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0,
                        (struct sockaddr *)&servaddr, &len);
    if (rcvBytes < 0) {
      perror("Error: ");
      exit(0);
    }
    buff[rcvBytes] = '\0';
    printf("Reply from server: %s\n", buff);
  }

  // int n;
  // char sendline[MAXLINE], recvline[MAXLINE + 1];
  // struct sockaddr_in servaddr;
  // if (connect(sockfd, (struct sockaddr *)&servaddr, len)) {
  //   perror("Error");
  //   exit(0);
  // };
  // while (fgets(sendline, MAXLINE, stdin) != NULL) {
  //   send(sockfd, sendline, strlen(sendline), 0);
  //   n = recv(sockfd, recvline, MAXLINE, 0);
  //   recvline[n] = 0;
  //   /* null terminate */
  //   printf("%s", recvline);
  // }
  close(sockfd);
  return;
}
