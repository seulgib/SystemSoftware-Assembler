#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct OPTAB {
    char name[8];
    int len;
} Wordtab[] = { {"LDA",3},{"STA",4},{"ADD",5},{"TIX",2},{"CMP",6} };

void PrintAssm(char* name_buf, int name_size);

int main(void) {
    char name_buf[8];
    int wordtab_size = sizeof(Wordtab) / 12;
    FILE* fp;
    fp = fopen("command.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open file...\n"); exit(1);
    }
    while (fgets(name_buf, sizeof(name_buf), fp) != NULL) {
        if (name_buf[0] == '\n') continue;
        if (name_buf[strlen(name_buf) - 1] == '\n')
            name_buf[strlen(name_buf) - 1] = '\0';
        PrintAssm(name_buf, wordtab_size);
    }
    fclose(fp);
}

void PrintAssm(char* name_buf, int wordtab_size) {
    static int k = 1;
    static int loc = 0;
    int i, res = -1;
    for (i = 0; i < wordtab_size; i++) {
        res = strcmp(name_buf, Wordtab[i].name);//두 문자열 같으면 0 리턴
        if (res == 0)break;
    }
    if (i == wordtab_size) {
        printf("Undefined word\n");
    }
    else {
        printf("%02d, %.2X, %s, %.2d\n", k, loc, Wordtab[i].name, Wordtab[i].len);
        loc += Wordtab[i].len;
        k++;

        
    }
}