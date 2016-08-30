// NetClient.h: interface for the CNetClient class.
//
//////////////////////////////////////////////////////////////////////


#pragma once

 
#pragma comment(lib,"RakNet3.0.lib")
 

#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>

#include "Kbhit.h"
#include "WindowsIncludes.h" // Sleep


#include "windows.h"

#include "vector"


namespace Socket
{
	unsigned char GetPacketIdentifier(Packet *p);

//通过回调函数调用上层处理函数
typedef void __stdcall ProcessRecvData(char * pData,unsigned long DataLength);



typedef struct 
{
  int size;
  char* pData;
}SMsg;

class CNetClient  
{
public:
	CNetClient();
	virtual ~CNetClient();
public:
	bool Init(ProcessRecvData* pProcessRecvData,char* szSvrAddr,unsigned long iSvrPort);
	void UnInit();
	bool SendMsg(char * pData,unsigned long DataLength);

	void CNetClient::WriteLogString(char* strLog);

	RakPeerInterface * getClient(){return client;};

	CRITICAL_SECTION SendMsgSection;
	std::vector<SMsg*> sendpack;


	ProcessRecvData* m_pProcessRecvData;
	
protected:


private:

	// raknet网络对象
	RakPeerInterface *client;
	SystemAddress clientID;
	
	bool isConnected; 

};

} // namespace Socket
