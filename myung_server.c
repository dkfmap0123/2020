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

//void *go_unity(void *); //채팅 메세지를 보내는 함수
void *come_unity(void *); //채팅 메세지를 받는 함수
void *go_ras(void *); //채팅 메세지를 받는 함수
void *go_ras2(void *); //채팅 메세지를 받는 함수
void *go_read(void *);
void *go_handle(void *);
void *go_break(void *);

int pushClient(int, char*); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제

#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000

pthread_t thread_unity, thread_unity2, thread_ard;
pthread_t thread_read, thread_handle, thread_break;
pthread_mutex_t mutex;

char    escape[ ] = "exit";
char    greeting[ ] = "now start\n";
char    CODE200[ ] = "Sorry No More Connection\n";
char	client_name[CHATDATA];


char read_device[] = "/dev/ttyUSB0";
int read_fd;
unsigned long read_baud = 9600;

char handle_device[] = "/dev/ttyUSB1";
int handle_fd;
unsigned long handle_baud = 9600;

char break_device[] = "/dev/ttyUSB2";
int break_fd;
unsigned long break_baud = 9600;

char midstr, midstr2, midstr3, midstr4;





struct inform_c
{
	int c_num;
	char c_name[CHATDATA];
};
struct inform_c inform_c[MAX_CLIENT]; //구조체 배열 선언


int main(int argc, char *argv[ ])this.distance_txt = GameObject.Find("distance");
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
			//pthread_create(&thread_unity, NULL, go_unity, (void *)&c_socket);
			// 유니티로 센서값 보내는 쓰레드 시작
			//pthread_create(&thread_unity2, NULL, come_unity, (void *)&c_socket);
			// 유니티에서 오는 말 받는 쓰레드 시작 

			pthread_create(&thread_read, NULL, go_read, (void *)&c_socket);
			pthread_create(&thread_handle, NULL, go_handle, (void *)&c_socket);
			pthread_create(&thread_break, NULL, go_break, (void *)&c_socket);
		}
		else if( !strncmp(inform_c[res].c_name, "ras", 3) )
		{
			pthread_create(&thread_ard, NULL, go_ras, (void *)&c_socket);

		} 
            
        }
    }
}

void *go_read(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	read_fd = serialOpen(read_device, read_baud);
	wiringPiSetup();
	while(1)
	{
		while(serialDataAvail(read_fd))
		{
			midstr = serialGetchar(read_fd);
			printf("바퀴:%d\n",midstr);
			sprintf(chatData,"read\t%d\n",midstr);
			write(c_socket, chatData, strlen(chatData));
			serialFlush(read_fd);
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

/*
void *go_unity(void *arg)
{
    	int c_socket = *((int *)arg);
    	int i, n;
	char chatData[CHATDATA], chatData2[CHATDATA];
	char midstr, midstr2;
	char *ptr;

	char send_d[] = "0\t1\n";

	int old_read = 0;
	int new_read = 0;
	
	//read_fd = serialOpen(read_device, read_baud);
	//handle_fd = serialOpen(handle_device, handle_baud);
	//wiringPiSetup();

    	while(1) 
	{
		

        	memset(chatData, 0, sizeof(chatData));
		
		while(serialDataAvail(handle_fd)&&serialDataAvail(read_fd))
		{
			
			
			//midstr = serialGetchar(read_fd);
			//midstr2 = serialGetchar(handle_fd);
			//int re = (int)midstr2;
			//re = re - 100;
			//float re2 = (float)re / 100;
			//sprintf(chatData, "%d\t%.2f\n", midstr, midstr2);
			//write(c_socket, chatData, strlen(chatData));
			//printf("data:%d %d\n", midstr, midstr2);
		
			//printf("보낸것: %d\n", chatData);	
			//printf("바퀴 : %d\n", new_read);
			//printf("핸들 : %.2f\n",re2);		
			//serialFlush(read_fd);
			//serialFlush(handle_fd);
			
			//sprintf(chatData, "%d %d\n", midstr, midstr2);		
			
			

			memset(chatData, 0, sizeof(chatData));

			
		}

	}
}
*/

void *come_unity(void *arg)
{
	int i;
	int c_socket = *((int *)arg);
	char comeData[CHATDATA];
	memset(comeData, 0, sizeof(comeData));
	static int retval = 999;
	while(1)
	{
		read(c_socket, comeData, sizeof(comeData));
		if(!strncmp(comeData, "quit", 4))
		{
			popClient(c_socket);
			
		}
		else
		{
			for(i=0; i<MAX_CLIENT; i++)
			{
				if(inform_c[i].c_num == c_socket)
				{
					printf("%d번째 클라이언트의 말 : %s\n", i, comeData);
					break;
				}
			}
		}
		memset(comeData, 0, sizeof(comeData));
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


