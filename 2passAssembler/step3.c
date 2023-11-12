//label, instruction, operand를 구분해 출력
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char symbol[50][10], instruction[50][10], operand[50][10];
int symCnt=0, instCnt=0, opCnt = 0;

void tokenCnt(char* bp);

int main(void) {
    FILE* fp;
    char buf[80];
    int num = 0;
    fp = fopen("sample.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open file...\n"); exit(1);
    }
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        tokenCnt(buf);
    }
    fclose(fp);
    printf("[Symbol]\n\n");
    for (int i = 0; i < symCnt; i++) {
        if (num < 10) {
            printf("%s\t", symbol[i]);
            num++;
        }
        else if (num == 10) {
            printf("\n");
            num = 0;
            i -= 1;//공백 출력은 symbol출력에서 제외해야하므로
        }
    }
    num = 0;
    printf("\n\n[instruction]\n\n");
    for (int i = 0; i < instCnt; i++) {
        if (num < 10) {
            printf("%s\t", instruction[i]);
            num++;
        }
        else if (num == 10) {
            printf("\n");
            num = 0;
            i -= 1;
        }
    }
    printf("\n\n[operand]\n\n");
    num = 0;
    for (int i = 0; i < opCnt; i++) {
        if (num < 10) {
            printf("%s\t", operand[i]);
            num++;
        }
        else if (num == 10) {
            printf("\n");
            num = 0;
            i -= 1;
        }
    }
    printf("\n\n");
    return 0;
}

void tokenCnt(char* bp) {
    char* cp, * test;
    int n = 0;
    char* result = strtok(bp, " \t\n");
    while (result != NULL) {
        while (1) {
            if (n < 4) {//line(0), label(1), instruction(2), operand(3)
                switch (n) {
                case 0:n++;
                    break;
                case 1:
                    strcpy(symbol[symCnt], result);
                    symCnt++;
                    n++;
                    break;
                case 2:
                    strcpy(instruction[instCnt], result);
                    instCnt++;
                    n++;
                    break;
                case 3:
                    strcpy(operand[opCnt], result);
                    opCnt++;
                    n++;
                    break;
                }
            }
            else n = 0;
            break;
        }
        result = strtok(NULL, "\t\n");
    }
}