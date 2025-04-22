#ifndef DATABASE_H
#define DATABASE_H

#define MAX_REGISTROS 100
#define TAM_NOME 50
#define TAM_COMANDO 256
#define TAM_RESPOSTA 256

typedef struct 
{
    int id;
    char nome[TAM_NOME];
} Registro;

typedef struct 
{
    char comando[TAM_COMANDO];
    char resposta[TAM_RESPOSTA];
} IPCData;

#endif