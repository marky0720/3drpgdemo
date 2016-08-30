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


////ʹ��boost�ڴ��
//#include <boost/pool/pool.hpp>
//#include <boost/pool/object_pool.hpp>
//using namespace boost;

namespace Socket
{


//����ͻ��ṹ
typedef struct
{
	SystemAddress  addr;
	int ID;  //˳��id
	RakNetGUID guid;	
	int worldMap; //�����ɫ���ĸ������ͼ ��ʼ��-1��ʾ��û��½.
	int  sendcount;		//�ۼƽ��յ��ͻ����ݴ���
}ClientInfo;


typedef struct 
{
	int size;
	char pData[1024*4];
	RakNetGUID guid;
}SMsg;



//�ص��������ݺ���ԭ��
typedef void __stdcall ProcessRecvData( RakNetGUID guid,
									   char * pData,
									   unsigned long DataLength);

#define PackSize 1024/4

class MySOCKServer
{
public:
	MySOCKServer(void);
	~MySOCKServer(void);

	//��SMsgΪ��λ�ĵ��ڴ��
//	boost::object_pool<SMsg>  myObjectPool;
	
	int update();

	bool Init(ProcessRecvData* pProcessRecvData,	unsigned long iSvrPort);

	// We copy this from Multiplayer.cpp to keep things all in one file for this example
	unsigned char GetPacketIdentifier(Packet *p);

	RakPeerInterface *server;

	ProcessRecvData* m_pProcessRecvData;

	const ClientInfo* getClientInfoByGUID(RakNetGUID guid);

	const ClientInfo* getClientInfoByID(int ID);

	//���¿ͻ����ڵ������ͼid
	void setClientWorldMap(RakNetGUID guid,int world);
	
	void setClientWorldMap(int ID,int world);

	//��ͬ��һ�������ͼ�ڵ���ҷ�������
	void SendMsgToAllInWorld(char * pData,unsigned long Length,int worldMap);
	
	void SendMsgToOtherInWorld(RakNetGUID except,char * pData,unsigned long Length,int worldMap);
	
	void SendMsgToOtherInWorld(int exceptID,char * pData,unsigned long Length,int worldMap);
	
	//���ַ��ͷ���.ָ���ͻ�id��guid
	bool SendMsg(RakNetGUID guid,char * pData,unsigned long Length);
	bool SendMsg(int ID,char * pData,unsigned long Length);
	
	//���·��Ϳͻ��İ���
	int updateClientSendCount(RakNetGUID guid);
	
	bool isStart(){return IsStart;};

	CRITICAL_SECTION SendMsgSection;
	
	std::vector<SMsg*> sendpack;

	void addClient(ClientInfo* r);
	
	int getConnectedClientNum();
	
	bool DisConnectClient(const SystemAddress clientID);
	
	//�Ƴ��ͻ���Ϣ
	void RemoveClient(RakNetGUID guid);
private:
	//�ͻ�list�ٽ�
	CRITICAL_SECTION g_cs_clientlist;
	//�������ӵĿͻ�list
	std::vector<ClientInfo*> clientlist;

	bool IsStart;
protected:
	//�Ƿ�������
	bool isalreadyinlist(RakNetGUID guid);
};


} // namespace Socket