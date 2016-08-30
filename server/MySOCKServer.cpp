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
			
			//-1˵���ÿͻ����˳�.�Ͳ��������ݸ�����.
			if(ccc == -1) {

			}else{
				//���û�������Ϣ
				cnet->server->Send(pa->pData, (int) pa->size, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pa->guid, false);


				//��¼����־
				char test[255];
				sprintf(test,"���͸���ɫ:%s ������:%d ��С:%d �ۼƷ��ʹ���:%d",pa->guid.ToString(),(unsigned char)pa->pData[1],pa->size,ccc);
				WriteToLog(test);

				printf("���͸���ɫ:%s ������:%d ��С:%d �ۼƷ��ʹ���:%d\n",pa->guid.ToString(),(unsigned char)pa->pData[1],pa->size,ccc);

			}

		
			vector_oper = cnet->sendpack.erase(vector_oper);
			
			//�ڴ���յ��ڴ��
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
			case ID_DISCONNECTION_NOTIFICATION:  //�ͻ��������Ͽ�����

				cnet->m_pProcessRecvData(p->guid, 0,0);
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				
				cnet->RemoveClient(p->guid);

				// Connection lost normally
				//printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString(true));;
				break;

				//�¿ͻ�����
			case ID_NEW_INCOMING_CONNECTION:
				{
					// Somebody connected.  We have their IP now
					printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
					clientID=p->systemAddress; // Record the player ID of the client
					ClientInfo* info=new ClientInfo;
					if(!info){
						MessageBox(0,"�ڴ�����ʧ��!","error",0);
					}

					info->sendcount=0;			//0
					info->guid=p->guid;   //guid��ʶ
					info->addr=p->systemAddress; //ip��ַ
					info->worldMap=-1; //��ɫδ��½ 

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

			case ID_CONNECTION_LOST: //���Ӷ�ʧ.�ͻ��˷������Ͽ���.�����������ʱ���ղ����ͻ�����Ӧ������
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

				//��Ϣ�ص�
				//��Ϣ�ص�
				
				//cnet->m_pProcessRecvData(p->guid,(char*)p->data,p->length);
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				//		printf("%s\n", p->data);

				// Relay the message.  We prefix the name for other clients.  This demonstrates
				// That messages can be changed on the server before being broadcast
				// Sending is the same as before
				//ת����Ϣ
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
		//����ر�
		server->Shutdown(800);
		RakNetworkFactory::DestroyRakPeerInterface(server);
	}
	
}


//�����ʼ��
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

	//��ʼ����.���4������,30���������.
	bool b = server->Startup(2, 30, &socketDescriptor, 1 );

	//�������������.
	server->SetMaximumIncomingConnections(200);

	//�����ɹ�
	if (b){
		puts("�����������ɹ�,���ͻ�������200.\n �ȴ��ͻ�����.");
		IsStart=true;
	}
	else
	{ 
		IsStart=false;
		puts("����������ʧ��, ������ֹ.");
		exit(1);
	}

	//����ż��ping 
	server->SetOccasionalPing(true);

	//���ò��ɿ���ʱ
	server->SetUnreliableTimeout(1000);

	m_pProcessRecvData=pProcessRecvData;

	printf("����IP: %s\n", server->GetLocalIP(0));
	printf("����GUID: %s\n", server->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS).ToString());
	
	return b;
}


//�Ƴ���ɫ��Ϣlist
void MySOCKServer::RemoveClient(RakNetGUID guid)
{
	//�û�list���Ƴ�
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);
	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* p=(*list_oper);
		if(guid==p->guid) 
		{
			clientlist.erase(list_oper);//�����Ƴ�һ������
			delete p;//�ڴ����
			p=NULL;//�������Ұָ��
			break;
		}
	}
	//////---------------------------------------------------------------------------------------------///////////////
	LeaveCriticalSection(&g_cs_clientlist);


	////����msglist���Ƴ��Ը��û�Ҫ���͵���Ϣ
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

// ��ӿͻ�
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

	//1.�ڴ��ȡһ��smsg�ṹ
	//SMsg *s = (SMsg *)myObjectPool.malloc();   
	//2.SMsg*s=new SMsg;
	SMsg*s =new SMsg;
	
	//char* tem=new char[Length+1];
	if(s == NULL){
		MessageBox(0,"�ڴ�����ʧ��!SendMsg ","error",0);
	}
//	s->pData=tem;

	s->pData[0]=ID_USER_PACKET_ENUM; //�����ֽڹ̶�Ϊ�û���
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

//���·��Ϳͻ��İ������������´���.����Ҳ����ÿͻ�,�ͷ���-1 
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

	//δ�ҵ�����ͻ�,˵�������˳���.
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

//��ĳ�����ͼ�����û�������Ϣ
void MySOCKServer::SendMsgToAllInWorld(char * pData,unsigned long Length ,int worldMap)
{
	//////---------------------------------------------------------------------------------------------///////////////
	::EnterCriticalSection(&g_cs_clientlist);

	std::vector<ClientInfo*>::iterator  list_oper;
	for(list_oper=clientlist.begin();list_oper!=clientlist.end();++list_oper)
	{
		ClientInfo* c=(*list_oper);
		//��ͬһ����������
		if(c->worldMap==worldMap){
			SendMsg(c->guid,pData,Length);
		}

	}

	//////---------------------------------------------------------------------------------------------///////////////
	::LeaveCriticalSection(&g_cs_clientlist);
}

//���¿ͻ����ڵ������ͼid
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

//���¿ͻ����ڵ������ͼid
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
		//�ų�expect�ͻ�
		if(exceptID==c->ID) continue;

		//����ͬһ�������ͼ��.����
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
		//�ų�expect�ͻ�
		if(except==c->guid) continue;

		//����ͬһ�������ͼ��.����
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

	//�Ͽ�
	server->CloseConnection(clientID, true, 0);


	return false;
}




} // namespace Socket
