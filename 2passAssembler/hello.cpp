#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 100
#define INDEX_REGISTER_OFFSET 32768

struct Instruction {
    char name[8];
    int length;
} instructions[] = { /* OPTAB ������ �Է� */ };

struct Symbol {
    char label[10];
    int location;
    int objectCode;
    char opcode[10];
    char operand[10];
} symbolTable[MAX_LENGTH];

int startAddress;
long locationCounter = 0;
long pass2LocationCounter = 0;
int symbolCount = 0;
int lineCount = 0;
int errorCode = 0;

void processLinePass1(char* line);
void processLinePass2(char* line, FILE* output);
int writeIntermediateFile(FILE* file, int errorCode);

int main() {
    FILE* inputFile, * intermediateFile, * objectFile;
    char buffer[80];

    inputFile = fopen("input.txt", "r");
    intermediateFile = fopen("Intermediate.txt", "w");
    objectFile = fopen("ObjectCode.txt", "w");

    // ���� ���� ���� ó��
    if (!inputFile || !intermediateFile || !objectFile) {
        fprintf(stderr, "File error\n");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), inputFile)) {
        processLinePass1(buffer);
        if (writeIntermediateFile(intermediateFile, errorCode)) break;
    }

    fclose(inputFile);
    rewind(intermediateFile);

    while (fgets(buffer, sizeof(buffer), intermediateFile)) {
        processLinePass2(buffer, objectFile);
        if (errorCode) break;
    }

    fclose(intermediateFile);
    fclose(objectFile);

    return 0;
}

void processLinePass1(char* line) {
    // Pass 1 ó�� ����
}

void processLinePass2(char* line, FILE* output) {
    // Pass 2 ó�� ����
}

int writeIntermediateFile(FILE* file, int errorCode) {
    // �߰� ���� �ۼ� ����
    return 0;
}
