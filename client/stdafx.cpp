// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// 1to2.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
//�����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// FUNCTION : sphericalXYZ ////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 


//�õ�ĳ���ı�
bool GetTxtByRowNum(char* filename,char* Dest_str,int RowNum)
{
	if(GetTxtRowNum(filename)==0)//����ļ���0��,�򷵻�ʧ��
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
					row++;     //������У�row����1
				}
				
				i++;
				if(row>(RowNum-1))//�����ǰ����������Ҫ������,˵���Ѿ�������,����
				{
				  break;
				}
			}
			//Dest_str[count]='\0';
			fclose(fin);
	return true;

}
//�õ��ı�����
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
					row++;     //������У�row����1
				}
				i++;
			}
			fclose(fin);

			if(i==0)
			{
				//printf("0��");
				return 0;
			}else
			{
				//printf("%d��",row+1);
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
   //����Ķ��ַ�����ռ�ĸ���
      DWORD dwNum = WideCharToMultiByte(CP_OEMCP,0,lpcwszStr,-1,NULL,0,NULL,FALSE)+4;
	 // printf("����:%d\n",dwNum);
	  char* temp=new char[dwNum];
     WChar2MByte(lpcwszStr,temp,dwNum);
	
   return temp;
}





//�����
void TextLeft( const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight)
{
//	irr::gui::IGUIEnvironment* ui=0;
//	ui=getIrrUI();
//	if(ui==0) return;
//
//	WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 
//	gui::IGUIFont *font;
//
// //STSONG.TTF ��������   SIMFANG.TTF  STFANGSO.TTF ���ķ���   SIMSUN.TTC����,������          SIMKAI.TTF���Ŀ���        Fixedsys
//	font = ui->getDXFont("����",false,fontsize,0,weight);
//
//		int textsize=strlen(str);
//core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(textsize*fontsize,fontsize)); 
//		font->draw(pwStr,AbsoluteRect,color);
//		delete []pwStr;
	//	delete []temp;
}

//����� �Զ�����wrapWide����ÿ�еĳ���
void TextLeft(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight,int wrapWide)
{
	//irr::gui::IGUIEnvironment* ui=0;
	//ui=getIrrUI();
	//if(ui==0) return;

	//
	//gui::IGUIFont *font;

	////STSONG.TTF ��������   SIMFANG.TTF  STFANGSO.TTF ���ķ���   SIMSUN.TTC����,������          SIMKAI.TTF���Ŀ���        Fixedsys
	//font = ui->getDXFont("����",false,fontsize,0,weight);


	//int textsize=strlen(str);

	//WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 

	////ͳ��Ҫ��������
	//int row=textsize*fontsize/wrapWide; //������*ÿ�ֿ��/ÿ�еĿ�� �õ�����
	//if(row==0) row=1;


	//core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(wrapWide,row*fontsize)); 
	//font->draw(pwStr,AbsoluteRect,color);
	//			
	//	
	//delete []pwStr;
}

//���ж���
void TextCenter(const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight)
{
//	irr::gui::IGUIEnvironment* ui=0;
//	ui=getIrrUI();
//	if(ui==0) return;
//
//WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)str); 
//	gui::IGUIFont *font;
//    //STSONG.TTF ��������   SIMFANG.TTF  STFANGSO.TTF ���ķ���   SIMSUN.TTC����,������          SIMKAI.TTF���Ŀ���        Fixedsys
//	font = ui->getDXFont("����",true,fontsize,0,weight);
//		int textsize=strlen(str);
//	core::rect<s32> AbsoluteRect( pos, pos+ core::dimension2d<s32>(textsize*fontsize,fontsize)); 
//	font->draw(pwStr,AbsoluteRect,color);
//	delete []pwStr;
}



/*******************************************************************************************

ini��д

*******************************************************************************************/

// -----------------------------------------------------------------------------------------
//  ���ַ���  ����ַ��� ��RetValue�ַ��� 255����
//  ����ֵ true ���ɹ� false ��ʧ��
// -----------------------------------------------------------------------------------------
bool IniGetString( char *strFile, char *lpAppName, char *lpKeyName,char* RetValue )
{

	DWORD ret=GetPrivateProfileString (lpAppName,lpKeyName, NULL, RetValue, 255, strFile);  //��
	if(ret==0){
		//MessageBox(0,"ini��ʧ��!","",0);
		return false;
	}else{
		return true;
	}
}

// ������
int IniGetInt(  char *strFile, char *lpAppName, char *lpKeyName )
{
	char inBuf[255];
	DWORD ret=GetPrivateProfileString (lpAppName,lpKeyName, NULL, inBuf, 255, strFile);  //��
	if(ret==0){
		//MessageBox(0,"ini��ʧ��!","",0);
		return -1;
	}else{
		ret=atoi(inBuf);
		return ret;
	}

}

// -----------------------------------------------------------------------------------------
 



//�õ�irr����
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
		deviceParam.Stencilbuffer=false; //�������shadow��־
		

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
	sprintf(temp,"%d��%d�� %d:%d:%d",sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);

	fprintf(log, "%s  %s\n",temp, str);
	fclose(log);
	return 0;
}



int randomNum(int min,int max)
{
	struct _timeb timebuffer;//����һ���ṹ
	_ftime(&timebuffer);//��ʼ��timebuffer
	srand((unsigned short int)timebuffer.millitm);//timebuffer.millitm��ú���

	int x ;

	x=(int)rand() % max+ min;       // random number min-max

	return x;

}

#include "weaponhlsl.h"


//�õ�����hlsl�ļ����ڴ���
IReadFile * getWeaponHLSLFile(IFileSystem * pFS)
{


	//�������ļ��ӿڣ������ر�ע���3�����������׺һ��Ҫ���ļ���ʽһ�£�����õ���bmp����Ӧ����testpng.bmp
	IReadFile * pMFR = pFS->createMemoryReadFile(weapon,weapon_size,"weapon.hlsl",true);
	return pMFR;
}

#include "xbjweapon.h"

//�õ�����xbjhlsl�ļ����ڴ���
IReadFile * getWeaponXBJHLSLFile(IFileSystem * pFS)
{

	//�������ļ��ӿڣ������ر�ע���3�����������׺һ��Ҫ���ļ���ʽһ�£�����õ���bmp����Ӧ����testpng.bmp
	IReadFile * pMFR = pFS->createMemoryReadFile(xbjweapon, xbjweapon_size,"xbjweapon.hlsl",true);
	return pMFR;
}
