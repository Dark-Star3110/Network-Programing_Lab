#include "lib.h"
#define BUFF_SIZE 100
#define PORT 8080
int sockfd, rcvBytes, sendBytes;
socklen_t len;

char buff[BUFF_SIZE + 1];
struct sockaddr_in servaddr, cliaddr;

void main() {
  int SERV_PORT = PORT;
  char mesg[BUFF_SIZE + 1];
  int rcvBytes;
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
    len = sizeof(cliaddr);
    rcvBytes =
        recvfrom(sockfd, buff, BUFF_SIZE, 0, (struct sockaddr *)&cliaddr, &len);
    if (rcvBytes < 0) {
      perror("Error: ");
      exit(0);
    }
    buff[rcvBytes] = '\0';
    printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr),
           ntohs(cliaddr.sin_port), buff);
    printf("send to client: ");
    fgets(mesg, BUFF_SIZE, stdin);
    rcvBytes = strlen(mesg);
    sendBytes =
        sendto(sockfd, mesg, rcvBytes, 0, (struct sockaddr *)&cliaddr, len);
    if (sendBytes < 0) {
      perror("Error: ");
      exit(0);
    }
  }
  return;
}
