//어셈블리어 입력받아 token화 후 개수 return
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
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
        n += tokenCnt(buf);
    }
    fclose(fp);
    printf("token Count: %d\n", n);
}

int tokenCnt(char* bp) {
    char* cp;
    int n = 0;
    //strtok("문자열", "구분자")
    for (cp = strtok(bp, " \t\n"); cp != NULL;) {
        n++;
        cp = strtok(NULL, " \t\n");
    }
    return(n);
}