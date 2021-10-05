#include <stdlib.h>
#include <stdio.h>
#include <string.h> 


#define TAM_MAX_REG 64
#define DELIM_STR "|"
#define CMP_CNT 4

typedef struct cabecalho{
    int ped;
}cabecalho;

int input(char * str, int size) {
    int i = 0;    
    char c = getchar();    
    while (c != '\n') {        
        if (i < size - 1) {            
            str[i] = c;            
            i++;                 
        }        
        c = getchar();   
    }    
    str[i] = '\0';    
    return i;
}
short leia_reg(char * buffer, int tam, FILE* arq){
    short comp_reg;

    if (fread(&comp_reg, sizeof(comp_reg), 1, arq) == 0) {
        return 0;
    }

    if (comp_reg < tam) {
        fread(buffer, sizeof(char), comp_reg, arq);
        buffer[comp_reg] = '\0';
        return comp_reg;
    } else {
        printf("Buffer overflow\n");
        return 0;
    }
}
void concatena_campo(char * buffer, char * campo){
    strcat(buffer, campo);
    strcat(buffer, DELIM_STR);
}

short readRegister(char * buffer, int maxTam, FILE* toReadFile){
    short regSize = 0;
    char * temp;
    int i = 0;
    char bufferCopy[TAM_MAX_REG];
    memset(bufferCopy, 0, TAM_MAX_REG);

    if(fread(bufferCopy, sizeof(char), maxTam, toReadFile) == 0){
        return 0;
    }

    strcat(buffer, strtok(bufferCopy, DELIM_STR));
    strcat(buffer, DELIM_STR);

    
    regSize += strlen(buffer);

    while(i < CMP_CNT-1){
        temp = strtok(NULL, DELIM_STR);
        strcat(buffer, temp);
        strcat(buffer, DELIM_STR);
        regSize += 1 + strlen(temp);
        i++;
    }

    return regSize;
}
int writeRegister(char * buffer, short size, FILE* file){
    fwrite(buffer, sizeof(char), size, file);
}

int verificaPED(){

}

// --------- operacao de busca------
int busca(char * key, FILE* file){
    printf("\ndentro do busca: %s", key);

    int regSize;
    char* possibleKey;
    char buffer[TAM_MAX_REG];
    memset(buffer, 0, TAM_MAX_REG);

    fseek(file, sizeof(cabecalho), SEEK_SET);
    regSize = readRegister(buffer, TAM_MAX_REG, file);
    // printf("\n%s", buffer);
    possibleKey = strtok(buffer, DELIM_STR);
    // printf("\n%d, %s", strlen(possibleKey), possibleKey);
    // printf("\n%d, %s", strlen(key), key);
    memset(buffer, 0, TAM_MAX_REG);


    // printf("\n%d, %d", strcmp(key, possibleKey), regSize);

    while(strcmp(key, possibleKey) != 0 && regSize != 0){
        // printf("pinto");
        memset(buffer, 0, TAM_MAX_REG);
        regSize = readRegister(buffer, TAM_MAX_REG, file);
        // printf("\n%d", regSize);
        // printf("\n%s", buffer);
        if (regSize != 0){
            possibleKey = strtok(buffer, DELIM_STR);
        }
        // printf("\npossible %s", possibleKey);
        // printf("\nregsize %d", regSize);
        // printf("\nstrcmp %d", strcmp(key, possibleKey));
    }
    if (strcmp(key, possibleKey) == 0){
        printf("\nachou: %s, %s", key, possibleKey);
        return ftell(file)/TAM_MAX_REG -1;

    }
    else{
        printf("\nnao achou");
        return -1;
    }


}
int consulta(char * key, FILE* file){
    char buffer[TAM_MAX_REG];
    memset(buffer, 0, TAM_MAX_REG);

    int rrn = busca(key, file);
    if (rrn == -1){
        printf("\nNao encontrei nadaa");
        return 0;
    }
    fseek(file, rrn*TAM_MAX_REG + sizeof(cabecalho), SEEK_SET);
    readRegister(buffer, TAM_MAX_REG, file);
    printf("\n%s  (RRN = %d - byte-offset %d)", buffer, rrn, rrn*TAM_MAX_REG + sizeof(cabecalho));
    // byteoffset = rrn * TAM_MAX_REG;
}

// // ----------operacao de insercao--------
// // rrn = verificaPED()
// int insercao(int rrn, char * registro, FILE* arq){

// }

// //-----------operacao de remocao---------
// // rrn =busca(key, arq)
// int remove(int rrn, FILE* arq){

// }


short readWriteRegister(char * buffer, int maxTam, FILE* toReadFile, FILE* toWriteFile){
    short regSize = 0;
    int i = 0;
    char bufferCopy[TAM_MAX_REG];
    bufferCopy[0] = '\0';
    bufferCopy[1] = '\0';
    bufferCopy[2] = '\0';

    if(fread(buffer, sizeof(char), maxTam, toReadFile) == 0){
        return 0;
    }

    char *temp = strtok(buffer, DELIM_STR);
    strcat(bufferCopy, temp);
    strcat(bufferCopy, DELIM_STR);

    // printf("\n%s", buffer);
    
    regSize = 1 + strlen(temp);

    while(i < CMP_CNT-1){
        temp = strtok(NULL, DELIM_STR);
        strcat(bufferCopy, temp);
        strcat(bufferCopy, DELIM_STR);
        regSize += 1 + strlen(temp);
        i++;
    }
    fwrite(bufferCopy, 1, TAM_MAX_REG, toWriteFile);

    return regSize;
}

int importacao(char * arq){
    FILE *entrada, *dadosPtr;
    char *bufferPtr;
    int conta_reg, conta_campo;
    char *campo, *campo2;
    short comp_reg, currentReg;
    cabecalho cabeca;
    cabeca.ped = -1;
    char buffer[TAM_MAX_REG], bufferCabecalho[sizeof(cabecalho)];
    memset(buffer, 0, TAM_MAX_REG);

    if ((entrada = fopen(arq, "rb")) == NULL) {
        printf("Erro na criacao do arquivo --- programa abortado\n");
        exit(EXIT_FAILURE);
    }
    if ((dadosPtr = fopen("dados.dat", "wb")) == NULL) {
        printf("Erro na criacao do arquivo 'dados' --- programa abortado\n");
        exit(EXIT_FAILURE);
    }


    // ----metodo 1----

    // comp_reg = readWriteRegister(buffer, TAM_MAX_REG, entrada, dadosPtr);
    // do{
    //     fseek(entrada, comp_reg, SEEK_SET);
    //     currentReg = readWriteRegister(buffer, TAM_MAX_REG, entrada, dadosPtr);
    //     printf("\n%d", currentReg);
    //     comp_reg += currentReg;
    // }while(currentReg != 0);

    //-----metodo 2----

    printf("\n%d", cabeca.ped);
    comp_reg = readRegister(buffer, TAM_MAX_REG, entrada);
    printf("\n%d,   %s", comp_reg, buffer);
    // c = i +'0';
    // char * temp = cabeca.ped + '0';
    sprintf(bufferCabecalho, "%d", cabeca.ped);
    writeRegister(bufferCabecalho, sizeof(cabecalho), dadosPtr);
    do{
        writeRegister(buffer, TAM_MAX_REG, dadosPtr);
        memset(buffer, 0, TAM_MAX_REG);
        fseek(entrada, comp_reg, SEEK_SET);
        currentReg = readRegister(buffer, TAM_MAX_REG, entrada);
        comp_reg += currentReg;
        printf("\n%d,   %s", currentReg, buffer);
    }while(currentReg != 0);





    fclose(entrada);
    fclose(dadosPtr);
    return 0;
}

int executa_operacoes(char * arq){
    FILE* entrada, *dadosPtr;
    char buffer[TAM_MAX_REG+2];

    if ((entrada = fopen(arq, "rb")) == NULL) {
        printf("Erro na criacao do arquivo --- programa abortado\n");
        exit(EXIT_FAILURE);
    }
    if ((dadosPtr = fopen("dados.dat", "rb")) == NULL) {
        printf("Erro na criacao do arquivo --- programa abortado\n");
        exit(EXIT_FAILURE);
    }

    short comp_reg = strlen(fgets(buffer, TAM_MAX_REG+2, entrada));
    while(comp_reg != 0){
        printf("\n%s", buffer);

        if (buffer[strlen(buffer)-1] == 10){
            buffer[strlen(buffer)-1] = 0;
        }
        if (buffer[strlen(buffer)-1] == 13){
            buffer[strlen(buffer)-1] = 0;
        }

        if (strncmp(buffer, "b", 1) == 0){
            printf("\nBusca");
            strtok(buffer, " ");

            consulta(strtok(NULL, " "), dadosPtr);
        }
        else if (strncmp(buffer, "i", 1) == 0){
            printf("\nInsercao");
        }    
        else if (strncmp(buffer, "r", 1) == 0){
            printf("\nRemocao");
            
        }
        memset(buffer, 0, TAM_MAX_REG+2);
        comp_reg = strlen(fgets(buffer, TAM_MAX_REG+2, entrada));
    }

    fclose(entrada);
    fclose(dadosPtr);

}

int main (int argc, char *argv[]){

    if (argc == 3 && strcmp(argv[1], "-i" ) == 0){
        printf("Modo de importacao ativado ... nome do arquivo = %s\n", argv[2]);
        importacao(argv[2]);
    }

    else if (argc == 3 && strcmp(argv[1], "-e") == 0) {

        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        executa_operacoes(argv[2]);
    }

    else if (argc == 2 && strcmp(argv[1], "-p") == 0) {

        printf("Modo de impressao da PED ativado ...\n");
        //imprime_ped();
    }
    
    else {
        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i|-e) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}