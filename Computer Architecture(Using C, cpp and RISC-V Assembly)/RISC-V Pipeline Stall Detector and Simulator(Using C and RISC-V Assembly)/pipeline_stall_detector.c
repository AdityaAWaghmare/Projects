#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct Instruction{
    char string[33];
    
    int nop_without_forwarding;//nops before this instruction
    int nop_with_forwarding;

} Instruction;
//--------------------------------------------------------------------------------------------------------------------------------------------------------

void translate_register_name(char register_name[10]){
    if(register_name[0] == 'x'){//no need to do anything
    }
    else if(register_name[0] == 'X'){
        register_name[0] = 'x';
    }
    else if(strcmp(register_name, "zero") == 0){
        strcpy(register_name, "x0");
    }
    else if(strcmp(register_name, "ra") == 0){
        strcpy(register_name, "x1");
    }
    else if(strcmp(register_name, "sp") == 0){
        strcpy(register_name, "x2");
    }
    else if(strcmp(register_name, "gp") == 0){
        strcpy(register_name, "x3");
    }
    else if(strcmp(register_name, "tp") == 0){
        strcpy(register_name, "x4");
    }
    else if(strcmp(register_name, "t0") == 0){
        strcpy(register_name, "x5");
    }
    else if(strcmp(register_name, "t1") == 0){
        strcpy(register_name, "x6");
    }
    else if(strcmp(register_name, "t2") == 0){
        strcpy(register_name, "x7");
    }
    else if(strcmp(register_name, "s0") == 0 || strcmp(register_name, "fp") == 0){
        strcpy(register_name, "x8");
    }
    else if(strcmp(register_name, "s1") == 0){
        strcpy(register_name, "x9");
    }
    else if(strcmp(register_name, "a0") == 0){
        strcpy(register_name, "x10");
    }
    else if(strcmp(register_name, "a1") == 0){
        strcpy(register_name, "x11");
    }
    else if(strcmp(register_name, "a2") == 0){
        strcpy(register_name, "x12");
    }
    else if(strcmp(register_name, "a3") == 0){
        strcpy(register_name, "x13");
    }
    else if(strcmp(register_name, "a4") == 0){
        strcpy(register_name, "x14");
    }
    else if(strcmp(register_name, "a5") == 0){
        strcpy(register_name, "x15");
    }
    else if(strcmp(register_name, "a6") == 0){
        strcpy(register_name, "x16");
    }
    else if(strcmp(register_name, "a7") == 0){
        strcpy(register_name, "x17");
    }
    else if(strcmp(register_name, "s2") == 0){
        strcpy(register_name, "x18");
    }
    else if(strcmp(register_name, "s3") == 0){
        strcpy(register_name, "x19");
    }
    else if(strcmp(register_name, "s4") == 0){
        strcpy(register_name, "x20");
    }
    else if(strcmp(register_name, "s5") == 0){
        strcpy(register_name, "x21");
    }
    else if(strcmp(register_name, "s6") == 0){
        strcpy(register_name, "x22");
    }
    else if(strcmp(register_name, "s7") == 0){
        strcpy(register_name, "x23");
    }
    else if(strcmp(register_name, "s8") == 0){
        strcpy(register_name, "x24");
    }
    else if(strcmp(register_name, "s9") == 0){
        strcpy(register_name, "x25");
    }
    else if(strcmp(register_name, "s10") == 0){
        strcpy(register_name, "x26");
    }
    else if(strcmp(register_name, "s11") == 0){
        strcpy(register_name, "x27");
    }
    else if(strcmp(register_name, "t3") == 0){
        strcpy(register_name, "x28");
    }
    else if(strcmp(register_name, "t4") == 0){
        strcpy(register_name, "x29");
    }
    else if(strcmp(register_name, "t5") == 0){
        strcpy(register_name, "x30");
    }
    else if(strcmp(register_name, "t6") == 0){
        strcpy(register_name, "x31");
    }
    // else{
    //     strcpy(register_name, "");// in this case for immediate values make them empty string
    // }
}

void stop_at_character(char string[], char stopper){
    int index = 0;
    while(index < strlen(string) && string[index] != stopper){
        index++;
    }

    string[index] = '\0';
}

void get_rs1_rs2(char rs1[10], char rs2[10], char original_instruction[33]){
    char instruction[33];
    strcpy(instruction, original_instruction);//original inst passed by ref, so cannot make changes 

    char *token = strtok(instruction, " ");//token contains instruction name
    char name[10];//instruction name 
    strcpy(name, token);

    token = strtok(NULL, " ");//now it contains rd
    
    char rd[10];//dest name
    strcpy(rd, token);
    stop_at_character(rd, ',');

    token = strtok(NULL, " ");//now it contains rs1

    strcpy(rs1, token);
    stop_at_character(rs1, ',');

    for(int i=0; i<strlen(rs1); i++){//for load or store
        if(rs1[i] == '('){
            stop_at_character(rs1 + (i+1), ')');//end string at ')'

            char buffer[10];
            strcpy(buffer, rs1);//for undefined behaviour if any
            strcpy(rs1, buffer+(i+1));
            break;
        }
    }

    token = strtok(NULL, " ");//contains rs2 if it exists else null

    if(token != NULL){
        strcpy(rs2, token);
    }
    else{
        strcpy(rs2, "l/s");//for load or store
    }
    
    stop_at_character(rs2, '\n');//get rid of newline character

    if(name[0] == 's' && strlen(name) ==  2){// for s both of this are sources and not dest
        strcpy(rs2, rs1);
        strcpy(rs1, rd);
    }

    translate_register_name(rs1);
    //printf("rs1 = %s\n", rs1);
    translate_register_name(rs2);
}

int get_rd(char rd[10], char original_instruction[33]){
    char instruction[33];
    strcpy(instruction, original_instruction);

    char *token = strtok(instruction, " ");//token contains instruction name
    char name[10];
    strcpy(name, token);
    token = strtok(NULL, " ");//now it contains rd

    strcpy(rd, token);
    stop_at_character(rd, ',');

    translate_register_name(rd);

    if(name[0] == 'l'){
        return 1;//for load
    }
    else if(name[0] = 's' && strlen(name) == 2){
        return 2;//for store
    }
    else{
        return 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------

void get_instructions_with_stalls(int IC, Instruction instructions[IC]){
    
    for(int current=0; current<IC; current++){
        if(current == 0) continue;//no stalls before first instruction

        //getting rs1 and rs2 for current instruction
        char rs1[10], rs2[10];

        get_rs1_rs2(rs1, rs2, instructions[current].string);

        char prev_rd[10];
        int type = get_rd(prev_rd, instructions[current - 1].string);

        //when prev_rd is not x0
        //prev instruction is not store
        //rs1 or rs2 uses prev_rd
        if(strcmp(prev_rd, "x0") != 0 && type != 2 &&(strcmp(rs1, prev_rd) == 0 || strcmp(rs2, prev_rd) == 0) ){
            instructions[current].nop_without_forwarding = 2;//nop for prev instruction withoud forwarding

            //for load
            if(type == 1){
                instructions[current].nop_with_forwarding = 1;
            }
        }
        else{//only when no hazard in prev instruction we check for prev to prev
            if(current == 1) continue;//no prev to prev inst present in this case
            if(instructions[current-1].nop_without_forwarding > 0) continue;//more than equal to 2 inst gap already

            char prev_prev_rd[10];
            get_rd(prev_prev_rd, instructions[current - 2].string);
            
            //when match and not store inst
            if(type != 2 && (strcmp(rs1, prev_prev_rd) == 0 || strcmp(rs2, prev_prev_rd) == 0) ){
                instructions[current].nop_without_forwarding = 1;
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------------------------------

void print(int IC, Instruction instructions[IC]){

    int cycles = IC+4;
    printf("\nWithout Forwarding : \n");
    for(int i=0; i<IC; i++){
        int nops = instructions[i].nop_without_forwarding;
        cycles += nops;

        for(int j=0; j<nops; j++){
            printf("nop\n");
        }

        printf("%s", instructions[i].string);
    }
    printf("\nCycles = %d\n", cycles);
    printf("\n\n");


    cycles = IC+4;
    printf("With Forwarding : \n");
    for(int i=0; i<IC; i++){
        int nops = instructions[i].nop_with_forwarding;
        cycles += nops;

        for(int j=0; j<nops; j++){
            printf("nop\n");
        }

        printf("%s", instructions[i].string);
    }
    printf("\nCycles = %d\n", cycles);
    printf("\n");
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
FILE *readyFile(){
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

int get_instruction_count(FILE *file){
    char line[33];
    int IC = 0;
    while (fgets(line, sizeof(line), file) != NULL) IC++;// getting no. of instructions

    rewind(file);
    return IC;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
int main(){

    FILE *file = readyFile();
    int IC = get_instruction_count(file);

    if(IC == 0){
        fclose(file);
        return 0;
    }

    Instruction instructions[IC];
    int number = 0;
    while (fgets(instructions[number].string, sizeof(instructions[number].string), file) != NULL){
        instructions[number].nop_with_forwarding = instructions[number].nop_without_forwarding = 0;//initializing values
        number++;
    }

    // print(IC, instructions);
    // return 0;//test

    get_instructions_with_stalls(IC, instructions);

    print(IC, instructions);
    
    fclose(file);
    return 0;
}