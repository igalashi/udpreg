/*************************************************
*                                                *
* SiTCP Bus Control Protocol                     *
* Header file                                    *
*                                                *
* 2009/08/14 Tomohisa Uchida                     *
*                                                *
*************************************************/

struct bcp_header{
  unsigned char type;
  unsigned char command;
  unsigned char id;
  unsigned char length;
  unsigned int address;
};

#define RBCP_VER 0xFF
#define RBCP_CMD_WR 0x80
#define RBCP_CMD_RD 0xC0
