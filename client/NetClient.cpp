// NetClient.cpp: implementation of the CNetClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetClient.h"



namespace Socket
{


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(Packet *p)
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


DWORD _stdcall RecvProc(LPVOID pParam)
{
	CNetClient* cnet=(	CNetClient*)pParam;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	//循环输入
	while (1)
	{
		
		// This sleep keeps RakNet responsive
		Sleep(30);

		//定义包指针
		Packet* p;


			::EnterCriticalSection(&cnet->SendMsgSection);

			std::vector<SMsg*>::iterator vector_oper;
			for(vector_oper=cnet->sendpack.begin();vector_oper!=cnet->sendpack.end();++vector_oper)
			{
				SMsg* pa=*vector_oper;
				
				printf("发送数据包 命令ID:%d 包大小:%d\n",(unsigned char)pa->pData[1],pa->size);

				cnet->getClient()->Send(pa->pData, (int) pa->size, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				vector_oper = cnet->sendpack.erase(vector_oper);
				delete pa->pData;
				pa->pData=NULL;
				delete pa;
				pa=NULL;
				break;
				
			}

			::LeaveCriticalSection(&cnet->SendMsgSection);
		

			//释放上一次的包       接收下一个包               
			for (p=cnet->getClient()->Receive(); p; cnet->getClient()->DeallocatePacket(p), p=cnet->getClient()->Receive())
			{
				// We got a packet, get the identifier with our handy function
				// 得到一个包,然后查看他的标识符
				packetIdentifier = Socket::GetPacketIdentifier(p);

				// Check if this is a network message packet
				//检查是否这个是网络消息包
				switch (packetIdentifier)
				{
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally 正常连接丢失
					printf("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_ALREADY_CONNECTED:
					//已连接
					printf("ID_ALREADY_CONNECTED\n");
					break;
				case ID_INCOMPATIBLE_PROTOCOL_VERSION: //不相容协议版本
					printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
					break;
				case ID_REMOTE_DISCONNECTION_NOTIFICATION: // Server telling the clients of another client disconnecting gracefully.  You can manually broadcast this in a peer to peer enviroment if you want.
					printf("ID_REMOTE_DISCONNECTION_NOTIFICATION\n"); 
					break;
				case ID_REMOTE_CONNECTION_LOST: // Server telling the clients of another client disconnecting forcefully.  You can manually broadcast this in a peer to peer enviroment if you want.
					printf("ID_REMOTE_CONNECTION_LOST\n");
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION: // Server telling the clients of another client connecting.  You can manually broadcast this in a peer to peer enviroment if you want.
					printf("ID_REMOTE_NEW_INCOMING_CONNECTION\n");
					break;
				case ID_CONNECTION_BANNED: // Banned from this server
					printf("We are banned from this server.\n");
					break;			
				case ID_CONNECTION_ATTEMPT_FAILED:
					printf("Connection attempt failed\n");
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					// Sorry, the server is full.  I don't do anything here but
					// A real app should tell the user
					printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
					break;
				case ID_MODIFIED_PACKET:
					// Cheater!
					printf("ID_MODIFIED_PACKET\n");
					break;

				case ID_INVALID_PASSWORD:
					printf("ID_INVALID_PASSWORD\n");
					break;

				case ID_CONNECTION_LOST:
					// Couldn't deliver(传送) a reliable(可靠的) packet - i.e. the other system was abnormally(异常) terminated(终止)

					printf("ID_CONNECTION_LOST\n");
					break;

				case ID_CONNECTION_REQUEST_ACCEPTED:
					// This tells the client they have connected
					//告诉客户他们已经连接ok了.

					printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", 
						p->systemAddress.ToString(true), p->guid.ToString());
					printf("My external address is %s\n", cnet->getClient()->GetExternalID(p->systemAddress).ToString(true));
					break;

				case ID_USER_PACKET_ENUM:
					//跳过首字节. 所以length要-1 data+1 中间的数据才是我们的包.
					cnet->m_pProcessRecvData( (char*)p->data+1,p->length-1);
					break;
				default:
					// It's a client, so just show the message
					//客户显示消息
					//printf("%s\n", p->data);
					
				//	cnet->m_pProcessRecvData((char*)p->data,p->length);
					break;
				}
			}

		
	}

	return 0;
}

CNetClient::CNetClient()
{
	//初始化raknet网络对象
	client=RakNetworkFactory::GetRakPeerInterface();

	clientID=UNASSIGNED_SYSTEM_ADDRESS;
	bool startok=false;
	int port=20;
	//反复监听udp某个可用端口,只到监听成功一个可用端口才跳出
while (startok!=true)
{
	SocketDescriptor socketDescriptor (port,0);

	startok=client->Startup(8, //最大连接
		30, //线程休眠时间
		&socketDescriptor, 1);

	port++;
}
	

	 //设置偶尔ping
	 client->SetOccasionalPing(true);

	 //初始化发送消息msg的临界
	 ::InitializeCriticalSection(&SendMsgSection);

	 //创建接收数据线程
	 DWORD id;
	 CreateThread(0,0,RecvProc,this,0,&id);

	 

}
CNetClient::~CNetClient()
{
	UnInit();
}


bool CNetClient::Init(ProcessRecvData* pProcessRecvData,char* ip, unsigned long serverPort)
{
	if(pProcessRecvData==NULL //回调函数空
		|| ip==NULL //地址空
		)
		return false;	
	
	m_pProcessRecvData=pProcessRecvData;

	char password[255]="abcdefghahahohooyeee!!";

	//连接服务器 ip,端口 发送的字符串.这里用做登陆的验证码
	bool b = client->Connect(ip, serverPort, password, (int) strlen(password));	
	//返回true表示连接成功
	if (b)
		puts("连接成功");
	else
	{
		puts("连接失败");
		exit(1);
	}

	return true;

	
}
void CNetClient::UnInit()
{
	client->Shutdown(300);
	RakNetworkFactory::DestroyRakPeerInterface(client);

}

//发送数据.
bool CNetClient::SendMsg(char * pData,unsigned long DataLength)
{		
		//未调用初始化函数
		if( pData==NULL || DataLength==0)return false;
	
		SMsg* s=new SMsg;
		char* tem=new char[DataLength+1]; //用户数据大小+首字节包命令标志.
		if(tem==NULL){
			MessageBox(0,"内存申请失败!","error",0);
		}
		tem[0]=ID_USER_PACKET_ENUM; //首字节标志用户包而不是raknet的或其他的
		//跳过首字节. 后面的才附带我们的数据
		memcpy(tem+1,pData,DataLength);
		s->pData=tem;
		s->size=DataLength+1;



		::EnterCriticalSection(&SendMsgSection);
		sendpack.push_back(s);
		::LeaveCriticalSection(&SendMsgSection);

		return true;
}

/*
 输出日志
*/
void CNetClient::WriteLogString(char* strLog)
{
	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);

    FILE* log;
    log = fopen("NetSvrLog.txt", "a+");
    if (log == NULL)
    return ;
	fprintf(log, "%d年%d月%d日%d时%d分%d秒--%s\n", sysTm.wYear,sysTm.wMonth,sysTm.wDay,sysTm.wHour,sysTm.wMinute,sysTm.wSecond,strLog);
    fclose(log);

}



} // namespace Socket

