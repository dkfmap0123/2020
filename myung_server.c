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

void *socket_connect(void *);

void *go_handle(void *);
void *go_break(void *);
void *go_heart(void *);
void *go_stop(void *);
void *quit_wait(void *);


int pushClient(int, char*); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제



pthread_t thread_unity, thread_back, thread_ard;
pthread_t thread_handle, thread_break, thread_heart, thread_stop, thread_quit, thread_connect;
pthread_mutex_t mutex;

char    escape[ ] = "exit";
char    greeting[ ] = "now start\n";
char    CODE200[ ] = "Sorry No More Connection\n";
char	client_name[CHATDATA];




char handle_device[] = "/dev/ttyUSB1";
int handle_fd;
unsigned long handle_baud = 9600;

char break_device[] = "/dev/ttyUSB3";
int break_fd;
unsigned long break_baud = 9600;

char heart_device[] = "/dev/ttyUSB0";
int heart_fd;
unsigned long heart_baud = 9600;

char stop_device[] = "/dev/ttyUSB2";
int stop_fd;
unsigned long stop_baud = 9600;

char midstr1, midstr2, midstr3, midstr4, midstr5;





struct inform_c
{
	int c_num;
	char c_name[CHATDATA];

	pthread_t handle_thread;
	pthread_t break_thread;
	pthread_t heart_thread;
	pthread_t stop_thread;
};
struct inform_c inform_c[MAX_CLIENT]; //구조체 배열 선언


int main(int argc, char *argv[ ])
{
    int count; 
    int status;
    for(count=0; count<MAX_CLIENT; count++)
    {
	inform_c[count].c_num = INVALID_SOCK;
    }//구조체 배열내의 c_socket 정보 초기화

	pthread_create(&thread_connect, NULL, socket_connect, (void *)&count);
	pthread_join(thread_connect, (void **)&status);	
		
	
}



void *socket_connect(void *arg)
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int    len;
    int    i, j, n;
    int    res;
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        exit(0);
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        exit(0);
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        exit(0);
    }


	
    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
	read(c_socket, client_name ,sizeof(client_name)); 
	//클라이언트 접속과 동시에 클라이언트가 unity인지 라즈베리인지 판별하기위한 이름을 받아와 client_name 배열에 저장
        res = pushClient(c_socket, client_name);
	//클라이언트 정보 저장
	printf("%d번째 접속자의 이름은 %s\n", res, inform_c[res].c_name);
	

		
			pthread_create(&thread_back, NULL, come_back, (void *)&c_socket);
		
		
			if(!strncmp(inform_c[res].c_name, "uni", 3))
			{
				pthread_create(&thread_handle, NULL, go_handle, (void *)&c_socket);
			}
			
			//pthread_create(&thread_break, NULL, go_break, (void *)&c_socket);
			//pthread_create(&thread_heart, NULL, go_heart, (void *)&c_socket); 
			//pthread_create(&thread_stop, NULL, go_stop, (void *)&c_socket);
		
			pthread_create(&thread_quit, NULL, quit_wait, (void *)&c_socket);
    }


}




void *go_handle(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];

	


	int i;
	handle_fd = serialOpen(handle_device, handle_baud);
	wiringPiSetup();
	while(1)
	{
		while(serialDataAvail(handle_fd))
		{
			midstr1 = serialGetchar(handle_fd);
			int re = (int)midstr1;
			re = re - 100;
			float re2 = (float)re / 100;
			//printf("핸들:%.2f\n",re2);
			sprintf(chatData,"handle\t%.2f\n",re2);
			//write(c_socket,chatData,strlen(chatData));
			for(i=0; i<100; i++)
			{
				char chacha[CHATDATA];
				sprintf(chacha, "%db\n", i);
				printf("%s\n", chacha);
				write(c_socket, chacha, strlen(chacha));
				sleep(1);
				
			}
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
			midstr2 = serialGetchar(break_fd);
			printf("브레이크%d\n",midstr2);
			sprintf(chatData, "break\t%d\n", midstr2);
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
			printf("심박:%d\n",midstr3);
			sprintf(chatData, "heart\t%d\n", midstr3);
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

void *go_stop(void *arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	stop_fd = serialOpen(stop_device, stop_baud);
	wiringPiSetup();
	while(1)
	{
		while(serialDataAvail(stop_fd))
		{
			midstr4 = serialGetchar(stop_fd);
			printf("일시정지%d\n",midstr4);
			sprintf(chatData, "stop\t%d\n",midstr4);
			write(c_socket, chatData, strlen(chatData));
			serialFlush(stop_fd);
		}
	}
}

void *quit_wait(void *arg)
{
	int c_socket = *((int *)arg);
	int k;

	char quitData[CHATDATA];
	
		for(k=0; k<MAX_CLIENT; k++)
		{
			read(c_socket, quitData, sizeof(quitData));
			if(!strncmp(quitData, "quit", 4))
			{
				printf("%s 종료\n", inform_c[k].c_name);
				popClient(c_socket);
			}
		}		
		

}


void *come_back(void *arg)
{
	int i, j;
	int read_size;
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
				read_size = read(inform_c[i].c_num, comeData, sizeof(comeData));
				
				if(!strncmp(comeData, "quit", 4))
				{
					popClient(inform_c[i].c_num);
				}
				else if(read_size > 0)
				{
					printf("바퀴:%s\n", comeData);
					sprintf(chatData, "read\t%s\n", comeData);
				}
				
				
				for(j=0; j<MAX_CLIENT; j++)
				{
					if(!strncmp(inform_c[j].c_name, "uni", 3))
					{
						write(inform_c[j].c_num, chatData, strlen(chatData));
					}
					
				}
			}

			
		}
		
		memset(comeData, 0, sizeof(comeData));
		memset(chatData, 0, sizeof(chatData));
		i=0;
		j=0;
		
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
	close(c_socket);
	
	
}

