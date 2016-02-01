#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>

#define NUM_THREADS     20

class socketqueue {
   std::queue<int> stlqueue;
public:
   void push(int sock){
      stlqueue.push(sock);
   }
   int pop(){
      int rval = stlqueue.front();
      stlqueue.pop();
      return(rval);
   }

} sockqueue;

//also see slides to know what to do

void *PrintHello(void *threadid)
{
//for((;;)
// get socket from the queue
//read request
//respond
   long tid;
   tid = (long)threadid;
   printf("Hello World! It's me, thread #%ld!\n", tid);
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for(int i = 0; i < 10; i++){
      sockqueue.push(i);
   }
   for(int i = 0; i < 10; i++){
      std::cout<<"Got "<<sockqueue.pop()<<std::endl;
   }

   exit(0);

   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
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