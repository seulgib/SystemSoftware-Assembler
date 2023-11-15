#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100
#define XREG 32768 // X 레지스터 더할 값 
int pass1(char* bp);
int writeFile(int res, FILE* sample_1st); // pass 1의 실행 결과를 파일에 출력

int p1_start, existOpcode;//opcode가 OPTAB에 정의된 단어인지?
int locctr = 0, flag=0, flag3=0, j=0, res=0; // 위치 계수기 LOCCTR 
//int p2_locctr = 0;
// flag: pass 1 에서 오류, 루프정지, 이벤트 등 상태 나타냄
// flag3: space를 파일의 lable에 출력하기 위한 flag
//j: SYMTAB에 레이블 추가될 때마다 증가
//res: 파일 출력시 루프 탈출 플래그



/*
int p2_start = 0;
int l = 0, T = 10;
int a = 1, b = 11;
*/

//symtab과  optable 정의
struct SYMTAB {
    int symloc; // lable의 주소 
    //int object;->pass2 정의할때 필요
    char label[10], opcode[10], operand[10];
} symtab[100];


//instruction 이름과 길이
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

    //파일 에러처리
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
        //중간파일 생성
        res = writeFile(res, imediatefile); if (res == 4) break;
     
    }
    //fclose(fp);

    printf("\n\n******* Intermediate File *******\n\n");
    printf("[locctr][label][opcode][operand]\n");
    /*fessk: 파일 포인터 0으로 이동시키는 함수라고 한다.*/
    fseek(imediatefile, 0, SEEK_SET);
    while (fgets(buffer, sizeof(buffer), imediatefile) != NULL) { // 중간 파일 내용 출력 
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
    rewind(sample_1st);//파일포인터 초기화
    while (fgets(buf, sizeof(buf), sample_1st) != NULL) { // Sample.txt 파일을 한줄씩 읽어옴
        flag = pass2(buf, sample_o);
        if (flag == 1)//'END'를 만났을 때 flag=1
            break;
    }
    */
    //파일 close
    fclose(imediatefile);
    //fclose(sample_o);
    free(token);
    
    return 0;
}

int pass1(char* bp) {
    int i, k;
    flag3 = 0;///공백 라벨 출력시 필요
    int tmp_loc = 1;//임시위치값
    char tmp_lable[30];//임시라벨 저장
    char* token = strtok(bp, "\t\n");
    if (token == NULL) return 2; // token이 NULL이면 \n 출력하도록 (flag값 2 리턴)  
   
    if (locctr == 0) {//locctr==0인 경우에 대해
        for (i = 0; i < 2; i++) {
            if (i == 1)
                strcpy(tmp_lable, token);
            token = strtok(NULL, "\t\n");//다음 token 읽어옴
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
        else//opcode가 START가 아니면, opcode부분이 START일때까지 다음줄 읽어옴.
            return 4;
    }
    else { //locctr!= 0 이면 실행(START 읽은 후부터)
        symtab[j].symloc = locctr;
        for (i = 0; i < 3; i++) {//label, opcode, operand 처리
            if (!strcmp(token, " ")) {
                token = strtok(NULL, "\t\n");continue;
            }


            if (i == 0) {

            }
            else if (i == 1) {
                for (k = 0; k < j; k++) {
                    if (!strcmp(token, symtab[k].label)) {
                        // 현재 token을 SYMTAB 순회해서 비교해서 같으면 중복 메시지 출력 후 라인 무시 
                        printf("중복된 심볼정의입니다.\n");
                        return 4;
                    }
                }
                strcpy(symtab[j].label, token);
                symtab[j].symloc = locctr;
            }
            else if (i == 2) {
                strcpy(symtab[j].opcode, token); // opcode 저장 
                if (!strcmp(symtab[j].opcode, "END")) { // opcode가 END이면 oprand 저장 및 프로그램 길이 구한 후 줄 읽기 종료
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    //cnt += 5;
                    j++;
                    locctr -= symtab[0].symloc;
                    return 3;
                }
                //Byte는 +1, Word는 +3
                else if (!strcmp(symtab[j].opcode, "BYTE")) { //opcode가 BYTE이면 oprand 첫 부분이 무엇인지 구분 
                    token = strtok(NULL, "\t\n");
                    strcpy(symtab[j].operand, token);
                    if (symtab[j].operand[0] == 'C') tmp_loc = strlen(symtab[j].operand) - 3; //따옴표2개랑 C 제외해서 문자길이 구하기(ASCII)

                    else if (symtab[j].operand[0] == 'X') { // oprand의 첫 글자가 X인 경우(Hexa)
                        tmp_loc = 1;
                    }
                    locctr += tmp_loc;//locctr값 증가
                    break;
                }
                else if (!strcmp(symtab[j].opcode, "WORD")) { // opcode가 WORD이면 locctr에 3을 더함 
                    token = strtok(NULL, "\t\n"); strcpy(symtab[j].operand, token);
                    locctr += 3;
                }
                //RESB면 1*(operand), RESW면 3*(opreand)
                else if (!strcmp(symtab[j].opcode, "RESB")) { // opcode가 RESB이면 oprand의 10진수 숫자를 locctr에 더함
                    token = strtok(NULL, "\t\n"); strcpy(symtab[j].operand, token);
                    tmp_loc = 1 * atoi(symtab[j].operand);
                    locctr += tmp_loc;//RESB의 경우는 BYTE단위이니까 그대로 locctr에 더해주면 됨.
                }
                else if (!strcmp(symtab[j].opcode, "RESW")) { // opcode가 RESW이면 1word=3byte
                    token = strtok(NULL, "\t\n"); strcpy(symtab[j].operand, token);
                    locctr += 3 * atoi(symtab[j].operand);
                }
                else // 나머지 opcode가 나오면 OPTAB에 정의되어 있는 단어인지 판독  
                    for (i = 0; i < sizeof(optab) / sizeof(optab[0]); i++) {
                        if (!strcmp(optab[i].name, symtab[j].opcode)) {
                            // opcode가 OPTAB에 정의되어 있는 단어이면 locctr에 3을 더하고 정상 flag 설정  
                            token = strtok(NULL, "\t\n");
                            strcpy(symtab[j].operand, token); locctr += 3;
                            existOpcode = 1;
                        }
                    }
                if (existOpcode == 0) { // OPTAB에 정의된 단어가 아니면 오류메시지 출력 후 다음 줄 읽어옴  
                    printf("OPTAB에 정의되지 않은 명령어입니다.\n");
                    return 4;
                }
            }
            token = strtok(NULL, "\t\n"); // NULL 다음 token 읽어옴 
        }
    }
    j++; // SYMTAB의 lable 개수 증가
    
    return 0;
}

int writeFile(int res, FILE* sample_1st) { // pass1을 console에 출력하면서 동시에 txt파일로 출력 
    switch (flag) {
    case 0:
        if (flag3 == 1) { // lable 부분이 공백이면 파일의 lable 부분에 space 출력 (나중에 구분하기 위함) 
            fprintf(sample_1st, "%X\t%s%s\t%s\n", symtab[j - 1].symloc, " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
            break;
        }
        else
            fprintf(sample_1st, "%X\t%s\t%s\t%s\n", symtab[j - 1].symloc, symtab[j - 1].label, symtab[j - 1].opcode, symtab[j - 1].operand);
    case 2: 
        fputs("\n", sample_1st); // 입력 파일에 빈 줄이 나오면 txt파일에 빈 줄 출력
        break;
    case 3:
        fprintf(sample_1st, "%s%s%s\t%s\n", " \t", " \t", symtab[j - 1].opcode, symtab[j - 1].operand);
        return 4;
        break;
    case 4:
        break;//그냥 pass
    default:
        return 4;




    }
}
