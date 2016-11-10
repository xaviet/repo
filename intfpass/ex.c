#include <stdio.h>  
#include <time.h>  
#include <sys/time.h>  
#include <stdlib.h>  
#include <signal.h>  
  
static int count = 0;  
static struct itimerval oldtv;  
  
void set_timer()  
{  
    struct itimerval itv;  
    itv.it_interval.tv_sec = 0;  
    itv.it_interval.tv_usec = 1000;  
    itv.it_value.tv_sec = 0;  
    itv.it_value.tv_usec = 1000;  
    setitimer(ITIMER_REAL, &itv, &oldtv);  
}  
  
void signal_handler(int m)  
{  
    count ++;  
    //printf("%d\n", count);  
}  
  
int main()  
{  
    printf("%d\n", count);  
    signal(SIGALRM, signal_handler);  
    set_timer();  
    while(count < 1000);  
    exit(0);  
    printf("%d\n", count);  
    return 1;  
}  



//#include <sys/time.h>  
//#include <sys/select.h>  
//#include <time.h>  
//#include <stdio.h>  
   
//int setTimer(int seconds,int mseconds,int* c)  
//{  
  //struct timeval temp;  
  
  //temp.tv_sec=0;  
  //temp.tv_usec=4000;  
  
  //select(0,NULL,NULL,NULL,&temp); 
  //*c+=1; 
  ////printf("timer\n");  
  
  //return(0);  
//}  
  
//int main()  
//{  
  //int i;  
  //int c=0;
  //printf("%d\n",c);
  //for(i=0;i<1000;i++)  
  //{
    ////printf("%d\n",i);
    //setTimer(1,0,&c);  
  //}
  //printf("%d\n",c);
  //return(0);  
//}  
