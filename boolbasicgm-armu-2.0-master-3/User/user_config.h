#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H


#define __CUSTOMER_NAME   "BOOLBASICGM"
#define __DEVICE_TYPE     "OBDS5S7600V09"    // 04 -12台测试车辆    05-- V1.7硬件 200台   06--V1.8硬件  07-带电池版本  08-GD单片机   09-国民技术单片机  
//#define __VERSION      (0x19031500)
#define DEVICE_TYPE_MAX_LEN 32
//#define __HW_VERSION      "1.0.0"

//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"123.127.164.36\",50001\r\n" //
//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"60.12.185.130\",39002\r\n" //德力
//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"117.158.239.171\",8866\r\n" //德力_new
//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"120.220.14.212\",19006\r\n"  //沂星
//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"123.127.164.36\",50001\r\n"  // 通联

//#define GB17691_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",60002\r\n"  // 网络助手测试用
//#define GB17691_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",60022\r\n"  // 通联
//#define GB17691_SERVER_IP_PORT    "\r\nAT+CIPOPEN=0,\"TCP\",\"106.14.52.96\",6097\r\n"  // 上海平台

//#define GB17691_SERVER_IP_PORT    "\r\nAT+CIPOPEN=0,\"TCP\",\"222.143.40.245\",10133\r\n"  // 郑州新平台
//#define GB17691_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",60023\r\n"  // 布尔国六平台
//#define GB17691_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",9400\r\n"  // VMS生产平台

#define GB17691_SERVER_IP_PORT    "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",9200\r\n"  //VMS测试平台第一链路    基础版连接第二链路

//#define GB17691_SERVER_IP_PORT    "\r\nAT+CIPOPEN=0,\"TCP\",\"10.34.1.76\",4000\r\n"  //艾可蓝


//#define GB17691_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",60023\r\n"  // 布尔国六平台
//#define GB17691_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"122.112.203.25\",6099\r\n"  // 杭州平台
//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"111.30.107.158\",60032\r\n"  // 通联
//#define GB32960_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"123.127.164.36\",50001\r\n"  // 通联

//#define GB32960_SERVER_IP_PORT "\r\nAT+CIPSTART=\"TCP\",\"111.30.107.158\",60023\r\n"

//#define TEST_SERVER_IP_PORT  "\r\nAT+CIPOPEN=1,\"TCP\",\"39.104.144.129\",20000\r\n"  //测试服务器-亚美服务器

//#define TEST_SERVER_IP_PORT  "\r\nAT+CIPOPEN=1,\"TCP\",\"111.30.107.158\",9400\r\n"  //测试服务器-亚美服务器

//#define FTP_SERVER_IP_PORT      "AT+CFTPSLOGIN=\"67.208.93.232\",21,\"zyf\",\"zyf\",0\r\n"
#define FTP_SERVER_IP_PORT   "AT+CFTPSLOGIN=\"123.57.210.236\",21,\"admin1\",\"tjbool@2020\",0\r\n"
#define VERSION_SERVER_IP_PORT  "\r\nAT+CIPOPEN=0,\"TCP\",\"123.127.164.36\",50004\r\n"

//#define debug 1

#define VER0 (0x21070502)    //发布第一版
#define VER1 (0x21070801)    //发布第一版
#define VER2 (0x21071501)    //发布第一版

//#ifdef VER63
//#ifndef CURRENT_VER
// #define CURRENT_VER VER63
//#endif
//#else
// #define VER63 0xffffffff
//#endif

//#ifdef VER62
//#ifndef CURRENT_VER
// #define CURRENT_VER VER62
//#endif
//#else
// #define VER62 0xffffffff
//#endif

//#ifdef VER61
//#ifndef CURRENT_VER
// #define CURRENT_VER VER61
//#endif
//#else
// #define VER61 0xffffffff
//#endif

//#ifdef VER60
//#ifndef CURRENT_VER
//#define CURRENT_VER VER60
//#endif
//#else
// #define VER60 0xffffffff
//#endif

//#ifdef VER59
//#ifndef CURRENT_VER
// #define CURRENT_VER VER59
//#endif
//#else
// #define VER59 0xffffffff
//#endif
// 
//#ifdef VER58
//#ifndef CURRENT_VER
// #define CURRENT_VER VER58
//#endif
//#else
// #define VER58 0xffffffff
//#endif 
// 
//#ifdef VER57
//#ifndef CURRENT_VER
// #define CURRENT_VER VER57
//#endif
//#else
// #define VER57 0xffffffff
//#endif

//#ifdef VER56
//#ifndef CURRENT_VER
// #define CURRENT_VER VER56
//#endif
//#else
// #define VER56 0xffffffff
//#endif

//#ifdef VER55
//#ifndef CURRENT_VER
// #define CURRENT_VER VER55
//#endif
//#else
// #define VER55 0xffffffff
//#endif

//#ifdef VER54
//#ifndef CURRENT_VER
// #define CURRENT_VER VER54
//#endif
//#else
// #define VER54 0xffffffff
//#endif

//#ifdef VER53
//#ifndef CURRENT_VER
// #define CURRENT_VER VER53
//#endif
//#else
// #define VER53 0xffffffff
//#endif

//#ifdef VER52
//#ifndef CURRENT_VER
// #define CURRENT_VER VER52
//#endif
//#else
// #define VER52 0xffffffff
//#endif

//#ifdef VER51
//#ifndef CURRENT_VER
// #define CURRENT_VER VER51
//#endif
//#else
// #define VER51 0xffffffff
//#endif

//#ifdef VER50
//#ifndef CURRENT_VER
// #define CURRENT_VER VER50
//#endif
//#else
// #define VER50 0xffffffff
//#endif

//#ifdef VER49
//#ifndef CURRENT_VER
// #define CURRENT_VER VER49
//#endif
//#else
// #define VER49 0xffffffff
//#endif
// 
//#ifdef VER48
//#ifndef CURRENT_VER
// #define CURRENT_VER VER48
//#endif
//#else
// #define VER48 0xffffffff
//#endif
// 
//#ifdef VER47
//#ifndef CURRENT_VER
// #define CURRENT_VER VER47
//#endif
//#else
// #define VER47 0xffffffff
//#endif
//	
//#ifdef VER46
//#ifndef CURRENT_VER
// #define CURRENT_VER VER46
//#endif
//#else
// #define VER46 0xffffffff
//#endif

//#ifdef VER45
//#ifndef CURRENT_VER
// #define CURRENT_VER VER45
//#endif
//#else
// #define VER45 0xffffffff
//#endif

//#ifdef VER44
//#ifndef CURRENT_VER
// #define CURRENT_VER VER44
//#endif
//#else
// #define VER44 0xffffffff
//#endif

//#ifdef VER43
//#ifndef CURRENT_VER
// #define CURRENT_VER VER43
//#endif
//#else
// #define VER43 0xffffffff
//#endif

//#ifdef VER42
//#ifndef CURRENT_VER
// #define CURRENT_VER VER42
//#endif
//#else
// #define VER42 0xffffffff
//#endif
// 
//#ifdef VER41
//#ifndef CURRENT_VER
// #define CURRENT_VER VER41
//#endif
//#else
// #define VER41 0xffffffff
//#endif

//#ifdef VER40
//#ifndef CURRENT_VER
// #define CURRENT_VER VER40
//#endif
//#else
// #define VER40 0xffffffff
//#endif

//#ifdef VER39
//#ifndef CURRENT_VER
// #define CURRENT_VER VER39
//#endif
//#else
// #define VER39 0xffffffff
//#endif

//#ifdef VER38
//#ifndef CURRENT_VER
// #define CURRENT_VER VER38
//#endif
//#else
// #define VER38 0xffffffff
//#endif

//#ifdef VER37
//#ifndef CURRENT_VER
// #define CURRENT_VER VER37
//#endif
//#else
// #define VER37 0xffffffff
//#endif

//#ifdef VER36
//#ifndef CURRENT_VER
// #define CURRENT_VER VER36
//#endif
//#else
// #define VER36 0xffffffff
//#endif

//#ifdef VER35
//#ifndef CURRENT_VER
// #define CURRENT_VER VER35
//#endif
//#else
// #define VER35 0xffffffff
//#endif

//#ifdef VER34
//#ifndef CURRENT_VER
// #define CURRENT_VER VER34
//#endif
//#else
// #define VER34 0xffffffff
//#endif

//#ifdef VER33
//#ifndef CURRENT_VER
// #define CURRENT_VER VER33
//#endif
//#else
// #define VER33 0xffffffff
//#endif


//#ifdef VER32
//#ifndef CURRENT_VER
// #define CURRENT_VER VER32
//#endif
//#else
// #define VER32 0xffffffff
//#endif

//#ifdef VER31
//#ifndef CURRENT_VER
// #define CURRENT_VER VER31
//#endif
//#else
// #define VER31 0xffffffff
//#endif

//#ifdef VER30
//#ifndef CURRENT_VER
// #define CURRENT_VER VER30
//#endif
//#else
// #define VER30 0xffffffff
//#endif

//#ifdef VER29
//#ifndef CURRENT_VER
// #define CURRENT_VER VER29
//#endif
//#else
// #define VER29 0xffffffff
//#endif

//#ifdef VER28
//#ifndef CURRENT_VER
// #define CURRENT_VER VER28
//#endif
//#else
// #define VER28 0xffffffff
//#endif
// 
//#ifdef VER27
//#ifndef CURRENT_VER
// #define CURRENT_VER VER27
//#endif
//#else
// #define VER27 0xffffffff
//#endif 
// 
//#ifdef VER26
//#ifndef CURRENT_VER
// #define CURRENT_VER VER26
//#endif
//#else
// #define VER26 0xffffffff
//#endif

//#ifdef VER25
//#ifndef CURRENT_VER
// #define CURRENT_VER VER25
//#endif
//#else
// #define VER25 0xffffffff
//#endif

//#ifdef VER24
//#ifndef CURRENT_VER
// #define CURRENT_VER VER24
//#endif
//#else
// #define VER24 0xffffffff
//#endif

//#ifdef VER23
//#ifndef CURRENT_VER
// #define CURRENT_VER VER23
//#endif
//#else
// #define VER23 0xffffffff
//#endif

//#ifdef VER22
//#ifndef CURRENT_VER
// #define CURRENT_VER VER22
//#endif
//#else
// #define VER22 0xffffffff
//#endif

//#ifdef VER21
//#ifndef CURRENT_VER
// #define CURRENT_VER VER21
//#endif
//#else
// #define VER21 0xffffffff
//#endif

//#ifdef VER20
//#ifndef CURRENT_VER
// #define CURRENT_VER VER20
//#endif
//#else
// #define VER20 0xffffffff
//#endif

//#ifdef VER19
//#ifndef CURRENT_VER
// #define CURRENT_VER VER19
//#endif
//#else
// #define VER19 0xffffffff
//#endif

//#ifdef VER18
//#ifndef CURRENT_VER
// #define CURRENT_VER VER18
//#endif
//#else
// #define VER18 0xffffffff
//#endif
// 
//#ifdef VER17
//#ifndef CURRENT_VER
// #define CURRENT_VER VER17
//#endif
//#else
// #define VER17 0xffffffff
//#endif
// 
//#ifdef VER16
//#ifndef CURRENT_VER
// #define CURRENT_VER VER16
//#endif
//#else
// #define VER16 0xffffffff
//#endif
	
#ifdef VER15
#ifndef CURRENT_VER
 #define CURRENT_VER VER15
#endif
#else
 #define VER15 0xffffffff
#endif

#ifdef VER14
#ifndef CURRENT_VER
 #define CURRENT_VER VER14
#endif
#else
 #define VER14 0xffffffff
#endif

#ifdef VER13
#ifndef CURRENT_VER
 #define CURRENT_VER VER13
#endif
#else
 #define VER13 0xffffffff
#endif

#ifdef VER12
#ifndef CURRENT_VER
 #define CURRENT_VER VER12
#endif
#else
 #define VER12 0xffffffff
#endif

#ifdef VER11
#ifndef CURRENT_VER
 #define CURRENT_VER VER11
#endif
#else
 #define VER11 0xffffffff
#endif
 
#ifdef VER10
#ifndef CURRENT_VER
 #define CURRENT_VER VER10
#endif
#else
 #define VER10 0xffffffff
#endif

#ifdef VER9
#ifndef CURRENT_VER
 #define CURRENT_VER VER9
#endif
#else
 #define VER9 0xffffffff
#endif

#ifdef VER8
#ifndef CURRENT_VER
 #define CURRENT_VER VER8
#endif
#else
 #define VER8 0xffffffff
#endif

#ifdef VER7
#ifndef CURRENT_VER
 #define CURRENT_VER VER7
#endif
#else
 #define VER7 0xffffffff
#endif

#ifdef VER6
#ifndef CURRENT_VER
 #define CURRENT_VER VER6
#endif
#else
 #define VER6 0xffffffff
#endif

#ifdef VER5
#ifndef CURRENT_VER
 #define CURRENT_VER VER5
#endif
#else
 #define VER5 0xffffffff
#endif

#ifdef VER4
#ifndef CURRENT_VER
 #define CURRENT_VER VER4
#endif
#else
 #define VER4 0xffffffff
#endif

#ifdef VER3
#ifndef CURRENT_VER
 #define CURRENT_VER VER3
#endif
#else
 #define VER3 0xffffffff
#endif

#ifdef VER2
#ifndef CURRENT_VER
 #define CURRENT_VER VER2
#endif
#else
 #define VER2 0xffffffff
#endif

#ifdef VER1
#ifndef CURRENT_VER
 #define CURRENT_VER VER1
#endif
#else
 #define VER1 0xffffffff
#endif

#ifdef VER0
#ifndef CURRENT_VER
 #define CURRENT_VER VER0
#endif
#else
 #define VER0 0xffffffff
#endif





#endif

