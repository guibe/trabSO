#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include "dataLista.h"

// Estruturas globais
Registro banco[MAX_REGISTROS];
int total_registros = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Processa comandos (INSERT/DELETE/SELECT)
void processar_comando(const char* comando, char* resposta) {
    pthread_mutex_lock(&mutex);

    // Exemplo: INSERT id=5 nome=Maria
    if (strstr(comando, "INSERT") != nullptr) {
        int id;
        char nome[50];
        sscanf(comando, "INSERT id=%d nome=%49s", &id, nome);

        if (total_registros < MAX_REGISTROS) {
            banco[total_registros].id = id;
            strcpy(banco[total_registros].nome, nome);
            total_registros++;
            strcpy(resposta, "INSERT OK");
        } else {
            strcpy(resposta, "ERRO: Banco cheio");
        }

    // Exemplo: DELETE id=5
    } else if (strstr(comando, "DELETE") != nullptr) {
        int id;
        sscanf(comando, "DELETE id=%d", &id);

        for (int i = 0; i < total_registros; i++) {
            if (banco[i].id == id) {
                // Remove registro (substitui pelo último)
                banco[i] = banco[total_registros - 1];
                total_registros--;
                strcpy(resposta, "DELETE OK");
                break;
            }
        }
    } else {
        strcpy(resposta, "COMANDO DESCONHECIDO");
    }

    pthread_mutex_unlock(&mutex);
}

// Thread para tratar requisições
void* thread_handler(void* arg) {
    IPCData* ipc_data = (IPCData*)arg;

    while (true) {
        if (strlen(ipc_data->comando) > 0) {
            processar_comando(ipc_data->comando, ipc_data->resposta);
            memset(ipc_data->comando, 0, sizeof(ipc_data->comando)); // Limpa o comando
        }
        usleep(100000); // Espera 100ms para evitar uso excessivo da CPU
    }
    return nullptr;
}

int main() {
    // Cria memória compartilhada
    int fd = shm_open("/bd_ipc", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(IPCData));
    IPCData* ipc_data = (IPCData*)mmap(nullptr, sizeof(IPCData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Cria pool de threads
    pthread_t thread1, thread2;
    pthread_create(&thread1, nullptr, thread_handler, ipc_data);
    pthread_create(&thread2, nullptr, thread_handler, ipc_data);

    std::cout << "Servidor iniciado. Aguardando comandos..." << std::endl;

    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    // Limpeza
    munmap(ipc_data, sizeof(IPCData));
    shm_unlink("/bd_ipc");
    return 0;
}