// 头文件引用
//==============================================================================
#include "driver/oled.h"
#include "driver/oledfont.h"	// 字符表
//==============================================================================

// OLED相关函数
//==============================================================================
// IIC初始化( SCL==IO14、SDA==IO2 )
//-----------------------------------------------------------
void ICACHE_FLASH_ATTR IIC_Init_JX(void)
{
	i2c_master_gpio_init();		// 初始化SCL(IO14)、SDA(IO2)
}
//-----------------------------------------------------------

// 向OLED写入指令字节
//----------------------------------------------------------------------------
u8 ICACHE_FLASH_ATTR OLED_Write_Command(u8 OLED_Byte)
{
	i2c_master_start();					// 发送起始信号

	i2c_master_writeByte(0x78);			// 选择总线上的OLED[0111 100X B]
	if(i2c_master_checkAck()==false)
	{i2c_master_stop(); return 0;}

	i2c_master_writeByte(0x00);			// [0x00]表示下一字节写入的是[指令]
	if(i2c_master_checkAck()==false)
	{i2c_master_stop(); return 0;}

	i2c_master_writeByte(OLED_Byte);	// [指令代码]
	if(i2c_master_checkAck()==false)
	{i2c_master_stop(); return 0;}

	i2c_master_stop();					// 发送停止信号

	return true;						// 返回写入成功
}
//----------------------------------------------------------------------------

// 向OLED写入数据字节
//----------------------------------------------------------------------------
u8 ICACHE_FLASH_ATTR OLED_Write_Data(u8 OLED_Byte)
{
	i2c_master_start();					// 发送起始信号

	i2c_master_writeByte(0x78);			// 选择总线上的OLED[0111 100X B]
	if(i2c_master_checkAck()==false)
	{i2c_master_stop(); return 0;}

	i2c_master_writeByte(0x40);			// [0x40]表示下一字节写入的是[数据]
	if(i2c_master_checkAck()==false)
	{i2c_master_stop(); return 0;}

	i2c_master_writeByte(OLED_Byte);	// [具体数据]
	if(i2c_master_checkAck()==false)
	{i2c_master_stop(); return 0;}

	i2c_master_stop();					// 发送停止信号

	return true;						// 返回写入成功
}
//----------------------------------------------------------------------------

// 向OLED写入一字节数据/指令
//---------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_WR_Byte(u8 OLED_Byte, u8 OLED_Type)
{
	if(OLED_Type)
		OLED_Write_Data(OLED_Byte);		// 写入数据
	else
		OLED_Write_Command(OLED_Byte);	// 写入指令
}
//---------------------------------------------------------------

// 整屏写入某值
//------------------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_Clear(void)
{
	u8 N_Page, N_row;

	for(N_Page=0; N_Page<8; N_Page++)
	{
		OLED_WR_Byte(0xb0+N_Page,OLED_CMD);	// 从0～7页依次写入
		OLED_WR_Byte(0x00,OLED_CMD);      	// 列低地址
		OLED_WR_Byte(0x10,OLED_CMD);      	// 列高地址

		for(N_row=0; N_row<128; N_row++)OLED_WR_Byte(0x00,OLED_DATA);
	}
}
//------------------------------------------------------------------------

// 设置数据写入的起始行、列
//------------------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_Set_Pos(u8 x, u8 y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);				// 写入页地址

	OLED_WR_Byte((x&0x0f),OLED_CMD);  			// 写入列的地址(低半字节)

	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);	// 写入列的地址(高半字节)
}
//------------------------------------------------------------------------

// 初始化OLED
//-----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_Init(void)
{
	IIC_Init_JX();					// 初始化IIC

	system_soft_wdt_feed();			// 喂狗(防止ESP8266复位)

	delay_ms(100);					// 延迟(必须要有)

	OLED_WR_Byte(0xAE,OLED_CMD);	// 关闭显示

	OLED_WR_Byte(0x00,OLED_CMD);	// 设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);	// 设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);	// 设置起始行地址
	OLED_WR_Byte(0xB0,OLED_CMD);	// 设置页地址

	OLED_WR_Byte(0x81,OLED_CMD); 	// 对比度设置，可设置亮度
	OLED_WR_Byte(0xFF,OLED_CMD);	// 265

	OLED_WR_Byte(0xA1,OLED_CMD);	// 设置段(SEG)的起始映射地址
	OLED_WR_Byte(0xA6,OLED_CMD);	// 正常显示；0xa7逆显示

	OLED_WR_Byte(0xA8,OLED_CMD);	// 设置驱动路数（16~64）
	OLED_WR_Byte(0x3F,OLED_CMD);	// 64duty

	OLED_WR_Byte(0xC8,OLED_CMD);	// 重映射模式，COM[N-1]~COM0扫描

	OLED_WR_Byte(0xD3,OLED_CMD);	// 设置显示偏移
	OLED_WR_Byte(0x00,OLED_CMD);	// 无偏移

	OLED_WR_Byte(0xD5,OLED_CMD);	// 设置震荡器分频
	OLED_WR_Byte(0x80,OLED_CMD);	// 使用默认值

	OLED_WR_Byte(0xD9,OLED_CMD);	// 设置 Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);	// 使用官方推荐值

	OLED_WR_Byte(0xDA,OLED_CMD);	// 设置 com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);	// 使用默认值

	OLED_WR_Byte(0xDB,OLED_CMD);	// 设置 Vcomh，可调节亮度（默认）
	OLED_WR_Byte(0x40,OLED_CMD);	// 使用官方推荐值

	OLED_WR_Byte(0x8D,OLED_CMD);	// 设置OLED电荷泵
	OLED_WR_Byte(0x14,OLED_CMD);	// 开显示

	OLED_WR_Byte(0xAF,OLED_CMD);	// 开启OLED面板显示

	OLED_Clear();        			// 清屏

	OLED_Set_Pos(0,0); 				// 设置数据写入的起始行、列
}

// 在指定坐标处显示一个字符
//-----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_ShowChar(u8 x, u8 y, u8 Show_char)
{
	u8 c=0,i=0;

	c=Show_char-' '; 				// 获取字符的偏移量

	if(x>Max_Column-1){x=0;y=y+2;}	// 当列数超出范围，则另起2页

	if(SIZE ==16) 					// 字符大小为[8*16]：一个字符分两页
	{
		// 画第一页
		//-------------------------------------------------------
		OLED_Set_Pos(x,y);						// 设置画点起始处
		for(i=0;i<8;i++)  						// 循环8次(8列)
		OLED_WR_Byte(F8X16[c*16+i],OLED_DATA); 	// 找到字模

		// 画第二页
		//-------------------------------------------------------
		OLED_Set_Pos(x,y+1); 					// 页数加1
		for(i=0;i<8;i++)  						// 循环8次
		OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);// 把第二页画完
	}
}
//-----------------------------------------------------------------------------

// 在指定坐标起始处显示字符串
//-------------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_ShowString(u8 x, u8 y, u8 * Show_char)
{
	u8 N_Char = 0;		// 字符序号

	while (Show_char[N_Char]!='\0') 	// 如果不是最后一个字符
	{
		OLED_ShowChar(x,y,Show_char[N_Char]); 	// 显示一个字符

		x += 8;					// 列数加8，一个字符占8列

		if(x>=128){x=0;y+=2;} 	// 当x>=128，另起一页

		N_Char++; 				// 指向下一个字符
	}
}
//-------------------------------------------------------------------


// 在指定位置显示IP地址(点分十进制)
//-----------------------------------------------------------------------------
void ICACHE_FLASH_ATTR OLED_ShowIP(u8 x, u8 y, u8*Array_IP)
{
	u8 N_IP_Byte = 0;		// IP字节序号

	// 循环显示4个IP字节(由高到低字节显示)
	//----------------------------------------------------------
	for(; N_IP_Byte<4; N_IP_Byte++)
	{
		// 显示百位/十位
		//------------------------------------------------------
		if(Array_IP[N_IP_Byte]/100)		// 判断百位?=0
		{
			OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]/100); x+=8;

			// 显示十位（百位!=0）
			//---------------------------------------------------------
			//if(Array_IP[N_IP_Byte]%100/10)
			{ OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]%100/10); x+=8; }

		}

		// 显示十位（百位==0）
		//---------------------------------------------------------
		else if(Array_IP[N_IP_Byte]%100/10)		// 判断十位?=0
		{ OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]%100/10); x+=8; }


		// 显示个位
		//---------------------------------------------------------
		//if(Array_IP[C_IP_Byte]%100%10)
		{ OLED_ShowChar(x,y,48+Array_IP[N_IP_Byte]%100%10); x+=8; }

		// 显示【.】
		if(N_IP_Byte<3)
		{ OLED_ShowChar(x,y,'.'); x+=8; }
	}
}
//-----------------------------------------------------------------------------
//==============================================================================
