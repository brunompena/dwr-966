#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
main(int argc, char *argv[])
{
    unsigned char buf[sizeof(struct in6_addr)];
    int s, prefix;
    char str[INET6_ADDRSTRLEN];

   if (argc != 3) {
        fprintf(stderr, "Usage: %s string prefix\n", argv[0]);
        exit(EXIT_FAILURE);
    }

   prefix = atoi(argv[2]);
   if (prefix < 0 || prefix > 128) {
        fprintf(stderr, "prefix out of range (0~128)");
        exit(EXIT_FAILURE);
   }


   s = inet_pton(AF_INET6, argv[1], buf);
    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        exit(EXIT_FAILURE);
    }

   switch (prefix % 8) {
       case 0:
           break;

       case 1:
           buf[prefix/8] &= 128;
           prefix += 7;
           break;

       case 2:
           buf[prefix/8] &= 192;
           prefix += 6;
           break;

       case 3:
           buf[prefix/8] &= 224;
           prefix += 5;
           break;

       case 4:
           buf[prefix/8] &= 240;
           prefix += 4;
           break;

       case 5:
           buf[prefix/8] &= 248;
           prefix += 3;
           break;

       case 6:
           buf[prefix/8] &= 252;
           prefix += 2;
           break;

       case 7:
           buf[prefix/8] &= 254;
           prefix += 1;
           break;

   }

   memset(buf + prefix/8, 0, 16 - (prefix/8));

   if (inet_ntop(AF_INET6, buf, str, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
   }

   printf("%s\n", str);

   return 0;
}
