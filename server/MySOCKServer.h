#pragma once



#include <iostream>
#include <tchar.h>

#include <stdio.h>

#include "windows.h"

#include <vector>
#include <string>
#include <algorithm>
#include <list>
using namespace std;


  
#pragma comment(lib,"RakNet3.0.lib")
 


#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#ifdef _WIN32
#include "Kbhit.h"
#endif
#include <stdio.h>
#include <string.h>

#if defined(_CONSOLE_2)
#include "Console2SampleIncludes.h"
#endif


////使用boost内存池
//#include <boost/pool/pool.hpp>
//#include <boost/pool/object_pool.hpp>
//using namespace boost;

namespace Socket
{


//定义客户结构
typedef struct
{
	SystemAddress  addr;
	int ID;  //顺序id
	RakNetGUID guid;	
	int worldMap; //定义角色在哪个世界地图 初始化-1表示还没登陆.
	int  sendcount;		//累计接收到客户数据次数
}ClientInfo;


typedef struct 
{
	int size;
	char pData[1024*4];
	RakNetGUID guid;
}SMsg;



//回调处理数据函数原型
typedef void __stdcall ProcessRecvData( RakNetGUID guid,
									   char * pData,
									   unsigned long DataLength);

#define PackSize 1024/4

class MySOCKServer
{
public:
	MySOCKServer(void);
	~MySOCKServer(void);

	//以SMsg为单位的的内存池
//	boost::object_pool<SMsg>  myObjectPool;
	
	int update();

	bool Init(ProcessRecvData* pProcessRecvData,	unsigned long iSvrPort);

	// We copy this from Multiplayer.cpp to keep things all in one file for this example
	unsigned char GetPacketIdentifier(Packet *p);

	RakPeerInterface *server;

	ProcessRecvData* m_pProcessRecvData;

	const ClientInfo* getClientInfoByGUID(RakNetGUID guid);

	const ClientInfo* getClientInfoByID(int ID);

	//更新客户所在的世界地图id
	void setClientWorldMap(RakNetGUID guid,int world);
	
	void setClientWorldMap(int ID,int world);

	//向同在一个世界地图内的玩家发送数据
	void SendMsgToAllInWorld(char * pData,unsigned long Length,int worldMap);
	
	void SendMsgToOtherInWorld(RakNetGUID except,char * pData,unsigned long Length,int worldMap);
	
	void SendMsgToOtherInWorld(int exceptID,char * pData,unsigned long Length,int worldMap);
	
	//两种发送方法.指定客户id或guid
	bool SendMsg(RakNetGUID guid,char * pData,unsigned long Length);
	bool SendMsg(int ID,char * pData,unsigned long Length);
	
	//更新发送客户的包数
	int updateClientSendCount(RakNetGUID guid);
	
	bool isStart(){return IsStart;};

	CRITICAL_SECTION SendMsgSection;
	
	std::vector<SMsg*> sendpack;

	void addClient(ClientInfo* r);
	
	int getConnectedClientNum();
	
	bool DisConnectClient(const SystemAddress clientID);
	
	//移除客户信息
	void RemoveClient(RakNetGUID guid);
private:
	//客户list临界
	CRITICAL_SECTION g_cs_clientlist;
	//保存连接的客户list
	std::vector<ClientInfo*> clientlist;

	bool IsStart;
protected:
	//是否已连接
	bool isalreadyinlist(RakNetGUID guid);
};


} // namespace Socket