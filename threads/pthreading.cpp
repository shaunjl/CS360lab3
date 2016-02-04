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
   int hSocket, hServerSocket;
   hServerSocket = (int)(size_t) ss_void;
   for(;;){
      hSocket = sockqueue.pop();
              printf("\nGot a connection from %X (%d)\n",
              Address.sin_addr.s_addr,
              ntohs(Address.sin_port));
     memset(pBuffer,0,sizeof(pBuffer));
     int rval = read(hSocket,pBuffer,BUFFER_SIZE);
     printf("Got from browser %d\n%s\n",rval, pBuffer);
 #define MAXPATH 1000
     char path[MAXPATH];
     rval = sscanf(pBuffer,"GET %s HTTP/1.1",path);
     printf("Got rval %d, path %s\n",rval,path);
     char fullpath[MAXPATH];
     sprintf(fullpath,"%s%s",strBaseDir, path);
     printf("fullpath %s\n",fullpath);
     respond(hSocket, fullpath, path);
     /* close socket */
     linger lin;
     unsigned int y=sizeof(lin);
     lin.l_onoff=1;
     lin.l_linger=10;
     setsockopt(hServerSocket, SOL_SOCKET, SO_LINGER,&lin,sizeof(lin));
     shutdown(hSocket, SHUT_RDWR);
     if(close(hSocket) == SOCKET_ERROR)
     {
      printf("\nCould not close socket\n");
      return 0;
        }
   }
   
}

int main (int argc, char *argv[])
{
   int hServerSocket = 4;
   pthread_t threads[NUM_THREADS];
   sem_init(&full, PTHREAD_PROCESS_PRIVATE, 0);
   sem_init(&empty, PTHREAD_PROCESS_PRIVATE, NUM_QUEUE);
   sem_init(&mutex, PTHREAD_PROCESS_PRIVATE, 1);
   int rc;
   long t;
   for(t=0; t<NUM_THREADS; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, acceptRequest, (void *)hServerSocket);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }
   
   for(;;){
      hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
      sockqueue.push(hSocket);
   }

   /* Last thing that main() should do */
   pthread_exit(NULL);
}