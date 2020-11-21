#ifndef OLED_H_
#define OLED_H_

// 头文件引用
//==============================================================================
#include "c_types.h"			// 数据类型声明
#include "driver/i2c_master.h"	// IIC
//==============================================================================

// 宏定义
//=============================================================================
#define		OLED_CMD  	0		// 命令
#define		OLED_DATA 	1		// 数据

#define 	SIZE 		16		//显示字符的大小
#define 	Max_Column	128		//最大列数
#define		Max_Row		64		//最大行数
#define		X_WIDTH 	128		//X轴的宽度
#define		Y_WIDTH 	64	    //Y轴的宽度
#define		IIC_ACK		0		//应答
#define		IIC_NO_ACK	1		//不应答
//=============================================================================

// 函数声明
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
