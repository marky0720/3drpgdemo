// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
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

#include <sys/timeb.h>  //_ftime 无返回值,但获得本地的时间数据并且填充进传递进来指向时间结构的变量中

#include <time.h> 

//#define assert(exp)     ((void)0)

//角色属性
typedef struct
{
	char cmd; //命令
	unsigned short sock; //客户标识
	char worldMap_id; // 角色在哪个世界地图
	char charSex; //角色性别 1 男 2 女
	unsigned short Level; //等级
	int Exp; //当前经验
	int NextLevelExp; //升下一级所需经验
	unsigned short usePoint;	//可用属性点数
	unsigned short Power_Point; //力量点数
	unsigned short Minjie_Point; //敏捷点数
	unsigned short Tili_Point;  //体力点数
	unsigned short HP;		//血 
	unsigned short MAX_HP;  //最大
	unsigned short Defense; //防御力
	unsigned short Attack;  //攻击力 基本攻击力,加点提高的攻击力, 不是拿武器时的.
	irr::core::vector3df pos;  //坐标
	irr::core::vector3df rot;  //旋转
	irr::core::vector3df target; //要移动的目的
	bool RunMode;		    //跑模式 还是走;
	bool isDeath;			//已挂
	char roleName[20];      //角色名
	char currState;		    //当前动作
}RoleProperty;

//角色的包裹结构
typedef struct{
	char cmd;
	unsigned short sock;
	unsigned short currWeapon; //当前角色手上的武器
	unsigned short weaponparam1; //光武类型 0表示普通. 1紫色 2乱里的火 3乱里的冰 4乱里的毒
	unsigned short weaponparam2;
	unsigned short weaponparam3;
	unsigned short weaponparam4;
	unsigned short weaponparam5;
	unsigned short weaponparam6;
	char row1;
	char col1;
	char row2;
	char col2;
	int money;			    //角色金钱
	unsigned short itembox[4][6]; //4行6列 物品编号,包括武器,防具,消耗品.
	unsigned short itemParam1[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam2[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam3[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam4[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam5[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam6[4][6]; //4行6列 物品对应的参数
}Package;


#define Skill_1 1   //极光防御
#define Skill_2 2   //时空之轮

//角色的技能表结构
typedef struct{
	char cmd;
	unsigned short sock;
	unsigned short skill[10]; //10个技能  0表示未学习 1表示修炼等级 最大9级 .技能有效时间随等级增大而延长
}RoleSkillTable;

//怪物属性
typedef struct
{
	char cmd;				//命令
	unsigned short ID;		//id 
	char worldMap_id;		// 怪物在哪个世界地图
	char Name[20];    //怪物名
	unsigned short HP;		//血 
	unsigned short MAX_HP;  //最大
	unsigned short Defense; //防御力
	unsigned short Attack;  //攻击力
	bool isDeath;			//已挂
	irr::core::vector3df pos;  //坐标
	irr::core::vector3df rot;  //旋转
	char currState; //当前动作
	irr::core::vector3df target; //要移动的目的
	unsigned short damage; //被攻击的伤害
	unsigned short killedByUser; //被哪个角色杀死的 客户端不用
}MonsterProperty;


//怪物基本属性.
typedef struct
{
    int type; //怪物类型   /移动速度/重生时间/暴物率/暴钱率/暴物最大等级
    char Name[25];//名字
	unsigned short Level ;//等级
	unsigned short HP;   //最大血
	unsigned short Min_Damage; //最小伤害
	unsigned short Max_Damage; //最大伤害
    unsigned short Defense; //防御力
	unsigned short MaxMove_distense; //怪物在刷怪点移动范围最大半径
	unsigned short Attack_distense;	//攻击距离
	unsigned short View_distense; //怪物视野范围
    unsigned short MoveSpeed ; //移动速度
	unsigned short ReBornTime; //重生时间
    unsigned short ItemDropPer; //物品暴率
	unsigned short MoneyDropPer; //金钱暴率
	unsigned short MaxItemDropLevel; //暴物品最高等级
}SMonsterInfo;

//NPC属性
typedef struct
{
	char cmd;					//命令
	unsigned short ID;			//id 
	char worldMap_id;			// npc在哪个世界地图
	irr::core::vector3df pos;   //坐标
	irr::core::vector3df rot;   //旋转
	char strNPCName[25];		//npc名称
	char strNpcTalk[255];		//npc对角色说的话
	char strBtnText[25];        //对话框按钮上的字
}NPCProperty;


//单个物品结构信息
typedef struct{
	unsigned short item_index ;		 //物品编号,1000以内都是武器 1000-2000 是消耗品		 2000-3000 是装备	
	unsigned short itemParam1; 
	unsigned short itemParam2; 
	unsigned short itemParam3; 
	unsigned short itemParam4; 
	unsigned short itemParam5; 
	unsigned short itemParam6; 

	int itemPrice;			 //物品价格
	char itemName[25];               //物品名称
}SItem;


//商店包裹结构
typedef struct{
	char cmd;
	
	char worldMap_id;			// 包裹属于npc
	unsigned short NPC_ID;			// NPC ID

	char row1;
	char col1;

	unsigned short itembox[4][6]; //4行6列 物品编号,包括武器,防具,消耗品.
	unsigned short itemParam1[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam2[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam3[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam4[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam5[4][6]; //4行6列 物品对应的参数
	unsigned short itemParam6[4][6]; //4行6列 物品对应的参数

	int itemPrice[4][6];			 //物品价格

}ShopPackage;


typedef struct
{ 
	char cmd; //命令
	int sock;  //
	char msg[255]; //最大喊话长度
}ServerRoleMSG;



//客户结构
typedef struct
{
	SOCKET sock;//
	int size; //pData大小 
	char pData[1024*4]; //固定包最大4k 估计够用了 
}ClientMsg;




//角色/怪物状态
#define  cRun  1		//跑
#define  cWalk 2	//走
#define  cAttack 3	//攻击
#define  cIdle 4		//战立
#define  cReadyAttack 5//预备攻击
#define  cDeath 6 //挂了
#define  cAttack2 7	//攻击2



#include "MessageIdentifiers.h"

//角色消息
#define CMD_C2S_UserRequest_LOGIN  1 //角色初登陆 请求所有信息
#define CMD_UserUPDATE  2 //角色所有信息请求更新 
#define CMD_S2C_User_LOGIN  3 //得到主角色所有信息
#define CMD_S2C_NEWUSER  4  //角色所有信息,发送给其他人的.
#define CMD_QUIT  5	 //角色退出
#define CMD_USERMSG  6  //角色喊话
#define CMD_C2S_UNKNOWUSER  7  //请求未知角色信息
#define CMD_S2C_USERUNDERATTACK  8  //角色被攻击信息
#define CMD_C2S_USERCollectItem  9  //角色捡地面物品
#define CMD_S2C_USERMoneyUpdate  10 //角色金钱更新 
#define CMD_C2S_Request_AddPoint  11 //角色请求加属性点 
#define CMD_C2S_Request_Prop  12 //角色请求属性面版信息,比如hp 攻击力,防御力. 力量,敏捷,体力.经验
#define CMD_S2C_Response_Prop  13 //服务器对角色请求属性面版信息反馈
//#define CMD_C2S_Request_AllOnlineRole 0x14  //客户对服务器请求所有在线角色
//#define CMD_S2C_Response_AllOnlineRole 0x15  //服务器响应14命令
#define CMD_C2S_Request_ChangeWeapon 14  //客户对服务器请求换武器
#define CMD_S2C_Response_ChangeWeapon  15  //服务器响应,广播给其他在线角色
#define CMD_C2S_Request_Package  16  //客户对服务器请求装备包裹
#define CMD_S2C_Response_Package  17  //服务器响应装备包裹
#define CMD_C2S_Request_ChangePackageItem  18  //客户请求包裹里两个物品交换位置
#define CMD_S2C_Response_ChangeWorld  19  //服务器要求客户切换地图
#define CMD_C2S_Request_DropPackageItem  20 //客户请求销毁包裹里某物品
#define CMD_C2S_Request_UsePackageHPItem  21  //客户对服务器请求使用装备包裹里的hp恢复消耗品
#define CMD_C2S_Request_SellPackageItem 22  //客户请求出售该物品


//怪物消息
#define MonsterCMD_LOGIN  40 //有新怪物加入
#define MonsterCMD_UPDATE 41 //怪物更新信息
#define MonsterCMD_UnderAttack  42 //怪物被攻击
#define MonsterCMD_UNKNOWMONSTER  43 //请求未知怪物的信息
#define MonsterCMD_MonsterAll  44 //请求所有怪物信息,一个发送一次


//场景物品消息
#define SceneCMD_ITEMDROP  50 //有掉落物品添加
#define SceneCMD_ITEMDROPAll  51 //请求所有掉落物品信息,一个SceneItem发送一次
#define SceneCMD_ITEMDROPAllNum  52 //请求所有掉落物品数,只需发送一次结构
#define SceneCMD_ITEMDROPRemove  53 //某物品消失 


#define CMD_C2S_UserRequest_NPC 70 //角色请求npc信息
#define CMD_S2C_NPC_Shop		71 //npc商店发送给客户
#define CMD_C2S_UserBuyShop     72 //角色请求购买npc的商店


//物品类型
#define ITEMType_Money  1 //金币
#define ITEMType_Item   2 //物品

//物品结构
typedef struct
{
	char cmd; //场景物品消息
	char worldMap_id; // 物品在哪个世界地图
	char item_Type; //物品类型 就两种 1金币 2物品
	unsigned short item_index;	   //物品索引
	char itemName[20]; //物品名称
	vector3df pos; //物品坐标
	unsigned short param1;
	unsigned short param2;
	unsigned short param3;
	unsigned short param4;
	unsigned short param5;
	unsigned short param6;
}SceneItem;

//物品结构
typedef struct
{
	char cmd; //场景物品消息
	int itemnum; //物品总数
}SceneItemNum;





//武器编号定义 用于weaponmgr itemgui
#define SRJ_Weapon  1  //双刃剑
#define TWJ_Weapon  2  //天无剑
#define CWJ_Weapon  3  //楚王剑
#define JXKJ_Weapon  4  //机械阔剑
#define DMGSJ_Weapon  5  //大马格氏剑
#define XBJ_Weapon  6  //玄冰剑
#define BLD_Weapon  7  //白蓮刀
#define BLD_Weapon  7  //白蓮刀
#define STD_Weapon  8  //熟铁刀
#define Cai7_Weapon  9  //7彩剑




//定义传送点结构 存储地图传送点
typedef struct  
{
	char worldMap_id; // 世界地图id
	vector3df teleport_POS; //传送点
	char NextworldMap_id; //传送到另一个世界地图的id
	vector3df teleport_StartPos; //传送到新世界的角色初始化坐标

}STeleport;


//读取所有物品
void ReadAllItems();

//得到物品.
const SItem* getItembyID(int ItemID);

//得到武器攻击力
int getItemAttackById(int itemID);


/*******************************************************************************************

ini读写

*******************************************************************************************/

// -----------------------------------------------------------------------------------------
//  读字符串 注意 用完了要delete 这个字符串 传RetValue字符串 255长度
//  返回值 true 读成功 false 读失败
// -----------------------------------------------------------------------------------------
bool IniGetString( char *strFile, char *lpAppName, char *lpKeyName,char* RetValue );

// 读数字
int IniGetInt(  char *strFile, char *lpAppName, char *lpKeyName );

bool GetTxtByRowNum(char* filename,char* Dest_str,int RowNum);

//得到文本行数
int GetTxtRowNum(char* filename);




int WriteToLog(char* str);

int randomNum(int min,int max);