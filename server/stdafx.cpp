// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// iocp.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
//�����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������





/*******************************************************************************************

ini��д

*******************************************************************************************/

// -----------------------------------------------------------------------------------------
//  ���ַ��� ע�� ������Ҫdelete ����ַ��� ��RetValue�ַ��� 255����
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




//��list���ñ���.��Ϊ�Ժ�ֻ����
static irr::core::list<SItem*> allitems_list;  //������Ʒ�浽�б�

//�õ���Ʒ������.
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

//�õ�����������
int getItemAttackById(int ItemID)
{
	int attack=-1;
	//1000����������
	if(ItemID<1000){
		irr::core::list<SItem*>::Iterator  list_oper;
		for(list_oper=allitems_list.begin();list_oper!=allitems_list.end();++list_oper)
		{
			SItem* p=(*list_oper);
			if(p->item_index==ItemID) 
			{
				attack=p->itemParam2; //����param2�ǹ�����
				break;
			}
		}
		return attack;
	}else {
		return attack;
	}
}

//��ȡ������Ʒ
void ReadAllItems()
{
	char File[255]="data//items.txt";
	FILE* log;
	log = fopen(File, "r");
	//�ļ�������.����
	if (log == NULL)
		return ;
	char strtemp[255]="";

	printf("===ȫ����Ʒ���ö�ȡ��ʼ===\n");

	int rownum=GetTxtRowNum(File);
	for(int i=1;i<=rownum;i++){
		//ÿ�ж�ȡ
		GetTxtByRowNum(File,strtemp,i);
		if(strtemp[0]=='/' && strtemp[1]=='/'){
			//printf("%d����ע��\n",i-1);
			continue; //������ע��.������һ��
		}else{

			//����һ��������.��ʾ�Ǹ���Ʒ��Ϣ
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

	printf("===ȫ����Ʒ���ö�ȡ���!��%d����Ʒ===\n",allitems_list.getSize());
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
	sprintf(temp,"%d��%d�� %d:%d:%d",sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);

	fprintf(log, "%s -%s\n",temp, str);
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
