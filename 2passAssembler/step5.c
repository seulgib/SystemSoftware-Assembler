#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int len = 0;
void printLenWord(char* bp);

int main(void) {
    FILE* fp;
    char buf[80];
    fp = fopen("words.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open file...\n"); exit(1);
    }
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        printLenWord(buf);
        fclose(fp);
        printf("%X", len);
        return 0;
    }    
}

void printLenWord(char* bp) {
    for (int i = 0; bp[i] != '\0'; i++) {
        if (bp[i] == '\n')bp[i] = '\0';
    }
    printf("%.2X:%s\n", len, bp);
    len += strlen(bp);
}
