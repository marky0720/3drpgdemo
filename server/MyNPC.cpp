#include "StdAfx.h"
#include ".\mynpc.h"


//��ʼ��NPC
MyNPC::MyNPC(NPCProperty* npcporp,Socket::MySOCKServer* netser)
{
	this->netser=netser;

	shoppack=0;

	memcpy((char*)&npc_prop,(char*)npcporp,sizeof(RoleProperty));

	//��ʾ��npc�пͻ���������
	if(npc_prop.cmd!=-1){

       ShopPackage* sh=new ShopPackage;
		shoppack=sh;
		shoppack->NPC_ID=npc_prop.ID;
		shoppack->worldMap_id=npc_prop.worldMap_id;


	   //npc�̵��������
	   for(int i=0;i<4;i++){
		   for(int j=0;j<6;j++){
			   sh->itembox[i][j]=0; //����Ʒ
			   sh->itemParam1[i][j]=0; //��Ʒ����1����
			   sh->itemParam2[i][j]=0; //��Ʒ����2����
			   sh->itemParam3[i][j]=0; //��Ʒ����3����
			   sh->itemParam4[i][j]=0; //��Ʒ����4����
			   sh->itemParam5[i][j]=0; //��Ʒ����5����
			   sh->itemParam6[i][j]=0; //��Ʒ����6����
			   sh->itemPrice[i][j]=0;		   //�۸�
		   }
	   }

	   //�������ļ��ж�ȡ�̵���Ʒ��shoppack
	   ReadNPCShopConfig();


	}
}

MyNPC::~MyNPC(void)
{
}


void MyNPC::ReadNPCShopConfig()
{

		char File[255]="data//shop_npc_%d_%d.txt"; //npc�̵��ļ� �������� ��npc id
		sprintf(File,"data//shop_npc_%d_%d.txt",npc_prop.worldMap_id, npc_prop.ID);
		FILE* log;
		log = fopen(File, "r");
		//�ļ�������.����
		if (log == NULL)
		{
			printf("�ļ�%s������.����\n",File);
			return ;
		}	
		char strtemp[255]="";


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
					SItem item ;

					
					//��һ�����ȡ��ʽ�����̵���Ʒ����
					sscanf(strtemp, "%d  %d  %d  %d  %d  %d  %d", 
						&item.item_index,
						&item.itemParam1,&item.itemParam2,
						&item.itemParam3,&item.itemParam4,
						&item.itemParam5,&item.itemParam6
						
						); 

					//��Ʒ�۸����̵��ﶨ��.��ȫ����Ʒtxt��
					item.itemPrice=getItembyID(item.item_index)->itemPrice;

					printf("npc%d map:%d����Ʒ%d  %d  %d  %d  %d  %d  %d  %d\n",npc_prop.ID , npc_prop.worldMap_id,item.item_index,
						item.itemParam1,item.itemParam2,
						item.itemParam3,item.itemParam4,
						item.itemParam5,item.itemParam6,item.itemPrice);

					//1000����������
					if(item.item_index<1000){
						//npc�̵�����������������Բ����̵��ﶨ��.����item.txt��ͳһ���������� 
						item.itemParam2=getItemAttackById(item.item_index);
						if(item.itemParam1>0) item.itemParam2*=1.8; //param1�ǹ�����Ч��.�������ӱ�
					}else if(item.item_index>=1000 && item.item_index<2000){
						//����Ʒ
						//�ָ�ֵ������ͳһ�ļ��ﶨ��
						if(item.item_index==1000)  //������ҩ ��param4
						    item.itemParam1=getItembyID(item.item_index)->itemParam4;
						if(item.item_index==1001)  //����ħ��ҩ ��param5
							item.itemParam1=getItembyID(item.item_index)->itemParam5;
						  
					}

					
					PushItemInShopPack(item);


				

				
				}
			}
		}


}


//���̵�ո�װ��һ����Ʒ
void MyNPC::PushItemInShopPack(SItem item )
{

	//��һ���ո�װ����Ʒ
	for(int i=0;i<4;i++){
		for(int j=0;j<6;j++){
			if(shoppack->itembox[i][j]==0)  //����Ʒ
			{
					shoppack->itembox[i][j]=item.item_index;
					shoppack->itemParam1[i][j]=item.itemParam1;
					shoppack->itemParam2[i][j]=item.itemParam2;
					shoppack->itemParam3[i][j]=item.itemParam3;
					shoppack->itemParam4[i][j]=item.itemParam4;
					shoppack->itemParam5[i][j]=item.itemParam5;
					shoppack->itemParam6[i][j]=item.itemParam6;
					shoppack->itemPrice[i][j]=item.itemPrice;
					 
				return;
			}
		}
	}

}