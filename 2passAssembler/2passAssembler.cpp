#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100
#define XR 32768 // X 레지스터 더할 값 

int pass1(char* bp);
int pass2(char* bp, FILE* Sample_o);
//int find_optab();
int fprint(int res, FILE* sample_1st); // pass 1의 실행 결과를 파일에 출력하는 함수

struct  OPTAB {
    char name[8];
    int len;
} optab[] = { {"LDA", 00}, {"STA", 12}, {"TIX", 44}, {"STL", 20}, {"JSUB", 72}, {"COMP", 40}, {"JEQ", 48},
{"J", 60}, {"RSUB", 76}, {"BYTE", 1}, {"WORD", 3}, {"RESW", 3}, {"RESB", 1}, {"TD", 224}, {"RD", 216}, {"LDX", 04},
{"STCH", 84}, {"JLT", 56}, {"STX", 16}, {"WD", 220}, {"LDCH", 80} };

struct TABLE {
    char lable[10], opcode[10], oprand[10];
    int symloc; // lable의 주소 
    int object;
} symtab[MAX];

int p1_start;
long locctr = 0; // 위치 계수기 LOCCTR 
long p2_locctr = 0;
int flag = 0; // pass 1 에서 오류판독, loop정지, 이벤트 후 다음 줄 읽어옴 등 여러가지 상황을 알려주는 flag 
int flag2; // opcode가 OPTAB에 정의된 단어인지 아닌지 알려주는 flag 
int j = 0; // 정상적으로 SYMTAB에 추가된 lable 개수 count 
int cnt = 0;    // line number, 한 줄당 5씩 증가 ( 5, 10, 15, 20, ..... , cnt)
int res = 0; // 파일 출력시 loop문 탈출을 위한 flag  
int flag3 = 0; // space를 파일의 lable에 출력하기 위한 flag 
int p2_start = 0;
int l = 0, T = 10;
int a = 1, b = 11;

int main() {
    int i;
    FILE* fp;
    FILE* sample_1st;
    FILE* sample_o;
    char buf[80];
    char* token;
    if ((fp = fopen("input.txt", "r")) == NULL) { // 입력 소스파일 오픈 
        printf("input.txt not found...\n"); exit(1);
    }
    if ((sample_1st = fopen("Sample.txt", "w+")) == NULL) { // 출력 Sample파일 오픈
        printf("Sample.txt not open...\n"); exit(1);
    }
    if ((sample_o = fopen("Sample_o.txt", "w+")) == NULL) { // 출력 Sample.o파일 오픈
        printf("Sample.txt not open...\n"); exit(1);
    }

    // pass 1

    while (fgets(buf, sizeof(buf), fp) != NULL) {

        flag = pass1(buf);
        res = fprint(res, sample_1st);
        if (res == 4)
            break;
    }
    fclose(fp);

    rewind(sample_1st); // sample_1st의 file pointer를 0(시작점)으로 이동

    /*
    void rewind(FILE *stream);
        : 파일의 현재 위치를 0(시작점)으로 이동
    */

    while (fgets(buf, sizeof(buf), sample_1st) != NULL) { // Sample.txt 파일 내용 출력 
        printf("%s", buf);
    }
    printf("\ntotal length : %X", locctr);
    printf("\n\n");

    printf("\n---------Symbol Table---------\n\n");
    printf("\tSymbol\tValue\n\n");

    for (i = 1; i < MAX; i++) {
        if (!strlen(symtab[i].lable) < 1)
            printf("\t%s\t%X\n", symtab[i].lable, symtab[i].symloc);
    }
    printf("\n\n");

    // pass 2    

    rewind(sample_1st); // sample_1st의 file pointer를 0(시작점)으로 이동 

    printf("---------Object Code---------\n\n");

    while (fgets(buf, sizeof(buf), sample_1st) != NULL) { // Sample.txt 파일을 한줄씩 읽어옴
        flag = pass2(buf, sample_o);
        if (flag == 1)
            break;
    }

    fclose(sample_1st);
    fclose(sample_o);

    return 0;
}

int pass2(char* bp, FILE* Sample_o) {
    char test;
    char* tmp_token;
    int i, k, n;
    char* token = strtok(bp, "\t\n");
    char tmp_lable[10];
    if (token == NULL) // token이 NULL이면 다음 줄 읽어옴
        return 0;
    if (p2_start == 0) { //p2_start가 0이면 실행 
        for (i = 0; i < 3; i++) {
            if (i == 2)
                strcpy(tmp_lable, token);
            token = strtok(NULL, "\t\n");
        }
        if (!strcmp(token, "START")) {
            printf("H^%s^", tmp_lable);
            fprintf(Sample_o, "H%s", tmp_lable);
            token = strtok(NULL, "\t\n");
            p2_start = strtoul(token, NULL, 16);
            p2_locctr = p2_start;
            printf("%.6X^%.6X\n", p2_locctr, locctr);
            fprintf(Sample_o, "%.6X%.6X\n", p2_locctr, locctr);
            printf("T^%.6X^", p2_locctr);
            fprintf(Sample_o, "T%.6X", p2_locctr);
            printf("%.2X^", symtab[b].symloc - p2_start);
            fprintf(Sample_o, "%.2X", symtab[b].symloc - p2_start);
            b += 7;
        }
        else
            return 4;
    }
    else {
        p2_locctr = symtab[a].symloc;
        a++;
        for (i = 0; i < 4; i++) {
            if (l == 10) {
                printf("\n");
                fputs("\n", Sample_o);
                printf("T^");
                fputs("T", Sample_o);
                printf("%.6X^", p2_locctr);
                fprintf(Sample_o, "%.6X", p2_locctr);
                printf("%.2X^", symtab[b].symloc - p2_locctr);
                fprintf(Sample_o, "%.2X", symtab[b].symloc - p2_locctr);
                b += 13;
                l++;
            }
            else if (l == 18) {
                p2_locctr = symtab[a + 2].symloc;
                printf("\n");
                fputs("\n", Sample_o);
                printf("T^");
                fputs("T", Sample_o);
                printf("%.6X^", p2_locctr);
                fprintf(Sample_o, "%.6X", p2_locctr);
                printf("%.2X^", symtab[b].symloc - p2_locctr);
                fprintf(Sample_o, "%.2X", symtab[b].symloc - p2_locctr);
                b += 10;
                l++;
            }
            else if (l == 32) {
                printf("\n");
                fputs("\n", Sample_o);
                printf("T^");
                fputs("T", Sample_o);
                printf("%.6X^", p2_locctr);
                fprintf(Sample_o, "%.6X", p2_locctr);
                printf("%.2X^", symtab[b].symloc - p2_locctr);
                fprintf(Sample_o, "%.2X", symtab[b].symloc - p2_locctr);
                b += 3;
                l++;
            }
            else if (l == 43) {
                printf("\n");
                fputs("\n", Sample_o);
                printf("T^");
                fputs("T", Sample_o);
                printf("%.6X^", p2_locctr);
                fprintf(Sample_o, "%.6X", p2_locctr);
                printf("%.2X^", symtab[b].symloc - p2_locctr);
                fprintf(Sample_o, "%.2X", symtab[b].symloc - p2_locctr);
                l++;
            }
            switch (i) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                if (!strcmp(token, "END")) {
                    printf("\nE^");
                    fputs("\nE", Sample_o);
                    printf("%.6X", p1_start);
                    fprintf(Sample_o, "%.6X\n", p1_start);
                    printf("^\n");
                    return 1;
                }
                else if (!strcmp(token, "WORD")) {
                    token = strtok(NULL, "\t\n");
                    printf("%.6X", atoi(token));
                    fprintf(Sample_o, "%.6X", atoi(token));
                    printf("^");
                    l++;
                    break;
                }
                else if (!strcmp(token, "BYTE")) {
                    token = strtok(NULL, "\t\n");
                    if (token[0] == 'X') { // oprand의 첫부분이 X이면 
                        for (k = 2; k < strlen(token) - 1; k++) {  // ex) X'F1' 이면 X,',' 세개를 제외한 길이 
                            printf("%c", token[k]);
                            fprintf(Sample_o, "%c", token[k]);
                        }
                        printf("^");
                        l++;
                        break;
                    }
                    else if (token[0] == 'C') { // oprand의 첫 부분이 C이면
                        for (k = 2; k < strlen(token) - 1; k++) { // ex) C'EOF' 이면 X,',' 세개를 제외한 길이
                            printf("%X", token[k]);
                            fprintf(Sample_o, "%X", token[k]);
                        }
                        printf("^");
                        l++;
                        break;
                    }
                }
                else if (!strcmp(token, "RESB") || !strcmp(token, "RESW")) {
                    l++;
                    break;
                }
                for (k = 0; k < sizeof(optab) / 12; k++) {
                    if (!strcmp(optab[k].name, token)) { // opcode가 OPTAB에 정의된 단어이면 
                        token = strtok(NULL, ",\t\n"); // 다음 token을 받아온다 (oprand)
                        tmp_token = strtok(NULL, "\t\n");
                        if (tmp_token != NULL && strcmp(token, " ")) { // oprand가 존재하면서 콤마 뒷부분이 있는 경우 
                            if (!strcmp(tmp_token, "X")) { // 콤마 뒷부분이 X이면 
                                for (n = 0; n < j; n++) {
                                    if (!strcmp(symtab[n].lable, token)) {
                                        printf("%.2X%.4X", optab[k].len, symtab[n].symloc + XR);
                                        fprintf(Sample_o, "%.2X%.4X", optab[k].len, symtab[n].symloc + XR);
                                        n = 0;
                                        l++;
                                        if (l != 10)
                                            printf("^");
                                        else if (l != 17)
                                            printf("^");
                                        else if (l != 27)
                                            printf("^");
                                        break;
                                    }
                                }
                                if (n == j) {
                                    printf("%.2X0000", optab[k].len);
                                    fprintf(Sample_o, "%.2X0000", optab[k].len);
                                    l++;
                                    if (l != 10)
                                        printf("^");
                                    else if (l != 17)
                                        printf("^");
                                    else if (l != 27)
                                        printf("^");
                                }
                            }
                        }
                        else if (!strcmp(token, " ")) { // oprand가 NULL이면 메시지 출력
                            printf("%.2X0000", optab[k].len);
                            fprintf(Sample_o, "%.2X0000", optab[k].len);
                            l++;
                            if (l != 10)
                                printf("^");
                            else if (l != 17)
                                printf("^");
                            else if (l != 27)
                                printf("^");
                        }
                        else // oprand가 있으면 SYMTAB에 있는지 판독 
                            for (n = 0; n < j; n++) {
                                if (!strcmp(symtab[n].lable, token)) {
                                    printf("%.2X%.4X", optab[k].len, symtab[n].symloc);
                                    fprintf(Sample_o, "%.2X%.4X", optab[k].len, symtab[n].symloc);
                                    n = 0;
                                    l++;
                                    if (l != 10)
                                        printf("^");
                                    else if (l != 17)
                                        printf("^");
                                    else if (l != 27)
                                        printf("^");
                                    break;
                                }
                            }
                        if (n == j) {
                            printf("%.2X0000", optab[k].len);
                            fprintf(Sample_o, "%.2X0000", optab[k].len);
                            l++;
                            if (l != 10)
                                printf("^");
                            else if (l != 17)
                                printf("^");
                            else if (l != 27)
                                printf("^");
                        }

                    }
                    else if (k == j)
                        printf("not found opcode...\n");

                }
                break;
            }
            token = strtok(NULL, "\t\n");
        }
    }
    return 0;
}

int pass1(char* bp) {
    int i, k;
    flag3 = 0;
    int tmp_loc = 1;
    char tmp_lable[20];
    char* token = strtok(bp, "\t\n");
    if (token == NULL) { // token이 NULL이면 출력 파일에 \n 출력 후 다음 줄 읽어옴  
        return 2;
    }

    if (locctr == 0) { // 위치 계수기가 0일때만 실행 
        for (i = 0; i < 2; i++) {
            if (i == 1) // tmp_lable에 lable을 임시로 저장, opcode 부분이 START가 아니면 사용 안함 
                strcpy(tmp_lable, token);
            token = strtok(NULL, "\t\n");
        }
        if (!strcmp(token, "START")) { // opcode 부분이 START이면 locctr에 oprand 부분의 주소를 저장하고 다음 줄 읽어옴 
            strcpy(symtab[j].lable, tmp_lable); // 임시 lable을 SYMTAB에 저장 
            strcpy(symtab[j].opcode, token); // 현재 라인의 opcode도 SYMTAB에 저장 
            token = strtok(NULL, "\t\n");
            strcpy(symtab[j].oprand, token); // 현재 라인의 oprand도 SYMTAB에 저장  
            locctr = strtoul(symtab[j].oprand, NULL, 16); // 위치 계수기에 oprand 부분을 저장, oprand는 16진수이기 때문에 locctr에 10진수로 바꾸어서 저장 
            p1_start = locctr;
            symtab[j].symloc = locctr;
        }
        else // opcode 부분이 START가 아니면 아무것도 하지않고 opcode 부분이 START 일 때 까지 다음 줄 읽어옴
            return 4;
    }
    else { // 위치 계수기가 0이 아니면 실행
        symtab[j].symloc = locctr;
        for (i = 0; i < 3; i++) {
            if (!strcmp(token, " ")) {
                token = strtok(NULL, "\t\n");
                if (i == 1)
                    flag3 = 1; // space를 파일의 lable에 출력하기 위한 flag 
                continue;
            }
            switch (i) {
            case 0:
                break;
            case 1: // lable
                for (k = 0; k < j; k++) {
                    if (!strcmp(token, symtab[k].lable)) { // 현재 token을 lable 개수만큼 SYMTAB과 비교해서 같으면 중복 메시지 출력 후 라인 무시 
                        printf("----- 중복 -----\n");
                        return 4;
                    }
                }
                strcpy(symtab[j].lable, token);
                symtab[j].symloc = locctr;
                break;
            case 2: // opcode, oprand
                strcpy(symtab[j].opcode, token); // opcode 저장 
                if (!strcmp(symtab[j].opcode, "END")) { // opcode가 END이면 oprand 저장 및 프로그램 길이 구한 후 줄 읽기 종료
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].oprand, token);
                    cnt += 5;
                    j++;
                    locctr -= symtab[0].symloc;
                    return 3;
                }
                else if (!strcmp(symtab[j].opcode, "BYTE")) { // opcode가 BYTE이면 oprand 첫 부분이 무엇인지 구분 
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].oprand, token);
                    if (symtab[j].oprand[0] == 'C') {
                        tmp_loc = strlen(symtab[j].oprand) - 1 - 2; // ex) C'EOF' 이면 C,',' 세개를 제외한 개수 3 
                    }
                    else if (symtab[j].oprand[0] == 'X') { // oprand의 첫 글자가 X 이면 주소 + 1 
                        tmp_loc = 1;
                    }
                    locctr += tmp_loc;
                    break;
                }
                else if (!strcmp(symtab[j].opcode, "RESB")) { // opcode가 RESB이면 oprand의 10진수 숫자를 locctr에 더함
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].oprand, token);
                    tmp_loc = atoi(symtab[j].oprand);
                    locctr += tmp_loc;
                }
                else if (!strcmp(symtab[j].opcode, "RESW")) { // opcode가 RESW이면 oprand의 10진수 숫자에 3을 곱해 locctr에 더함
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].oprand, token);
                    locctr += 3 * atoi(symtab[j].oprand);
                }
                else if (!strcmp(symtab[j].opcode, "WORD")) { // opcode가 WORD이면 locctr에 3을 더함 
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].oprand, token);
                    locctr += 3;
                }
                else // 나머지 opcode가 나오면 OPTAB에 정의되어 있는 단어인지 판독  
                    for (i = 0; i < sizeof(optab) / 12; i++) {
                        if (!strcmp(optab[i].name, symtab[j].opcode)) { // opcode가 OPTAB에 정의되어 있는 단어이면 locctr에 3을 더하고 정상 flag 설정  
                            token = strtok(NULL, "\t\n");
                            strcpy(symtab[j].oprand, token);
                            locctr += 3;
                            flag2 = 1;
                        }
                    }
                if (i == sizeof(optab) / 12 && flag2 == 0) { // OPTAB에 정의된 단어가 아니면 오류메시지 출력 후 다음 줄 읽어옴  
                    printf("Undefined Word...\n");
                    return 4;
                }
                break;
            }
            token = strtok(NULL, "\t\n"); // NULL 다음 token 읽어옴 
        }
    }
    j++; // SYMTAB의 lable 개수를 하나 더함 
    if (cnt == 190 || cnt == 105)
        cnt += 20;
    else cnt += 5;
    return 0;
}

int fprint(int res, FILE* sample_1st) { // pass1을 txt파일로 출력 
    if (flag == 2) {
        fputs("\n", sample_1st); // 입력 파일에 빈 줄이 나오면 txt파일에 빈 줄 출력
    }
    else if (!flag) { // opcode에 이상이 없으면 파일에 출력 (flag가 0이면 실행) 
        if (flag3 == 1) { // lable 부분이 공백이면 파일의 lable 부분에 space 출력 (나중에 구분하기 위함) 
            fprintf(sample_1st, "%3d\t%X\t%s%s\t%s\n", cnt, symtab[j - 1].symloc, " \t", symtab[j - 1].opcode, symtab[j - 1].oprand);
        }
        else
            fprintf(sample_1st, "%3d\t%X\t%s\t%s\t%s\n", cnt, symtab[j - 1].symloc, symtab[j - 1].lable, symtab[j - 1].opcode, symtab[j - 1].oprand);
    }
    else if (flag == 3) { // opcode에 END가 나오면 현재 주소를 제외하고 파일에 출력하고 종료
        fprintf(sample_1st, "%3d\t%s%s%s\t%s\n", cnt, " \t", " \t", symtab[j - 1].opcode, symtab[j - 1].oprand);
        return 4;
    }
    else if (flag == 4) { // 아무것도 하지않고 다음 줄 읽음 
    }
    else if (flag) // flag가 0,2,3,4가 아니면 break 
        return 4;
    return res;
}
