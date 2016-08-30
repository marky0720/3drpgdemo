// stdafx.cpp : 只包括标准包含文件的源文件
// iocp.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
//引用任何所需的附加头文件，而不是在此文件中引用





/*******************************************************************************************

ini读写

*******************************************************************************************/

// -----------------------------------------------------------------------------------------
//  读字符串 注意 用完了要delete 这个字符串 传RetValue字符串 255长度
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




//该list不用保护.因为以后都只读他
static irr::core::list<SItem*> allitems_list;  //所有物品存到列表

//得到物品的属性.
const SItem* getItembyID(int ItemID)
{
	irr::core::list<SItem*>::Iterator  list_oper;
	for(list_oper=allitems_list.begin();list_oper!=allitems_list.end();++list_oper)
	{
		SItem* p=(*list_oper);
		if(p->item_index==ItemID) 
		{
			return p;

		}
	}

	return 0;
}

//得到武器攻击力
int getItemAttackById(int ItemID)
{
	int attack=-1;
	//1000以内是武器
	if(ItemID<1000){
		irr::core::list<SItem*>::Iterator  list_oper;
		for(list_oper=allitems_list.begin();list_oper!=allitems_list.end();++list_oper)
		{
			SItem* p=(*list_oper);
			if(p->item_index==ItemID) 
			{
				attack=p->itemParam2; //武器param2是攻击力
				break;
			}
		}
		return attack;
	}else {
		return attack;
	}
}

//读取所有物品
void ReadAllItems()
{
	char File[255]="data//items.txt";
	FILE* log;
	log = fopen(File, "r");
	//文件不存在.返回
	if (log == NULL)
		return ;
	char strtemp[255]="";

	printf("===全局物品配置读取开始===\n");

	int rownum=GetTxtRowNum(File);
	for(int i=1;i<=rownum;i++){
		//每行读取
		GetTxtByRowNum(File,strtemp,i);
		if(strtemp[0]=='/' && strtemp[1]=='/'){
			//printf("%d行是注释\n",i-1);
			continue; //该行是注释.继续下一个
		}else{

			//即第一个是数字.表示是个物品信息
			if(strtemp[0]>=48 && strtemp[0]<=57 ){
				SItem* item=new SItem;

				sscanf(strtemp, "%d  %s  %d  %d  %d  %d  %d  %d  %d", 
					&item->item_index,&item->itemName,
					&item->itemParam1,&item->itemParam2,
					&item->itemParam3,&item->itemParam4,
					&item->itemParam5,&item->itemParam6,
					&item->itemPrice
					); 

				printf("%d  %s  %d  %d  %d  %d  %d  %d  %d\n",item->item_index,item->itemName,
					item->itemParam1,item->itemParam2,
					item->itemParam3,item->itemParam4,
					item->itemParam5,item->itemParam6,
					item->itemPrice);

				allitems_list.push_back(item);
			}
		}
	}

	printf("===全局物品配置读取完毕!共%d个物品===\n",allitems_list.getSize());
}




#define LOGFILE "server.txt"
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

	fprintf(log, "%s -%s\n",temp, str);
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
