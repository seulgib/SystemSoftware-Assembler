#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100
#define XREG 32768 // X �������� ���� �� 
int pass1(char* bp);
int writeFile(int res, FILE* sample_1st); // pass 1�� ���� ����� ���Ͽ� ���

int p1_start, existOpcode;//opcode�� OPTAB�� ���ǵ� �ܾ�����?
int locctr = 0, flag=0, flag3=0, j=0, res=0; // ��ġ ����� LOCCTR 
//int p2_locctr = 0;
// flag: pass 1 ���� ����, ��������, �̺�Ʈ �� ���� ��Ÿ��
// flag3: space�� ������ lable�� ����ϱ� ���� flag
//j: SYMTAB�� ���̺� �߰��� ������ ����
//res: ���� ��½� ���� Ż�� �÷���



/*
int p2_start = 0;
int l = 0, T = 10;
int a = 1, b = 11;
*/

//symtab��  optable ����
struct SYMTAB {
    int symloc; // lable�� �ּ� 
    //int object;->pass2 �����Ҷ� �ʿ�
    char label[10], opcode[10], operand[10];
} symtab[100];


//instruction �̸��� ����
struct  OPTAB {
    char name[8];
    int len;
}optab[] = { {"LDA", 00}, {"BYTE", 1},   {"RESB", 1},{"WORD", 3},{"RESW", 3},{"STA", 12}, {"TIX", 44}, {"STL", 20}, 
    {"JSUB", 72}, {"COMP", 40}, {"JEQ", 48},{"J", 60}, {"RSUB", 76},  {"LDX", 04},
    {"STCH", 84}, {"JLT", 56}, {"STX", 16}, {"LDCH", 80}, {"TD", 224}, {"RD", 216},{"WD", 220}
};



int main() {
    int i = 0;
    FILE* srcfile;
    srcfile = fopen("input.txt", "r");
    FILE* imediatefile;
    imediatefile = fopen("Sample.txt", "w+");
    FILE* objectfile;
    objectfile = fopen("Sample_o.txt", "w+");
    FILE* symfile;
    symfile = fopen("symfile.txt", "w+");
    char buffer[80];
    char* token;
    token = (char*)malloc(sizeof(char) * 20);

    //���� ����ó��
    if (!srcfile) {
        fprintf(stderr, "Can't open srcfile...\n"); exit(1);
    }
    if (!imediatefile) {
        fprintf(stderr, "Can't open intfile...\n"); exit(1);
    }
    if (!objectfile) {
        fprintf(stderr, "Can't open objfile...\n"); exit(1);
    }


    // pass 1
    
    while (fgets(buffer, sizeof(buffer), srcfile) != NULL) {
        flag = pass1(buffer);
        //�߰����� ����
        res = writeFile(res, imediatefile); if (res == 4) break;
     
    }
    //fclose(fp);

    printf("\n\n******* Intermediate File *******\n\n");
    printf("[locctr][label][opcode][operand]\n");
    /*fessk: ���� ������ 0���� �̵���Ű�� �Լ���� �Ѵ�.*/
    fseek(imediatefile, 0, SEEK_SET);
    while (fgets(buffer, sizeof(buffer), imediatefile) != NULL) { // �߰� ���� ���� ��� 
        printf("%s", buffer);
    }
    printf("\n\n");

    printf("\n******* Symbol Table *******\n\n\t[Symbol]  [Value]\n\n");
    for (i = 1; i < MAX; i++) {
        if (strlen(symtab[i].label) !=0)
            printf("\t%s\t%X\n", symtab[i].label, symtab[i].symloc);
            fprintf(symfile,"%s\t%X\n", symtab[i].label, symtab[i].symloc);
    }
    printf("\n\n");

    // pass 2    

     
    printf("******* Object Code *******\n\n");
    /*
    rewind(sample_1st);//���������� �ʱ�ȭ
    while (fgets(buf, sizeof(buf), sample_1st) != NULL) { // Sample.txt ������ ���پ� �о��
        flag = pass2(buf, sample_o);
        if (flag == 1)//'END'�� ������ �� flag=1
            break;
    }
    */
    //���� close
    fclose(imediatefile);
    //fclose(sample_o);
    free(token);
    
    return 0;
}

int pass1(char* bp) {
    int i, k;
    flag3 = 0;///���� �� ��½� �ʿ�
    int tmp_loc = 1;//�ӽ���ġ��
    char tmp_lable[30];//�ӽö� ����
    char* token = strtok(bp, "\t\n");
    if (token == NULL) return 2; // token�� NULL�̸� \n ����ϵ��� (flag�� 2 ����)  
   
    if (locctr == 0) {//locctr==0�� ��쿡 ����
        for (i = 0; i < 2; i++) {
            if (i == 1)
                strcpy(tmp_lable, token);
            token = strtok(NULL, "\t\n");//���� token �о��
        }
        if (!strcmp(token, "START")) {
            strcpy(symtab[j].label, tmp_lable);
            strcpy(symtab[j].opcode, token);
            token = strtok(NULL, "\t\n");
            strcpy(symtab[j].operand, token);
            locctr = strtoul(symtab[j].operand, NULL, 16);
            p1_start = locctr;
            symtab[j].symloc = locctr;
        }
        else//opcode�� START�� �ƴϸ�, opcode�κ��� START�϶����� ������ �о��.
            return 4;
    }
    else { //locctr!= 0 �̸� ����(START ���� �ĺ���)
        symtab[j].symloc = locctr;
        for (i = 0; i < 3; i++) {//label, opcode, operand ó��
            if (!strcmp(token, " ")) {
                token = strtok(NULL, "\t\n");continue;
            }


            if (i == 0) {

            }
            else if (i == 1) {
                for (k = 0; k < j; k++) {
                    if (!strcmp(token, symtab[k].label)) {
                        // ���� token�� SYMTAB ��ȸ�ؼ� ���ؼ� ������ �ߺ� �޽��� ��� �� ���� ���� 
                        printf("�ߺ��� �ɺ������Դϴ�.\n");
                        return 4;
                    }
                }
                strcpy(symtab[j].label, token);
                symtab[j].symloc = locctr;
            }
            else if (i == 2) {
                strcpy(symtab[j].opcode, token); // opcode ���� 
                if (!strcmp(symtab[j].opcode, "END")) { // opcode�� END�̸� oprand ���� �� ���α׷� ���� ���� �� �� �б� ����
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    //cnt += 5;
                    j++;
                    locctr -= symtab[0].symloc;
                    return 3;
                }
                //Byte�� +1, Word�� +3
                else if (!strcmp(symtab[j].opcode, "BYTE")) { //opcode�� BYTE�̸� oprand ù �κ��� �������� ���� 
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    if (symtab[j].operand[0] == 'C') tmp_loc = strlen(symtab[j].operand) - 3; //����ǥ2���� C �����ؼ� ���ڱ��� ���ϱ�(ASCII)

                    else if (symtab[j].operand[0] == 'X') { // oprand�� ù ���ڰ� X�� ���(Hexa)
                        tmp_loc = 1;
                    }
                    locctr += tmp_loc;//locctr�� ����
                    break;
                }
                else if (!strcmp(symtab[j].opcode, "WORD")) { // opcode�� WORD�̸� locctr�� 3�� ���� 
                    token = strtok(NULL, "\t\n"); strcpy(symtab[j].operand, token);
                    locctr += 3;
                }
                //RESB�� 1*(operand), RESW�� 3*(opreand)
                else if (!strcmp(symtab[j].opcode, "RESB")) { // opcode�� RESB�̸� oprand�� 10���� ���ڸ� locctr�� ����
                    token = strtok(NULL, "\t\n"); strcpy(symtab[j].operand, token);
                    tmp_loc = 1 * atoi(symtab[j].operand);
                    locctr += tmp_loc;//RESB�� ���� BYTE�����̴ϱ� �״�� locctr�� �����ָ� ��.
                }
                else if (!strcmp(symtab[j].opcode, "RESW")) { // opcode�� RESW�̸� 1word=3byte
                    token = strtok(NULL, "\t\n"); strcpy(symtab[j].operand, token);
                    locctr += 3 * atoi(symtab[j].operand);
                }
                else // ������ opcode�� ������ OPTAB�� ���ǵǾ� �ִ� �ܾ����� �ǵ�  
                    for (i = 0; i < sizeof(optab) / sizeof(optab[0]); i++) {
                        if (!strcmp(optab[i].name, symtab[j].opcode)) {
                            // opcode�� OPTAB�� ���ǵǾ� �ִ� �ܾ��̸� locctr�� 3�� ���ϰ� ���� flag ����  
                            token = strtok(NULL, "\t\n");
                            strcpy(symtab[j].operand, token); locctr += 3;
                            existOpcode = 1;
                        }
                    }
                if (existOpcode == 0) { // OPTAB�� ���ǵ� �ܾ �ƴϸ� �����޽��� ��� �� ���� �� �о��  
                    printf("OPTAB�� ���ǵ��� ���� ��ɾ��Դϴ�.\n");
                    return 4;
                }
            }
            token = strtok(NULL, "\t\n"); // NULL ���� token �о�� 
        }
    }
    j++; // SYMTAB�� lable ���� ����
    
    return 0;
}

int writeFile(int res, FILE* sample_1st) { // pass1�� console�� ����ϸ鼭 ���ÿ� txt���Ϸ� ��� 
    switch (flag) {
    case 0:
        if (flag3 == 1) { // lable �κ��� �����̸� ������ lable �κп� space ��� (���߿� �����ϱ� ����) 
            fprintf(sample_1st, "%X\t%s%s\t%s\n", symtab[j - 1].symloc, " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
            break;
        }
        else
            fprintf(sample_1st, "%X\t%s\t%s\t%s\n", symtab[j - 1].symloc, symtab[j - 1].label, symtab[j - 1].opcode, symtab[j - 1].operand);
    case 2: 
        fputs("\n", sample_1st); // �Է� ���Ͽ� �� ���� ������ txt���Ͽ� �� �� ���
        break;
    case 3:
        fprintf(sample_1st, "%s%s%s\t%s\n", " \t", " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
        return 4;
        break;
    case 4:
        break;//�׳� pass
    default:
        return 4;




    }
}
