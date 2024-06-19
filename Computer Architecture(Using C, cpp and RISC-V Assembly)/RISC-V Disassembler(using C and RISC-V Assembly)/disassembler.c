#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

void hexToBinary(char hex[]){
    char binary[64] = "";
    int length = strlen(hex); 

    for(int i=0; i<length; i++){
        if(hex[i] == '0'){
            strcat(binary, "0000");
        }
        else if(hex[i] == '1'){
            strcat(binary, "0001");
        }
        else if(hex[i] == '2'){
            strcat(binary, "0010");
        }
        else if(hex[i] == '3'){
            strcat(binary, "0011");
        }
        else if(hex[i] == '4'){
            strcat(binary, "0100");
        }
        else if(hex[i] == '5'){
            strcat(binary, "0101");
        }
        else if(hex[i] == '6'){
            strcat(binary, "0110");
        }
        else if(hex[i] == '7'){
            strcat(binary, "0111");
        }
        else if(hex[i] == '8'){
            strcat(binary, "1000");
        }
        else if(hex[i] == '9'){
            strcat(binary, "1001");
        }
        else if(hex[i] == 'a' || hex[i] == 'A'){
            strcat(binary, "1010");
        }
        else if(hex[i] == 'b' || hex[i] == 'B'){
            strcat(binary, "1011");
        }
        else if(hex[i] == 'c' || hex[i] == 'C'){
            strcat(binary, "1100");
        }
        else if(hex[i] == 'd' || hex[i] == 'D'){
            strcat(binary, "1101");
        }
        else if(hex[i] == 'e' || hex[i] == 'E'){
            strcat(binary, "1110");
        }
        else if(hex[i] == 'f' || hex[i] == 'F'){
            strcat(binary, "1111");
        }
    }

    strcpy(hex, binary);
}

int binaryToDecimalString(char binary[], char ifSigned[]){
    int length = strlen(binary);
    if(length == 0){
        return 0;
    }

    int decimal = 0;

    if(strcmp(ifSigned, "signed") == 0){
        if(binary[0] == '1'){
            int weight = 1;
            for(int j = 0; j<(length-1 - 0); j++){
                weight *= 2;
            }
            decimal -= weight;
        }
    }
    else if(strcmp(ifSigned, "unsigned") == 0){
        if(binary[0] == '1'){
            int weight = 1;
            for(int j = 0; j<(length-1 - 0); j++){
                weight *= 2;
            }
            decimal += weight;
        }
    }

    for(int i=1; i<length; i++){
        if(binary[i] == '1'){
            int weight = 1;
            for(int j = 0; j<(length-1 - i); j++){
                weight *= 2;
            }
            decimal += weight;
        }
    }
    strcpy(binary, "");
    sprintf(binary, "%d", decimal);

    return decimal;
}

void initialiseLabels(int IC, int labels[IC]){
    for(int i=0 ;i<IC; i++){
        labels[i] = -1;
    }
}

int generateNewLabel(){
    static int label = -1;
    label++;
    return label;
}

void stringSlice(char output[], char input[], int start, int end) {
    int length = end - start;
    strncpy(output, input + start, length);
    output[length] = '\0';
}

void R_format(int IC, int labels[IC], int instructionNumber, char instruction[33]){
    char funct3[4] = "", funct7[8] = "";
    stringSlice(funct3, instruction, 31-14, 31-11);
    stringSlice(funct7, instruction, 31-31, 31-24);

    char rd[6] = "", rs1[6] = "", rs2[6] = "";
    stringSlice(rd, instruction, 31-11, 31-6);
    stringSlice(rs1, instruction, 31-19, 31-14);
    stringSlice(rs2, instruction, 31-24, 31-19);

    binaryToDecimalString(rd, "unsigned");
    binaryToDecimalString(rs1, "unsigned");
    binaryToDecimalString(rs2, "unsigned");

    char suffix[33] = "";
    snprintf(suffix, sizeof(suffix), "x%s, x%s, x%s", rd, rs1, rs2);

    if(strcmp(funct3, "000")==0){//add, sub
        if(strcmp(funct7, "0000000")==0){//add
            strcpy(instruction, "add  ");
        }
        else{//sub
            strcpy(instruction, "sub  ");
        }
    }
    else if(strcmp(funct3, "100")==0){//xor
        strcpy(instruction, "xor  ");
    }
    else if(strcmp(funct3, "110")==0){//or
        strcpy(instruction, "or   ");
    }
    else if(strcmp(funct3, "111")==0){//and
        strcpy(instruction, "and  ");
    }
    else if(strcmp(funct3, "001")==0){//sll
        strcpy(instruction, "sll  ");
    }
    else if(strcmp(funct3, "101")==0){//srl, sra
        if(strcmp(funct7, "0000000")==0){//srl
            strcpy(instruction, "srl  ");
        }
        else if(strcmp(funct7, "0100000")==0){//sra
            strcpy(instruction, "sra  ");
        }
        else{
            printf("Error: instruction \"%s\" is supposed to be R format, funct3 = 0x5(srl or sra) but funct7 is invalid\n", instruction);
            return;
        }
    }
    else{
        printf("Error: instruction \"%s\" is supposed to be R format but funct3 is invalid (or not in instructions given in problem statement)\n", instruction);
        return;
    }

    strcat(instruction, suffix);
}

void I_format(int IC, int labels[IC], int instructionNumber, char instruction[33], char opcode[8]){
    char funct3[4] = "";
    stringSlice(funct3, instruction, 31-14, 31-11);

    char rd[6] = "", rs1[6] = "";
    stringSlice(rd, instruction, 31-11, 31-6);
    stringSlice(rs1, instruction, 31-19, 31-14);

    binaryToDecimalString(rd, "unsigned");
    binaryToDecimalString(rs1, "unsigned");

    char imm[13] = "";
    if(strcmp(opcode, "0010011") == 0 && (strcmp(funct3, "001") == 0 || strcmp(funct3, "101") == 0)){//for slli, srli, srai
        stringSlice(imm, instruction, 31-31+6, 31-19);
    }
    else{//for rest
        stringSlice(imm, instruction, 31-31, 31-19);
    }

    char immUnsigned[13] = "";
    strcpy(immUnsigned, imm);

    binaryToDecimalString(immUnsigned, "unsigned");
    binaryToDecimalString(imm, "signed");

    char suffix[33] = "";

    if(strcmp(opcode, "0010011") == 0){
        //I-1-format
        //ex, addi
        char imm2[7] = "";
        stringSlice(imm2, instruction, 31-31, 31-25);


        if(strcmp(funct3, "000")==0){//addi
            strcpy(instruction, "addi ");
        }
        else if(strcmp(funct3, "100")==0){//xori
            strcpy(instruction, "xori ");
        }
        else if(strcmp(funct3, "110")==0){//ori
            strcpy(instruction, "ori  ");
        }
        else if(strcmp(funct3, "111")==0){//andi
            strcpy(instruction, "andi ");
        }
        else if(strcmp(funct3, "001")==0){//slli
            strcpy(imm, immUnsigned);
            strcpy(instruction, "slli ");
        }
        else if(strcmp(funct3, "101")==0){//srli, srai
            strcpy(imm, immUnsigned);

            if(strcmp(imm2, "000000")==0){//srli
                strcpy(instruction, "srli ");
            }
            else if(strcmp(imm2, "010000")==0){//srai
                strcpy(instruction, "srai ");
            }
            else{
                printf("Error: instruction \"%s\" is supposed to be I format, funct3 = 0x5(srli or srai) but funct7 is invalid\n", instruction);
                return;
            }
        }
        else{
            printf("Error: instruction \"%s\" is supposed to be I format but funct3 is invalid (or not in instructions given in problem statement)\n", instruction);
            return;
        }

        snprintf(suffix, sizeof(suffix), "x%s, x%s, %s", rd, rs1, imm);
    }
    else if(strcmp(opcode, "0000011") == 0){
        //I-2-format
        //ex, ld

        snprintf(suffix, sizeof(suffix), "x%s, %s(x%s)", rd, imm, rs1);

        if(strcmp(funct3, "000")==0){//lb
            strcpy(instruction, "lb   ");
        }
        else if(strcmp(funct3, "001")==0){//lh
            strcpy(instruction, "lh   ");
        }
        else if(strcmp(funct3, "010")==0){//lw
            strcpy(instruction, "lw   ");
        }
        else if(strcmp(funct3, "011")==0){//ld
            strcpy(instruction, "ld   ");
        }
        else if(strcmp(funct3, "100")==0){//lbu
            strcpy(instruction, "lbu  ");
        }
        else if(strcmp(funct3, "101")==0){//lhu
            strcpy(instruction, "lhu  ");
        }
        else if(strcmp(funct3, "110")==0){//lwu
            strcpy(instruction, "lwu  ");
        }
        else{
            printf("Error: instruction \"%s\" is supposed to be I format but funct3 is invalid (or not in instructions given in problem statement)\n", instruction);
            return;
        }
    }
    else if(strcmp(opcode, "1100111") == 0){
        //I-3-format
        //ex, jalr

        snprintf(suffix, sizeof(suffix), "x%s, %s(x%s)", rd, imm, rs1);

        if(strcmp(funct3, "000")==0){//jalr
            strcpy(instruction, "jalr ");
        }
        else{
            printf("Error: instruction \"%s\" is supposed to be I format but funct3 is invalid (or not in instructions given in problem statement)\n", instruction);
            return;
        }
    }

    strcat(instruction, suffix);
}

void S_format(int IC, int labels[IC], int instructionNumber, char instruction[33], char opcode[8]){
    char funct3[4] = "", imm[13] = "";;
    stringSlice(funct3, instruction, 31-14, 31-11);
    stringSlice(imm, instruction, 31-31, 31-24);

    char imm2[6] = "", rs1[6] = "", rs2[6] = "";
    stringSlice(imm2, instruction, 31-11, 31-6);
    stringSlice(rs1, instruction, 31-19, 31-14);
    stringSlice(rs2, instruction, 31-24, 31-19);

    strcat(imm, imm2);

    binaryToDecimalString(imm, "signed");
    binaryToDecimalString(rs1, "unsigned");
    binaryToDecimalString(rs2, "unsigned");

    char suffix[33] = "";
    snprintf(suffix, sizeof(suffix), "x%s, %s(x%s)", rs2, imm, rs1);

    if(strcmp(funct3, "000")==0){//sb
        strcpy(instruction, "sb   ");
    }
    else if(strcmp(funct3, "001")==0){//sh
        strcpy(instruction, "sh   ");
    }
    else if(strcmp(funct3, "010")==0){//sw
        strcpy(instruction, "sw   ");
    }
    else if(strcmp(funct3, "011")==0){//sd
        strcpy(instruction, "sd   ");
    }
    else{
        printf("Error: instruction \"%s\" is supposed to be S format but funct3 is invalid (or not in instructions given in problem statement)\n", instruction);
        return;
    }

    strcat(instruction, suffix);
}

void B_format(int IC, int labels[IC], int instructionNumber, char instruction[33], char opcode[8]){
    char funct3[4] = "", imm[13] = "";
    stringSlice(funct3, instruction, 31-14, 31-11);
    stringSlice(imm, instruction, 31-31, 31-31+1);

    char buffer[10] = "";
    stringSlice(buffer, instruction, 31-7, 31-6);
    strcat(imm, buffer);

    strcpy(buffer, "");
    stringSlice(buffer, instruction, 31-31+1, 31-24);
    strcat(imm, buffer);

    strcpy(buffer, "");
    stringSlice(buffer, instruction, 31-11, 31-7);
    strcat(imm, buffer);

    char rs1[6] = "", rs2[6] = "";
    stringSlice(rs1, instruction, 31-19, 31-14);
    stringSlice(rs2, instruction, 31-24, 31-19);

    int int_imm = binaryToDecimalString(imm, "signed");
    binaryToDecimalString(rs1, "unsigned");
    binaryToDecimalString(rs2, "unsigned");

    //labelling
    int jump = int_imm/2;
    int referredIndex = instructionNumber+jump;
    int label;
    if(labels[referredIndex] != -1){
        label = labels[referredIndex];
    }
    else{
        label = generateNewLabel();
        labels[referredIndex] = label;
    }

    char suffix[33] = "";
    snprintf(suffix, sizeof(suffix), "x%s, x%s, L%d", rs1, rs2, label);

    if(strcmp(funct3, "000")==0){//beq
        strcpy(instruction, "beq  ");
    }
    else if(strcmp(funct3, "001")==0){//bnq
        strcpy(instruction, "bnq  ");
    }
    else if(strcmp(funct3, "100")==0){//blt
        strcpy(instruction, "blt  ");
    }
    else if(strcmp(funct3, "101")==0){//bge
        strcpy(instruction, "bge  ");
    }
    else if(strcmp(funct3, "110")==0){//bltu
        strcpy(instruction, "bltu ");
    }
    else if(strcmp(funct3, "111")==0){//bgeu
        strcpy(instruction, "bgeu ");
    }
    else{
        printf("Error: instruction \"%s\" is supposed to be B format but funct3 is invalid (or not in instructions given in problem statement)\n", instruction);
        return;
    }

    strcat(instruction, suffix);
}

void J_format(int IC, int labels[IC], int instructionNumber, char instruction[33], char opcode[8]){
    char rd[6] = "", imm[21] = "";
    stringSlice(rd, instruction, 31-11, 31-6);

    stringSlice(imm, instruction, 31-31, 31-31+1);

    char buffer[20] = "";
    stringSlice(buffer, instruction, 31-31+12, 31-11);
    strcat(imm, buffer);

    strcpy(buffer, "");
    stringSlice(buffer, instruction, 31-31+11, 31-31+12);
    strcat(imm, buffer);

    strcpy(buffer, "");
    stringSlice(buffer, instruction, 31-31+1, 31-31+11);
    strcat(imm, buffer);

    int int_imm = binaryToDecimalString(imm, "signed");
    binaryToDecimalString(rd, "unsigned");

    //labelling
    int jump = int_imm/2;
    int referredIndex = instructionNumber+jump;
    int label;
    if(labels[referredIndex] != -1){
        label = labels[referredIndex];
    }
    else{
        label = generateNewLabel();
        labels[referredIndex] = label;
    }

    char suffix[33] = "";
    snprintf(suffix, sizeof(suffix), "x%s, L%d", rd, label);
    strcpy(instruction, "jal  ");
    strcat(instruction, suffix);
}

void U_format(int IC, int labels[IC], int instructionNumber, char instruction[33], char opcode[8]){
    char rd[6] = "", imm[21] = "";
    stringSlice(rd, instruction, 31-11, 31-6);
    stringSlice(imm, instruction, 31-31, 31-11);

    binaryToDecimalString(imm, "signed");
    binaryToDecimalString(rd, "unsigned");

    char suffix[33] = "";
    snprintf(suffix, sizeof(suffix), "x%s, %s", rd, imm);
    strcpy(instruction, "lui  ");
    strcat(instruction, suffix);
}

void disassemble(int IC, int labels[IC], int instructionNumber, char instruction[33]){
    char opcode[8] = "";
    strcpy(opcode, instruction+(32-7));

    if(strcmp(opcode, "0110011") == 0){
        //R-format
        //ex, add
        R_format(IC, labels, instructionNumber, instruction);

    }
    else if(strcmp(opcode, "0010011") == 0 || strcmp(opcode, "0000011") == 0 || strcmp(opcode, "1100111") == 0){
        //I-format
        //ex, addi, ld, jalr
        I_format(IC, labels, instructionNumber, instruction, opcode);
    }
    else if(strcmp(opcode, "0100011") == 0){
        //S-format
        //ex, sd
        S_format(IC, labels, instructionNumber, instruction, opcode);
    }
    else if(strcmp(opcode, "1100011") == 0){
        //B-format
        //ex, beq
        B_format(IC, labels, instructionNumber, instruction, opcode);
    }
    else if(strcmp(opcode, "1101111") == 0){
        //J-format
        //ex, jal
        J_format(IC, labels, instructionNumber, instruction, opcode);
    }
    else if(strcmp(opcode, "0110111") == 0){
        //U-format
        //ex, lui
        U_format(IC, labels, instructionNumber, instruction, opcode);
    }
    else{
        printf("Error: Instruction \"%s\" does not have a valid opcode\n", instruction);
    }
}

void disassembler(int IC, int labels[IC], char instructions[IC][33]){
    initialiseLabels(IC, labels);

    for(int instructionNumber = 0; instructionNumber<IC; instructionNumber++){
        disassemble(IC, labels, instructionNumber, instructions[instructionNumber]);
    }
}

void printInstructions(int IC, int labels[IC], char instructions[IC][33]){
    printf("\nDisassembled Instructions : \n\n");
    int ifLabelled = 0;

    for(int i=0; i<IC; i++){
        if(labels[i] != -1){
            ifLabelled = 1;
            printf("\nL%d :\n", labels[i]);
        }
      //  else if(ifLabelled == 1){
       //     printf("     ");
        //}
        printf("%s\n", instructions[i]);
    }
}

FILE * readyFile(){
    char fileName[64];
    printf("Enter input file name : ");
    scanf("%s", fileName);

    FILE *file = fopen(fileName, "r");

    while(file == NULL){
        printf("Error opening file\n");

        printf("Try again : ");
        char newFileName[64];
        scanf("%s", newFileName);

        file = fopen(newFileName, "r");
    }
    return file;
}

int main(){

    FILE * file = readyFile();

    char line[33];
    int IC = 0;
    while (fgets(line, sizeof(line), file) != NULL){
    	if(line[0] == ' ' || line[0] == '\n' || line[0] == '\t') continue;
        IC++;
    }

    if(IC == 0){
        fclose(file);
        return 0;
    }

    rewind(file);
    char instructions[IC][33];
    int labels[IC];

    int cnt = 0;
    while (fgets(line, sizeof(line), file) != NULL){
    	if(line[0] == ' ' || line[0] == '\n' || line[0] == '\t') continue;
    	
        if(line[0] == '0' && (line[1] == 'x' || line[1] == 'X')){
            char buffer[33] = "";
            strcpy(buffer, line+2);
            strcpy(line, buffer);
        }
        hexToBinary(line);
        strcpy(instructions[cnt], line);
        cnt++;
    }

    disassembler(IC, labels, instructions);
    printInstructions(IC, labels, instructions);

    fclose(file);
    return 0;
}
