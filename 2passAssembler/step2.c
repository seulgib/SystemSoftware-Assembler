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
    //fgets(str, sizeof(str), fp): ���Ͽ��� ������� ���ڿ��� �ִ� ����, �ѹ��� ������ �� ���ڿ��� ���� ���� ����, ���ڸ� ���� ��Ʈ���� �ĺ��ϴ� ����������
    //80byte�� �Էµ�.
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
        //�׳� n++ �ߴ� �κ��� alpha, num, other �����ؼ� ���� ����
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