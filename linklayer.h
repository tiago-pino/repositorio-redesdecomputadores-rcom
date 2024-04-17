#ifndef LINKLAYER
#define LINKLAYER

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>                               ////ACRESCENTEI////Acrescentei para controlar o timeout

typedef struct linkLayer{
    char serialPort[50];
    int role; //defines the role of the program: 0==Transmitter, 1=Receiver
    int baudRate;
    int numTries;
    int timeOut;
} linkLayer;

//ROLE
#define NOT_DEFINED -1
#define TRANSMITTER 0
#define RECEIVER 1


//SIZE of maximum acceptable payload; maximum number of bytes that application layer should send to link layer
#define MAX_PAYLOAD_SIZE 1000

//CONNECTION deafault values
#define BAUDRATE_DEFAULT B38400
#define MAX_RETRANSMISSIONS_DEFAULT 3
#define TIMEOUT_DEFAULT 4
#define _POSIX_SOURCE 1 /* POSIX compliant source */

//MISC
#define FALSE 0
#define TRUE 1


volatile int STOP=FALSE;


////
#define FLAG 0x5c
#define ADDRESS_sent_by_Sender 0x03
#define ADDRESS_answers_from_Receiver 0x03
#define ADDRESS_sent_by_Receiver 0x01
#define ADDRESS_answers_from_Sender 0x01

#define CONTROL_SET 0x08            ////Control_Set
#define CONTROL_UA 0x06             ////Control_Unsigned Acknowlegment
                                ////BCC1=XOR(A,C)
////




// Opens a conection using the "port" parameters defined in struct linkLayer, returns "-1" on error and "1" on sucess
int llopen(linkLayer connectionParameters);
// Sends data in buf with size bufSize
int llwrite(char* buf, int bufSize);
// Receive data in packet
int llread(char* packet);
// Closes previously opened connection; if showStatistics==TRUE, link layer should print statistics in the console on close
int llclose(int showStatistics);



////
int SET(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                       ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;     ////0x03
    buf2[2]=CONTROL_SET;                ////0x08
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                       ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int UA_Receiver(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x03
    buf2[2]=CONTROL_UA;                         ////0x06
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}




llopen(linkLayer connectionParameters){
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[MAX_PAYLOAD_SIZE];         ////Defino um buffer que comporte o payload todo. Se calhar é MAX_PAYLOAD_SIZE+2 por causa do /n mas não sei se é necessário

    if ((connectionParameters.serialPort == NULL) ||
         ((strcmp("/dev/ttyS10", connectionParameters.serialPort)!=0) &&
          (strcmp("/dev/ttyS11", connectionParameters.serialPort)!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(NOT_DEFINED);      //NOT_DEFINED (-1) porque as portas não coincidem com as que deviam
    }
    
    
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { 
        perror(argv[1]); 
        exit(-1); 
        }

    if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;             ////IMPORTANTE////                     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;             ////////VER///////                     /* blocking read until 5 chars received */

    /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) próximo(s) caracter(es)
    */


    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    
    
    if(connectionParameters.role == 0){
        int numero_tentativas=0;                    ////Defino uma variavel para defenir o numetro de tentativas de coneção que faço
        

        SET(fd);
        
/*        int timeout = TIMEOUT_DEFAULT;              ////Defino o timeout com o default
        struct timespec inicio, atual;              ////Para defenir variaveis par verificar o timeout
        
        clock_gettime(CLOCK_REALTIME, &inicio);     ////Obtenho o tempo inicial

        while (STOP==FALSE) {           ////       /* loop for input */
        
/*
////Para verificar o timeout; tenho de verificar em que unidades está o timeout (segundos,milisegundos, etc)
        clock_gettime(CLOCK_REALTIME, &atual);      ////Obtenho o tempo atual
        if(TIMEOUT_DEFAULT<connectionParameters.timeOut){   ////Verifico se o timeout default é menor do que o indicado nos parametros da ligação para caso seja inferior defenir o timeout com o dos parametros da ligação
            timeout=connectionParameters.timeOut;
        }
        if(atual.tv_sec> timeout){
            printf("Erro: TimeOut\n");
            numero_tentativas++;
            if (numero_tentativas>connectionParameters.numTries){
                exit(-1);                  ////se ocorrer timeout devolve -1 que indica erro
            }
            else{
                break;
            }
        }
////
*/

        res = read(fd,buf,MAX_PAYLOAD_SIZE);                    ////retorna depois de ler MAX_PAYLOAD_SIZE chars do input                /* returns after 5 chars have been input */
        //buf[res]=0;               /* so we can printf... */
        printf("buf0: %x \n", buf[0]);
        printf("buf0: %x \n", buf[1]);
        printf("buf0: %x \n", buf[2]);
        printf("buf0: %x \n", buf[3]);
        printf("buf0: %x \n", buf[4]);
        printf("buf0: %c \n", buf[0]);
        printf("buf0: %c \n", buf[1]);
        printf("buf0: %c \n", buf[2]);
        printf("buf0: %c \n", buf[3]);
        printf("buf0: %c \n", buf[4]);
        printf("%d\n",fd);
        if((buf[0]==0x5c) && (buf[1]==0x03) && (buf[2]==0x06) && (buf[3]==(0x03^0x06)) && (buf[4]==0x5c)){          ////Verifico se recebi a mensagem de SET para enviar um UA
              //I_0(fd);
              exit(1);                  ////caso receba UA(unsigned acknowlegment) retorna com sucesso
              //break;
        }
        printf(":%s:%d\n", buf, res);
        if (buf[0]=='z') STOP=TRUE;
        }
    }

    if(connectionParameters.role == 1){
        while (STOP==FALSE) {                /* loop for input */
        
        res = read(fd,buf,255);   /* returns after 5 chars have been input */
        //buf[res]=0;               /* so we can printf... */
        printf("buf0: %x \n", buf[0]);
        printf("buf0: %x \n", buf[1]);
        printf("buf0: %x \n", buf[2]);
        printf("buf0: %x \n", buf[3]);
        printf("buf0: %x \n", buf[4]);
        printf("buf0: %c \n", buf[0]);
        printf("buf0: %c \n", buf[1]);
        printf("buf0: %c \n", buf[2]);
        printf("buf0: %c \n", buf[3]);
        printf("buf0: %c \n", buf[4]);
        printf("%d\n",fd);
        if((buf[0]==0x5c) && (buf[1]==0x03) && (buf[2]==0x08) && (buf[3]==(0x03^0x08)) && (buf[4]==0x5c)){          ////Verifico se recebi a mensagem de SET para enviar um UA
              UA_Receiver(fd);
              exit(1);              ////caso receba SET(unsigned acknowlegment) retorna com sucesso
              //break;
        }
        printf(":%s:%d\n", buf, res);
        if (buf[0]=='z') STOP=TRUE;
        }
    }
    
}



#endif
