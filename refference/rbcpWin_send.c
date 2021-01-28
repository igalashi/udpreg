/*************************************************
*                                                *
* SiTCP Bus Control Protocol                     *
* Send function                                  *
*                                                *
* 2009/08/14 Tomohisa Uchida                     *
*                                                *
*************************************************/

#ifdef WIN32
#define __USE_W32_SOCKETS
#include <winsock.h>
#include <stdio.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define u_short unsigned short int
#define closesocket close
#endif
int rbcpWin_send(char* ipAddr, u_short port, struct bcp_header* sndHeader, char* sndData){

#ifdef WIN32
  WSADATA wsaData;
#else
#define SOCKET int
typedef struct sockaddr_in SOCKADDR_IN;
#endif
  SOCKADDR_IN sitcpAddr;

  struct timeval timeout;
  fd_set setSelect;
  
  SOCKET sock;
  int sndDataLen;
  int cmdPckLen;

  char sndBuf[2048];
  int i, j = 0;
  int rcvdBytes;
  char rcvdBuf[1024];
  int numReTrans =0;

  /* Create a Socket */
  puts("\nCreate socket...\n");

#ifdef WIN32
  if(WSAStartup(MAKEWORD(1,1),&wsaData) != 0){
    perror("WSAStartup Error!\n");
    return -1;
  }
#endif
     
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  /*
  if(sock < 0){
    perror("socket() is failed!\n");
    WSACleanup();
    return -2;
  }
  */
  sitcpAddr.sin_family      = AF_INET;
  sitcpAddr.sin_port        = htons(port);
  sitcpAddr.sin_addr.s_addr = inet_addr(ipAddr);

  sndDataLen = (int)sndHeader->length;

  printf(" Length = %i\n",sndDataLen);
	

  memcpy(sndBuf,sndHeader, sizeof(struct bcp_header));

  if(sndHeader->command==RBCP_CMD_WR){
    memcpy(sndBuf+sizeof(struct bcp_header),sndData,sndDataLen);
    cmdPckLen=sndDataLen + sizeof(struct bcp_header);
  }else{
    cmdPckLen=sizeof(struct bcp_header);
  }


#ifdef DEBUG
  for(i=0; i< cmdPckLen;i++){
    if(j==0) {
      printf("\t[%.3x]:%.2x ",i,(unsigned char)sndBuf[i]);
      j++;
    }else if(j==3){
      printf("%.2x\n",(unsigned char)sndBuf[i]);
      j=0;
    }else{
      printf("%.2x ",(unsigned char)sndBuf[i]);
      j++;
    }
  }
  if(j!=3) printf("\n");
#endif

  /* send a packet*/

  sendto(sock, sndBuf, cmdPckLen, 0, (struct sockaddr *)&sitcpAddr, sizeof(sitcpAddr));
  puts("The packet have been sent!\n");

  /* Receive packets*/
  
  puts("\nWait to receive the ACK packet...");


  while(numReTrans<2){

    FD_ZERO(&setSelect);
    FD_SET(sock, &setSelect);

    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
 
    if(select(sock+1, &setSelect, NULL, NULL,&timeout)==0){
      /* time out */
      puts("\n***** Timeout ! *****");
      sndHeader->id++;
      memcpy(sndBuf,sndHeader, sizeof(struct bcp_header));
      sendto(sock, sndBuf, cmdPckLen, 0, (struct sockaddr *)&sitcpAddr, sizeof(sitcpAddr));
      numReTrans++;
      FD_ZERO(&setSelect);
      FD_SET(sock, &setSelect);
    } else {
      /* receive packet */
      if(FD_ISSET(sock,&setSelect)){
	rcvdBytes=recvfrom(sock, rcvdBuf, 2048, 0, NULL, NULL);
	rcvdBuf[rcvdBytes]=0;
#ifdef DEBUG
	puts("\n***** A pacekt is received ! *****.");
	puts("Received data:");

	j=0;

	for(i=0; i<rcvdBytes; i++){
	  if(j==0) {
	    printf("\t[%.3x]:%.2x ",i, sendHeader,(unsigned char)rcvdBuf[i]);
	    j++;
	  }else if(j==3){
	    printf("%.2x\n",(unsigned char)rcvdBuf[i]);
	    j=0;
	  }else{
	    printf("%.2x ",(unsigned char)rcvdBuf[i]);
	    j++;
	  }
	  if(i==7) printf("\n Data:\n");
	}

	if(j!=3) puts(" ");
#else
	puts("\nRecived data:\n");

	j=0;

	for(i=8; i<rcvdBytes; i++){
	  if(j==0) {
	    printf("[0x%.8x] %.2x ",ntohl(sndHeader->address)+i-8,(unsigned char)rcvdBuf[i]);
	    j++;
	  }else if(j==7){
	    printf("%.2x\n",(unsigned char)rcvdBuf[i]);
	    j=0;
	  }else if(j==4){
	    printf("- %.2x ",(unsigned char)rcvdBuf[i]);
	    j++;
	  }else{
	    printf("%.2x ",(unsigned char)rcvdBuf[i]);
	    j++;
	  }
	}

	if(j!=15) puts("\n");

#endif
	numReTrans = 3;
	closesocket(sock);
#ifdef WIN32
	WSACleanup();
#endif

	return(rcvdBytes);
      }
    }
  }
  closesocket(sock);
#ifdef WIN32
  WSACleanup();
#endif
  return -3;
}
