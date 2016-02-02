#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <semaphore.h>


#define NUM_THREADS     10
#define NUM_QUEUE       20

sem_t full, empty, mutex;

class socketqueue {
   std::queue<int> stlqueue;
public:
   void push(int sock){
      sem_wait(&empty);
      sem_wait(&mutex);
      stlqueue.push(sock);
      sem_post(&mutex);
      sem_post(&full);
   }
   int pop(){
      sem_wait(&full);
      sem_wait(&mutex);
      int rval = stlqueue.front();
      stlqueue.pop();
      sem_post(&mutex);
      sem_post(&empty);
      return(rval);
   }

} sockqueue;

//also see slides to know what to do

void *acceptRequest(void* ss_void)
{
//for((;;)
// get socket from the queue
//read request
//respond
   hServerSocket = (int)(size_t) ss_void;
   std::cout<<"Got "<<sockqueue.pop()<<std::endl;
   std::cout<<"HserverSocket: "<<hServerSocket<<std::endl;
   
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
   sem_init(&empty, PTHREAD_PROCESS_PRIVATE, NUM_QUEUE);
   sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);
   int rc;
   long t;
   for(int i = 0; i < 10; i++){
      sockqueue.push(i);
   }

   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, acceptRequest, (void *)hServerSocket);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   
//set up socket, bind, listen
//for(;;) {
//  fd = accept
//put fd in queue
//}

   /* Last thing that main() should do */
   pthread_exit(NULL);
}