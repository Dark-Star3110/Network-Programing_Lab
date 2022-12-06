#include "lib.h"

struct hostent *he;
struct in_addr **addr_list;
struct in_addr addr;

int isIPAdr(char *ip) {
  char *token = strtok(ip, ".");
  int i, length;
  length = strlen(token);
  for (i = 0; i < length; i++) {
    if (!isdigit(token[i])) {
      return 0;
    }
  }
  return 1;
}

void main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("INVALID PARAMETER\n");
    exit(0);
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
