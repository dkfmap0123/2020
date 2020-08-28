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

void *come_ard1(void *); //채팅 메세지를 보내는 함수
void *come_ard2(void *);


pthread_t thread_unity1, thread_unity2, thread_ard1, thread_ard2;
pthread_mutex_t mutex;

#define CHATDATA 1024

char device[] = "/dev/ttyUSB0";
char device2[] = "/dev/ttyUSB1";

char *sum_ard_data[2];

char *ptr, *ptr2;
int fd, fd2;
unsigned long baud = 9600;

void main()
{
	char midstr, midstr2;
	int index=0;
	char *ptr;
	
	
	
	fd = serialOpen(device, baud);
	fd2 = serialOpen(device2, baud);
	wiringPiSetup();
	serialFlush(fd);
	serialFlush(fd2);

	while(1)
	{
		while(serialDataAvail(fd) && serialDataAvail(fd2))
		{
			midstr= serialGetchar(fd);
			midstr2= serialGetchar(fd2);
			printf("%d %f\n", midstr, midstr2);
			serialFlush(fd);
			serialFlush(fd2);
			

			
			
				
				
				
		}
	}

}





