#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
struct OPTAB { // 연산코드 테이블
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
    //어셈블러 지시자
    { "START",5 },{ "END",2 },{ "BYTE",1 },{ "RESW",4 },{ "RESB",3 },{ "WORD",6 }
};
struct SYMTAB { // 레이블에 대응하는 주소 값을 저장하는 테이블
    char symtab_label[10];
    int loc;
}symtab[200];
//변수들
int symtab_size = 0; // symtab의 원소 개수
int startAdress; // 목적 프로그램의 시작 주소
int programLength; // 목적 프로그램의 총 길이
char opcode[10], label[10], operand[10]; // 연산자, 레이블, 피연산자
int LOCCTR = 0; // 레이블에 대응되는 주소 값을 추적하기 위한 변수
int objectcode = 0x000000; // 목적코드
int operandAddress; // 피연산자의 주소
int symtab_index = 0; // 레이블의 주소를 symtab에서 찾기 위한 인덱스
int opcode_index = 0; // 연산자의 코드를 optab에서 찾기 위한 인덱스
char objectcode_text[100]; // 목적코드를 문자열로 변환시키기 위한 변수
char text_record[100]; // 목적프로그램의 T레코드의 목적 코드를 저장하기 위한 변수
int text_count = 0; // T레코드 한줄에 있는 목적 코드 개수
int TS; // 해당 레코드의 시작 주소
FILE* fp; // 파일 입력
//opcode 테이블에서 탐색
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
//symbol 테이블에서 탐색
int searchsymbol(char* cp) {
    int i;
    char* dp = malloc(strlen(cp) + 1); // 수정된 메모리 할당
    strcpy(dp, cp);
    if (cp[strlen(cp) - 2] == ',') {
        dp = strtok(dp, ",");
    }
    for (i = 0; i < symtab_size; ++i) {
        if (strcmp(dp, symtab[i].symtab_label) == 0) {
            symtab_index = i;
            free(dp); // 메모리 해제
            return TRUE;
        }
    }
    free(dp); // 수정된 메모리 해제 위치
    return FALSE;
}

// label, opcode, operand 구하기
void getCode(char* buf) {
    char* cp;
    int a, i, sw = FALSE;
    strcpy(opcode, ""); strcpy(label, ""); strcpy(operand, "");
    for (cp = strtok(buf, " \t\n"); cp != NULL;) {
        a = atoi(cp);
        if (!a) { // 숫자가 아니라 문자라면
            for (i = 0; i < strlen(cp); ++i) {
                if (cp[i] < 65 || cp[i]>90) {
                    sw = TRUE;
                    break;
                }
            }
            if (sw == FALSE) { // 기타문자가 아닌 알파벳 문자라면
                if (searchop(cp) == TRUE) { // 줄에서 두번째가 opcode라면 opcode로 인식
                    strcpy(opcode, cp);
                    if (strcmp(opcode, "RSUB") != 0) { // RSUB는 피연산자가 없기때문에
                        cp = strtok(NULL, " \t\n");
                        strcpy(operand, cp);
                    }
                }
                else { // 줄에서 두번째가 opcode가 아니라면 label로 인식
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
//텍스트 레코드 출력
void write_T_record() {
    printf("T%.6X%.2X%s\n", TS, strlen(text_record) / 2, text_record); // 시작주소 목적코드길이 목적코드 출력
    strcpy(text_record, "");
    text_count = 0;
}
//opcode가 BYTE일때 목적코드를 구하기
int get_byte_code(char* cp) {
    int i, n, nsum, k, j;
    if (operand[0] == 'C') { // C의 경우 
        nsum = 0;
        for (i = 2; i < strlen(cp) - 2; ++i) {
            nsum += (int)cp[i];
            nsum = nsum << 8;
        }
        nsum += cp[strlen(cp) - 2];
    }
    else if (operand[0] == 'X') { // X의 경우
        nsum = 0;
        for (i = 2; i < strlen(cp) - 2; ++i) {
            if (cp[i] > 65 && cp[i] < 90) // 알파벳이라면
                n = (int)cp[i] - 55;
            else // 숫자라면
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
//loc 구하기
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
//패스 1
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
                if (searchsymbol(label) == TRUE) { // 레이블이름이 중복이라면
                    fprintf(stderr, "Label already exists...\n");
                }
                else {
                    if (strcmp(label, "") != 0) { //레이블이 있는 경우 등록
                        strcpy(symtab[symtab_size].symtab_label, label);
                        symtab[symtab_size].loc = LOCCTR;
                        ++symtab_size;
                    }
                }
                if (searchop(opcode) == TRUE) { // loc증가
                    plus_locctr(opcode);
                }
            }
        }
        programLength = LOCCTR - startAdress; // 프로그램 총 길이
    }
}
//패스 2
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
    printf("H%s\t%.6X%.6X\n", label, objectcode, programLength); // H레코드 출력
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        getCode(buf);
        if (strcmp(opcode, "END") != 0) {
            if (buf[0] != '.') {
                if (searchop(opcode)) { // opcode가 optab에 있다면
                    plus_locctr(opcode);
                    if (strcmp(opcode, "BYTE") == 0) { // BTYE일때 목적코드
                        objectcode = get_byte_code(operand);
                        n = strlen(operand) - 3;
                        sprintf(objectcode_text, "%.*X", n, objectcode); // 목적코드 문자열로 전환
                        strcat(text_record, objectcode_text); // T레코드에 합치기
                        ++text_count;
                    }
                    else if (strcmp(opcode, "WORD") == 0) { //WORD일때 목적코드
                        objectcode = 0x000000;
                        objectcode += atoi(operand); // 피연산자가 상수이기때문에
                        sprintf(objectcode_text, "%.6X", objectcode); // 목적코드 문자열로 전환
                        strcat(text_record, objectcode_text); // T레코드에 합치기
                        ++text_count;
                    }
                    else if (strcmp(opcode, "RESW") != 0 && strcmp(opcode, "RESB") != 0) { // 그외에 목적코드
                        if (searchsymbol(operand)) { //symtab에 있으면 해당되는 loc 등록
                            operandAddress = symtab[symtab_index].loc;
                        }
                        else {
                            if (strcmp(operand, "") != 0) { // 피연산자가 존재했는데도 못찾은 거라면 피연산자 에러
                                operandAddress = 0;
                                fprintf(stderr, "The operand does not exist on the symboltable....\n", opcode, operand);
                            }
                            operandAddress = 0;
                        }
                        objectcode = optab[opcode_index].code; // 연산자코드 합성
                        objectcode = objectcode << 16;
                        objectcode += operandAddress; // 피연산자 주소 합성
                        if (operand[strlen(operand) - 2] == ',') { // X비트 사용시
                            objectcode += 0x008000;
                        }
                        sprintf(objectcode_text, "%.6X", objectcode); // 목적코드 문자열로 전환
                        strcat(text_record, objectcode_text); // T레코드에 합치기
                        ++text_count;
                    }
                    else if (strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
                        //RESW 와 RESB는 목적코드가 없고, T레코드에서 이 다음부터 출력되기 때문에
                        if (strcmp(text_record, "") != 0) { // 목적코드가 있다면 출력
                            write_T_record();
                            TS = LOCCTR; // T레코드 시작주소 변경
                        }
                        TS = LOCCTR; // 없다면 T레코드 시작주소만 변경
                    }
                    if (text_count == 10) { // 코드의개수가 10개가 되면 출력
                        write_T_record();
                        TS = LOCCTR; // T레코드 시작주소 변경
                    }
                }
            }
        }
    }
    write_T_record(); // 마지막 줄 출력
    printf("E%.6X\n", startAdress); // E레코드 출력 
}
void openfile() {
    if ((fp = fopen("scrfile.txt", "r")) == NULL) {
        fprintf(stderr, "file not found...\n"); exit(1);
    }
}
int main() {
    openfile();
    pass_one(fp); // 패스1
    openfile();
    pass_two(fp); // 패스2
}