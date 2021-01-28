#include <stdio.h>
#include <string.h>

unsigned int myAtoi(char* str){
  char temp[256];
  int i=0, j=0, value=0, endProc=0;
  strcpy(temp,str);

  if((temp[i]=='0')||(temp[i]==' ')) i++;
  if((temp[i]=='x')||(temp[i]=='X')){
    i++;
    j++;
    while((endProc==0)&&(temp[i]!='\0')){
      if((temp[i]>='0')&&(temp[i]<='9')){
	value=value*16;
	value+=temp[i]-'0';
	i++;
      }else if((temp[i]>='a')&&(temp[i]<='f')){
	value=value*16;
	value+=temp[i]-'a'+10;
	i++;
      }else if((temp[i]>='A')&&(temp[i]<='F')){
	value=value*16;
	value+=temp[i]-'A'+10;
	i++;
      }else{
	endProc=1;
	puts("endProc");
      }
    }
    if(j>8){
      puts("Error: too large value is detected.");
      return 0xFFFFFFFF;
    }
  }else{
    while((temp[i]>='0')&&(temp[i]<='9')){
      value=value*10;
      value+=temp[i]-'0';
      i++;
      j++;
      if(j>10){
	puts("Error: too large value is detected.");
	return 0xFFFFFFFF;
      }
    }
  }

  return value;
}
