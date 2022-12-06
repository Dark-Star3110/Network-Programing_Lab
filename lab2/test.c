#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>  //  getname,gethost
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct hostent *hostInf;
struct in_addr **ipAddr_alias_list;
struct in_addr ipAddr;

int checkIpAddr(char *ipAddr) {
  char *token = strtok(ipAddr, ".");
  int i, len;
  len = strlen(token);
  for (i = 0; i < len; i++) {
    if (!i(token[i])) {
      return 0;
    }
  }
  return 1;
}

void main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("missing parameter\n");
    exit(1);
  }
  char homepage[30];
  strcpy(homepage, argv[2]);
  if ((strcmp(argv[1], "1") == 0 && isIPAdr(homepage) == 0) ||
      (strcmp(argv[1], "2") == 0 && isIPAdr(homepage) == 1)) {
    printf("Wrong parameter\n");
    exit(0);
  }
  int i;

  if (strcmp(argv[1], "2") == 0) {
    he = gethostbyname(argv[2]);
    if (he == NULL) {
      printf("Not found information \n");
      exit(1);
    }
    // print information about this host:
    printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_addr));
    printf("Alias IP:\n");
    addr_list = (struct in_addr **)he->h_addr_list;
    for (i = 1; addr_list[i] != NULL; i++) {
      printf("\t%s\n", inet_ntoa(*addr_list[i]));
    }
    printf("\n");
  }

  if (strcmp(argv[1], "1") == 0) {
    // get the host name of ip:
    inet_aton(argv[2], &addr);
    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he == NULL) {
      printf("Not found information \n");
      exit(1);
    }
    printf("Official name: %s\n", he->h_name);
  }
  return;
}
