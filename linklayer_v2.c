#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "linklayer.h"




//////////////////////////////////////
//state machine
volatile int STOP=FALSE;
int state = 0;
#define START 0
#define FLAGRCV 1
#define ARCV 2
#define CRCV 3
#define BCCOK 4
#define STOP_STATE_MACHINE 5
int reject = 0;
int n_sequencia_write = 0;
int n_sequencia_read = 1;

///////////////////////////////////

int tentativas = 1;
int fd = 3;     //Como não me é passado o argumento do file descriptor defino-o aqui.   //Inicio a 3 porque é o filedescriptor abitual. Não é o mais recomendado
////
#define FLAG 0x5c
#define ADDRESS_sent_by_Sender 0x01
#define ADDRESS_answers_from_Receiver 0x01
#define ADDRESS_sent_by_Receiver 0x03
#define ADDRESS_answers_from_Sender 0x03

#define CONTROL_SET 0x07            ////Control_Set
#define CONTROL_UA 0x06             ////Control_Unsigned Acknowlegment
                                ////BCC1=XOR(A,C)
#define CONTROL_I0 0x80
#define CONTROL_I1 0xC0
#define CONTROL_RR0 0x01
#define CONTROL_RR1 0x11
#define CONTROL_REJ0 0x05
#define CONTROL_REJ1 0x15
#define CONTROL_DISC 0x10
////

////I, SET e DISC frames are designated commands
////UA, RR and REJ frames replies


void atende()                   // atende alarme
{
    //printf("alarme # %d\n", conta);
    //flag=1;
    tentativas++;
}



void statemachine_SET(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x07) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x07)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}

void statemachine_UA_open(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x06) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x06)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}



void statemachine_DISC_0(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x10) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x10)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}


void statemachine_DISC_1(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x03) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x10) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x03^0x10)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}

void statemachine_UA_close(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x03) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x06) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x03^0x06)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}



//////////////////////////////////////////////////////////////////////////

void statemachine_I0(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x80) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x80)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}


void statemachine_I1(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0xC0) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0xC0)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}







void statemachine_RR0_REJ0(unsigned char byte)
{
    
    switch (state)
    {
        case START:
            if(byte==0x5c){
                state = FLAGRCV;
                reject=0; 
            }
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x01){
                state = CRCV;
                reject=0;
            } 
            else if(byte==0x05){
                state = CRCV;
                reject=1;
            } 
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x01)) state = BCCOK;
            else if(byte==(0x01^0x05)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break; 
        case BCCOK:
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}



void statemachine_RR1_REJ1(unsigned char byte)
{   
    
    switch (state)
    {
        case START:
            if(byte==0x5c){
                state = FLAGRCV;
                reject=0;
            }
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x11){
                state = CRCV;
                reject=0;
            } 
            else if(byte==0x15){
                state = CRCV;
                reject=1;
            } 
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
            
        case CRCV:
            if(byte==(0x01^0x11)) state = BCCOK;
            else if(byte==(0x01^0x15)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}

/*
void statemachine_REJ0(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x05) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x05)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}



void statemachine_REJ1(unsigned char byte)
{
    switch (state)
    {
        case START:
            if(byte==0x5c) state = FLAGRCV;
            break;
        case FLAGRCV:
            if(byte==0x01) state = ARCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;   
            break;
        case ARCV:
            if(byte==0x15) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x15)) state = BCCOK;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START; 
            break;  
        case BCCOK:  
            if(byte==0x5c) state = STOP_STATE_MACHINE;
            else state = START;  
            break;
        default:
            printf("Saiu da maquina de estados!\n");
            break;
            
    }
}
*/







////
int SET(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                       ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;     ////0x01
    buf2[2]=CONTROL_SET;                ////0x07
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                       ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int UA_open(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x01
    buf2[2]=CONTROL_UA;                         ////0x06
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}


int I_0(int fd, unsigned char *buf, int tamanho){
    int res;
    int n_carateres=1,i=0;
    unsigned char buf2[10];
    unsigned char BCC2 = 0x00;
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;             ////0x01
    buf2[2]=CONTROL_I0;                         ////0x80
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    //buf2[5] = '\n';
    res = write(fd,buf2,5);
    //printf("%d bytes written\n", res);
    //printf("%x; %u; %c\n",buf2[2]);
    //printf("Ola: %s.\nfd=%d\n",buf2,fd);
    //printf("Ola: %s.\nfd=%d\n",buf,fd);
    while(i<tamanho){
        BCC2 = BCC2^buf[i];
        if(buf[i]==0x5c){
            //printf("%d bytes written_1\n", res);
            unsigned char _0x5d_ = (unsigned char)0x5d;
            res = write(fd,&_0x5d_,1);
            //printf("%d bytes written_2\n", res);
            unsigned char _0x7c_ = (unsigned char)0x7c;
            res = write(fd,&_0x7c_,1);
            //printf("%d bytes written_3\n", res);
        }
        else if(buf[i]==0x5d){
            //printf("%d bytes written_4\n", res);
            unsigned char _0x5d_ = (unsigned char)0x5d;
            res = write(fd,&_0x5d_,1);
            unsigned char _0x7d_ =(unsigned char) 0x7d;
            res = write(fd,&_0x7d_,1);
        }
        else{
            res = write(fd,&buf[i],1);
            //printf("%d bytes written_4\n", res);
        }
        i++;
    }
    //printf("%d bytes written\n", res);
    res = write(fd,&BBC2,1);
    //printf("%d bytes written\n", res);
    unsigned char _0x5c_ = (unsigned char)0x5c;
    res = write(fd,&_0x5c_,1);

        /*printf("buf20: %x \n", buf2[0]);
        printf("buf20: %x \n", buf2[1]);
        printf("buf20: %x \n", buf2[2]);
        printf("buf20: %x \n", buf2[3]);
        printf("buf20: %x \n", buf2[4]);
        printf("buf20: %c \n", buf2[0]);
        printf("buf20: %c \n", buf2[1]);
        printf("buf20: %c \n", buf2[2]);
        printf("buf20: %c \n", buf2[3]);
        printf("buf20: %c \n", buf2[4]);*/
    /*buf2[0]=0x5c;
    buf2[1]=0x03;
    buf2[2]=0x08;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=0x5c;
    buf2[5] = '\n';*/
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    

    printf("%d bytes written\n", res);

    return tamanho;
    

}


int I_1(int fd, char *buf, int tamanho){
    int res;
    int n_carateres=1,i=0;
    unsigned char buf2[10];
    unsigned char BCC2 = 0x00;
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;             ////0x01
    buf2[2]=CONTROL_I1;                         ////0xC0
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    //buf2[5] = '\n';
    while(i<tamanho+){       //porque contando com o header ficam mais 5 unidades de dados
        BCC2 = BCC2^buf[i];
        if(buf[i]==0x5c){
            unsigned char _0x5d_ = (unsigned char)0x5d;
            res = write(fd,&_0x5d_,1);
            unsigned char _0x7c_ = (unsigned char)0x7c;
            res = write(fd,&_0x7c_,1);
        }
        else if(buf[i]==0x5d){
            unsigned char _0x5d_ = (unsigned char)0x5d;
            res = write(fd,&_0x5d_,1);
            unsigned char _0x7d_ =(unsigned char) 0x7d;
            res = write(fd,&_0x7d_,1);
        }
        else{
            res = write(fd,&buf[i],1);
        }
        i++;
    }
    res = write(fd,&BBC2,1);
    unsigned char _0x5c_ = (unsigned char)0x5c;
    res = write(fd,&_0x5c_,1);

        /*printf("buf20: %x \n", buf2[0]);
        printf("buf20: %x \n", buf2[1]);
        printf("buf20: %x \n", buf2[2]);
        printf("buf20: %x \n", buf2[3]);
        printf("buf20: %x \n", buf2[4]);
        printf("buf20: %c \n", buf2[0]);
        printf("buf20: %c \n", buf2[1]);
        printf("buf20: %c \n", buf2[2]);
        printf("buf20: %c \n", buf2[3]);
        printf("buf20: %c \n", buf2[4]);*/
    /*buf2[0]=0x5c;
    buf2[1]=0x03;
    buf2[2]=0x08;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=0x5c;
    buf2[5] = '\n';*/
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    

    printf("%d bytes written\n", res);

    return tamanho;
    

}

int RR1(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x03????????????????????????????
    buf2[2]=CONTROL_RR1;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int RR0(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x03????????????????????????????
    buf2[2]=CONTROL_RR0;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}


int REJ1(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x03????????????????????????????
    buf2[2]=CONTROL_REJ1;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int REJ0(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x03????????????????????????????
    buf2[2]=CONTROL_REJ0;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}








int UA_close(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Sender;        ////0x01
    buf2[2]=CONTROL_UA;                         ////0x06
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}




int DISC_0(int fd){
    
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                                   ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;          ////0x03
    buf2[2]=CONTROL_DISC;                           ////0x08
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                                   ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}


int DISC_1(int fd){
    
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                                   ////0x5c
    buf2[1]=ADDRESS_sent_by_Receiver;          ////0x03
    buf2[2]=CONTROL_DISC;                           ////0x08
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                                   ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}





llopen(linkLayer connectionParameters){
    int c, res;//fd,
    struct termios oldtio,newtio;
    ////char buf[MAX_PAYLOAD_SIZE];         ////Defino um buffer que comporte o payload todo. Se calhar é MAX_PAYLOAD_SIZE+2 por causa do /n mas não sei se é necessário
    char valor_lido;        //Para comportar os valores lidos da porta série
    state = 0;              //Garante que em todas as chamadas, o estado inicial da state machine é START
    STOP = FALSE;
    tentativas = 1;

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
        

        








        
/*        int timeout = TIMEOUT_DEFAULT;              ////Defino o timeout com o default
        struct timespec inicio, atual;              ////Para defenir variaveis par verificar o timeout
        
        clock_gettime(CLOCK_REALTIME, &inicio);     ////Obtenho o tempo inicial

*/        //while (STOP==FALSE) {           ////       /* loop for input */
        
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















///////Fazer Retransmição........
    
    
        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
                
            alarm(connectionParameters.timeOut);                                        ////inicia o alarme para o timeout defenido


            SET(fd);
                
            while(STOP==FALSE){
                        
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ////////
                statemachine_UA_open(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    STOP=TRUE;
                    state=START;
                    alarm(0);           //Para parar o alarme
                    UA_close(fd);       //posso por isto depois do while
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            }




            printf(":%s:%d\n", buf, res);
            if (buf[0]=='z') STOP=TRUE;
        }
                

    }
            
    



    if(connectionParameters.role == 1){
        
        while(STOP==FALSE){
                
            res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
            ////////
            /*if (res < 0) {
                perror("Erro na leitura");
                break;
            }*/
            ///////////
            statemachine_SET(valor_lido);
            printf("STATE IS: %d\n",state);
            
            if(state==STOP_STATE_MACHINE){
                STOP=TRUE;
                state=START;
            } 
            printf(":%x:%d\n", valor_lido, res);                ////Para comentar
            if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
        }
            //SET
            
        UA_open(fd);
        
    
    }
    if(tentativas==connectionParameters.numTries){

        printf("Erro ao estabelecer a ligacao.\n")
        return -1;
    }
    return fd;          //Retorna o filedescriptor da ligação estabelecida id da ligação 

}




































// Closes previously opened connection; if showStatistics==TRUE, link layer should print statistics in the console on close
int llclose(linkLayer connectionParameters, int showStatistics){
    state = 0;          //Para a state machine começãr no inicio                //Garante que em todas as chamadas, o estado inicial da state machine é START
    
    int res;
    ////char buf[MAX_PAYLOAD_SIZE];         ////Defino um buffer que comporte o payload todo. Se calhar é MAX_PAYLOAD_SIZE+2 por causa do /n mas não sei se é necessário
    char valor_lido;        //Para comportar os valores lidos da porta série           
    STOP = FALSE;
    tentativas = 1;
    
    
    
    if(connectionParameters.role == 0){
        
        
        ////////////
        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


            alarm(connectionParameters.timeOut);               
            
            DISC_0(fd);

            while(STOP==FALSE){
                            
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ///////////
                statemachine_DISC_1(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    STOP=TRUE;
                    state=START;
                    ///////////////
                    alarm(0);           //Para parar o alarme
                    /////////////////
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z'){
                    STOP=TRUE;                     ////Para comentar
                } 
            }
        }
    }



    if(connectionParameters.role == 1){
        

        

        while(STOP==FALSE){
                        
            res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
            statemachine_DISC_0(valor_lido);
            printf("STATE IS: %d\n",state);
            if(state==STOP_STATE_MACHINE){
                STOP=TRUE;
                state=START;
            } 
            printf(":%x:%d\n", valor_lido, res);                ////Para comentar
            if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
        }


        ////////////
        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


            alarm(connectionParameters.timeOut); 




            DISC_1(fd);

            while(STOP==FALSE){
                            
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ///////////
                statemachine_UA_close(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    STOP=TRUE;
                    state=START;
                    ///////////////
                    alarm(0);           //Para parar o alarme
                    /////////////////
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            }
        ///////////////////
        }
        ///////////////////
    }
    //sleep(1);                         ////Não sei para que serve
    
    if(tentativas==connectionParameters.numTries){

        printf("Erro ao fechar a ligacao.\n")
        return -1;
    }
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 1;    
}




// Sends data in buf with size bufSize
int llwrite(unsigned char* buf, int bufSize){
    state = 0;          //Para a state machine começãr no inicio                //Garante que em todas as chamadas, o estado inicial da state machine é START
    
    int res;
    ////char buf[MAX_PAYLOAD_SIZE];         ////Defino um buffer que comporte o payload todo. Se calhar é MAX_PAYLOAD_SIZE+2 por causa do /n mas não sei se é necessário
    char valor_lido;        //Para comportar os valores lidos da porta série           
    STOP = FALSE;
    tentativas = 1;
    
    
    STOP = FALSE;
    int n_carateres_escritos=0;


    if(n_sequencia_write==0){
            

        ////////////
/*        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


            alarm(connectionParameters.timeOut);
*/

            n_carateres_escritos=I_0(fd,buf,bufSize);        ///Envio do I_0
            
            while(STOP==FALSE){
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ///////////
                statemachine_RR1_REJ1(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    if(reject==0){  //Verifiico se recebi RR(reject==0) ou REJ(reject==1) para reenviar o I(frame de dados)
                        STOP=TRUE;
                    }
                    else{
                        STOP=FALSE;
                        I_0(fd,buf,bufSize);                //Para retransmitir
                    }
                    state=START;
                    ///////////////
/*                    alarm(0); */          //Para parar o alarme
                    /////////////////
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            }
        ///////////////////
/*        }*/
        ///////////////////
    }
    else if(n_sequencia_write==1){

        ////////////
/*        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


            alarm(connectionParameters.timeOut);
*/    
            n_carateres_escritos=I_1(fd,buf,bufSize);

            while(STOP==FALSE){
                
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ///////////
                statemachine_RR0_REJ0(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    if(reject==0){  //Verifiico se recebi RR(reject==0) ou REJ(reject==1) para reenviar o I(frame de dados)
                        STOP=TRUE;
                    }
                    else{
                        STOP=FALSE;
                        I_1(fd,buf,bufSize);
                    }
                    state=START;
                    ///////////////
/*                   alarm(0);       */     //Para parar o alarme
                    /////////////////
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            
            }
        ///////////////////
/*        }*/
        ///////////////////
    }
    else{
        printf("Erro no número de sequência.\n");
        return -1;
    }


    n_sequencia_write=!n_sequencia_write;
    return n_carateres_escritos;////////////////*   /*/*/**//*/*
}
















// Receive data in packet
int llread(unsigned char* packet){
    state = 0;          //Para a state machine começãr no inicio                //Garante que em todas as chamadas, o estado inicial da state machine é START
    
    int res;
    ////char buf[MAX_PAYLOAD_SIZE];         ////Defino um buffer que comporte o payload todo. Se calhar é MAX_PAYLOAD_SIZE+2 por causa do /n mas não sei se é necessário
    char valor_lido;        //Para comportar os valores lidos da porta série           
    STOP = FALSE;
    tentativas = 1;
    
    
    STOP = FALSE;
    int n_carateres_lidos=0;

    int i = 0;
    int ret;
    char BCC2=0x00;
    char BCC2_anterior;
    char BCC2_enviado;
    unsigned char valor_lido;
    unsigned char valor_lido2;


    if(n_sequencia_read==0){
            

        ////////////
/*        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


            alarm(connectionParameters.timeOut);
*/

            ////n_carateres_escritos=I_0(fd,buf,bufSize);        ///Envio do I_0
            

        reject=1;
        while (reject==1){
        
        reject=0;


            while(STOP==FALSE){
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ///////////
                statemachine_I1(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    if(reject==0){  //Verifiico se recebi RR(reject==0) ou REJ(reject==1) para reenviar o I(frame de dados)
                        STOP=TRUE;
                    }
                    else{
                        STOP=FALSE;
                    }
                    state=START;
                    ///////////////
/*                    alarm(0);  */         //Para parar o alarme
                    /////////////////
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            }

            ///Parte de receber dados

            STOP=FALSE;
            reject=0;
            while(STOP==FALSE){
                
                ret = read(fd, &valor_lido,1);
                if(valor_lido==0x5d){
                    ret = read(fd,&valor_lido_2,1);
                    if(valor_lido2==0x7c){
                        packet[i]=0x5c;
                    }
                    else if(valor_lido2==0x7d){
                        packet[i]==0x5d;
                    }
                    else{
                        printf("Sequencia de 0x5d seguda de um valor diferente dos espectados.\n");
                    }
                }
                else if(valor_lido==0x5c){
                    BCC2_enviado=packet[i-1];
                    packet[i-1]="/n";              ////Para limpar do buffer o que considerei dado, masera o BCC2_enviado
                    BCC2=BCC2_anterior;         ////Porque faço XOR com o BCC2enviado no loop anterior, e este não deve contar para o XOR
                    n_carateres_lidos=i-1;      ////Porque li o BCC2enviado que não é dado e por isso tenho de retirar este dado
                    break;                      ///Porque neste estado termina a receção de dados devido a ter chegado á ultima flag
                }
                else{
                    packet[i]=valor_lido;
                }
                BCC2_anterior=BCC2;
                BCC2=BCC2^packet[i];
                i++;
            }

            if(BCC2==BCC2_enviado){
                reject=0;           //Não é necessário. Não faz nada
                printf("Sucesso.\n");
                RR0(fd);
                return n_carateres_lidos;
            }
            else{
                STOP=FALSE;        //Para fazer retransmição...
                reject=1;
                REJ0(fd);
            }

        }


        ///////////////////
/*        }*/
        ///////////////////
    }
    else if(n_sequencia_read==1){

        ////////////
/*        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao

        
        while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


            alarm(connectionParameters.timeOut);
*/    
            //n_carateres_escritos=I_1(fd,buf,bufSize);

        reject=1;
        while (reject==1){
        
        reject=0;

            while(STOP==FALSE){
                
                res = read(fd,&valor_lido,1);         //Lê um caratér e envia para a máquina de estados verificar para que estado avança
                ////////
                /*if (res < 0) {
                    perror("Erro na leitura");
                    break;
                }*/
                ///////////

                statemachine_I0(valor_lido);
                printf("STATE IS: %d\n",state);
                if(state==STOP_STATE_MACHINE){
                    STOP=TRUE;
                    state=START;
                    ///////////////
/*                    alarm(0);   */        //Para parar o alarme
                    /////////////////
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            
            }



            ///Parte de receber dados

            STOP=FALSE;
            reject=0;
            while(STOP==FALSE){
                
                ret = read(fd, &valor_lido,1);
                if(valor_lido==0x5d){
                    ret = read(fd,&valor_lido_2,1);
                    if(valor_lido2==0x7c){
                        packet[i]=0x5c;
                    }
                    else if(valor_lido2==0x7d){
                        packet[i]==0x5d;
                    }
                    else{
                        printf("Sequencia de 0x5d seguda de um valor diferente dos espectados.\n");
                    }
                }
                else if(valor_lido==0x5c){
                    BCC2_enviado=packet[i-1];
                    packet[i-1]="/n";              ////Para limpar do buffer o que considerei dado, masera o BCC2_enviado
                    BCC2=BCC2_anterior;         ////Porque faço XOR com o BCC2enviado no loop anterior, e este não deve contar para o XOR
                    n_carateres_lidos=i-1;      ////Porque li o BCC2enviado que não é dado e por isso tenho de retirar este dado
                    break;                      ///Porque neste estado termina a receção de dados devido a ter chegado á ultima flag
                }
                else{
                    packet[i]=valor_lido;
                }
                BCC2_anterior=BCC2;
                BCC2=BCC2^packet[i];
                i++;
            }

            if(BCC2==BCC2_enviado){
                reject=0;           //Não é necessário. Não faz nada
                printf("Sucesso.\n");
                RR1(fd);
                return n_carateres_lidos;
            }
            else{
                STOP=FALSE;        //Para fazer retransmição...
                reject=1;
                REJ1(fd);
            }

        }














        ///////////////////
/*        }*/
        ///////////////////
    }
    else{
        printf("Erro no número de sequência.\n");
        return -1;
    }

    
    n_sequencia_read=!n_sequencia_read;
    return n_carateres_lidos;////////////////*   /*/*/**//*/*
}

















///int llwrite(unsigned char* buf, int bufSize);
///int llread(unsigned char* packet);
/*

int i = 0;
int ret;
char BCC2=0x00;
char BCC2_anterior;
char BCC2_enviado;
char valor_lido;
char valor_lido2;

while(state==BCCOK){
    ret = read(fd, &valor_lido,1);
    statemachine_I0(valor_lido);
    if(valor_lido==0x5d){
        ret = read(fd,&valor_lido_2,1);
        if(valor_lido2==0x7c){
            buf[i]=0x5c;
        }
        if(valor_lido2==0x7d){
            buf[i]==0x5d;
        }
        else{
            printf("Sequencia de 0x5d seguda de um valor diferente dos espectados.\n")
        }
    }
    if(valor_lido==0x5c){
        BCC2_enviado=buf[i-1];
        buf[i-1]="/n";              ////Para limpar do buffer o que considerei dado, masera o BCC2_enviado
        BCC2=BCC2_anterior;         ////Porque faço XOR com o BCC2enviado no loop anterior, e este não deve contar para o XOR
        break;                      ///Porque neste estado termina a receção de dados devido a ter chegado á ultima flag
    }
    BCC2_anterior=BCC2;
    BCC2=BCC2^buf[i];
    i++;
}

if(BCC2==BCC2_enviado){
    printf("Sucesso.\n");
    return 1;
}
else{
    state=BBCOK;        //Para fazer retransmição...
}


*/







//int llread(unsigned char* packet){
/*

int i = 0;
int ret;
char BCC2=0x00;
char BCC2_anterior;
char BCC2_enviado;
char valor_lido;
char valor_lido2;
STOP=FALSE;
while(STOP==FALSE){
    ret = read(fd, &valor_lido,1);
    if(valor_lido==0x5d){
        ret = read(fd,&valor_lido_2,1);
        if(valor_lido2==0x7c){
            packet[i]=0x5c;
        }
        else if(valor_lido2==0x7d){
            packet[i]==0x5d;
        }
        else{
            printf("Sequencia de 0x5d seguda de um valor diferente dos espectados.\n")
        }
    }
    if(valor_lido==0x5c){
        BCC2_enviado=packet[i-1];
        packet[i-1]="/n";              ////Para limpar do buffer o que considerei dado, masera o BCC2_enviado
        BCC2=BCC2_anterior;         ////Porque faço XOR com o BCC2enviado no loop anterior, e este não deve contar para o XOR
        n_carateres_lidos=i-1;      ////Porque li o BCC2enviado que não é dado e por isso tenho de retirar este dado
        break;                      ///Porque neste estado termina a receção de dados devido a ter chegado á ultima flag
    }
    BCC2_anterior=BCC2;
    BCC2=BCC2^packet[i];
    i++;
}

if(BCC2==BCC2_enviado){
    printf("Sucesso.\n");
    return n_carateres_lidos;
}
else{
    STOP=FALSE;        //Para fazer retransmição...
    reject=1;
    REJ0(fd);
}


*/

