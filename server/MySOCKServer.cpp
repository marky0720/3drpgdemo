#include "StdAfx.h"
#include ".\mysockserver.h"


namespace Socket
{



int MySOCKServer::update(   )
{
	MySOCKServer* cnet=this;
	unsigned char packetIdentifier;
	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;
	Packet* p;

 
	if (cnet->isStart()==false)
	{
		return 1;
	}
 
	{
 

		::EnterCriticalSection(&cnet->SendMsgSection);

		std::vector<SMsg*>::iterator vector_oper;
		for(vector_oper=cnet->sendpack.begin();vector_oper!=cnet->sendpack.end();++vector_oper)
		{
			SMsg* pa=*vector_oper;

			int ccc=cnet->updateClientSendCount(pa->guid);
			
			//-1说明该客户已退出.就不发送数据给他了.
			if(ccc == -1) {

			}else{
				//向单用户发送信息
				cnet->server->Send(pa->pData, (int) pa->size, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pa->guid, false);


				//记录到日志
				char test[255];
				sprintf(test,"发送给角色:%s 包命令:%d 大小:%d 累计发送次数:%d",pa->guid.ToString(),(unsigned char)pa->pData[1],pa->size,ccc);
				WriteToLog(test);

				printf("发送给角色:%s 包命令:%d 大小:%d 累计发送次数:%d\n",pa->guid.ToString(),(unsigned char)pa->pData[1],pa->size,ccc);

			}

		
			vector_oper = cnet->sendpack.erase(vector_oper);
			
			//内存回收到内存池
		//	cnet->myObjectPool.free(pa);

			//delete pa->pData;
			//pa->pData=NULL;

			delete pa;
			pa=NULL;
			break;

		}

		::LeaveCriticalSection(&cnet->SendMsgSection);



		for (p=cnet->server->Receive(); p; cnet->server->DeallocatePacket(p), p=cnet->server->Receive())
		{
			// We got a packet, get the identifier with our handy function
			packetIdentifier = cnet->GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:  //客户端正常断开网络

				cnet->m_pProcessRecvData(p->guid, 0,0);
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				
				cnet->RemoveClient(p->guid);

				// Connection lost normally
				//printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString(true));;
				break;

				//新客户连接
			case ID_NEW_INCOMING_CONNECTION:
				{
					// Somebody connected.  We have their IP now
					printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
					clientID=p->systemAddress; // Record the player ID of the client
					ClientInfo* info=new ClientInfo;
					if(!info){
						MessageBox(0,"内存申请失败!","error",0);
					}

					info->sendcount=0;			//0
					info->guid=p->guid;   //guid标识
					info->addr=p->systemAddress; //ip地址
					info->worldMap=-1; //角色未登陆 

					info->ID=cnet->getConnectedClientNum()+'a';
					printf("clientinfo  guid:%s map:%d id:%d\n" ,info->guid.ToString(),info->worldMap,info->ID);
				
					cnet->addClient(info);

					break;
				}

			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;

			case ID_MODIFIED_PACKET:
				// Cheater!
				printf("ID_MODIFIED_PACKET\n");
				break;

			case ID_CONNECTION_LOST: //连接丢失.客户端非正常断开后.服务端连续超时接收不到客户端响应发生的
				{
					cnet->m_pProcessRecvData(p->guid, 0,0);

					// Couldn't deliver a reliable packet - i.e. the other system was abnormally
					// terminated
					//printf("ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString(true));
					cnet->RemoveClient(p->guid);
					break;
				}

			case ID_USER_PACKET_ENUM:
				
				cnet->m_pProcessRecvData(p->guid, (char*)p->data+1,p->length-1);

				break;
			default:

				//消息回调
				//消息回调
				
				//cnet->m_pProcessRecvData(p->guid,(char*)p->data,p->length);
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				//		printf("%s\n", p->data);

				// Relay the message.  We prefix the name for other clients.  This demonstrates
				// That messages can be changed on the server before being broadcast
				// Sending is the same as before
				//转发消息
				//		sprintf(message, "%s", p->data);
				//		cnet->server->Send(message, (const int) strlen(message)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);


				break;
			}

		}

	}

	return 0;
}

unsigned char MySOCKServer::GetPacketIdentifier(Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else
		return (unsigned char) p->data[0];
}

 

MySOCKServer::MySOCKServer(void)
{
	::InitializeCriticalSection(&g_cs_clientlist);


	::InitializeCriticalSection(&SendMsgSection);


	IsStart=false;

 
}

MySOCKServer::~MySOCKServer(void)
{

	if(IsStart){
		IsStart=false;
		//网络关闭
		server->Shutdown(800);
		RakNetworkFactory::DestroyRakPeerInterface(server);
	}
	
}


//网络初始化
bool MySOCKServer::Init(ProcessRecvData* pProcessRecvData,
						unsigned long iSvrPort)
{
	if(IsStart || pProcessRecvData==NULL)return false;

	char password[255]="abcdefghahahohooyeee!!";

	server=RakNetworkFactory::GetRakPeerInterface();
	server->SetIncomingPassword(password, (int)strlen(password));
	server->SetTimeoutTime(5000,UNASSIGNED_SYSTEM_ADDRESS);

	SystemAddress clientID=UNASSIGNED_SYSTEM_ADDRESS;
	SocketDescriptor socketDescriptor(iSvrPort,0);

	//开始服务.最大4个连接,30毫秒的休眠.
	bool b = server->Startup(2, 30, &socketDescriptor, 1 );

	//设置最大连接数.
	server->SetMaximumIncomingConnections(200);

	//开启成功
	if (b){
		puts("服务器开启成功,最大客户连接数200.\n 等待客户连接.");
		IsStart=true;
	}
	else
	{ 
		IsStart=false;
		puts("服务器开启失败, 程序终止.");
		exit(1);
	}

	//设置偶尔ping 
	server->SetOccasionalPing(true);

	//设置不可靠超时
	server->SetUnreliableTimeout(1000);

	m_pProcessRecvData=pProcessRecvData;

	printf("本机IP: %s\n", server->GetLocalIP(0));
	printf("本机GUID: %s\n", server->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString());
	
	return b;
}


//移除角色信息list
void MySOCKServer::RemoveClient(RakNetGUID guid)
{
	//用户list里移除
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);
	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* p=(*list_oper);
		if(guid==p->guid) 
		{
			clientlist.erase(list_oper);//链表移除一个对象
			delete p;//内存回收
			p=NULL;//避免产生野指针
			break;
		}
	}
	//////---------------------------------------------------------------------------------------------///////////////
	LeaveCriticalSection(&g_cs_clientlist);


	////发送msglist里移除对该用户要发送的信息
	//::EnterCriticalSection(&SendMsgSection);
	//std::vector<SMsg*>::iterator vector_oper;
	//for(vector_oper=sendpack.begin();vector_oper!=sendpack.end();++vector_oper)
	//{
	//	SMsg* pa=*vector_oper;
	//	if(pa){
	//		if(pa->guid==guid){
	//			vector_oper = sendpack.erase(vector_oper);
	//			delete pa->pData;
	//			pa->pData=0;
	//			delete pa;
	//			pa=0;
	//		}
	//	}

	//}

	//::LeaveCriticalSection(&SendMsgSection);

}

// 添加客户
void MySOCKServer::addClient(ClientInfo* r)
{
	::EnterCriticalSection(&g_cs_clientlist);
	clientlist.push_back(r);
	::LeaveCriticalSection(&g_cs_clientlist);
}


bool MySOCKServer::isalreadyinlist(RakNetGUID guid)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		if((*list_oper)->guid==guid){
			::LeaveCriticalSection(&g_cs_clientlist);
			return true;
		}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
	return false;

}

bool MySOCKServer::SendMsg(RakNetGUID guid,char * pData,unsigned long Length)
{
	if(pData==NULL || Length==0 || !IsStart)return false;

	//1.内存池取一块smsg结构
	//SMsg *s = (SMsg *)myObjectPool.malloc();   
	//2.SMsg*s=new SMsg;
	SMsg*s =new SMsg;
	
	//char* tem=new char[Length+1];
	if(s == NULL){
		MessageBox(0,"内存申请失败!SendMsg ","error",0);
	}
//	s->pData=tem;

	s->pData[0]=ID_USER_PACKET_ENUM; //包首字节固定为用户包
	memcpy(s->pData+1,pData,Length);

	s->size=Length+1;
	s->guid=guid;

	::EnterCriticalSection(&SendMsgSection);
	sendpack.push_back(s);
	::LeaveCriticalSection(&SendMsgSection);

	return false;
}

bool MySOCKServer::SendMsg(int ID,char * pData,unsigned long Length)
{
	if(pData==NULL || Length==0 || !IsStart)return false;

	return  SendMsg(getClientInfoByID(ID)->guid,pData,Length);
}

const ClientInfo* MySOCKServer::getClientInfoByID(int ID)
{
	ClientInfo* c2=0;
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		if(c->ID==ID) {
			c2=c;
			break;
		}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);

	return c2;
}

//更新发送客户的包数并返回最新次数.如果找不到该客户,就返回-1 
int MySOCKServer::updateClientSendCount(RakNetGUID guid)
{
	int count=0;

	bool found=false;
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		if(c->guid==guid) {
			found=true;
			c->sendcount++;
			count=c->sendcount;
			break;
		}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);

	//未找到这个客户,说明他已退出了.
	if(found==false) return -1;

	return count;
}


const ClientInfo* MySOCKServer::getClientInfoByGUID(RakNetGUID guid)
{
	ClientInfo* c2=0;
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		if(c->guid==guid) {
			c2=c;
			break;
		}
	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);

	return c2;
}

int MySOCKServer::getConnectedClientNum()
{
	int n=0;
	//::EnterCriticalSection(&g_cs_clientlist);
	n=clientlist.size();
	//////---------------------------------------------------------------------------------------------///////////////
	//::LeaveCriticalSection(&g_cs_clientlist);
	return n;
}

//向某世界地图所有用户发送信息
void MySOCKServer::SendMsgToAllInWorld(char * pData,unsigned long Length ,int worldMap)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		//在同一个世界则发送
		if(c->worldMap==worldMap){
			SendMsg(c->guid,pData,Length);
		}

	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
}

//更新客户所在的世界地图id
void MySOCKServer::setClientWorldMap(int ID,int world)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{

		if((*list_oper)->ID==ID){
			(*list_oper)->worldMap=world;
			break;
		}

	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
}

//更新客户所在的世界地图id
void MySOCKServer::setClientWorldMap(RakNetGUID guid,int world)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{

		if((*list_oper)->guid==guid){
			(*list_oper)->worldMap=world;
			break;
		}

	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
}

void MySOCKServer::SendMsgToOtherInWorld(int exceptID,char * pData,unsigned long Length,int worldMap)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		//排除expect客户
		if(exceptID==c->ID) continue;

		//若在同一个世界地图内.则发送
		if(c->worldMap==worldMap){
			SendMsg(c->guid,pData,Length);	
		}

	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
}


void MySOCKServer::SendMsgToOtherInWorld(RakNetGUID except,char * pData,unsigned long Length,int worldMap)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		//排除expect客户
		if(except==c->guid) continue;

		//若在同一个世界地图内.则发送
		if(c->worldMap==worldMap){
			SendMsg(c->guid,pData,Length);	
		}

	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
}


bool MySOCKServer::DisConnectClient(const SystemAddress clientID)
{
	if(!IsStart  )return false;

	//断开
	server->CloseConnection(clientID, true, 0);


	return false;
}




} // namespace Socket
