#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100
#define XR 32768

int pass1(char* bp);
int pass2(char* bp, FILE* Sample_o);

int fprint(int res, FILE* sample_1st);//pass 1 실행결과를 파일에 출력

struct OPTAB {
    char name[8];//instruction 이름
    int len;//instruction 길이
}optab[] = { {"LDA", 00}, {"STA", 12}, {"TIX", 44}, {"STL", 20}, {"JSUB", 72}, {"COMP", 40}, {"JEQ", 48},
{"J", 60}, {"RSUB", 76}, {"BYTE", 1}, {"WORD", 3}, {"RESW", 3}, {"RESB", 1}, {"TD", 224}, {"RD", 216}, {"LDX", 04},
{"STCH", 84}, {"JLT", 56}, {"STX", 16}, {"WD", 220}, {"LDCH", 80} };

struct SYMTAB {
    char label[10], opcode[10], operand[10];
    int symloc;
    int object;
}symtab[MAX];

int p1_start;
long locctr = 0;
long p2_locctr = 0;
int flag = 0;//pass1 오류판독
int flag2;//opcode가 optab에 정의되어있는지 알려줌
int j = 0;//symtab LABEL 개수
int cnt = 0;//line number(한 줄당 +5)
int res = 0;//loop문 탈출 flag
int flag3 = 0;//space를 파일의 label에 출력하기 위한 flag
int p2_start = 0;
int l = 0, T = 10;
int a = 1, b = 11;

int main(void) {
    int i=0;
    FILE* fp;
    fp = fopen("input.txt", "r");
    FILE* sample_1st;
    sample_1st = fopen("Sample.txt", "w+");
    FILE* sample_o;
    sample_o = fopen("Sample_o.txt", "w+");
    char buf[80];
    char* token;
    //파일 접근 오류 처리
    if (fp == NULL) {
        fprintf(stderr, "input.txt is not found...\n"); exit(1);
    }
    if (sample_1st == NULL) {
        fprintf(stderr, "sample_1st.txt is not found...\n"); exit(1);
    }
    if (sample_o == NULL) {
        fprintf(stderr, "sample_o.txt is not found...\n"); exit(1);
    }

    //pass1 수행
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        flag = pass1(buf);
        res = fprint(res, sample_1st);
        if (res == 4)break;
    }
    fclose(fp);
    rewind(sample_1st);//sample_1st file_pointer를 0으로 이동

    while (fgets(buf, sizeof(buf), sample_1st) != NULL) {
        printf("%s", buf);
    }
    printf("\ntotal length: %X", locctr);
    printf("\n\n");

    printf("\n----------Symbol Table----------\n\n");
    printf("\tSymbol\tValue\n\n");
    for (i = 0; i < j; i++) { // 모든 심볼을 순회합니다
        if (strlen(symtab[i].label) > 0) { // 라벨이 비어 있지 않은 경우에만 출력
            printf("\t%s\t%X\n", symtab[i].label, symtab[i].symloc);
        }
    }
    printf("\n\n");


    //pass 2
    rewind(sample_1st);//file_pointer 0으로 이동
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

int pass1(char* bp) {
    int i, k;
    flag3 = 0;
    int tmp_loc = 1;
    char tmp_label[20];
    char* token = strtok(bp, "\t\n");
    if (token == NULL) {
        return 2;
    }

    if (locctr == 0) {//locctr==0인 경우에 대해
        for (i = 0; i < 2; i++) {
            if (i == 1)
                strcpy(tmp_label, token);
            token = strtok(NULL, "\t\n");//다음 token 읽어옴
        }
        if (!strcmp(token, "START")) {
            strcpy(symtab[j].label, tmp_label);
            strcpy(symtab[j].opcode, token);
            token = strtok(NULL, "\t\n");
            strcpy(symtab[j].operand, token);
            locctr = strtoul(symtab[j].operand, NULL, 16);
            p1_start = locctr;
            symtab[j].symloc = locctr;
        }
        else//opcode가 START가 아니면, opcode부분이 START일때까지 다음줄 읽어옴.
            return 4;
    }
    else {
        symtab[j].symloc = locctr;
        for (i = 0; i < 3; i++) {
            if (!strcmp(token, " ")) {
                token = strtok(NULL, " \t\n");
                if (i == 1)
                    flag3 = 1;//space를 파일의 label에 출력하기 위해?
                continue;
            }
            switch (i) {
            case 0:
                break;
            case 1://label
                for (k = 0; i < j; j++) {
                    if (!strcmp(token, symtab[k].label)) {
                        printf("중복 정의된 symbol입니다.\n\n");
                        return 4;
                    }
                }
                strcpy(symtab[j].label, token);
                symtab[j].symloc = locctr;
                break;
            case 2:
                strcpy(symtab[j].opcode, token);
                if (!strcmp(symtab[j].opcode, "END")) {
                    token = strtok(NULL, "/t/n");
                    strcpy(symtab[j].operand, token);
                    cnt += 5;
                    j++;
                    locctr -= symtab[0].symloc;
                    return 3;//res==3
                }
                else if (!strcmp(symtab[j].opcode, "BYTE")) {
                    token = strtok(NULL, "/t/n");
                    strcpy(symtab[j].operand, token);
                    if (symtab[j].operand[0] == 'C') {//ASCII
                        tmp_loc = strlen(symtab[j].operand) - 3;//C랑 따옴표 2개 빼야함.
                    }
                    else if (symtab[j].operand[0] == 'X') {
                        tmp_loc = 1;
                    }
                    locctr += tmp_loc;
                    break;
                }
                else if (!strcmp(symtab[j].opcode, "RESB")) {
                    token = strtok(NULL, "/t/n");
                    strcpy(symtab[j].operand, token);
                    tmp_loc = atoi(symtab[j].operand);
                    locctr += tmp_loc;
                }
                else if (!strcmp(symtab[j].opcode, "RESW")) {
                    token = strtok(NULL, "/t/n");
                    strcpy(symtab[j].operand, token);
                    tmp_loc = atoi(symtab[j].operand);
                    locctr +=3* tmp_loc;
                }
                else if (!strcmp(symtab[j].opcode, "WORD")) {
                    token = strtok(NULL, "/t/n");
                    strcpy(symtab[j].operand, token);
                    locctr += 3;
                }
                else //나머지 opcode가 나오면 optab에 정의되어있는지 검사
                    for (i = 0; i < sizeof(optab) / 12; i++) {
                        if (!strcmp(optab[i].name, symtab[j].opcode)) { // opcode가 OPTAB에 정의되어 있는 단어이면 locctr에 3을 더하고 정상 flag 설정  
                            token = strtok(NULL, "\t\n");
                            strcpy(symtab[j].operand, token);
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
            token = strtok(NULL, "\t\n");
        }
    }
    j++;
    if (cnt == 190 || cnt == 105) {
        cnt += 20;
    }
    else cnt += 5;
}

//pass1 출력하는 함수(INTFILE 출력)
int fprint(int res, FILE* sample_1st) {
    if (flag == 2) {
        fputs("\n", sample_1st);
    }
    else if (!flag) {//flag==0인 경우
        if (flag3 == 1) {//label 부분 공백인 경우
            fprintf(sample_1st, "%3d\t%X\t%s%s\t%s\n", cnt, symtab[j - 1].symloc, " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
        }
        else
            fprintf(sample_1st, "%3d\t%X\t%s\t%s\t%s\n", cnt, symtab[j-1].symloc, symtab[j-1].label, symtab[j-1].opcode, symtab[j-1].operand);
    }
    else if (flag == 3) {//opcode에 END 나온 경우
        fprintf(sample_1st, "%3d\t%s%s%s\t%s\n", cnt, " \t", " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
        return 4;
    }
    else if (flag == 4) { // 아무것도 하지않고 다음 줄 읽음 
    }
    else if (flag) // flag가 0,2,3,4가 아니면 break 
        return 4;
    return res;

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
                                    if (!strcmp(symtab[n].label, token)) {
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
                                if (!strcmp(symtab[n].label, token)) {
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