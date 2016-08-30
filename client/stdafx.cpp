// stdafx.cpp : 只包括标准包含文件的源文件
// 1to2.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
//引用任何所需的附加头文件，而不是在此文件中引用

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// FUNCTION : sphericalXYZ ////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//得到某行文本
bool GetTxtByRowNum(char* filename,char* Dest_str,int RowNum)
{
	if(GetTxtRowNum(filename)==0)//如果文件是0行,则返回失败
	{
	  return false;
	}
	FILE *fin; 
	fin = fopen(filename,"rb");
	char ch;
			int i=0,row=0;
			::memset(Dest_str,'\0',255);
			int count=0;
			while (!feof(fin))
			{ 
				ch=fgetc(fin);
				if((RowNum-1)==row)
				{
					//printf("row:%d count:%d ch:%c \r\n",row,count,ch);
					Dest_str[count]=ch;
					if(ch=='\n')
					{
					//	printf("row:%d ch=\\n\r\n",row);
						Dest_str[count]='\0';
						break;
					}
					count++;
				}
				if((ch=='\n'))
				{
					row++;     //如果换行，row增加1
				}
				
				i++;
				if(row>(RowNum-1))//如果当前的行数大于要读的行,说明已经读过了,跳出
				{
				  break;
				}
			}
			//Dest_str[count]='\0';
			fclose(fin);
	return true;

}
//得到文本行数
int GetTxtRowNum(char* filename)
{
	FILE *fin; 
	fin = fopen(filename,"rb");
			char ch;
			int i=0,row=0;
			ch=fgetc(fin);
			while (!feof(fin))
			{ 
				ch=fgetc(fin);
				if((ch=='\n'))
				{
					row++;     //如果换行，row增加1
				}
				i++;
			}
			fclose(fin);

			if(i==0)
			{
				//printf("0行");
				return 0;
			}else
			{
				//printf("%d行",row+1);
				return row+1;
			}
}





LPWSTR ConvertLPCSTRToLPWSTR (char* pCstring)
{
	LPWSTR pszOut = NULL; 
	if (pCstring != NULL)
	{
		int nInputStrLen = strlen (pCstring); 
		// Double NULL Termination 
		int nOutputStrLen = MultiByteToWideChar(CP_ACP, 0, pCstring, nInputStrLen, NULL, 0) + 2;
		pszOut = new WCHAR [nOutputStrLen]; 
			if(pszOut)
			{
				memset (pszOut, 0x00, sizeof (WCHAR)*nOutputStrLen);
				MultiByteToWideChar (CP_ACP, 0, pCstring, nInputStrLen, pszOut, nInputStrLen);
			}
	}
	return pszOut; 
}

BOOL WChar2MByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
   DWORD dwMinSize;
   dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
   if(dwSize < dwMinSize)
   {
    return FALSE;
   }
   WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
   return TRUE;
}

char* WCharToMByte(LPCWSTR lpcwszStr)
  {
   //所需的短字符数组空间的个数
      DWORD dwNum = WideCharToMultiByte(CP_OEMCP,0,lpcwszStr,-1,NULL,0,NULL,FALSE)+4;
	 // printf("长度:%d\n",dwNum);
	  char* temp=new char[dwNum];
     WChar2MByte(lpcwszStr,temp,dwNum);
	
   return temp;
}





//左对齐
void TextLeft( const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight)
{
//	irr::gui::IGUIEnvironment* ui=0;
//	ui=getIrrUI();
//	if(ui==0) return;
//
//	WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 
//	gui::IGUIFont *font;
//
// //STSONG.TTF 华文宋体   SIMFANG.TTF  STFANGSO.TTF 华文仿宋   SIMSUN.TTC宋体,新宋体          SIMKAI.TTF华文楷体        Fixedsys
//	font = ui->getDXFont("宋体",false,fontsize,0,weight);
//
//		int textsize=strlen(str);
//core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(textsize*fontsize,fontsize)); 
//		font->draw(pwStr,AbsoluteRect,color);
//		delete []pwStr;
	//	delete []temp;
}

//左对齐 自动换行wrapWide就是每行的长度
void TextLeft(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight,int wrapWide)
{
	//irr::gui::IGUIEnvironment* ui=0;
	//ui=getIrrUI();
	//if(ui==0) return;

	//
	//gui::IGUIFont *font;

	////STSONG.TTF 华文宋体   SIMFANG.TTF  STFANGSO.TTF 华文仿宋   SIMSUN.TTC宋体,新宋体          SIMKAI.TTF华文楷体        Fixedsys
	//font = ui->getDXFont("宋体",false,fontsize,0,weight);


	//int textsize=strlen(str);

	//WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 

	////统计要换多少行
	//int row=textsize*fontsize/wrapWide; //总字数*每字宽度/每行的宽度 得到行数
	//if(row==0) row=1;


	//core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(wrapWide,row*fontsize)); 
	//font->draw(pwStr,AbsoluteRect,color);
	//			
	//	
	//delete []pwStr;
}

//居中对齐
void TextCenter(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight)
{
//	irr::gui::IGUIEnvironment* ui=0;
//	ui=getIrrUI();
//	if(ui==0) return;
//
//WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 
//	gui::IGUIFont *font;
//    //STSONG.TTF 华文宋体   SIMFANG.TTF  STFANGSO.TTF 华文仿宋   SIMSUN.TTC宋体,新宋体          SIMKAI.TTF华文楷体        Fixedsys
//	font = ui->getDXFont("宋体",true,fontsize,0,weight);
//		int textsize=strlen(str);
//	core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(textsize*fontsize,fontsize)); 
//	font->draw(pwStr,AbsoluteRect,color);
//	delete []pwStr;
}



/*******************************************************************************************

ini读写

*******************************************************************************************/

// -----------------------------------------------------------------------------------------
//  读字符串  这个字符串 传RetValue字符串 255长度
//  返回值 true 读成功 false 读失败
// -----------------------------------------------------------------------------------------
bool IniGetString( char *strFile, char *lpAppName, char *lpKeyName,char* RetValue )
{

	DWORD ret=GetPrivateProfileString (lpAppName,lpKeyName, NULL, RetValue, 255, strFile);  //读
	if(ret==0){
		//MessageBox(0,"ini读失败!","",0);
		return false;
	}else{
		return true;
	}
}

// 读数字
int IniGetInt(  char *strFile, char *lpAppName, char *lpKeyName )
{
	char inBuf[255];
	DWORD ret=GetPrivateProfileString (lpAppName,lpKeyName, NULL, inBuf, 255, strFile);  //读
	if(ret==0){
		//MessageBox(0,"ini读失败!","",0);
		return -1;
	}else{
		ret=atoi(inBuf);
		return ret;
	}

}

// -----------------------------------------------------------------------------------------
 



//得到irr引擎
irr::IrrlichtDevice * getIrrDevice()
{
	 static irr::IrrlichtDevice* device=0;
  if(device==0){


	     SIrrlichtCreationParameters deviceParam;
		
		deviceParam.Doublebuffer=false;
		deviceParam.DriverType=irr::video::EDT_DIRECT3D9;

		deviceParam.WindowSize.Width = 800;
		deviceParam.WindowSize.Height = 600;
		deviceParam.Fullscreen = false;
	//	deviceParam.Bits = 32;
	//	deviceParam.ZBufferBits = 32;
		deviceParam.Vsync = false;
		deviceParam.AntiAlias=true;
		deviceParam.Stencilbuffer=false; //这个就是shadow标志
		

	  device = createDeviceEx(deviceParam);
		
	  
	//	  EDT_DIRECT3D9, dimension2d<u32> (800,600), 16, false, 	  shadows, false);

	  
  }

  return device;
}

irr::video::IVideoDriver* getIrrDriver()
{
 static irr::video::IVideoDriver* drv=0;
 if(drv==0) drv=getIrrDevice()->getVideoDriver();
 if(drv) return drv;
 else return 0;
}

irr::gui::IGUIEnvironment* getIrrUI()
{
 static irr::gui::IGUIEnvironment* ui=0;
 if(ui==0) ui=getIrrDevice()->getGUIEnvironment();
 if(ui) return ui;
 else return 0;
}

irr::scene::ISceneManager* getIrrSmgr()
{
 static irr::scene::ISceneManager* smgr=0;
 if(smgr==0) smgr=getIrrDevice()->getSceneManager();
 if(smgr) return smgr;
 else return 0;
}



#define LOGFILE "client_log.txt"
int WriteToLog(char* str)
{
	FILE* log;
	log = fopen(LOGFILE, "a+");
	if (log == NULL)
		return -1;

	SYSTEMTIME   sm   =   {   0   }; 
	GetLocalTime(&sm); 

	char temp[255];
	sprintf(temp,"%d月%d日 %d:%d:%d",sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);

	fprintf(log, "%s  %s\n",temp, str);
	fclose(log);
	return 0;
}



int randomNum(int min,int max)
{
	struct _timeb timebuffer;//定义一个结构
	_ftime(&timebuffer);//初始化timebuffer
	srand((unsigned short int)timebuffer.millitm);//timebuffer.millitm获得毫秒

	int x ;

	x=(int)rand() % max+ min;       // random number min-max

	return x;

}

#include "weaponhlsl.h"


//得到武器hlsl文件的内存句柄
IReadFile * getWeaponHLSLFile(IFileSystem * pFS)
{


	//创建读文件接口，这里特别注意第3个参数，其后缀一定要和文件格式一致，如果用的是bmp，则应该是testpng.bmp
	IReadFile * pMFR = pFS->createMemoryReadFile(weapon,weapon_size,"weapon.hlsl",true);
	return pMFR;
}

#include "xbjweapon.h"

//得到武器xbjhlsl文件的内存句柄
IReadFile * getWeaponXBJHLSLFile(IFileSystem * pFS)
{

	//创建读文件接口，这里特别注意第3个参数，其后缀一定要和文件格式一致，如果用的是bmp，则应该是testpng.bmp
	IReadFile * pMFR = pFS->createMemoryReadFile(xbjweapon, xbjweapon_size,"xbjweapon.hlsl",true);
	return pMFR;
}
