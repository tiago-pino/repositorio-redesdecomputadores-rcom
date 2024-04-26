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

///////////////////////////////////

int tentativas = 1;

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







void statemachine_RR0(unsigned char byte)
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
            if(byte==0x01) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x01)) state = BCCOK;
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



void statemachine_RR1(unsigned char byte)
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
            if(byte==0x11) state = CRCV;
            else if(byte==0x5c) state = FLAGRCV;
            else state = START;
            break;
        case CRCV:
            if(byte==(0x01^0x11)) state = BCCOK;
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


int I_0(int fd, char *buf2, int tamanho){
    int res;
    int n_carateres=1,i=0;
    //unsigned char buf2[10];
    char BBC2 = 0x00;
    while(i<tamanho){
        BBC2 = BBC2^buf2[i];
        if(buf2[i]==0x5c){
            res = write(fd,0x5d,1);
            res = write(fd,0x7c,1);
        }
        else if(buf2[i]==0x5d){
            res = write(fd,0x5d,1);
            res = write(fd,0x7d,1);
        }
        else{
            res = write(fd,buf2[i],1);
        }
        i++;
    }
    res = write(fd,BBC2,1);
    res = write(fd,0x5c,1);

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
    int fd,c, res;
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
int llclose(int fd, linkLayer connectionParameters, int showStatistics){
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