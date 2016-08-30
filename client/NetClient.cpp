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

	//ѭ������
	while (1)
	{
		
		// This sleep keeps RakNet responsive
		Sleep(30);

		//�����ָ��
		Packet* p;


			::EnterCriticalSection(&cnet->SendMsgSection);

			std::vector<SMsg*>::iterator vector_oper;
			for(vector_oper=cnet->sendpack.begin();vector_oper!=cnet->sendpack.end();++vector_oper)
			{
				SMsg* pa=*vector_oper;
				
				printf("�������ݰ� ����ID:%d ����С:%d\n",(unsigned char)pa->pData[1],pa->size);

				cnet->getClient()->Send(pa->pData, (int) pa->size, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
				vector_oper = cnet->sendpack.erase(vector_oper);
				delete pa->pData;
				pa->pData=NULL;
				delete pa;
				pa=NULL;
				break;
				
			}

			::LeaveCriticalSection(&cnet->SendMsgSection);
		

			//�ͷ���һ�εİ�       ������һ����               
			for (p=cnet->getClient()->Receive(); p; cnet->getClient()->DeallocatePacket(p), p=cnet->getClient()->Receive())
			{
				// We got a packet, get the identifier with our handy function
				// �õ�һ����,Ȼ��鿴���ı�ʶ��
				packetIdentifier = Socket::GetPacketIdentifier(p);

				// Check if this is a network message packet
				//����Ƿ������������Ϣ��
				switch (packetIdentifier)
				{
				case ID_DISCONNECTION_NOTIFICATION:
					// Connection lost normally �������Ӷ�ʧ
					printf("ID_DISCONNECTION_NOTIFICATION\n");
					break;
				case ID_ALREADY_CONNECTED:
					//������
					printf("ID_ALREADY_CONNECTED\n");
					break;
				case ID_INCOMPATIBLE_PROTOCOL_VERSION: //������Э��汾
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
					// Couldn't deliver(����) a reliable(�ɿ���) packet - i.e. the other system was abnormally(�쳣) terminated(��ֹ)

					printf("ID_CONNECTION_LOST\n");
					break;

				case ID_CONNECTION_REQUEST_ACCEPTED:
					// This tells the client they have connected
					//���߿ͻ������Ѿ�����ok��.

					printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", 
						p->systemAddress.ToString(true), p->guid.ToString());
					printf("My external address is %s\n", cnet->getClient()->GetExternalID(p->systemAddress).ToString(true));
					break;

				case ID_USER_PACKET_ENUM:
					//�������ֽ�. ����lengthҪ-1 data+1 �м�����ݲ������ǵİ�.
					cnet->m_pProcessRecvData( (char*)p->data+1,p->length-1);
					break;
				default:
					// It's a client, so just show the message
					//�ͻ���ʾ��Ϣ
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
	//��ʼ��raknet�������
	client=RakNetworkFactory::GetRakPeerInterface();

	clientID=UNASSIGNED_SYSTEM_ADDRESS;
	bool startok=false;
	int port=20;
	//��������udpĳ�����ö˿�,ֻ�������ɹ�һ�����ö˿ڲ�����
while (startok!=true)
{
	SocketDescriptor socketDescriptor (port,0);

	startok=client->Startup(8, //�������
		30, //�߳�����ʱ��
		&socketDescriptor, 1);

	port++;
}
	

	 //����ż��ping
	 client->SetOccasionalPing(true);

	 //��ʼ��������Ϣmsg���ٽ�
	 ::InitializeCriticalSection(&SendMsgSection);

	 //�������������߳�
	 DWORD id;
	 CreateThread(0,0,RecvProc,this,0,&id);

	 

}
CNetClient::~CNetClient()
{
	UnInit();
}


bool CNetClient::Init(ProcessRecvData* pProcessRecvData,char* ip, unsigned long serverPort)
{
	if(pProcessRecvData==NULL //�ص�������
		|| ip==NULL //��ַ��
		)
		return false;	
	
	m_pProcessRecvData=pProcessRecvData;

	char password[255]="abcdefghahahohooyeee!!";

	//���ӷ����� ip,�˿� ���͵��ַ���.����������½����֤��
	bool b = client->Connect(ip, serverPort, password, (int) strlen(password));	
	//����true��ʾ���ӳɹ�
	if (b)
		puts("���ӳɹ�");
	else
	{
		puts("����ʧ��");
		exit(1);
	}

	return true;

	
}
void CNetClient::UnInit()
{
	client->Shutdown(300);
	RakNetworkFactory::DestroyRakPeerInterface(client);

}

//��������.
bool CNetClient::SendMsg(char * pData,unsigned long DataLength)
{		
		//δ���ó�ʼ������
		if( pData==NULL || DataLength==0)return false;
	
		SMsg* s=new SMsg;
		char* tem=new char[DataLength+1]; //�û����ݴ�С+���ֽڰ������־.
		if(tem==NULL){
			MessageBox(0,"�ڴ�����ʧ��!","error",0);
		}
		tem[0]=ID_USER_PACKET_ENUM; //���ֽڱ�־�û���������raknet�Ļ�������
		//�������ֽ�. ����ĲŸ������ǵ�����
		memcpy(tem+1,pData,DataLength);
		s->pData=tem;
		s->size=DataLength+1;



		::EnterCriticalSection(&SendMsgSection);
		sendpack.push_back(s);
		::LeaveCriticalSection(&SendMsgSection);

		return true;
}

/*
 �����־
*/
void CNetClient::WriteLogString(char* strLog)
{
	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);

    FILE* log;
    log = fopen("NetSvrLog.txt", "a+");
    if (log == NULL)
    return ;
	fprintf(log, "%d��%d��%d��%dʱ%d��%d��--%s\n", sysTm.wYear,sysTm.wMonth,sysTm.wDay,sysTm.wHour,sysTm.wMinute,sysTm.wSecond,strLog);
    fclose(log);

}



} // namespace Socket

