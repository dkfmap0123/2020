// chat_client_thread.c
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>
#include        <netinet/in.h>
#include        <sys/socket.h>
#include        <sys/select.h>
#include   	<pthread.h>
#include   	<signal.h>
#include 	<wirePi.h>
#include	<wireSerial.h>
#include	<errno.h>

#define         CHATDATA        1024
#define		IP		220.68.167.80
#define		PORT		9000
char device[] = "/dev/ttyUSB0";
int fd;
unsigned long baud = 9600;

void*       do_send_chat(void *);
void*       do_receive_chat(void *);

pthread_t   thread_1, thread_2;

char            escape[] = "exit";
char            nickname[] = "ras";

main(int argc, char *argv[])
{
        int     c_socket;
        struct  sockaddr_in c_addr;
        int     len;
        char    chatData[CHATDATA];
        char    buf[CHATDATA];
        int     nfds;
        fd_set  read_fds;
        int     n;

        if (argc < 3) {
                printf("usgae : %s ip_address port_number\n", argv[0]);
                exit(-1);
        }

        c_socket = socket(PF_INET, SOCK_STREAM, 0);

        memset( &c_addr, 0, sizeof(c_addr) );
        c_addr.sin_addr.s_addr = inet_addr(IP);
        c_addr.sin_family = AF_INET;
        c_addr.sin_port = htons(atoi(PORT));


        if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1) {
                printf("Can not connect\n");
                return -1;
        }
	
	write(c_socket, nickname, strlen(nickname));
   	pthread_create(&thread_2, NULL, do_receive_chat, NULL);


   	close(c_socket);
}



void *do_receive_chat()
{
	char   chatData[CHATDATA];
	int   n;
	int index=0;
	char midstr[CHATDATA];
	char *ptr;

	fflush(stdout);
	fd = serialOpen(device, board);
	wirePiSetup();

	while(1) 
	{
		memset(chatData, 0, sizeof(chatData));
		while(serialDataAvail(fd))
		{
			midstr[index] = serialGetchar(fd);
			index = index + 1;
			fflush(stdout);

			if( (ptr=strchr(midstr, '\n')) != NULL)
			{
				strcpy(chatData, midstr);
				write(c_socket, chatData, strlen(chatData));
				index = 0;
				memset(chatData, 0, sizeof(chatData));
				memset(midstr, 0, sizeof(midstr));

				break;
			}
		}
  	 }
}
