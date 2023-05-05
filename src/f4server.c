#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <limits.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>


#include "semaphore.h"
#include "commands.h"
#include "f4logic.h"

static volatile int status = 1;

void intHandler(int dummy) {
    printf("Stopping... %d\n", dummy);
    status = 0;
}

int init_fifo(char name[]){
    remove(name);//TODO Think if multiple instance running
    if (mkfifo(name, S_IRUSR | S_IWUSR) != 0) {
        perror("Creation FIFO :");
        exit(1);
    }

    return open(name, O_RDONLY | O_NONBLOCK);
}


int main(int argc, char *argv[]){


    // controlla il numero di argomenti di argc
    // esecuzione del server con numero minore di parametri (o 0) comporta la stampa di un messaggio di aiuto
    if(argc != 5){
        printf("Usage: %s <row> <column> sym_1 sym_2\n", argv[0]);
        return 0;
    }

    // trasformiamo argv1 e 2 in interi
    int row = atoi(argv[1]);
    int column = atoi(argv[2]);

    //controllo validitá della matrice
    if(row < 5){
        printf("The input row must be >= 5\n");
        return 1;
    }
    if(column < 5){
        printf("The input column must be < 5\n");
        return 1;
    }

    //inizializziamo i simboli usati dai due giocatori che devono essere diversi tra di loro
    char symbols[2];
    if(*argv[3] == *argv[4]){
        printf("Can't use same symbols\n");
        return 1;
    }
    symbols[0] = *argv[3];
    symbols[1] = *argv[4];

    //Creation FIFO for init connection
    //No need to have sem has if the read or write is below PIPE_BUF then you can have multiple reader and writer
    //https://www.gnu.org/software/libc/manual/html_node/Pipe-Atomicity.html
    //https://stackoverflow.com/questions/17410334/pipe-and-thread-safety
    //Min size for PIPE_BUF is 512 byte

    signal(SIGINT, intHandler); // Read CTRL+C and stop the program


    //pipe id da file preciso per cominciare la connessione al server protetto da semaphore


    /*
     * S <- C (pid,mode,pipe)
     *
     *
     *
     * if single{
     *  single process fork
     * }else{
     * wait_antoher_client
     * S <- C (pid,mode)
     * linkpipe1 S -> C1
     * linkpipe2 S -> C2
     *
     *  S -> broadcast_all()
     *
     * pipe1("symbol[0]")
     * pipe2("symbol[1]")
     * fork(pipe1,pipe2)
     * create pipe_rcv (forse message queue)
     * create sem
     * broadcast_pipe(pipe_rcv.id,sem.id)
     *
     * shared_memory(matrix)
     * broadcast_pipe(shared_memory.id,sem.id)
     *
     * pipe1(your_turn)
     *
     * // S <- C(pid,column)
     *
     * // S <- C(pid,commando)
     *
     * while(wait(input)){
     * input = read_input();
     *
     * //Logic commands(pid, abbandono(CTRL-C)){
     * winner(other);
     * break;
     * }
     *
     * check_right(sender)
     *
     *
     *
     * //Logic sem
     * get_shared_mem(matrix)
     * int result = runf4logic(matrix,input,pid,size[matrix]);
     * save_shared_meme(matrix)
     * //Logic sem
     * broadcast_pipe("update")
     * // C(read_shared_mem)
     *
     * if(result == error)
     *  pipe<c>("error")
     * }
     * if(result == winner){
     * broadcast_pipe("winner","symbol")
     * break();
     * }
     * turn()
     * } <---
     * destroy sem,shared,pipe
     */




    // simboli vanno comunicati ai due giocatori

    //Logica per la connesione

    //Creare area memoria per matrice
}