#ifndef __OPENMV_E
#define __OPENMV_E
#include "includes.h"
#include "string.h"
#define MAX_len 9
/*
************
*Openmv数据*
************
*/
typedef struct
{
  unsigned char data[MAX_len];
  int Code_Data;
    
  int Color_data;
  int X;
  int Y;
}Openmv_Data;

/*
************
*单片机接收数据*
************
*/
typedef struct
{
  unsigned char data[MAX_len];
  int Code_Data;
}MCU_Data;

extern char dar[3];

extern Openmv_Data Code_data;
extern Openmv_Data Block_data;
void OPNMV_DataHandle(unsigned char ucData);

#endif









