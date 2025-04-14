#include <iostream>
#include <fcntl.h>
#include <cstring> 
#include <sys/mman.h>
#include <unistd.h>
#include "dataLista.h"

int main() {
    int fd = shm_open("/bd_ipc", O_RDWR, 0666);
    IPCData* ipc_data = (IPCData*)mmap(nullptr, sizeof(IPCData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    std::cout << "Digite um comando (ex: INSERT id=1 nome=João):" << std::endl;

    while (true) {
        std::string comando;
        std::getline(std::cin, comando);

        if (comando == "exit") break;

        // Envia comando para o servidor
        strncpy(ipc_data->comando, comando.c_str(), sizeof(ipc_data->comando));

        // Aguarda resposta
        while (strlen(ipc_data->resposta) == 0) {
            usleep(10000);
        }

        std::cout << "Resposta: " << ipc_data->resposta << std::endl;
        memset(ipc_data->resposta, 0, sizeof(ipc_data->resposta));
    }

    munmap(ipc_data, sizeof(IPCData));
    return 0;
}