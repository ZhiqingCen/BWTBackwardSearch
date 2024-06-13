/*
  A sample program made to help you with Assignment 2 - Raymond 26/6/23

  Bit operation version

  - It reads the binary RLB file and output the runs to a human readable format in stdout.
  - It doesn't calculate the final value for a multi-byte count (but it is very easy to extend it for that).

 */

#include<stdio.h>

int main(int argc, char *argv[])
{
    FILE *fd;
    unsigned char c;

    if (argc != 2) {
        printf("Usage: %s FILE.rlb\n", argv[0]);
        return 1;
    }

    fd = fopen(argv[1],"r");

    if (fd == NULL) {
        printf("Failed to open %s\n", argv[1]);
        return 1;
    }

    c = fgetc(fd);

    while(!feof(fd)) {
        if (c >> 7 == 1) {  /* if the first bit is 1, then c is a count */
 	    printf("{%d}", (unsigned) c & 0b01111111); /* bitwise AND to get the last 7 bits */
        } else
            putchar(c);

        c = fgetc(fd);
    }

    putchar('\n'); /* A newline to make the output easier to read */
    
    fclose(fd);
    return 0;
}
