// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#pragma warning(disable:4819)

#include "resource.h" //鼠标指针

#include <iostream>
#include <tchar.h>
#include <winsock2.h>
#include   <windows.h>

#include <string.h>  // strcpy_s


#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


#include <string>
#include <algorithm>
#include <list>
using namespace std;

bool GetTxtByRowNum(char* filename,char* Dest_str,int RowNum);
int GetTxtRowNum(char* filename);

LPWSTR ConvertLPCSTRToLPWSTR (char* pCstring);
char* WCharToMByte(LPCWSTR lpcwszStr);
BOOL WChar2MByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);


// TODO: 在此处引用程序要求的附加头文件


#include <sys/timeb.h>  //_ftime 无返回值,但获得本地的时间数据并且填充进传递进来指向时间结构的变量中

#include <time.h> 

#include <vector>



#include <stdio.h>
#include <irrKlang.h>
#include <conio.h>



using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") 


#include "typedef.h"



//设置鼠标指针
void SetCur( irr::video::IVideoDriver *drv , int type);

//左对齐
void TextLeft(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight);

//左对齐 自动换行wrapWide就是每行的长度
void TextLeft(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight,int wrapWide);

//居中对齐
void TextCenter(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight);

bool IniGetString( char *strFile, char *lpAppName, char *lpKeyName,char* RetValue );
// 读数字
int IniGetInt(  char *strFile, char *lpAppName, char *lpKeyName );


//irrklang声音引擎
ISoundEngine* getSoundEngine();
//得到irr引擎的对象
irr::IrrlichtDevice * getIrrDevice();

irr::video::IVideoDriver* getIrrDriver();
irr::gui::IGUIEnvironment* getIrrUI();
irr::scene::ISceneManager* getIrrSmgr();



int WriteToLog(char* str);


int randomNum(int min,int max);


//得到武器hlsl文件的内存句柄
IReadFile * getWeaponHLSLFile(IFileSystem * pFS);

//得到武器xbjhlsl文件的内存句柄
IReadFile * getWeaponXBJHLSLFile(IFileSystem * pFS);