#include "stdio.h"

char* reverseWords(char* s) 
{
  int i;
  char* startPos=s;
  char* stopPos=s;
  while(1)
  {
    if((*(char*)stopPos==' ')||(*(char*)stopPos=='\0'))
    {
      for(i=0;i<((int)stopPos-1-(int)startPos)/2;i++)
      {
        *(char*)(startPos+i)=*(char*)(startPos+i)^*(char*)(stopPos-1-i);
        *(char*)(stopPos-1-i)=*(char*)(startPos+i)^*(char*)(stopPos-1-i);
      }
      if(*(char*)stopPos=='\0')
      {
        break;
      }
      else
      {
        startPos=stopPos+1;
      }
    }
    ++stopPos;
  }
  return(s);
}

int main(void)
{
  char* ch="123";
  printf("%s\n",reverseWords(ch));
  float t_i=0;
  printf("Please input length(inch):");
  scanf("%f",&t_i);
  printf("%f inch equal to %f cm.\n",t_i,t_i*2.54);
  return(0);
}
