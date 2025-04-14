#ifndef DATABASE_H
#define DATABASE_H

#define MAX_REGISTROS 100

typedef struct {
    int id;
    char nome[50];
} Registro;

// Estrutura para comunicação via memória compartilhada
typedef struct {
    char comando[256];   // Ex: "INSERT id=1 nome=João"
    char resposta[256];  // Resposta do servidor
} IPCData;

#endif