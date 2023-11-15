#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
struct OPTAB { // �����ڵ� ���̺�
    char name[8];
    int code;
}optab[] = {
    { "ADD",0x18 },{ "ADDF",0x58 },{ "ADDR",0x90 },{ "AND",0x40 },{ "CLEAR",0xB4 },
    { "COMP",0x28 },{ "COMPF",0x88 },{ "COMPR",0xA0 },{ "DIV",0x24 },{ "DIVF",0x64 },
    { "DIVR",0x9C },{ "FIX",0xC4 },{ "FLOAT",0xC0 },{ "HIO",0xF4 },{ "J",0x3C },
    { "JEQ",0x30 },{ "JGT",0x34 },{ "JLT",0x38 },{ "JSUB",0x48 },{ "LDA",0x00 },
    { "LDB",0x68 },{ "LDCH",0x50 },{ "LDF",0x70 },{ "LDL",0x08 },{ "LDS",0x6C },
    { "LDT",0x74 },{ "LDX",0x04 },{ "LPS",0xD0 },{ "MUL",0x20 },{ "MULF",0x60 },
    { "MULR",0x98 },{ "NORM",0xC8 },{ "OR",0x44 },{ "RD",0xD8 },{ "RMO",0xAC },
    { "RSUB",0x4C },{ "SHIFTL",0xA4 },{ "SHIFTR",0xA8 },{ "SIO",0xF0 },{ "SSK",0xEC },
    { "STA",0x0C },{ "STB",0x78 },{ "STCH",0x54 },{ "STF",0x80 },{ "STI",0xD4 },
    { "STL",0x14 },{ "STS",0x7C },{ "STSW",0xE8 },{ "STT",0x84 },{ "STX",0x10 },
    { "SUB",0x1C },{ "SUBF",0x5C },{ "SUBR",0x94 },{ "SVC",0xB0 },{ "TD",0xE0 },
    { "TIO",0xF8 },{ "TIX",0x2C },{ "TIXR",0xB8 },{ "WD",0xDC },
    //����� ������
    { "START",5 },{ "END",2 },{ "BYTE",1 },{ "RESW",4 },{ "RESB",3 },{ "WORD",6 }
};
struct SYMTAB { // ���̺� �����ϴ� �ּ� ���� �����ϴ� ���̺�
    char symtab_label[10];
    int loc;
}symtab[200];
//������
int symtab_size = 0; // symtab�� ���� ����
int startAdress; // ���� ���α׷��� ���� �ּ�
int programLength; // ���� ���α׷��� �� ����
char opcode[10], label[10], operand[10]; // ������, ���̺�, �ǿ�����
int LOCCTR = 0; // ���̺� �����Ǵ� �ּ� ���� �����ϱ� ���� ����
int objectcode = 0x000000; // �����ڵ�
int operandAddress; // �ǿ������� �ּ�
int symtab_index = 0; // ���̺��� �ּҸ� symtab���� ã�� ���� �ε���
int opcode_index = 0; // �������� �ڵ带 optab���� ã�� ���� �ε���
char objectcode_text[100]; // �����ڵ带 ���ڿ��� ��ȯ��Ű�� ���� ����
char text_record[100]; // �������α׷��� T���ڵ��� ���� �ڵ带 �����ϱ� ���� ����
int text_count = 0; // T���ڵ� ���ٿ� �ִ� ���� �ڵ� ����
int TS; // �ش� ���ڵ��� ���� �ּ�
FILE* fp; // ���� �Է�
//opcode ���̺��� Ž��
int searchop(char* cp) {
    int i;
    for (i = 0; i < sizeof(optab) / sizeof(optab[0]); ++i) {
        if (strcmp(cp, optab[i].name) == 0) {
            opcode_index = i;
            return TRUE;
        }
    }
    return FALSE;
}
//symbol ���̺��� Ž��
int searchsymbol(char* cp) {
    int i;
    char* dp = malloc(strlen(cp) + 1); // ������ �޸� �Ҵ�
    strcpy(dp, cp);
    if (cp[strlen(cp) - 2] == ',') {
        dp = strtok(dp, ",");
    }
    for (i = 0; i < symtab_size; ++i) {
        if (strcmp(dp, symtab[i].symtab_label) == 0) {
            symtab_index = i;
            free(dp); // �޸� ����
            return TRUE;
        }
    }
    free(dp); // ������ �޸� ���� ��ġ
    return FALSE;
}

// label, opcode, operand ���ϱ�
void getCode(char* buf) {
    char* cp;
    int a, i, sw = FALSE;
    strcpy(opcode, ""); strcpy(label, ""); strcpy(operand, "");
    for (cp = strtok(buf, " \t\n"); cp != NULL;) {
        a = atoi(cp);
        if (!a) { // ���ڰ� �ƴ϶� ���ڶ��
            for (i = 0; i < strlen(cp); ++i) {
                if (cp[i] < 65 || cp[i]>90) {
                    sw = TRUE;
                    break;
                }
            }
            if (sw == FALSE) { // ��Ÿ���ڰ� �ƴ� ���ĺ� ���ڶ��
                if (searchop(cp) == TRUE) { // �ٿ��� �ι�°�� opcode��� opcode�� �ν�
                    strcpy(opcode, cp);
                    if (strcmp(opcode, "RSUB") != 0) { // RSUB�� �ǿ����ڰ� ���⶧����
                        cp = strtok(NULL, " \t\n");
                        strcpy(operand, cp);
                    }
                }
                else { // �ٿ��� �ι�°�� opcode�� �ƴ϶�� label�� �ν�
                    strcpy(label, cp);
                    cp = strtok(NULL, " \t\n");
                    strcpy(opcode, cp);
                    cp = strtok(NULL, " \t\n");
                    strcpy(operand, cp);
                }
            }
        }
        cp = strtok(NULL, " \t\n");
    }
}
//�ؽ�Ʈ ���ڵ� ���
void write_T_record() {
    printf("T%.6X%.2X%s\n", TS, strlen(text_record) / 2, text_record); // �����ּ� �����ڵ���� �����ڵ� ���
    strcpy(text_record, "");
    text_count = 0;
}
//opcode�� BYTE�϶� �����ڵ带 ���ϱ�
int get_byte_code(char* cp) {
    int i, n, nsum, k, j;
    if (operand[0] == 'C') { // C�� ��� 
        nsum = 0;
        for (i = 2; i < strlen(cp) - 2; ++i) {
            nsum += (int)cp[i];
            nsum = nsum << 8;
        }
        nsum += cp[strlen(cp) - 2];
    }
    else if (operand[0] == 'X') { // X�� ���
        nsum = 0;
        for (i = 2; i < strlen(cp) - 2; ++i) {
            if (cp[i] > 65 && cp[i] < 90) // ���ĺ��̶��
                n = (int)cp[i] - 55;
            else // ���ڶ��
                n = (int)cp[i] - 48;
            nsum += n;
            nsum = nsum << 4;
        }
        if (cp[strlen(cp) - 2] > 65 && cp[strlen(cp) - 2] < 90)
            nsum += cp[strlen(cp) - 2] - 55;
        else
            nsum += cp[strlen(cp) - 2] - 48;
    }
    return nsum;
}
//loc ���ϱ�
void plus_locctr(char* opcode) {
    if (strcmp(opcode, "WORD") == 0) {
        LOCCTR += 3;
    }
    else if (strcmp(opcode, "RESW") == 0) {
        LOCCTR += 3 * atoi(operand);
    }
    else if (strcmp(opcode, "RESB") == 0) {
        LOCCTR += atoi(operand);
    }
    else if (strcmp(opcode, "BYTE") == 0) {
        if (operand[0] == 'C')
            LOCCTR += strlen(operand) - 3;
        else if (operand[0] == 'X')
            LOCCTR += (strlen(operand) - 3) / 2;
    }
    else {
        LOCCTR += 3;
    }
}
//�н� 1
void pass_one(FILE* fp) {
    char buf[80];
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        getCode(buf);
        if (strcmp(opcode, "START") == 0) {
            LOCCTR = strtoul(operand, NULL, 16);
            startAdress = LOCCTR;
            continue;
        }
        if (strcmp(opcode, "END") != 0) {
            if (buf[0] != '.') {
                if (searchsymbol(label) == TRUE) { // ���̺��̸��� �ߺ��̶��
                    fprintf(stderr, "Label already exists...\n");
                }
                else {
                    if (strcmp(label, "") != 0) { //���̺��� �ִ� ��� ���
                        strcpy(symtab[symtab_size].symtab_label, label);
                        symtab[symtab_size].loc = LOCCTR;
                        ++symtab_size;
                    }
                }
                if (searchop(opcode) == TRUE) { // loc����
                    plus_locctr(opcode);
                }
            }
        }
        programLength = LOCCTR - startAdress; // ���α׷� �� ����
    }
}
//�н� 2
void pass_two(FILE* fp) {
    LOCCTR = startAdress;
    TS = LOCCTR;
    char buf[80];
    int n;
    int text_sw = FALSE;
    strcpy(text_record, "");
    fgets(buf, sizeof(buf), fp);
    getCode(buf);
    if (strcmp(opcode, "START") == 0) {
        objectcode += strtoul(operand, NULL, 16);
    }
    printf("H%s\t%.6X%.6X\n", label, objectcode, programLength); // H���ڵ� ���
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        getCode(buf);
        if (strcmp(opcode, "END") != 0) {
            if (buf[0] != '.') {
                if (searchop(opcode)) { // opcode�� optab�� �ִٸ�
                    plus_locctr(opcode);
                    if (strcmp(opcode, "BYTE") == 0) { // BTYE�϶� �����ڵ�
                        objectcode = get_byte_code(operand);
                        n = strlen(operand) - 3;
                        sprintf(objectcode_text, "%.*X", n, objectcode); // �����ڵ� ���ڿ��� ��ȯ
                        strcat(text_record, objectcode_text); // T���ڵ忡 ��ġ��
                        ++text_count;
                    }
                    else if (strcmp(opcode, "WORD") == 0) { //WORD�϶� �����ڵ�
                        objectcode = 0x000000;
                        objectcode += atoi(operand); // �ǿ����ڰ� ����̱⶧����
                        sprintf(objectcode_text, "%.6X", objectcode); // �����ڵ� ���ڿ��� ��ȯ
                        strcat(text_record, objectcode_text); // T���ڵ忡 ��ġ��
                        ++text_count;
                    }
                    else if (strcmp(opcode, "RESW") != 0 && strcmp(opcode, "RESB") != 0) { // �׿ܿ� �����ڵ�
                        if (searchsymbol(operand)) { //symtab�� ������ �ش�Ǵ� loc ���
                            operandAddress = symtab[symtab_index].loc;
                        }
                        else {
                            if (strcmp(operand, "") != 0) { // �ǿ����ڰ� �����ߴµ��� ��ã�� �Ŷ�� �ǿ����� ����
                                operandAddress = 0;
                                fprintf(stderr, "The operand does not exist on the symboltable....\n", opcode, operand);
                            }
                            operandAddress = 0;
                        }
                        objectcode = optab[opcode_index].code; // �������ڵ� �ռ�
                        objectcode = objectcode << 16;
                        objectcode += operandAddress; // �ǿ����� �ּ� �ռ�
                        if (operand[strlen(operand) - 2] == ',') { // X��Ʈ ����
                            objectcode += 0x008000;
                        }
                        sprintf(objectcode_text, "%.6X", objectcode); // �����ڵ� ���ڿ��� ��ȯ
                        strcat(text_record, objectcode_text); // T���ڵ忡 ��ġ��
                        ++text_count;
                    }
                    else if (strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
                        //RESW �� RESB�� �����ڵ尡 ����, T���ڵ忡�� �� �������� ��µǱ� ������
                        if (strcmp(text_record, "") != 0) { // �����ڵ尡 �ִٸ� ���
                            write_T_record();
                            TS = LOCCTR; // T���ڵ� �����ּ� ����
                        }
                        TS = LOCCTR; // ���ٸ� T���ڵ� �����ּҸ� ����
                    }
                    if (text_count == 10) { // �ڵ��ǰ����� 10���� �Ǹ� ���
                        write_T_record();
                        TS = LOCCTR; // T���ڵ� �����ּ� ����
                    }
                }
            }
        }
    }
    write_T_record(); // ������ �� ���
    printf("E%.6X\n", startAdress); // E���ڵ� ��� 
}
void openfile() {
    if ((fp = fopen("scrfile.txt", "r")) == NULL) {
        fprintf(stderr, "file not found...\n"); exit(1);
    }
}
int main() {
    openfile();
    pass_one(fp); // �н�1
    openfile();
    pass_two(fp); // �н�2
}