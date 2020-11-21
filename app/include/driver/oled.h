#ifndef OLED_H_
#define OLED_H_

// ͷ�ļ�����
//==============================================================================
#include "c_types.h"			// ������������
#include "driver/i2c_master.h"	// IIC
//==============================================================================

// �궨��
//=============================================================================
#define		OLED_CMD  	0		// ����
#define		OLED_DATA 	1		// ����

#define 	SIZE 		16		//��ʾ�ַ��Ĵ�С
#define 	Max_Column	128		//�������
#define		Max_Row		64		//�������
#define		X_WIDTH 	128		//X��Ŀ��
#define		Y_WIDTH 	64	    //Y��Ŀ��
#define		IIC_ACK		0		//Ӧ��
#define		IIC_NO_ACK	1		//��Ӧ��
//=============================================================================

// ��������
//=============================================================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time);

void ICACHE_FLASH_ATTR IIC_Init_JX(void);
u8 ICACHE_FLASH_ATTR OLED_Write_Command(u8 OLED_Byte);

u8 ICACHE_FLASH_ATTR OLED_Write_Data(u8 OLED_Byte);

void ICACHE_FLASH_ATTR OLED_WR_Byte(u8 OLED_Byte, u8 OLED_Type);

void ICACHE_FLASH_ATTR OLED_Clear(void);

void ICACHE_FLASH_ATTR OLED_Set_Pos(u8 x, u8 y);

void ICACHE_FLASH_ATTR OLED_Init(void);

void ICACHE_FLASH_ATTR OLED_ShowChar(u8 x, u8 y, u8 Show_char);

void ICACHE_FLASH_ATTR OLED_ShowString(u8 x, u8 y, u8 * Show_char);

//=============================================================================

#endif /* OLED_H_ */
