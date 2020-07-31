#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <wiringPi.h>
#include <wiringSerial.h>

void *do_unity(void *); //채팅 메세지를 보내는 함수
void *do_arduino(void *);
int pushClient(int); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제
pthread_t thread_unity, thread_arduino;
pthread_mutex_t mutex;
#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000
int    list_c[MAX_CLIENT];
char    escape[ ] = "exit";
char    greeting[ ] = "now start\n";
char    CODE200[ ] = "Sorry No More Connection\n";


char device[] = "/dev/ttyUSB0";
int fd;
unsigned long baud = 9600;


int main(int argc, char *argv[ ])
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int    len;
    int    i, j, n;
    int    res;
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        return -1;
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        return -1;
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        return -1;
    }
    for(i = 0; i < MAX_CLIENT; i++)
        list_c[i] = INVALID_SOCK;
    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
        res = pushClient(c_socket);
	
	
        if(res < 0) { //MAX_CLIENT만큼 이미 클라이언트가 접속해 있다면,
            write(c_socket, CODE200, strlen(CODE200));
            close(c_socket);
        } else {
            write(c_socket, greeting, strlen(greeting));
            pthread_create(&thread_unity, NULL, do_unity, (void *)&c_socket);
        }
    }
}


void *do_unity(void *arg)
{
    int c_socket = *((int *)arg);
    char chatData[CHATDATA];
    int i, n;
	int index=0;
	char midstr[1024];
	char *ptr;

	fflush(stdout);
	fd = serialOpen(device, baud);
	wiringPiSetup();

    while(1) {
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
			sprintf(chatData, "%s\r\n", chatData);			
			printf("내가 입력한 말 : %s", chatData);

			index=0;
			memset(chatData, 0, sizeof(chatData));
			memset(midstr, 0, sizeof(midstr));
			
			break;
			
		}
	}

    }
}

int pushClient(int c_socket) {
    //ADD c_socket to list_c array.
    //
    ///////////////////////////////
    //return -1, if list_c is full.
    //return the index of list_c which c_socket is added.
}
int popClient(int c_socket)
{
    close(c_socket);
    //REMOVE c_socket from list_c array.
    //
    ///////////////////////////////////
}
