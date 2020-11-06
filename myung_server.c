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

#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000



//void *go_unity(void *); //채팅 메세지를 보내는 함수
void *come_back(void *); //채팅 메세지를 받는 함수
void *go_ras(void *); //채팅 메세지를 받는 함수
void *go_ras2(void *); //채팅 메세지를 받는 함수

void *go_handle(void *);
void *go_break(void *);
void *go_heart(void *);

int pushClient(int, char*); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제



pthread_t thread_unity, thread_back, thread_ard;
pthread_t thread_handle, thread_break, thread_heart;
pthread_mutex_t mutex;

char    escape[ ] = "exit";
char    greeting[ ] = "now start\n";
char    CODE200[ ] = "Sorry No More Connection\n";
char	client_name[CHATDATA];




char handle_device[] = "/dev/ttyUSB1";
int handle_fd;
unsigned long handle_baud = 9600;

char break_device[] = "/dev/ttyUSB2";
int break_fd;
unsigned long break_baud = 9600;

char heart_device[] = "/dev/ttyUSB0";
int heart_fd;
unsigned long heart_baud = 9600;

char midstr, midstr2, midstr3, midstr4;





struct inform_c
{
	int c_num;
	char c_name[CHATDATA];
};
struct inform_c inform_c[MAX_CLIENT]; //구조체 배열 선언


int main(int argc, char *argv[ ])
{
   
    int count; 
    for(count=0; count<MAX_CLIENT; count++)
    {
	inform_c[count].c_num = INVALID_SOCK;
    }//구조체 배열내의 c_socket 정보 초기화


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



    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
	read(c_socket, client_name ,sizeof(client_name)); 
	//클라이언트 접속과 동시에 클라이언트가 unity인지 라즈베리인지 판별하기위한 이름을 받아와 client_name 배열에 저장
        res = pushClient(c_socket, client_name);
	//클라이언트 정보 저장
	printf("%d번째 접속자의 이름은 %s\n", res, inform_c[res].c_name);
	
        if(res < 0) { //허용 클라이언트 최대수를 초과한 경우
            write(c_socket, CODE200, strlen(CODE200));
            close(c_socket);
        } else { //평상시 실행될 부분

		if( !strncmp(inform_c[res].c_name, "uni", 3) )
		{    
			
			
		}
		else if( !strncmp(inform_c[res].c_name, "ras", 3) )
		{
			
			
		}

			//pthread_create(&thread_unity, NULL, go_unity, (void *)&c_socket);
			// 유니티로 센서값 보내는 쓰레드 시작
			
			

			
			//pthread_create(&thread_handle, NULL, go_handle, (void *)&c_socket);
			//pthread_create(&thread_break, NULL, go_break, (void *)&c_socket);
			//pthread_create(&thread_heart, NULL, go_heart, (void *)&c_socket); 

			//pthread_create(&thread_ard, NULL, go_ras, (void *)&c_socket);
			pthread_create(&thread_back, NULL, come_back, (void *)&c_socket);
            
        }
    }
}


void *go_handle(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	handle_fd = serialOpen(handle_device, handle_baud);
	wiringPiSetup();
	while(1)
	{
		while(serialDataAvail(handle_fd))
		{
			midstr2 = serialGetchar(handle_fd);
			int re = (int)midstr2;
			re = re - 100;
			float re2 = (float)re / 100;
			printf("핸들:%.2f\n",re2);
			sprintf(chatData,"handle\t%.2f\n",re2);
			write(c_socket,chatData,strlen(chatData));
			serialFlush(handle_fd);
		}
	}
}

void *go_break(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	break_fd = serialOpen(break_device, break_baud);
	wiringPiSetup();
	while(1)
	{
		while(serialDataAvail(break_fd))
		{
			midstr3 = serialGetchar(break_fd);
			printf("브레이크%d\n",midstr3);
			sprintf(chatData, "break\t%d\n", midstr3);
			write(c_socket, chatData, strlen(chatData));
			serialFlush(break_fd);
		}
	}
}

void *go_heart(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	heart_fd = serialOpen(heart_device, heart_baud);
	wiringPiSetup();
	while(1)
	{
		while(serialDataAvail(heart_fd))
		{
			midstr3 = serialGetchar(heart_fd);
			printf("심박:%d\n",midstr4);
			sprintf(chatData, "heart\t%d\n", midstr4);
			write(c_socket, chatData, strlen(chatData));
			serialFlush(heart_fd);
		}
	}
}

void *go_ras(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	int index=0;
	char midstr[CHATDATA];
	char *ptr;
	int i, n;

	

	while(1)
	{
		read(c_socket, chatData, sizeof(chatData));
		if(!strncmp(chatData, "quit", 4))
		{
			close(c_socket);
		}
		printf("%s\n", chatData);
	}
}



void *come_back(void *arg)
{
	int i;
	int c_socket = *((int *)arg);
	char comeData[CHATDATA];
	char chatData[CHATDATA];
	memset(comeData, 0, sizeof(comeData));
	memset(chatData, 0, sizeof(chatData));
	static int retval = 999;
	while(1)
	{
		
		
		
		
		for(i=0; i<MAX_CLIENT; i++)
		{
			if(!strncmp(inform_c[i].c_name, "ras", 3))
			{
				read(inform_c[i].c_num, comeData, sizeof(comeData));
				printf("ras : %s", comeData);
				sprintf(chatData, "read\t%s", comeData);
			}
			else if(!strncmp(inform_c[i].c_name, "uni", 3))
			{
				write(inform_c[i].c_num, chatData, strlen(chatData));
			}
			
		}
		
		memset(comeData, 0, sizeof(comeData));
		memset(chatData, 0, sizeof(chatData));
		
	}
	
	
}


int pushClient(int c_socket, char *name) {
	int i;
	int max_num;
	for(i=0; i<MAX_CLIENT; i++)
	{
		pthread_mutex_lock(&mutex);
		if(inform_c[i].c_num == INVALID_SOCK)
		{
			inform_c[i].c_num = c_socket;
			strcpy(inform_c[i].c_name, name);
			max_num = i;
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}

	if(max_num==MAX_CLIENT)
	{
		return -1;
	}
	else
	{
		i=0;
		return max_num;
	}
}

int popClient(int c_socket)
{
	int i;
	close(c_socket);
	
	for(i=0; i<MAX_CLIENT; i++)
	{
		pthread_mutex_lock(&mutex);
		if(inform_c[i].c_num == c_socket)
		{
			inform_c[i].c_num = INVALID_SOCK;
			printf("%d번째 클라 접속종료\n", i);
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
	
}

