//������� �Է¹޾� tokenȭ �� ���� return
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
    //fgets(str, sizeof(str), fp): ���Ͽ��� ������� ���ڿ��� �ִ� ����, �ѹ��� ������ �� ���ڿ��� ���� ���� ����, ���ڸ� ���� ��Ʈ���� �ĺ��ϴ� ����������
    //80byte�� �Էµ�.
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        n += tokenCnt(buf);
    }
    fclose(fp);
    printf("token Count: %d\n", n);
}

int tokenCnt(char* bp) {
    char* cp;
    int n = 0;
    //strtok("���ڿ�", "������")
    for (cp = strtok(bp, " \t\n"); cp != NULL;) {
        n++;
        cp = strtok(NULL, " \t\n");
    }
    return(n);
}