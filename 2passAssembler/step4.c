#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void tokenCnt(char* bp);
void hexToDec(char* test, int len);
void aToHex(char* ascii, int len);
int sum = 0, cnt = 0;

int main(void) {
    FILE* fp;
    char buf[80];
    fp = fopen("numb.txt", "r");
    if ((fp == NULL)) {
        fprintf(stderr, "Can't open file...\n"); exit(1);
    }
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        tokenCnt(buf);
    }
    fclose(fp);
    printf("\nsum=%d\n", sum);
    return 0;
}

void tokenCnt(char* bp) {
    int i, len;
    char test[10];
    char* result = strtok(bp, " \t\n");//token화
    while (result != NULL) {
        if (result[0] == 'X') {//Hexadecimal
            for (i = 2; i <= strlen(result) - 2; i++) {
                test[i - 2] = result[i];
                cnt++;
            }
            printf("token: %s\t", result);
            printf("\t\t");
            hexToDec(test, cnt);
            cnt = 0;
        }
        else if (result[0] == 'C') {//ASCII 
            printf("token: %s\t", result);
            for (i = 2; i <= strlen(result) - 2; i++) {
                test[i - 2] = result[i];
                cnt++;
            }
            test[cnt] = '\0';
            aToHex(test, cnt);
            cnt = 0;
        }
        else {//Decimal
            printf("token:%s\t", result);
            sum += atoi(result);
            printf("\t\tdec: %d\n", atoi(result));
        }
        result = strtok(NULL, " \t\n");
    }
}

void hexToDec(char* test, int len) {
    int decimal = 0;
    int i;
    int position = 0;
    for (i = len - 1; i >= 0; i--) {
        char ch = test[i];
        if (ch >= 48 && ch <= 57) {//'0'~'9'
            decimal += (ch - 48) * pow(16, position);
        }
        else if (ch >= 65 && ch <= 70) {//'A'~'Z'
            decimal += ((ch - 48) - 7) * pow(16, position);
        }
        else if (ch >= 97 && ch <= 102) {//'a'~'z'
            decimal += (ch - (97 - 10)) * pow(16, position);
        }
        position++;
    }
    printf("dec: %d\n", decimal);
    sum += decimal;
}

void aToHex(char* ascii, int len) {
    char outword[10];
    int tmpLen;
    for (int i = 0; i < len; i++) {
        //ASCII문자->16진수로 표현될 때 두자리 차지
        sprintf(outword + i * 2, "%02X", ascii[i]);
    }
    tmpLen = strlen(outword);
    printf("hex: %s\t", outword);
    hexToDec(outword, tmpLen);
}