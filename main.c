#include <stdlib.h>
#include <stdio.h>
#include <string.h> 


#define TAM_MAX_REG 64
#define DELIM_STR "|"
#define CMP_CNT 4

typedef struct cabecalho{
    int ped;
}cabecalho;
cabecalho cabeca;


short readRegister(char * buffer, int maxTam, FILE* toReadFile){
    short regSize = 0;
    char * temp;
    int i = 0;
    char bufferCopy[TAM_MAX_REG];
    memset(bufferCopy, 0, TAM_MAX_REG);
    int numero = fread(&bufferCopy, sizeof(char), maxTam, toReadFile);
    if (bufferCopy[0] == 0){
        return 0;
    }
    if(numero == 0){
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


int busca(char * key, FILE* file){

    int regSize;
    char* possibleKey;
    char buffer[TAM_MAX_REG];
    memset(buffer, 0, TAM_MAX_REG);

    fseek(file, sizeof(cabecalho), SEEK_SET);
    regSize = readRegister(buffer, TAM_MAX_REG, file);
    possibleKey = strtok(buffer, DELIM_STR);

    while((strncmp(possibleKey, "*", 1) == 0) || (strcmp(key, possibleKey) != 0 && regSize != 0)){
        memset(buffer, 0, TAM_MAX_REG);
        regSize = readRegister(buffer, TAM_MAX_REG, file);
        if (regSize != 0){
            possibleKey = strtok(buffer, DELIM_STR);
        }
        if (getc(file) == 42){
            fseek(file, 63, SEEK_CUR);
        }
        else{
            fseek(file, -1, SEEK_CUR);
        }
    }
    if (strcmp(key, possibleKey) == 0){
        return ftell(file)/TAM_MAX_REG -1;

    }
    else{
        return -1;
    }

}
int consulta(char * key, FILE* file){
    char buffer[TAM_MAX_REG];
    memset(buffer, 0, TAM_MAX_REG);

    int rrn = busca(key, file);
    if (rrn == -1){
        printf("\nErro: registro nao encontrado!");
        return 0;
    }
    fseek(file, rrn*TAM_MAX_REG + sizeof(cabecalho), SEEK_SET);
    readRegister(buffer, TAM_MAX_REG, file);
    printf("\n%s  (RRN = %d - byte-offset %d)", buffer, rrn, rrn*TAM_MAX_REG + sizeof(cabecalho));
}

int insere_registro(char* buffer, FILE *file)
{
    int byte_offset, ped;
    char holdPed[sizeof(cabecalho)];
    memset(holdPed, 0, sizeof(cabecalho));
    rewind(file);
    fgets(holdPed, sizeof(cabecalho), file);
    int pos = atoi(holdPed);

    if (pos != -1)
    {  
        
        byte_offset = pos * TAM_MAX_REG + sizeof(cabecalho) + sizeof(char);
        
        fseek(file, byte_offset, SEEK_SET);
        fread(&ped, sizeof(int), 1, file);
        fseek(file, pos * TAM_MAX_REG + sizeof(cabecalho), SEEK_SET);
        fwrite(buffer, TAM_MAX_REG, 1, file);
        cabeca.ped = ped;
        rewind(file);
        fwrite(&cabeca, sizeof(cabecalho), 1, file);
        return 1;
    }
    else
    {
        
        fseek(file, 0, SEEK_END);
        fwrite(buffer, sizeof(char), TAM_MAX_REG, file);
        return 0;
    }
}
int remove_registro(char * key, FILE* file)
{
    char numBuffer[sizeof(int)], holdPed[sizeof(int)];
    memset(holdPed, 0, sizeof(cabecalho));
    int rrn = busca(key, file);
    char ped[1];
    ped[0] = 42; 
    if (rrn == -1){
        printf("\nRegistro nao encontrado");
        return -1;
    }
    fseek(file, rrn*TAM_MAX_REG+sizeof(cabecalho), SEEK_SET);
    fwrite(ped, sizeof(char), 1, file);

    fseek(file, 0, SEEK_SET);
    fgets(holdPed, sizeof(cabecalho), file);
    
    fseek(file, rrn*TAM_MAX_REG+sizeof(cabecalho) + sizeof(char), SEEK_SET);
    fwrite(holdPed, sizeof(int)-1, 1, file);
    
    cabeca.ped = rrn;
    snprintf(holdPed, sizeof(cabecalho), "%d", cabeca.ped);

    char * t = holdPed;
    fseek(file, 0, SEEK_SET);
    fwrite(holdPed, sizeof(cabecalho), 1, file);
    rewind(file);
    fwrite(holdPed, sizeof(cabecalho), 1, file);

    printf("\nRegistro Removido");
    printf("\nPosicao: RRN: %d (byteoffset 1860)", rrn, rrn*TAM_MAX_REG+sizeof(cabecalho));
}

void imprime_ped(FILE *file)
{
    int i = 0, ped, byte_offset;
    char holdPed[sizeof(cabecalho)];
    rewind(file);
    fgets(holdPed, sizeof(cabecalho), file);
    ped = atoi(holdPed);

    printf("\nTOPO DO PED: %d", ped);
    while (ped != -1)
    {
        byte_offset = ped * TAM_MAX_REG + sizeof(cabecalho) + sizeof(char);
        fseek(file, byte_offset, SEEK_SET);
        fgets(holdPed, sizeof(cabecalho), file);
        ped = atoi(holdPed);
        if (ped != -1)
            printf(" -> %d", ped);
        i++;
    }
    printf("\nQuantidade de espacos na PED: %d", i);
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
    memset(bufferCabecalho, 0, TAM_MAX_REG);

    if ((entrada = fopen(arq, "rb")) == NULL) {
        printf("Erro na criacao do arquivo --- programa abortado\n");
        exit(EXIT_FAILURE);
    }
    if ((dadosPtr = fopen("dados.dat", "wb")) == NULL) {
        printf("Erro na criacao do arquivo 'dados' --- programa abortado\n");
        exit(EXIT_FAILURE);
    }


    printf("\n%d", cabeca.ped);
    comp_reg = readRegister(buffer, TAM_MAX_REG, entrada);
    printf("\n%d,   %s", comp_reg, buffer);
    snprintf(bufferCabecalho, sizeof(cabecalho), "%d", cabeca.ped);
    printf("\n%s--%d", bufferCabecalho, bufferCabecalho);
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
    if ((dadosPtr = fopen("dados.dat", "r+")) == NULL) {
        printf("Erro na criacao do arquivo --- programa abortado\n");
        exit(EXIT_FAILURE);
    }

    short comp_reg = strlen(fgets(buffer, TAM_MAX_REG+2, entrada));
    while(comp_reg != 0){

        if (buffer[strlen(buffer)-1] == 10){
            buffer[strlen(buffer)-1] = 0;
        }
        if (buffer[strlen(buffer)-1] == 13){
            buffer[strlen(buffer)-1] = 0;
        }

        if (strncmp(buffer, "b", 1) == 0){
            strtok(buffer, " ");
            char * t = strtok(NULL, " ");

            printf("\nBusca pelo registro de chave '%s'", t);

            consulta(t, dadosPtr);
        }
        else if (strncmp(buffer, "i", 1) == 0){
            strtok(buffer, " ");
            char* temp2 = strtok(NULL, "");
            int ins = insere_registro(temp2, dadosPtr);

            char* temp = strtok(temp2, "|");
            printf("\nInsercao do registro de chave '%s'", temp);
            if (ins == 1){
                int rrn = busca(temp, dadosPtr);
                printf("\nLocal: RRN = %d (byte-offset %d) [reutilizado]\n\n", rrn, rrn*TAM_MAX_REG+sizeof(cabecalho));
            }
            else{
                printf("\nLocal: Fim do arquivo");
            }

        }    
        else if (strncmp(buffer, "r", 1) == 0){
            strtok(buffer, " ");
            char * t = strtok(NULL, " ");
            printf("\nRemocao do registro '%s'", t);
            remove_registro(t, dadosPtr);
        }
        memset(buffer, 0, TAM_MAX_REG+2);
        comp_reg = strlen(fgets(buffer, TAM_MAX_REG+2, entrada));
        printf("\n");
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
        FILE* arq = fopen("dados.dat", "r+");
        imprime_ped(arq);
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