#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h> 
#include "processomanager.h"
#include "listaint.h"
#include "entradapcb.h"
#include "cpu.h"
#include "processo.h"
#include "tabelapcb.h"

int pipefd[2];
sem_t *empty;
sem_t *full;
sem_t *mutex;

void handler(int sig) {
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    close(pipefd[0]);
    close(pipefd[1]);

    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");
    sem_unlink("/mutex_sem");
    exit(EXIT_SUCCESS);
}

int main() {
    pid_t pid;
    int i = 0;
    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");
    sem_unlink("/mutex_sem");

    empty = sem_open("/empty_sem", O_CREAT | O_EXCL, 0644, 1);
    full = sem_open("/full_sem", O_CREAT | O_EXCL, 0644, 0);
    mutex = sem_open("/mutex_sem", O_CREAT | O_EXCL, 0644, 1);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        ProcessoManager *pm = prcsmngInit();
        char buffer[1];
        int retornoExec = PRCSMNG_SUCESSO_EXECUCAO;
        while (retornoExec == PRCSMNG_SUCESSO_EXECUCAO)
        {
            sem_wait(full);
            sem_wait(mutex);

            read(pipefd[0], buffer, 1);
            retornoExec = prcsmngExecuta(pm, buffer[0]);

            sem_post(mutex);
            sem_post(empty);
        }
        prcsmngDesaloca(pm);
        if(retornoExec == PRCSMNG_CODIGO_SAIDA){
            printf("PROGRAMA TERMINADO COM SUCESSO\n");
        }
        exit(EXIT_SUCCESS);
    } else {
        signal(SIGCHLD, handler); 
        while (true)
        {
            sem_wait(empty);
            sem_wait(mutex);
            char character;

            printf("Digite um caractere: ");
            scanf(" %c", &character);
            if(character >= 'a' || character <= 'z')
            {
                character = character - 32;
            }
            write(pipefd[1], &character, 1);

            sem_post(mutex);
            sem_post(full);
        }
    }

    return 0;
}
