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

sem_t semaphores[2];

void *thread_function_for_process3(void *arg)
{
    int thread_no = *((int *)arg);

    // Perform thread-specific actions here

    // Signal T3.1 to end after T3.5 ends
    if (thread_no == 5)
    {
        sem_wait(&semaphores[0]);
    }

    info(BEGIN, 3, thread_no);
     // Wait for T3.1 to start
    if (thread_no == 1)
    {
        sem_post(&semaphores[0]);
    }

    if (thread_no == 1)
    {
        sem_wait(&semaphores[1]);
    }
    if (thread_no == 5)
    {
        sem_post(&semaphores[1]);
    }
   
    info(END, 3, thread_no);
    pthread_exit(NULL);
}

void *thread_function_for_process5(void *arg)
{
    int thread_no = *((int *)arg);
    
    info(BEGIN, 5, thread_no);
    info(END, 5, thread_no);

    pthread_exit(NULL);
}

void *thread_function_for_process4(void *arg)
{
    int thread_no = *((int *)arg);
    
    info(BEGIN, 4, thread_no);
    info(END, 4, thread_no);

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    init();

    info(BEGIN, 1, 0);

    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8;

   // Creare și deschidere semafoare cu nume
  //  semaphores[0] = sem_open("/semaphore1", O_CREAT | O_EXCL, 0644, 0);
  //  semaphores[1] = sem_open("/semaphore2", O_CREAT | O_EXCL, 0644, 0);

    // Creare proces P2
    pid2 = fork();
    if (pid2 == 0)
    {
        info(BEGIN, 2, 0);

        // Creare proces P3
        pid3 = fork();
        if (pid3 == 0)
        {
              sem_init(&semaphores[0], 0, 0); // Semafor pentru așteptarea lui T3.1
             sem_init(&semaphores[1], 0, 0); // Semafor pentru așteptarea lui T3.5

            info(BEGIN, 3, 0);

            // P3 creează 5 thread-uri
            pthread_t thread_ids[5];
            int thread_numbers[5];

            for (int i = 0; i < 5; i++)
            {
                thread_numbers[i] = i + 1;

                pthread_create(&thread_ids[i], NULL, thread_function_for_process3, &thread_numbers[i]);
            }

             // Wait for T3.5 to end
          //  sem_wait(semaphores[1]);

            //Așteptarea terminării thread-urilor lui P3
            for (int i = 0; i < 5; i++)
            {
                pthread_join(thread_ids[i], NULL);
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
                // Sincronizare pentru așteptarea terminării procesului P8
                waitpid(pid8, 0, 0);

                info(END, 6, 0);
                exit(0);
            }

            // Sincronizare pentru așteptarea terminării procesului P6
            waitpid(pid6, 0, 0);

            

            info(END, 3, 0);
            exit(0);
        }

        // Sincronizare pentru așteptarea terminării procesului P3
        waitpid(pid3, 0, 0);

        // Signal T3.1 to end after T3.5 ends
        // sem_post(semaphores[0]);

        info(END, 2, 0);
        exit(0);
    }

    // Creare proces P4
    pid4 = fork();
    if (pid4 == 0)
    {
        info(BEGIN, 4, 0);

        // P4 creează 6 thread-uri
            pthread_t thread_ids[6];
            int thread_numbers[6];

            for (int i = 0; i < 6; i++)
            {
                thread_numbers[i] = i + 1;

                pthread_create(&thread_ids[i], NULL, thread_function_for_process4, &thread_numbers[i]);
            }

            //Așteptarea terminării thread-urilor lui P4
            for (int i = 0; i < 6; i++)
            {
                pthread_join(thread_ids[i], NULL);
            }

        // Creare proces P5
        pid5 = fork();
        if (pid5 == 0)
        {
            info(BEGIN, 5, 0);


            // P5 creează 44 thread-uri
            pthread_t thread_ids[44];
            int thread_numbers[44];

            for (int i = 0; i < 44; i++)
            {
                thread_numbers[i] = i + 1;

                pthread_create(&thread_ids[i], NULL, thread_function_for_process5, &thread_numbers[i]);
            }

            //Așteptarea terminării thread-urilor lui P5
            for (int i = 0; i < 44; i++)
            {
                pthread_join(thread_ids[i], NULL);
            }


            info(END, 5, 0);
            exit(0);
        }

        // Sincronizare pentru așteptarea terminării procesului P5
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

    
    waitpid(pid2, 0, 0);

    // Așteptarea terminării procesului P4
   waitpid(pid4, 0, 0);

    // Așteptarea terminării procesului P7
    waitpid(pid7, 0, 0);

    info(END, 1, 0);

    // Închidere și eliminare semafoare cu nume
  //  sem_close(semaphores[0]);
   // sem_close(semaphores[1]);
   // sem_unlink("/semaphore1");
   // sem_unlink("/semaphore2");
    return 0;
}
