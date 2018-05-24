#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>


struct timeval *timestamp;
volatile int sigcount=0;

void interrupt_handler(int sig){
	gettimeofday(&timestamp[sigcount],NULL);
	sigcount++;
}

int main(int argc, char const *argv[])
{

	if (argc != 3)
    {
        printf("Wrong number of argument!! Put the number of the samples and the distance between them! \n");
        exit(1);
    }

//Reading argument and create the buffer
	int n = atoi(argv[1]);
	float t = atof(argv[2]);

//Malloc the array of the timestamps
	timestamp = (struct timeval *)malloc(sizeof(struct timeval) * n);
	
//helping variable to separate sec and msec of input argument t 
	int h;
	float hh;
	h = (int) t;
	hh = (t - (float)h)*pow(10,6); 

//Necessary structs for setitimer
 	struct sigaction sa;
 	struct itimerval timer;

 /* Install timer_handler as the signal handler for SIGVTALRM. */
 	memset (&sa, 0, sizeof (sa));
 	sa.sa_handler = &interrupt_handler;
 	sigaction (SIGALRM, &sa, NULL);

 /* Configure the timer to expire after specific time */
 	timer.it_value.tv_sec = h;
 	timer.it_value.tv_usec = hh; 
 /* ... and the same every time after that. */
 	timer.it_interval.tv_sec = h;
 	timer.it_interval.tv_usec = hh;
 /* Start a virtual timer. It counts down whenever this process is
   executing. */
 	setitimer (ITIMER_REAL, &timer, NULL);

 /* Do busy work. */
 	while (sigcount<n){
 		if(sigcount>=0){
 			if(h>0){
 				usleep(h*pow(10,6)+hh);
 			}
 			else{
 				usleep(hh);
 			}
 		}
 	}

 /* Disable Timer */
 	timer.it_value.tv_sec = 0;
 	timer.it_value.tv_usec = 0;
 	timer.it_interval.tv_sec = 0;
 	timer.it_interval.tv_usec = 0;
 	setitimer (ITIMER_REAL, &timer, NULL);
 
 //The name of my file
 	char* name;
 	name = malloc(strlen(argv[1])+strlen(argv[2])+strlen("every")+1);
 	strcat(name,argv[1]);
 	strcat(name,"every");
 	strcat(name,argv[2]);

//The variable i am going to print in the txt file
 	double sample;

//Writing data to 'txt' file
 	FILE *f = fopen(name, "w");
	int i = 0;
    for (i; i < n; i++)
    {
        sample = (double) (timestamp[i].tv_usec*pow(10,-6) + timestamp[i].tv_sec);
        fprintf(f,"%lf \n", sample);
    }
	fclose(f);

//free malloc
	free(timestamp);
	free(name);
}