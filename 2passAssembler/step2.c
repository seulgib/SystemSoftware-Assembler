#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void tokenCnt(char* bp, int* num, int* alpha, int* other);

int main(void) {
    int num=0, alpha=0, other=0;
    FILE* fp;
    char buf[80];
    int n = 0;
    fp = fopen("sample.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open file...\n");
        exit(1);
    }
    //fgets(str, sizeof(str), fp): 파일에서 가지고온 문자열을 넣는 변수, 한번에 가지고 올 문자열의 길이 정보 변수, 문자를 읽을 스트림을 식별하는 파일포인터
    //80byte만 입력됨.
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        tokenCnt(buf, &num, &alpha, &other);
    }
    fclose(fp);
    printf("alpha: %d\n", alpha);
    printf("num: %d\n", num);
    printf("other: %d\n", other);
    return 0;
}

void tokenCnt(char* bp, int* num, int* alpha, int* other) {
    char* cp;
    int i, firstChar, nextChar;
    for (cp = strtok(bp, " \t\n"); cp != NULL;) {
        //그냥 n++ 했던 부분을 alpha, num, other 구분해서 개수 세기
        /*********************************************/
        if ('A' <= cp[0] && 'Z' >= cp[0])
            firstChar = -1;//alphabet
        else if ('0' <= cp[0] && '9' >= cp[0])
            firstChar = 0;//number
        else {
            firstChar = 1;//other
            nextChar = 1;
            break;
        }
        int len = strlen(cp);
        for (i = 0; i < len; i++) {
            if ('A' <= cp[i] && 'Z' >= cp[i])
                nextChar = -1;
            else if ('0' <= cp[i] && '9' >= cp[i])
                nextChar = 0;
            else 
                nextChar = 1;
            if (nextChar != firstChar) {
                nextChar = 1;
                break;
            }
            
        }
        switch(nextChar) {
            case -1:*alpha += 1; break;
            case 0:*num += 1; break;
            case 1:*other += 1; break;
            default:fprintf(stderr, "Undefined...\n"); exit(2);
        }
        /***************************************************/
        cp = strtok(NULL, " \t\n");

    }

}