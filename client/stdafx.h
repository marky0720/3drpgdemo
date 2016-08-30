// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once

#pragma warning(disable:4819)

#include "resource.h" //���ָ��

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


// TODO: �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�


#include <sys/timeb.h>  //_ftime �޷���ֵ,����ñ��ص�ʱ�����ݲ����������ݽ���ָ��ʱ��ṹ�ı�����

#include <time.h> 

#include <vector>



#include <stdio.h>
 
#include <conio.h>


 


#include "typedef.h"



//�������ָ��
void SetCur( irr::video::IVideoDriver *drv , int type);

//�����
void TextLeft(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight);

//����� �Զ�����wrapWide����ÿ�еĳ���
void TextLeft(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight,int wrapWide);

//���ж���
void TextCenter(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight);

bool IniGetString( char *strFile, char *lpAppName, char *lpKeyName,char* RetValue );
// ������
int IniGetInt(  char *strFile, char *lpAppName, char *lpKeyName );


 
//�õ�irr����Ķ���
irr::IrrlichtDevice * getIrrDevice();

irr::video::IVideoDriver* getIrrDriver();
irr::gui::IGUIEnvironment* getIrrUI();
irr::scene::ISceneManager* getIrrSmgr();



int WriteToLog(char* str);


int randomNum(int min,int max);


//�õ�����hlsl�ļ����ڴ���
IReadFile * getWeaponHLSLFile(IFileSystem * pFS);

//�õ�����xbjhlsl�ļ����ڴ���
IReadFile * getWeaponXBJHLSLFile(IFileSystem * pFS);