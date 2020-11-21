/* * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													//																	//
// 工程：	STA_UDP_Server							//	①：复位ESP8266，等待ESP8266接入WIFI(LED快闪3次)				//
//													//																	//
// 平台：	【技新电子】物联网开发板 ESP8266 V1.0	//	②：电脑接入相同的WIFI											//
//													//																	//
// 功能：	①：8266设置为STA模式，接入路由器WIFI	//	③：打开网络调试软件，创建UDP通信(如果有选择则设为UDP_Client)	//
//													//																	//
//			②：接入AP成功后，创建UDP通信			//	④：设置电脑端口==8888，启动UDP通信(作为Client)					//
//													//																	//
//			③：等待UDP_Client发送数据过来			//	⑤：设置远端(Server)端口：IP=ESP8266_IP、端口=8266				//
//													//																	//
//			④：根据接收到的数据控制LED的亮/灭		//	⑥：网络调试助手向远端(Server)发送消息							//
//													//																	//
//	版本：	V1.0									//	⑦：ESP8266根据接收到的消息控制LED的亮/灭						//
//													//																	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 头文件引用
//==================================================================================
#include "user_config.h"		// 用户配置
#include "driver/uart.h"  		// 串口
#include "driver/oled.h"  		// OLED

//#include "at_custom.h"
#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
#include "ip_addr.h"			// 被"espconn.h"使用。在"espconn.h"开头#include"ip_addr.h"或#include"ip_addr.h"放在"espconn.h"之前
#include "espconn.h"			// TCP/UDP接口
//#include "espnow.h"
#include "ets_sys.h"			// 回调函数
//#include "gpio.h"
#include "mem.h"				// 内存申请等函数
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX、软件定时器
//#include "ping.h"
//#include "pwm.h"
//#include "queue.h"
//#include "smartconfig.h"
//#include "sntp.h"
//#include "spi_flash.h"
//#include "upgrade.h"
#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro
//==================================================================================


// 宏定义
//==================================================================================
#define		ProjectName			"STA_UDP_Server"		// 工程名宏定义
//TODO
#define		ESP8266_STA_SSID	"happy316"			// 接入的WIFI名
#define		ESP8266_STA_PASS	"asd123456"		// 接入的WIFI密码
//#define		ESP8266_STA_SSID	"HXN's phone"			// 接入的WIFI名
//#define		ESP8266_STA_PASS	"1234509876"		// 接入的WIFI密码
#define		LED_ON				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0)		// LED亮
#define		LED_OFF				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1)		// LED灭
//==================================================================================


// 全局变量
//==================================================================================
os_timer_t OS_Timer_1;				// 定义软件定时器

struct espconn ST_NetCon;			// 网络连接结构体
//==================================================================================


// 毫秒延时函数
//===========================================
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
//===========================================


// ESP8266_STA初始化
//==============================================================================
void ICACHE_FLASH_ATTR ESP8266_STA_Init_JX()
{
	struct station_config STA_Config;	// STA参数结构体

	struct ip_info ST_ESP8266_IP;		// STA信息结构体

	// 设置ESP8266的工作模式
	//------------------------------------------------------------------------
	wifi_set_opmode(0x01);				// 设置为STA模式，并保存到Flash

	//TODO 这里修改了
	// 设置STA模式下的IP地址【ESP8266默认开启DHCP Client，接入WIFI时会自动分配IP地址】
	//--------------------------------------------------------------------------------
//	wifi_station_dhcpc_stop();						// 关闭 DHCP Client
//	IP4_ADDR(&ST_ESP8266_IP.ip,192,168,4,1);		// 配置IP地址
//	IP4_ADDR(&ST_ESP8266_IP.gw,192,168,1,1);		// 配置网关地址
//	IP4_ADDR(&ST_ESP8266_IP.netmask,255,255,255,0);	// 配置子网掩码
//	wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	// 设置STA模式下的IP地址


	// 结构体赋值，配置STA模式参数
	//-------------------------------------------------------------------------------
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA参数结构体 = 0
	os_strcpy(STA_Config.ssid,ESP8266_STA_SSID);				// 设置WIFI名
	os_strcpy(STA_Config.password,ESP8266_STA_PASS);			// 设置WIFI密码

	wifi_station_set_config(&STA_Config);	// 设置STA参数，并保存到Flash

	// wifi_station_connect();		// ESP8266连接WIFI
}
//=========================================================================================


// 成功发送网络数据的回调函数
//==========================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb_JX(void *arg)
{
	os_printf("\nESP8266_WIFI_Send_OK\n");
}
//==========================================================


// 成功接收网络数据的回调函数【参数1：网络传输结构体espconn指针、参数2：网络传输数据指针、参数3：数据长度】
//=========================================================================================================
void ICACHE_FLASH_ATTR ESP8266_WIFI_Recv_Cb_JX(void * arg, char * pdata, unsigned short len)
{
	struct espconn * T_arg = arg;		// 缓存网络连接结构体指针
	int i = 0;
	remot_info * P_port_info = NULL;	// 定义远端连接信息指针

	// 根据数据设置LED的亮/灭
	//-------------------------------------------------------------------------------
	//TODO 这里暂时更改了：
	if(pdata[0] == 'k' || pdata[0] == 'K'){
		LED_ON;			// 首字母为'k'/'K'，灯亮
		delay_ms(500); //延时0.5秒
		LED_OFF;
		i = 1;

	}
	//	else if(pdata[0] == 'g' || pdata[0] == 'G')	LED_OFF;	// 首字母为'g'/'G'，灯灭
	os_printf("\nESP8266_Receive_Data = %s\n",pdata);		// 串口打印接收到的数据


	// 获取远端信息【UDP通信是无连接的，向远端主机回应时需获取对方的IP/端口信息】
	//------------------------------------------------------------------------------------
	if(espconn_get_connection_info(T_arg, &P_port_info, 0)==ESPCONN_OK)	// 获取远端信息
	{
		T_arg->proto.udp->remote_port  = P_port_info->remote_port;		// 获取对方端口号
		T_arg->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// 获取对方IP地址
		T_arg->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		T_arg->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		T_arg->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];
		//os_memcpy(T_arg->proto.udp->remote_ip,P_port_info->remote_ip,4);	// 内存拷贝
	}

	//TODO 这里暂时更改了：

	//--------------------------------------------------------------------

	if (i == 1) {
//		espconn_send(T_arg, "OPEN OK!\n", os_strlen("OPEN OK!\n"));	// 向对方发送应答
//		system_soft_wdt_feed();
//		delay_ms(500);
//		system_soft_wdt_feed();
		OLED_ShowString(0,0," OPEN DOOR OK! ");	// ESP8266模式
		OLED_ShowString(0,2,"      :-)      ");				// ESP8266_IP地址
		OLED_ShowString(0,4,"Remote  = STA");	// 远端主机模式
		OLED_ShowString(0,6,"IP:");				// 远端主机IP地址
		OLED_ShowIP(24,6,T_arg->proto.udp->remote_ip);	// 显示远端主机IP地址
		for (i = 0; i < 6; i++) {
			system_soft_wdt_feed();
			delay_ms(500);
		}
		OLED_Clear();
		espconn_send(T_arg, "OPEN OK!\n", os_strlen("OPEN OK!\n"));	// 向对方发送应答
	} else {
		espconn_send(T_arg, "you can use 'k'or'K' to open the door\n",
				os_strlen("you can use 'k'or'K' to open the door\n"));// 向对方发送应答
	}

//	espconn_send(T_arg,"I hear you!\n",os_strlen("I hear you!\n"));	// 向对方发送应答
}
//=========================================================================================================



// 定义espconn型结构体
//-----------------------------------------------
//struct espconn ST_NetCon;	// 网络连接结构体

// 初始化网络连接(UDP通信)
//==================================================================================================
void ICACHE_FLASH_ATTR ESP8266_NetCon_Init_JX()
{
	// 结构体赋值
	//--------------------------------------------------------------------------
	ST_NetCon.type = ESPCONN_UDP;		// 设置通信协议为UDP

	ST_NetCon.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));	// 开辟内存

	// 此处无需设置目标IP/端口(ESP8266作为Server，不需要预先知道Client的IP/端口)
	//--------------------------------------------------------------------------
	ST_NetCon.proto.udp->local_port  = 8266 ;	// 设置本地端口
	//ST_NetCon.proto.udp->remote_port = 8888;	// 设置目标端口
	//ST_NetCon.proto.udp->remote_ip[0] = 192;	// 设置目标IP地址
	//ST_NetCon.proto.udp->remote_ip[1] = 168;
	//ST_NetCon.proto.udp->remote_ip[2] = 8;
	//ST_NetCon.proto.udp->remote_ip[3] = 47;

	// 注册回调函数
	//-------------------------------------------------------------------------------------------
	espconn_regist_sentcb(&ST_NetCon,ESP8266_WIFI_Send_Cb_JX);	// 注册网络数据发送成功的回调函数
	espconn_regist_recvcb(&ST_NetCon,ESP8266_WIFI_Recv_Cb_JX);	// 注册网络数据接收成功的回调函数

	// 调用UDP初始化API
	//-----------------------------------------------
	espconn_create(&ST_NetCon);		// 初始化DUP通信
}
//==================================================================================================


// 软件定时的回调函数
//=========================================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	u8 C_LED_Flash = 0;				// LED闪烁计次

	struct ip_info ST_ESP8266_IP;	// ESP8266的IP信息
	u8 ESP8266_IP[4];				// ESP8266的IP地址


	// 成功接入WIFI【STA模式下，如果开启DHCP(默认)，则ESO8266的IP地址由WIFI路由器自动分配】
	//-------------------------------------------------------------------------------------
	if( wifi_station_get_connect_status() == STATION_GOT_IP )	// 判断是否获取IP
	{
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// 获取STA的IP信息
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;			// IP地址高八位 == addr低八位
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;		// IP地址次高八位 == addr次低八位
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;		// IP地址次低八位 == addr次高八位
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;		// IP地址低八位 == addr高八位

		// 显示ESP8266的IP地址
		//-----------------------------------------------------------------------------------------------
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(24,2,ESP8266_IP);	// OLED显示ESP8266的IP地址
		//-----------------------------------------------------------------------------------------------

		// 接入WIFI成功后，LED快闪3次
		//----------------------------------------------------
		for(; C_LED_Flash<=5; C_LED_Flash++)
		{
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),(C_LED_Flash%2));
			delay_ms(100);
		}

		os_timer_disarm(&OS_Timer_1);	// 关闭定时器

		ESP8266_NetCon_Init_JX();		// 初始化网络连接(UDP通信)
	}
}
//=========================================================================================================

// 软件定时器初始化(ms毫秒)
//=====================================================================================
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	os_timer_disarm(&OS_Timer_1);	// 关闭定时器
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// 设置定时器
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // 使能定时器
}
//=====================================================================================

// LED初始化
//=============================================================================
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4设为IO口

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}
//=============================================================================


// user_init：entry of user application, init user function here
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");

	// OLED显示初始化
	//TODO
	//--------------------------------------------------------
	OLED_Init();							// OLED初始化
	OLED_ShowString(0,0,"ESP8266 = STA");	// ESP8266模式
	OLED_ShowString(0,2,"IP:");				// ESP8266_IP地址
	OLED_ShowString(0,4,"Remote  = STA");	// 远端主机模式
	OLED_ShowString(0,6,"IP:");				// 远端主机IP地址
	//--------------------------------------------------------

	LED_Init_JX();		// LED初始化


	ESP8266_STA_Init_JX();			// ESP8266_STA初始化

	OS_Timer_1_Init_JX(1000,1);		// 1秒定时(重复)
}
//==============================================================================


/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void){}
