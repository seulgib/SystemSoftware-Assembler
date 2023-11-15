#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 300
#define XR 32768 // X �������� ���� �� 

int pass1(char* bp);
int pass2(char* bp, FILE* Sample_o);
//int find_optab();
int fprint(int res, FILE* sample_1st); // pass 1�� ���� ����� ���Ͽ� ����ϴ� �Լ�

//instruction �̸��� ����
struct  OPTAB {
    char name[8];
    int len;
} optab[] = { {"LDA", 00}, {"STA", 12}, {"TIX", 44}, {"STL", 20}, {"JSUB", 72}, {"COMP", 40}, {"JEQ", 48},
{"J", 60}, {"RSUB", 76}, {"BYTE", 1}, {"WORD", 3}, {"RESW", 3}, {"RESB", 1}, {"TD", 224}, {"RD", 216}, {"LDX", 04},
{"STCH", 84}, {"JLT", 56}, {"STX", 16}, {"WD", 220}, {"LDCH", 80} };


struct SYMTAB {
    char label[10], opcode[10], operand[10];
    int symloc; // lable�� �ּ� 
    int object;
} symtab[MAX];

int p1_start;
int locctr = 0; // ��ġ ����� LOCCTR 
int p2_locctr = 0;
int flag = 0; // pass 1 ���� �����ǵ�, loop����, �̺�Ʈ �� ���� �� �о�� �� �������� ��Ȳ�� �˷��ִ� flag 
int flag2; // opcode�� OPTAB�� ���ǵ� �ܾ����� �ƴ��� �˷��ִ� flag 
int j = 0; // ���������� SYMTAB�� �߰��� lable ���� count 
int cnt = 0;    // line number, �� �ٴ� 5�� ���� ( 5, 10, 15, 20, ..... , cnt)
int res = 0; // ���� ��½� loop�� Ż���� ���� flag  
int flag3 = 0; // space�� ������ lable�� ����ϱ� ���� flag 
int p2_start = 0;
int l = 0, T = 10;
int a = 1, b = 11;

int main() {
    int i = 0;
    FILE* fp;
    fp = fopen("input.txt", "r");
    FILE* sample_1st;
    sample_1st = fopen("Sample.txt", "w+");
    FILE* sample_o;
    sample_o = fopen("Sample_o.txt", "w+");
    char buf[80];
    char* token;

    //���� ����ó��
    if (fp == NULL) {
        fprintf(stderr, "input.txt is not found...\n"); exit(1);
    }
    if (sample_1st == NULL) {
        fprintf(stderr, "sample_1st.txt is not found...\n"); exit(1);
    }
    if (sample_o == NULL) {
        fprintf(stderr, "sample_o.txt is not found...\n"); exit(1);
    }


    // pass 1

    while (fgets(buf, sizeof(buf), fp) != NULL) {

        flag = pass1(buf);
        res = fprint(res, sample_1st);
        if (res == 4)
            break;
    }
    fclose(fp);
    /*rewind(): ���� ������ 0���� �̵���Ű��*/
    rewind(sample_1st);

    while (fgets(buf, sizeof(buf), sample_1st) != NULL) { // Sample.txt ���� ���� ��� 
        printf("%s", buf);
    }
    printf("\n\n");

    printf("\n******* Symbol Table *******\n\n\t[Symbol]  [Value]\n\n");

    for (i = 1; i < MAX; i++) {
        if (strlen(symtab[i].label) !=0)
            printf("\t%s\t%X\n", symtab[i].label, symtab[i].symloc);
    }
    printf("\n\n");

    // pass 2    

     
    printf("******* Object Code *******\n\n");
    rewind(sample_1st);//���������� �ʱ�ȭ
    while (fgets(buf, sizeof(buf), sample_1st) != NULL) { // Sample.txt ������ ���پ� �о��
        flag = pass2(buf, sample_o);
        if (flag == 1)//'END'�� ������ �� flag=1
            break;
    }

    //���� close
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
    if (token == NULL) // token�� NULL�̸� ���� �� �о��
        return 0;
    if (p2_start == 0) { //p2_start�� 0�̸� ���� 
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
                    if (token[0] == 'X') { // oprand�� ù�κ��� X�̸� 
                        for (k = 2; k < strlen(token) - 1; k++) {  // ex) X'F1' �̸� X,',' ������ ������ ���� 
                            printf("%c", token[k]);
                            fprintf(Sample_o, "%c", token[k]);
                        }
                        printf("^");
                        l++;
                        break;
                    }
                    else if (token[0] == 'C') { // oprand�� ù �κ��� C�̸�
                        for (k = 2; k < strlen(token) - 1; k++) { // ex) C'EOF' �̸� X,',' ������ ������ ����
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
                    if (!strcmp(optab[k].name, token)) { // opcode�� OPTAB�� ���ǵ� �ܾ��̸� 
                        token = strtok(NULL, ",\t\n"); // ���� token�� �޾ƿ´� (oprand)
                        tmp_token = strtok(NULL, "\t\n");
                        if (tmp_token != NULL && strcmp(token, " ")) { // oprand�� �����ϸ鼭 �޸� �޺κ��� �ִ� ��� 
                            if (!strcmp(tmp_token, "X")) { // �޸� �޺κ��� X�̸� 
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
                        else if (!strcmp(token, " ")) { // oprand�� NULL�̸� �޽��� ���
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
                        else // oprand�� ������ SYMTAB�� �ִ��� �ǵ� 
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

int pass1(char* bp) {
    int i, k;
    flag3 = 0;///���� �� ��½� �ʿ�
    int tmp_loc = 1;//�ӽ���ġ��
    char tmp_lable[20];//�ӽö� ����
    char* token = strtok(bp, "\t\n");
    if (token == NULL) return 2; // token�� NULL�̸� \n ����ϵ��� (flag�� 2 ����)  

    if (locctr == 0) { //locctr 0���� pass1 ���� 
        for (i = 0; i < 2; i++) {
            
            strcpy(tmp_lable, token);//��ū �ӽú����� �����س���
            token = strtok(NULL, "\t\n");//������ū ȣ��
        }
        
            strcpy(symtab[j].label, tmp_lable);
            strcpy(symtab[j].opcode, token); 
            token = strtok(NULL, "\t\n");
            strcpy(symtab[j].operand, token); //oprand�� �ɺ����̺� ����  

            //Start ��ġ��  
            locctr = strtoul(symtab[j].operand, NULL, 16); 
            p1_start = locctr;
            symtab[j].symloc = locctr;
        
    
    }
    else { //locctr!= 0 �̸� ����(START ���� �ĺ���)
        symtab[j].symloc = locctr;
        for (i = 0; i < 3; i++) {//label, opcode, operand ó��
            if (!strcmp(token, " ")) {
                token = strtok(NULL, "\t\n");continue;
            }
            switch (i) {
            case 0:
                break;
            case 1: //label ó��
                for (k = 0; k < j; k++) {
                    if (!strcmp(token, symtab[k].label)) { // ���� token�� lable ������ŭ SYMTAB�� ���ؼ� ������ �ߺ� �޽��� ��� �� ���� ���� 
                        printf("----- �ߺ� -----\n");
                        return 4;
                    }
                }
                strcpy(symtab[j].label, token);
                symtab[j].symloc = locctr;
                break;
            case 2: // opcode, oprand
                strcpy(symtab[j].opcode, token); // opcode ���� 
                if (!strcmp(symtab[j].opcode, "END")) { // opcode�� END�̸� oprand ���� �� ���α׷� ���� ���� �� �� �б� ����
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    cnt += 5;
                    j++;
                    locctr -= symtab[0].symloc;
                    return 3;
                }
                else if (!strcmp(symtab[j].opcode, "BYTE")) { // opcode�� BYTE�̸� oprand ù �κ��� �������� ���� 
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    if (symtab[j].operand[0] == 'C') {
                        tmp_loc = strlen(symtab[j].operand) - 1 - 2; // ex) C'EOF' �̸� C,',' ������ ������ ���� 3 
                    }
                    else if (symtab[j].operand[0] == 'X') { // oprand�� ù ���ڰ� X �̸� �ּ� + 1 
                        tmp_loc = 1;
                    }
                    locctr += tmp_loc;
                    break;
                }
                else if (!strcmp(symtab[j].opcode, "RESB")) { // opcode�� RESB�̸� oprand�� 10���� ���ڸ� locctr�� ����
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    tmp_loc = atoi(symtab[j].operand);
                    locctr += tmp_loc;
                }
                else if (!strcmp(symtab[j].opcode, "RESW")) { // opcode�� RESW�̸� oprand�� 10���� ���ڿ� 3�� ���� locctr�� ����
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    locctr += 3 * atoi(symtab[j].operand);
                }
                else if (!strcmp(symtab[j].opcode, "WORD")) { // opcode�� WORD�̸� locctr�� 3�� ���� 
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    locctr += 3;
                }
                else // ������ opcode�� ������ OPTAB�� ���ǵǾ� �ִ� �ܾ����� �ǵ�  
                    for (i = 0; i < sizeof(optab) / 12; i++) {
                        if (!strcmp(optab[i].name, symtab[j].opcode)) { // opcode�� OPTAB�� ���ǵǾ� �ִ� �ܾ��̸� locctr�� 3�� ���ϰ� ���� flag ����  
                            token = strtok(NULL, "\t\n");
                            strcpy(symtab[j].operand, token);
                            locctr += 3;
                            flag2 = 1;
                        }
                    }
                if (i == sizeof(optab) / 12 && flag2 == 0) { // OPTAB�� ���ǵ� �ܾ �ƴϸ� �����޽��� ��� �� ���� �� �о��  
                    printf("Undefined Word...\n");
                    return 4;
                }
                break;
            }
            token = strtok(NULL, "\t\n"); // NULL ���� token �о�� 
        }
    }
    j++; // SYMTAB�� lable ������ �ϳ� ���� 
    if (cnt == 190 || cnt == 105)
        cnt += 20;
    else cnt += 5;
    return 0;
}

int fprint(int res, FILE* sample_1st) { // pass1�� txt���Ϸ� ��� 
    if (flag == 2) {
        fputs("\n", sample_1st); // �Է� ���Ͽ� �� ���� ������ txt���Ͽ� �� �� ���
    }
    else if (!flag) { // opcode�� �̻��� ������ ���Ͽ� ��� (flag�� 0�̸� ����) 
        if (flag3 == 1) { // lable �κ��� �����̸� ������ lable �κп� space ��� (���߿� �����ϱ� ����) 
            fprintf(sample_1st, "%3d\t%X\t%s%s\t%s\n", cnt, symtab[j - 1].symloc, " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
        }
        else
            fprintf(sample_1st, "%3d\t%X\t%s\t%s\t%s\n", cnt, symtab[j - 1].symloc, symtab[j - 1].label, symtab[j - 1].opcode, symtab[j - 1].operand);
    }
    else if (flag == 3) { // opcode�� END�� ������ ���� �ּҸ� �����ϰ� ���Ͽ� ����ϰ� ����
        fprintf(sample_1st, "%3d\t%s%s%s\t%s\n", cnt, " \t", " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
        return 4;
    }
    else if (flag == 4) { // �ƹ��͵� �����ʰ� ���� �� ���� 
    }
    else if (flag) // flag�� 0,2,3,4�� �ƴϸ� break 
        return 4;
    return res;
}
