#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "a2_helper.h"

sem_t semaphores[2];   //semafoare pentru cerinta 2.3
sem_t *semaphores_2[2]; //semafoare pentru cerinta 2.5

//functia pentru threadurile din procesul 3
void *thread_function_for_process3(void *arg)
{
    int thread_no = *((int *)arg);

    if (thread_no == 5)
    {
        sem_wait(&semaphores[0]);   
    }

    if(thread_no == 3) 
    {
        sem_wait(semaphores_2[0]);
    }

    info(BEGIN, 3, thread_no);

    if (thread_no == 1)
    {
        sem_post(&semaphores[0]);
    }

    if (thread_no == 1)
    {
        sem_wait(&semaphores[1]);
    }
    

    info(END, 3, thread_no);

    if (thread_no == 5)
    {
        sem_post(&semaphores[1]);
    }

    if(thread_no == 3) 
    {
        sem_post(semaphores_2[1]);
    }

    pthread_exit(NULL);
}

//functia pentru threadurile din procesul 5
void *thread_function_for_process5(void *arg)
{
    int thread_no = *((int *)arg);
    
    info(BEGIN, 5, thread_no);
    info(END, 5, thread_no);

    pthread_exit(NULL);
}

//functia pentru threadurile din procesul 4
void *thread_function_for_process4(void *arg)
{
    int thread_no = *((int *)arg);

    if (thread_no == 6)
    {
        sem_wait(semaphores_2[1]);
    }
    
    info(BEGIN, 4, thread_no);
    info(END, 4, thread_no);

    if (thread_no == 3)
    {
        sem_post(semaphores_2[0]);
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    init();

    info(BEGIN, 1, 0);

    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8;

   //semafoare cu nume pt cerinta 2.5
   //pt aceasta cerinta am avut nevoie de semafoarte cu nume, deoarece dorim sa sincronizam threaduri din procese diferite
   semaphores_2[0] = sem_open("/semaphore1", O_CREAT, 0644, 0);
   semaphores_2[1] = sem_open("/semaphore2", O_CREAT, 0644, 0);

    // Creare proces P2
    pid2 = fork();
    if (pid2 == 0)
    {
        info(BEGIN, 2, 0);

        // Creare proces P3
        pid3 = fork();
        if (pid3 == 0)
        {
            //semafoarte pt cerinta 2.3
             sem_init(&semaphores[0], 0, 0); 
             sem_init(&semaphores[1], 0, 0); 

            info(BEGIN, 3, 0);

            // P3 creează 5 threaduri
            pthread_t thIDs[5];
            int thNumbers[5];

            for (int i = 0; i < 5; i++)
            {
                thNumbers[i] = i + 1;

                pthread_create(&thIDs[i], NULL, thread_function_for_process3, &thNumbers[i]);
            }

          // sem_wait(semaphores[1]);

            //se asteapta sa se termine threadurile procesului 3
            for (int i = 0; i < 5; i++)
            {
                pthread_join(thIDs[i], NULL);
            }

            sem_destroy(&semaphores[0]);
            sem_destroy(&semaphores[1]);

            // Creare proces P6
            pid6 = fork();
            if (pid6 == 0)
            {
                info(BEGIN, 6, 0);

                // Creare proces P8
                pid8 = fork();
                if (pid8 == 0)
                {
                    info(BEGIN, 8, 0);

                    info(END, 8, 0);
                    exit(0);
                }
                // sincronizare pt asteptarea procesului P8
                waitpid(pid8, 0, 0);

                info(END, 6, 0);
                exit(0);
            }

            // sincronizare pt asteptarea procesului P6
            waitpid(pid6, 0, 0);

            

            info(END, 3, 0);
            exit(0);
        }

        // sincronizare pt asteptarea procesului 3
        waitpid(pid3, 0, 0);

        info(END, 2, 0);
        exit(0);
    }

    // Creare proces P4
    pid4 = fork();
    if (pid4 == 0)
    {
        info(BEGIN, 4, 0);

        // P4 creează 6 threaduri
            pthread_t thIDs[6];
            int thNumbers[6];

            for (int i = 0; i < 6; i++)
            {
                thNumbers[i] = i + 1;

                pthread_create(&thIDs[i], NULL, thread_function_for_process4, &thNumbers[i]);
            }

            //se asteapta sa se termine threadurile procesului 4
            for (int i = 0; i < 6; i++)
            {
                pthread_join(thIDs[i], NULL);
            }

        // Creare proces P5
        pid5 = fork();
        if (pid5 == 0)
        {
            info(BEGIN, 5, 0);


            // P5 creează 44 threaduri
            pthread_t thIDs[44];
            int thNumbers[44];

            for (int i = 0; i < 44; i++)
            {
                thNumbers[i] = i + 1;

                pthread_create(&thIDs[i], NULL, thread_function_for_process5, &thNumbers[i]);
            }

            //se asteapta sa se termine threadurile procesului 5
            for (int i = 0; i < 44; i++)
            {
                pthread_join(thIDs[i], NULL);
            }


            info(END, 5, 0);
            exit(0);
        }

         // sincronizare pt asteptarea procesului 5
       waitpid(pid5, 0, 0);

        info(END, 4, 0);
        exit(0);
    }

    // Creare proces P7
    pid7 = fork();
    if (pid7 == 0)
    {
        info(BEGIN, 7, 0);

        info(END, 7, 0);
        exit(0);
    }

    
    //se asteapta sa se termine procesul 2
    waitpid(pid2, 0, 0);

    //se asteapta sa se termine procesul 4
   waitpid(pid4, 0, 0);

    //se asteapta sa se termine procesul 7
    waitpid(pid7, 0, 0);

    info(END, 1, 0);

    // se inchid si se elimina semafoarele cu nume
   sem_close(semaphores_2[0]);
   sem_close(semaphores_2[1]);
   sem_unlink("/semaphore1");
   sem_unlink("/semaphore2");

    return 0;
}
