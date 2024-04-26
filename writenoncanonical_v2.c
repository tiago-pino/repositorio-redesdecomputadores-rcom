/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

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
#define START_DATA 6

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
    /*for(int i=0;i<5;i++){
        printf("Ola: %s; %x\n",buf2,buf2[i]);
    }*/
    
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
    int n_carateres=1,i=5;
    //unsigned char buf2[10];
    char BBC2 = 0x00;
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;             ////0x01
    buf2[2]=CONTROL_I0;                         ////0x80
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    //buf2[5] = '\n';
    
    while(i<(tamanho+5)){
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


int I_1(int fd, char *buf2, int tamanho){
    int res;
    int n_carateres=1,i=5;
    //unsigned char buf2[10];
    char BBC2 = 0x00;
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_sent_by_Sender;             ////0x01
    buf2[2]=CONTROL_I1;                         ////0xC0
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    //buf2[5] = '\n';
    while(i<(tamanho+5)){       //porque contando com o header ficam mais 5 unidades de dados
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


int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];
    int i, sum = 0, speed = 0;
    char valor_lido;            //Valores lidos da porta serie



    if ( (argc < 2) ||
         ((strcmp("/dev/ttyS10", argv[1])!=0) &&
          (strcmp("/dev/ttyS11", argv[1])!=0) )) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }


    /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
    */


    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd < 0) { perror(argv[1]); exit(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
        perror("tcgetattr");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */



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



/*    for (i = 0; i < 255; i++) {
        buf[i] = 'a';
    }   */

    /*testing*/
/*    buf[25] = '\n';*/

/*
    unsigned char buf2[10];
    buf2[0]=0x5c;
    buf2[1]=0x01;
    buf2[2]=0x06;
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=0x5c;

    buf2[5] = '\n';
    printf("Ola: %s; %c\n",buf2,buf[1]);
//    res = write(fd,buf,255);
    /*res = write(fd,buf,5);*/
/*    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);
*/
/*    SET(fd);

    while (STOP==FALSE) {       /* loop for input */
        
/*        res = read(fd,buf,255);   /* returns after 5 chars have been input */
        //buf[res]=0;               /* so we can printf... */
/*        printf("buf0: %x \n", buf[0]);
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
              break;
            
        }
        printf(":%s:%d\n", buf, res);
        if (buf[0]=='z') STOP=TRUE;
    }
*/











    /*    if(connectionParameters.role == 0){
        int numero_tentativas=0;                    ////Defino uma variavel para defenir o numetro de tentativas de coneção que faço        (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


       while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
                
            alarm(connectionParameters.timeOut);                                        ////inicia o alarme para o timeout defenido
*/ 

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
                } 
                printf(":%x:%d\n", valor_lido, res);                ////Para comentar
                if (valor_lido=='z') STOP=TRUE;                     ////Para comentar
            }




            printf(":%s:%d\n", buf, res);
            if (buf[0]=='z') STOP=TRUE;
        /*}*/
                

    /*}*/


    STOP = FALSE;
    /*
    O ciclo FOR e as instruções seguintes devem ser alterados de modo a respeitar
    o indicado no guião
    */


////////////
/*    (void) signal(SIGALRM, atende);  // instala rotina que atende interrupcao


    while(tentativas<connectionParameters.numTries && STOP==FALSE){                        //while(timeout) alarme.c
        ////////////        


        alarm(connectionParameters.timeOut);               
*/            
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
    /*}*/

    UA_close(fd);

    sleep(1);//////////////////////Importante
    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }


    ///////////////////
    /*
        if(tentativas==connectionParameters.numTries){

            printf("Erro ao estabelecer a ligacao.\n")
            return -1;
        }
    */
    ///////////////////

    

    close(fd);
    return 0;
}
