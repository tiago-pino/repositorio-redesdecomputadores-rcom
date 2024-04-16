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




int UA(int fd){
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


int RR1(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                               ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;      ////0x03????????????????????????????
    buf2[2]=////???????????????????????????????????????????????????????????????
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
    buf2[2]=////???????????????????????????????????????????????????????????????
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                               ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}

int DISC(int fd){
    int res;
    unsigned char buf2[10];
    buf2[0]=FLAG;                       ////0x5c
    buf2[1]=ADDRESS_answers_from_Receiver;     ////0x03
    buf2[2]=////??????????????????????????????????????????????
    buf2[3]=buf2[1]^buf2[2];
    buf2[4]=FLAG;                       ////0x5c
    buf2[5] = '\n';
//    printf("Ola: %s; %c\n",buf2,buf[1]);
    res = write(fd,buf2,5);

    printf("%d bytes written\n", res);

    return NULL;
}





volatile int STOP=FALSE;

int main(int argc, char** argv)
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

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
    
    while (STOP==FALSE) {       /* loop for input */
        
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
              UA(fd);
              break;
            
        }
        printf(":%s:%d\n", buf, res);
        if (buf[0]=='z') STOP=TRUE;
    }



    /*
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no guião
    */
    sleep(1);
    tcsetattr(fd,TCSANOW,&oldtio);
    close(fd);
    return 0;
}
