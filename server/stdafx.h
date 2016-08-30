// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���ǳ��õ��������ĵ���Ŀ�ض��İ����ļ�
//

#pragma once


#include <iostream>
#include <tchar.h>

#include <winsock2.h>


#include <vector>

#include <windows.h>
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



#include "stdlib.h"

#include <sys/timeb.h>  //_ftime �޷���ֵ,����ñ��ص�ʱ�����ݲ����������ݽ���ָ��ʱ��ṹ�ı�����

#include <time.h> 

//#define assert(exp)     ((void)0)

//��ɫ����
typedef struct
{
	char cmd; //����
	unsigned short sock; //�ͻ���ʶ
	char worldMap_id; // ��ɫ���ĸ������ͼ
	char charSex; //��ɫ�Ա� 1 �� 2 Ů
	unsigned short Level; //�ȼ�
	int Exp; //��ǰ����
	int NextLevelExp; //����һ�����辭��
	unsigned short usePoint;	//�������Ե���
	unsigned short Power_Point; //��������
	unsigned short Minjie_Point; //���ݵ���
	unsigned short Tili_Point;  //��������
	unsigned short HP;		//Ѫ 
	unsigned short MAX_HP;  //���
	unsigned short Defense; //������
	unsigned short Attack;  //������ ����������,�ӵ���ߵĹ�����, ����������ʱ��.
	irr::core::vector3df pos;  //����
	irr::core::vector3df rot;  //��ת
	irr::core::vector3df target; //Ҫ�ƶ���Ŀ��
	bool RunMode;		    //��ģʽ ������;
	bool isDeath;			//�ѹ�
	char roleName[20];      //��ɫ��
	char currState;		    //��ǰ����
}RoleProperty;

//��ɫ�İ����ṹ
typedef struct{
	char cmd;
	unsigned short sock;
	unsigned short currWeapon; //��ǰ��ɫ���ϵ�����
	unsigned short weaponparam1; //�������� 0��ʾ��ͨ. 1��ɫ 2����Ļ� 3����ı� 4����Ķ�
	unsigned short weaponparam2;
	unsigned short weaponparam3;
	unsigned short weaponparam4;
	unsigned short weaponparam5;
	unsigned short weaponparam6;
	char row1;
	char col1;
	char row2;
	char col2;
	int money;			    //��ɫ��Ǯ
	unsigned short itembox[4][6]; //4��6�� ��Ʒ���,��������,����,����Ʒ.
	unsigned short itemParam1[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam2[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam3[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam4[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam5[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam6[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
}Package;


#define Skill_1 1   //�������
#define Skill_2 2   //ʱ��֮��

//��ɫ�ļ��ܱ�ṹ
typedef struct{
	char cmd;
	unsigned short sock;
	unsigned short skill[10]; //10������  0��ʾδѧϰ 1��ʾ�����ȼ� ���9�� .������Чʱ����ȼ�������ӳ�
}RoleSkillTable;

//��������
typedef struct
{
	char cmd;				//����
	unsigned short ID;		//id 
	char worldMap_id;		// �������ĸ������ͼ
	char Name[20];    //������
	unsigned short HP;		//Ѫ 
	unsigned short MAX_HP;  //���
	unsigned short Defense; //������
	unsigned short Attack;  //������
	bool isDeath;			//�ѹ�
	irr::core::vector3df pos;  //����
	irr::core::vector3df rot;  //��ת
	char currState; //��ǰ����
	irr::core::vector3df target; //Ҫ�ƶ���Ŀ��
	unsigned short damage; //���������˺�
	unsigned short killedByUser; //���ĸ���ɫɱ���� �ͻ��˲���
}MonsterProperty;


//�����������.
typedef struct
{
    int type; //��������   /�ƶ��ٶ�/����ʱ��/������/��Ǯ��/�������ȼ�
    char Name[25];//����
	unsigned short Level ;//�ȼ�
	unsigned short HP;   //���Ѫ
	unsigned short Min_Damage; //��С�˺�
	unsigned short Max_Damage; //����˺�
    unsigned short Defense; //������
	unsigned short MaxMove_distense; //������ˢ�ֵ��ƶ���Χ���뾶
	unsigned short Attack_distense;	//��������
	unsigned short View_distense; //������Ұ��Χ
    unsigned short MoveSpeed ; //�ƶ��ٶ�
	unsigned short ReBornTime; //����ʱ��
    unsigned short ItemDropPer; //��Ʒ����
	unsigned short MoneyDropPer; //��Ǯ����
	unsigned short MaxItemDropLevel; //����Ʒ��ߵȼ�
}SMonsterInfo;

//NPC����
typedef struct
{
	char cmd;					//����
	unsigned short ID;			//id 
	char worldMap_id;			// npc���ĸ������ͼ
	irr::core::vector3df pos;   //����
	irr::core::vector3df rot;   //��ת
	char strNPCName[25];		//npc����
	char strNpcTalk[255];		//npc�Խ�ɫ˵�Ļ�
	char strBtnText[25];        //�Ի���ť�ϵ���
}NPCProperty;


//������Ʒ�ṹ��Ϣ
typedef struct{
	unsigned short item_index ;		 //��Ʒ���,1000���ڶ������� 1000-2000 ������Ʒ		 2000-3000 ��װ��	
	unsigned short itemParam1; 
	unsigned short itemParam2; 
	unsigned short itemParam3; 
	unsigned short itemParam4; 
	unsigned short itemParam5; 
	unsigned short itemParam6; 

	int itemPrice;			 //��Ʒ�۸�
	char itemName[25];               //��Ʒ����
}SItem;


//�̵�����ṹ
typedef struct{
	char cmd;
	
	char worldMap_id;			// ��������npc
	unsigned short NPC_ID;			// NPC ID

	char row1;
	char col1;

	unsigned short itembox[4][6]; //4��6�� ��Ʒ���,��������,����,����Ʒ.
	unsigned short itemParam1[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam2[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam3[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam4[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam5[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���
	unsigned short itemParam6[4][6]; //4��6�� ��Ʒ��Ӧ�Ĳ���

	int itemPrice[4][6];			 //��Ʒ�۸�

}ShopPackage;


typedef struct
{ 
	char cmd; //����
	int sock;  //
	char msg[255]; //��󺰻�����
}ServerRoleMSG;



//�ͻ��ṹ
typedef struct
{
	SOCKET sock;//
	int size; //pData��С 
	char pData[1024*4]; //�̶������4k ���ƹ����� 
}ClientMsg;




//��ɫ/����״̬
#define  cRun  1		//��
#define  cWalk 2	//��
#define  cAttack 3	//����
#define  cIdle 4		//ս��
#define  cReadyAttack 5//Ԥ������
#define  cDeath 6 //����
#define  cAttack2 7	//����2



#include "MessageIdentifiers.h"

//��ɫ��Ϣ
#define CMD_C2S_UserRequest_LOGIN  1 //��ɫ����½ ����������Ϣ
#define CMD_UserUPDATE  2 //��ɫ������Ϣ������� 
#define CMD_S2C_User_LOGIN  3 //�õ�����ɫ������Ϣ
#define CMD_S2C_NEWUSER  4  //��ɫ������Ϣ,���͸������˵�.
#define CMD_QUIT  5	 //��ɫ�˳�
#define CMD_USERMSG  6  //��ɫ����
#define CMD_C2S_UNKNOWUSER  7  //����δ֪��ɫ��Ϣ
#define CMD_S2C_USERUNDERATTACK  8  //��ɫ��������Ϣ
#define CMD_C2S_USERCollectItem  9  //��ɫ�������Ʒ
#define CMD_S2C_USERMoneyUpdate  10 //��ɫ��Ǯ���� 
#define CMD_C2S_Request_AddPoint  11 //��ɫ��������Ե� 
#define CMD_C2S_Request_Prop  12 //��ɫ�������������Ϣ,����hp ������,������. ����,����,����.����
#define CMD_S2C_Response_Prop  13 //�������Խ�ɫ�������������Ϣ����
//#define CMD_C2S_Request_AllOnlineRole 0x14  //�ͻ��Է����������������߽�ɫ
//#define CMD_S2C_Response_AllOnlineRole 0x15  //��������Ӧ14����
#define CMD_C2S_Request_ChangeWeapon 14  //�ͻ��Է�������������
#define CMD_S2C_Response_ChangeWeapon  15  //��������Ӧ,�㲥���������߽�ɫ
#define CMD_C2S_Request_Package  16  //�ͻ��Է���������װ������
#define CMD_S2C_Response_Package  17  //��������Ӧװ������
#define CMD_C2S_Request_ChangePackageItem  18  //�ͻ����������������Ʒ����λ��
#define CMD_S2C_Response_ChangeWorld  19  //������Ҫ��ͻ��л���ͼ
#define CMD_C2S_Request_DropPackageItem  20 //�ͻ��������ٰ�����ĳ��Ʒ
#define CMD_C2S_Request_UsePackageHPItem  21  //�ͻ��Է���������ʹ��װ���������hp�ָ�����Ʒ
#define CMD_C2S_Request_SellPackageItem 22  //�ͻ�������۸���Ʒ


//������Ϣ
#define MonsterCMD_LOGIN  40 //���¹������
#define MonsterCMD_UPDATE 41 //���������Ϣ
#define MonsterCMD_UnderAttack  42 //���ﱻ����
#define MonsterCMD_UNKNOWMONSTER  43 //����δ֪�������Ϣ
#define MonsterCMD_MonsterAll  44 //�������й�����Ϣ,һ������һ��


//������Ʒ��Ϣ
#define SceneCMD_ITEMDROP  50 //�е�����Ʒ���
#define SceneCMD_ITEMDROPAll  51 //�������е�����Ʒ��Ϣ,һ��SceneItem����һ��
#define SceneCMD_ITEMDROPAllNum  52 //�������е�����Ʒ��,ֻ�跢��һ�νṹ
#define SceneCMD_ITEMDROPRemove  53 //ĳ��Ʒ��ʧ 


#define CMD_C2S_UserRequest_NPC 70 //��ɫ����npc��Ϣ
#define CMD_S2C_NPC_Shop		71 //npc�̵귢�͸��ͻ�
#define CMD_C2S_UserBuyShop     72 //��ɫ������npc���̵�


//��Ʒ����
#define ITEMType_Money  1 //���
#define ITEMType_Item   2 //��Ʒ

//��Ʒ�ṹ
typedef struct
{
	char cmd; //������Ʒ��Ϣ
	char worldMap_id; // ��Ʒ���ĸ������ͼ
	char item_Type; //��Ʒ���� ������ 1��� 2��Ʒ
	unsigned short item_index;	   //��Ʒ����
	char itemName[20]; //��Ʒ����
	vector3df pos; //��Ʒ����
	unsigned short param1;
	unsigned short param2;
	unsigned short param3;
	unsigned short param4;
	unsigned short param5;
	unsigned short param6;
}SceneItem;

//��Ʒ�ṹ
typedef struct
{
	char cmd; //������Ʒ��Ϣ
	int itemnum; //��Ʒ����
}SceneItemNum;





//������Ŷ��� ����weaponmgr itemgui
#define SRJ_Weapon  1  //˫�н�
#define TWJ_Weapon  2  //���޽�
#define CWJ_Weapon  3  //������
#define JXKJ_Weapon  4  //��е����
#define DMGSJ_Weapon  5  //������Ͻ�
#define XBJ_Weapon  6  //������
#define BLD_Weapon  7  //��ɏ��
#define BLD_Weapon  7  //��ɏ��
#define STD_Weapon  8  //������
#define Cai7_Weapon  9  //7�ʽ�




//���崫�͵�ṹ �洢��ͼ���͵�
typedef struct  
{
	char worldMap_id; // �����ͼid
	vector3df teleport_POS; //���͵�
	char NextworldMap_id; //���͵���һ�������ͼ��id
	vector3df teleport_StartPos; //���͵�������Ľ�ɫ��ʼ������

}STeleport;


//��ȡ������Ʒ
void ReadAllItems();

//�õ���Ʒ.
const SItem* getItembyID(int ItemID);

//�õ�����������
int getItemAttackById(int itemID);


/*******************************************************************************************

ini��д

*******************************************************************************************/

// -----------------------------------------------------------------------------------------
//  ���ַ��� ע�� ������Ҫdelete ����ַ��� ��RetValue�ַ��� 255����
//  ����ֵ true ���ɹ� false ��ʧ��
// -----------------------------------------------------------------------------------------
bool IniGetString( char *strFile, char *lpAppName, char *lpKeyName,char* RetValue );

// ������
int IniGetInt(  char *strFile, char *lpAppName, char *lpKeyName );

bool GetTxtByRowNum(char* filename,char* Dest_str,int RowNum);

//�õ��ı�����
int GetTxtRowNum(char* filename);




int WriteToLog(char* str);

int randomNum(int min,int max);