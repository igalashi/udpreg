/********************************************************************
 *                                                                   *
 * SiTCP debug program for Remote Bus Control Protocl                *
 *                                                                   *
 * 2009/08/14 T.Uchida                                               *
 *                                                                   *
 * gcc -Wall -W -pedantic -mno-cygwin rbcpWin.c -lws2_32 -o rbcpWin  *
 *                                                                   *
 ********************************************************************/
/*
#define DEBUG
*/
#define MAX_LINE_SIZE 1024
#define MAX_ARG_SIZE 20
/*
#define RBCP_VER 0xFF
#define RBCP_CMD_WR 0x80
#define RBCP_CMD_RD 0xC0
*/
#define DEFAULT_IP 192.168.0.16
#define UDP_BUF_SIZE 2048

#include "rbcpWin.h"
#include "rbcpWin_send.c"
#include "myAtoi.c"

int rbcpWin_send(char* ipAddr, u_short port, struct bcp_header* sndHeader, char* sndData);

int OnHelp();

int DispatchCommand(char* pszVerb,
		    char* pszArg1,
		    char* pszArg2,
		    char* ipAddr,
		    unsigned int rbcpPort,
		    struct bcp_header* sndHeader
		    );

int myScanf(char* inBuf, char* argBuf1, char* argBuf2,  char* argBuf3);
int myGetArg(char* inBuf, int i, char* argBuf);

int main(int argc, char* argv[]){

  char sitcpIpAddr[20];
  u_short sitcpPort;

  struct bcp_header sndHeader;

  char tempKeyBuf[MAX_LINE_SIZE];
  char szVerb[MAX_ARG_SIZE];
  char szArg1[MAX_ARG_SIZE];
  char szArg2[MAX_ARG_SIZE];
  int rtnValue;

  if(argc != 3){
    puts("\nThis application controls bus of a SiTCP chip for debugging.");
    printf("Usage: %s <IP address> <Port #>\n\n", argv[0]);
    return -1;
  }else{
    strcpy(sitcpIpAddr,argv[1]);
    sitcpPort   = (u_short)atoi(argv[2]);
  }


  sndHeader.type=RBCP_VER;
  sndHeader.id=0;

  while(1){
    printf("SiTCP-RBCP$ ");
    fgets(tempKeyBuf, MAX_LINE_SIZE, stdin);
    if((rtnValue=myScanf(tempKeyBuf,szVerb, szArg1, szArg2))<0){
      printf("Erro myScanf(): %i\n",rtnValue);
      return -1;
    }

    sndHeader.id++;
      
    if(DispatchCommand(szVerb, szArg1, szArg2, sitcpIpAddr, sitcpPort, &sndHeader)<0) break;
  }
  return 0;
}


int DispatchCommand(char* pszVerb,
		    char* pszArg1,
		    char* pszArg2,
		    char* ipAddr,
		    unsigned int rbcpPort,
		    struct bcp_header* sndHeader
		    ){
  unsigned int tempInt;

  if(strcmp(pszVerb, "wrb") == 0){
    tempInt = myAtoi(pszArg2);    
    pszArg2[0]= (char)(0xFF & tempInt);

    sndHeader->command= RBCP_CMD_WR;
    sndHeader->length=1;
    sndHeader->address=htonl(myAtoi(pszArg1));
    
    return rbcpWin_send(ipAddr, rbcpPort, sndHeader, pszArg2);
  }
  else if(strcmp(pszVerb, "wrs") == 0){
    tempInt = myAtoi(pszArg2);    
    pszArg2[1]= (char)(0xFF & tempInt);
    pszArg2[0]= (char)((0xFF00 & tempInt)>>8);
 
    sndHeader->command= RBCP_CMD_WR;
    sndHeader->length=2;
    sndHeader->address=htonl(myAtoi(pszArg1));

    printf("Address = 0x%x\n",myAtoi(pszArg1));
    
    return rbcpWin_send(ipAddr, rbcpPort, sndHeader, pszArg2);
  }
  else if(strcmp(pszVerb, "wrw") == 0){
    tempInt = myAtoi(pszArg2);

    printf("tempInt = %x\n",tempInt);

    pszArg2[3]= (char)(0xFF & tempInt);
    pszArg2[2]= (char)((0xFF00 & tempInt)>>8);
    pszArg2[1]= (char)((0xFF0000 & tempInt)>>16);
    pszArg2[0]= (char)((0xFF000000 & tempInt)>>24);

    sndHeader->command= RBCP_CMD_WR;
    sndHeader->length=4;
    sndHeader->address=htonl(myAtoi(pszArg1));

    return rbcpWin_send(ipAddr, rbcpPort, sndHeader, pszArg2);
  }
  else if(strcmp(pszVerb, "rd") == 0){
    sndHeader->command= RBCP_CMD_RD;
    sndHeader->length=myAtoi(pszArg2);
    sndHeader->address=htonl(myAtoi(pszArg1));
    
    return rbcpWin_send(ipAddr, rbcpPort, sndHeader, pszArg2);
  }
  else if(strcmp(pszVerb, "help") == 0){
    return OnHelp();
  }
  else if(strcmp(pszVerb, "quit") == 0){
    return -1;
  }
  puts("Con not find the command!\n");
  return 0;
  
}


int OnHelp()
{
	puts("Command list\n");
	puts("wrb address byte_data");
	puts("wrs address short_data");
	puts("wrw address word_data");
	puts("rd address length");
	puts("quit");
	puts("quit from this program\n");

	return 0;
}

int myScanf(char* inBuf, char* argBuf1, char* argBuf2,  char* argBuf3)
{
  int i=0;

  argBuf1[0]='\0';
  argBuf2[0]='\0';
  argBuf3[0]='\0';

  if((i=myGetArg(inBuf, i, argBuf1))>0){
    if((i=myGetArg(inBuf, i, argBuf2))>0){
      return myGetArg(inBuf, i, argBuf3);
    }else{
      return i;
    }
  }else{
    return i;
  }
  return i;
}

int myGetArg(char* inBuf, int i, char* argBuf){
  int j;

  while(i<MAX_LINE_SIZE){
    if(inBuf[i]==' '){
      i++;
    }else if((inBuf[i]=='\n')||(inBuf[i]=='\r')){
      return 0;
    }else {
      break;
    }
  }

  for(j=0;j<MAX_ARG_SIZE;j++){
    if(i<MAX_LINE_SIZE){
      if(inBuf[i]==' '){
	argBuf[j]='\0';
      }else if((inBuf[i]=='\0')||(inBuf[i]=='\n')||(inBuf[i]=='\r')){
	argBuf[j]='\0';
	return 0;
      }else{
	argBuf[j]=inBuf[i];
	i++;
      }
    }else{
      return -1;
    }
  }
  return i;
}
