#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include "read.cpp"
#include <sstream>
#include <string>
#include <sys/signal.h>
#include <pthread.h>
#include <queue>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         10000
#define QUEUE_SIZE           5
#define STR_SIZE            150

std::queue<int> socketqueue;

void acceptRequest(void * ss_void)
{
    int hSocket, hServerSocket;  /* handle to socket */
    hSocket = socketqueue.front();
    socketqueue.pop();
    hServerSocket = (int)(size_t) ss_void;
    struct sockaddr_in Address; /* Internet socket address stuct */
    char pBuffer[BUFFER_SIZE];
    char strBaseDir[STR_SIZE];
    unsigned nReadAmount;
    void handler (int status); 
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
    }
}

int main(int argc, char* argv[])
{
    int hSocket,hServerSocket, numThreads;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;
    char strBaseDir[STR_SIZE];
    unsigned nReadAmount;
    void handler (int status); 

    if(argc < 4)
      {
        printf("\nUsage: server <host-port> <num-threads> <dir>\n");
        return 0;
      }
    else
      {
        nHostPort=atoi(argv[1]);
        numThreads=atoi(argv[2]);
        strcpy(strBaseDir,argv[3]);
      }

    printf("\nStarting server");

    printf("\nMaking socket");
    /* make a socket */
    hServerSocket=socket(AF_INET,SOCK_STREAM,0);

    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* fill address struct */
    Address.sin_addr.s_addr=INADDR_ANY;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nBinding to port %d",nHostPort);

    /* bind to a port */
    if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
                        == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
 /*  get port number */
    getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
    printf("opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

        printf("Server\n\
              sin_family        = %d\n\
              sin_addr.s_addr   = %d\n\
              sin_port          = %d\n"
              , Address.sin_family
              , Address.sin_addr.s_addr
              , ntohs(Address.sin_port)
            );


    printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }

    int optval = 1;
    setsockopt(hServerSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        // First set up the signal handler
    struct sigaction sigold, signew;

    signew.sa_handler=handler;
    sigemptyset(&signew.sa_mask);
    sigaddset(&signew.sa_mask,SIGINT);
    signew.sa_flags = SA_RESTART;
    sigaction(SIGPIPE,&signew,&sigold);
    sigaction(SIGHUP,&signew,&sigold);

   pthread_t threads[numThreads];
   int rc;
   long t;
   for(t=0; t<numThreads; t++){
      printf("In main: creating thread %ld\n", t);
      rc = pthread_create(&threads[t], NULL, acceptRequest, (void *)*hServerSocket);
      if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
      }
   }

    for(;;)
    {
        printf("\nWaiting for a connection\n");
        /* get the connected socket */
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
        socketqueue.push(hSocket);
    }

    /* Last thing that main() should do */
   pthread_exit(NULL);
}

void handler (int status)
{
    printf("received signal %d\n",status);
}
