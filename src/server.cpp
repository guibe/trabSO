#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include "dataLista.h"

Registro banco[MAX_REGISTROS];
int total_registros = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void processar_comando(const char* comando, char* resposta) 
{
    pthread_mutex_lock(&mutex);

    //ex: INSERT id=1 nome=Vinicius
    if (strstr(comando, "INSERT") != nullptr) 
    {
        int id;
        char nome[TAM_NOME];
        sscanf(comando, "INSERT id=%d nome=%49s", &id, nome);
        
        bool id_existe = false;
        for (int i = 0; i < total_registros; i++) 
        {
            if (banco[i].id == id) 
            {
                id_existe = true;
                break;
            }
        }
        if (id_existe) 
        {
            strcpy(resposta, "ERRO: id já existe :c");
        }
        else if (total_registros < MAX_REGISTROS) 
        {
            banco[total_registros].id = id;
            strcpy(banco[total_registros].nome, nome);
            total_registros++;
            strcpy(resposta, "INSERT OK");
        } else 
        {
            strcpy(resposta, "ERRO: Banco cheio :c");
        }

    //ex: DELETE id=2
    } 
    else if (strstr(comando, "DELETE") != nullptr) 
    {
        int id;
        sscanf(comando, "DELETE id=%d", &id);
        
        bool id_inexiste = true;
        for (int i = 0; i < total_registros; i++) 
        {
            if (banco[i].id == id) 
            {
                id_inexiste = false;
                break;
            }
        }
        if (id_inexiste) 
        {
            strcpy(resposta, "ERRO: id não encontrado :c");
        } 
        else 
        {
            for (int i = 0; i < total_registros; i++) 
            {
                if (banco[i].id == id) 
                {
                    banco[i] = banco[total_registros - 1];
                    total_registros--;
                    strcpy(resposta, "DELETE OK :D");
                    break;
                }
            }
        }
    } 
    else if (strstr(comando, "SHOW") != nullptr) 
    {
        if (total_registros == 0) 
        {
            strcpy(resposta, "BANCO DE DADOS VAZIO :c");
        } 
        else 
        {
            char temp[TAM_RESPOSTA] = "";
            strcat(resposta, "ID | NOME\n");
            strcat(resposta, "---------\n");
            
            for (int i = 0; i < total_registros; i++) 
            {
                char linha[100];
                snprintf(linha, sizeof(linha), "%2d | %s\n", banco[i].id, banco[i].nome);
                strcat(temp, linha);
            }
            strcat(resposta, temp);
        }
    }
    else if (strstr(comando, "SELECT") != nullptr) 
    {
        int id;
        
        sscanf(comando, "SELECT id=%d", &id);
        
        bool id_inexiste = true;
        for (int i = 0; i < total_registros; i++) 
        {
            if (banco[i].id == id) 
            {
                id_inexiste = false;
                break;
            }
        }
        if (id_inexiste) 
        {
            strcpy(resposta, "ERRO: id não encontrado :c");
        } 
        else 
        {
            snprintf(resposta, TAM_RESPOSTA, "REGISTRO ENCONTRADO:\n Nome: %s", banco[id-1].nome);
        }
    }

    else if (strstr(comando, "UPDATE") != nullptr) 
    {
        int id;
        char novoNome[TAM_NOME];
        sscanf(comando, "UPDATE id=%d NOME=%s", &id, novoNome);
        
        bool id_inexiste = true;
        for (int i = 0; i < total_registros; i++) 
        {
            if (banco[i].id == id) 
            {
                id_inexiste = false;
                break;
            }
        }
        if (id_inexiste) 
        {
            strcpy(resposta, "ERRO: id não encontrado :c");
        } 
        else 
        {
            int tamNom = strlen(novoNome);
            strncpy(banco[id].nome, novoNome, tamNom);
            snprintf(resposta, TAM_RESPOSTA, "REGISTRO ATUALIZADO:\n Nome: %s", banco[id].nome);
        }
    }

    else 
    {
        strcpy(resposta, "COMANDO DESCONHECIDO/INCORRETO");
    }

    pthread_mutex_unlock(&mutex);
}

//handler de requisições
void* thread_handler(void* arg) 
{
    IPCData* ipc_data = (IPCData*)arg;

    while (true) 
    {
        if (strlen(ipc_data->comando) > 0) 
        {
            processar_comando(ipc_data->comando, ipc_data->resposta);
            memset(ipc_data->comando, 0, sizeof(ipc_data->comando));
        }
        usleep(100000);
    }
    return nullptr;
}

int main() 
{
    int fd = shm_open("/bd_ipc", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(IPCData));
    IPCData* ipc_data = (IPCData*)mmap(nullptr, sizeof(IPCData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    pthread_t thread1, thread2;
    pthread_create(&thread1, nullptr, thread_handler, ipc_data);
    pthread_create(&thread2, nullptr, thread_handler, ipc_data);

    std::cout << "Servidor iniciado. Aguardando comandos..." << std::endl;

    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    munmap(ipc_data, sizeof(IPCData));
    shm_unlink("/bd_ipc");
    return 0;
}