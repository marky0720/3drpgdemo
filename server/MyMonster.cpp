#include "StdAfx.h"


#include "mymonster.h"


float MyMonster::getradian(float x,float y)
{
	static float EPSILON=0.0001f;
	if(fabs(x)<=EPSILON&&y>0)return 90.0f;
	if(fabs(x)<=EPSILON&&y<0)return 270.0f;
	if(fabs(y)<=EPSILON&&x>0)return 0.0f;
	if(fabs(y)<=EPSILON&&x<0)return 180.0f;

	float rel = asinf(fabsf(y));
	rel = rel*180.0f / PI;
	if(x>0&&y>0)return rel;
	if(x<0&&y>0)return 180.0f - rel;
	if(x<0&&y<0)return 180.0f + rel;
	if(x>0&&y<0)return 360.0f - rel;

	return 0;
}


MyMonster::~MyMonster(void)
{

}

MyMonster::MyMonster(MonsterProperty pp ,	SMonsterInfo* sm,	Socket::MySOCKServer* netser,int  worldmap)
{



	dwStart = GetTickCount();  //�õ���ʼʱ��
	attackStart=dwStart;

	//��������
	server=netser;


	yrot=90.0f;	//Ĭ����ת-90��

	movespeed=sm->MoveSpeed;//�ƶ��ٶ�

	//��������ĵ�
	center_pos=pp.pos;

	//------------------------------------------------------------------------------------------------------
	memset((char*)&prop,0,sizeof(MonsterProperty));
	memcpy((char*)&prop,(char*)&pp,sizeof(MonsterProperty));

	memcpy((char*)&m_info,(char*)sm,sizeof(SMonsterInfo));

	//prop.MAX_HP=1500;			//���hp
	//prop.HP=1000;				//���ﵱǰhp
	//prop.Attack=20;				//������
	//prop.Defense=5;				//������
	//prop.isDeath=false;			//�Ƿ�����
	//prop.pos=prop.pos;				//��ʼ������
	//prop.rot=prop.rot; //��ת
	//prop.currState=prop.currState;	//Ĭ�Ͻ�ɫ����
	//prop.target=vector3df(0,0,0);
	//prop.worldMap_id=worldmap; //�����ʼ�����ĸ������ͼ

	isArrived=false;	//δ����Ŀ�ĵ�

	//������
	m_breborn=false;
	//-----------------------------------------------------------------------------------------------------




}
void MyMonster::SetCharacter(RoleProperty* role){
	this->role=role;
}
//����ģ����ת��,���ڽ���ģ�Ͳ���Ӧ
void MyMonster::SetInitRotValue(float rot)
{
	yrot=rot;
}



//���ö���
void MyMonster::SetState(int s){

	//��
	if(s==cWalk && prop.currState!=cWalk){
		if(prop.currState==cWalk){
			return;
		}
		prop.currState=cWalk;

		//����
	}else if(s==cDeath && prop.currState!=cDeath){
		if(prop.currState==cDeath){
			return;
		}
		prop.currState=cDeath;

		//�Ź���
	}else if(s==cAttack && prop.currState!=cAttack){
		if(prop.currState==cAttack){
			return;
		}
		prop.currState=cAttack;

		//ͷ����
	}else if(s==cAttack2 && prop.currState!=cAttack2){
		if(prop.currState==cAttack2){
			return;
		}
		prop.currState=cAttack2;

		//վ
	}else if(s==cIdle && prop.currState!=cIdle){
		if(prop.currState==cIdle){
			return;
		}
		prop.currState=cIdle;

	}


}


void MyMonster::characterUnderAttack(int underattack)
{
	//��Ŀ���ɫ
	if(role==0) return;

	//��ɫ�ѹҾͲ���������
	if(role->isDeath) return;

	//��ɫ������
	if(role->HP>0){
		//		
		//���������ڹ�����,miss
		if(underattack<=role->Defense){
			//�������߽�ɫ�㲥,����ұ�������miss.
			role->cmd=CMD_S2C_USERUNDERATTACK;
			server->SendMsgToAllInWorld((char*)role,sizeof(RoleProperty),prop.worldMap_id);
			return;
		}
		int damage=underattack-role->Defense;
		//���hpС���յ����˺�ֵ.��ֱ��hpΪ0.
		if(role->HP<damage){
			damage=role->HP;
			role->HP=0;
		}
		else{
			role->HP=role->HP-damage;	//hp����
		}

		//�������߽�ɫ�㲥,����ұ�����.����hp��ʾ.
		role->cmd=CMD_S2C_USERUNDERATTACK;

		printf("����,characterUnderAttack��,׼��SendMsgToAll��ұ�����\n");
		server->SendMsgToAllInWorld((char*)role,sizeof(RoleProperty), prop.worldMap_id);

		//ͳ��hp�ٷֱ�
		s32 per=100*((float)role->HP/(float)role->MAX_HP);
		printf("��ɫ%s������! hp:%d sock:%d per:%d\n",role->roleName,role->HP,role->sock,per);

	}

}

void MyMonster::attack()
{
	printf("attack\n");
	//�Լ��ѹҾͲ��ܶ���
	if(prop.isDeath) return;


	attackdwEnd=GetTickCount();//�õ�����ʱ��
	DWORD usetime=attackdwEnd-attackStart;

	if(usetime>=1000)
	{
		attackStart=attackdwEnd;


		if(role->isDeath==false)
		{
			printf("���﹥����ɫ:%s  HP:%d  sock:%d\n",role->roleName,role->HP,role->sock);
			//���ö���
			struct _timeb timebuffer;//����һ���ṹ
			_ftime(&timebuffer);//��ʼ��timebuffer
			srand(timebuffer.millitm);//timebuffer.millitm��ú���
			int p=rand()%2;
			if(p==1)	SetState(cAttack);
			else  SetState(cAttack2);

			//��ɫ�����﹥��
			characterUnderAttack(prop.Attack);

		}else{
			printf("role %s HP:%d  sock:%d \n",role->roleName,role->HP,role->sock);
		}
	}

}


//�������ܹ���,�����ǹ���ֵ socket�ǹ�����
int MyMonster::UnderAttack( int attack,SOCKET attacker)
{


	//������С�ڷ�����...
	if(attack<=prop.Defense){
		//�������ܵ��˺�
		return 0;
	}

	int damage=attack-prop.Defense;
	//���hpС���յ����˺�ֵ.��ֱ��hpΪ0.
	if(prop.HP<damage){
		damage=prop.HP;
		prop.HP=0;
	}
	else{
		prop.HP=prop.HP-damage;	//hp����
	}

	//��¼�¹�����..
	prop.killedByUser=attacker;
	prop.damage=damage;

	printf("����id:%d �ܹ���%d ʣHP:%d\n",prop.ID,attack,prop.HP);


	if(prop.HP<=0){
		prop.isDeath=true;
		printf("��������isDeath:%d\n",prop.isDeath);
		SetState(cDeath);	//�������������ñ�ǺͶ���
	}

	return prop.damage;
}


void MyMonster::FaceToTarget(vector3df target_pos)
{
	//�ù��������ɫ
	vector2df orient1;//�����Ŀ������
	float theta1 , theta2;
	//target_pos=m_puser->getPosition();//�õ���ɫ����
	core::vector2df v2,current_2dpos;//�����Ŀ������͵�ǰ����
	v2.X=target_pos.X;	//����Ҫ�����2d����
	v2.Y=target_pos.Z;	//Y�߶Ȳ���Ҫ

	current_2dpos.X=prop.pos.X;//��ǰ����2d����
	current_2dpos.Y=prop.pos.Z;

	//�����ɫ������
	orient1=v2-current_2dpos;
	orient1.normalize();	//������һ��
	theta1 = getradian(this->orient.X,this->orient.Y);//�ֱ����������ļн�
	theta2 = getradian(orient1.X,orient1.Y);
	//��ǰ�������ת
	this->yrot += theta1 - theta2;//����1
	//	this->yrot +=orient1.getAngleWith(orient);//����2.ʹ��getAngleWithҲ���Լ�������������ļн�,����׼ȷ
	if(this->yrot >=360.0f)	this->yrot -= 360.0f;
	if(this->yrot <0.0f)	this->yrot += 360.0f;
	this->orient = orient1;

	prop.rot=vector3df(vector3df(0,yrot+180,0));
	//printf("��:%3.1f\r\n",theta1);
}

bool MyMonster::MoveTarget(vector3df target_pos)
{
	bool ret;
	prop.pos=prop.pos;
	if(prop.pos.X<(int)target_pos.X){
		prop.pos.X+=movespeed;
	}else if(prop.pos.X>(int)target_pos.X){
		prop.pos.X-=movespeed;
	}
	if(prop.pos.Z<(int)target_pos.Z){
		prop.pos.Z+=movespeed;
	}else if(prop.pos.Z>(int)target_pos.Z){
		prop.pos.Z-=movespeed;
	}
	prop.pos=prop.pos;
	//�ѵ���Ŀ���
	float distance =target_pos.getDistanceFrom(prop.pos);//�õ�����ͽ�ɫ�ľ���
	//printf("����%3.1f\n",distance);

	//���жϾ����Ƿ��ڹ���Ĺ�������(��Χ)��
	if(distance<m_info.Attack_distense ){
		ret=true;
	}else{
		ret=false;
	}

	SetState(cWalk);			

	//�����Ƕ�,����target
	this->FaceToTarget(target_pos);

	return ret;
}

//����ڹ�����Ұ��?
bool MyMonster::CharacterIsInMonsterView()
{
	if(role==0) return false;
	//�жϺͽ�ɫ����,�ڹ�����Χ��,�͹���,������ƶ�
	core::vector3df v2=role->pos;
	vector3df current_2dpos=prop.pos;
	float distance =v2.getDistanceFrom(current_2dpos);//�õ�����ͽ�ɫ�ľ���
	//printf("����%3.1f\n",distance);

	//����ڹ�����Ұ��?
	if(distance< m_info.View_distense){
		printf("���﷢�����%s! %3.1f %ud\n",role->roleName,distance , m_info.View_distense);
		return true; //����Ұ��
	}else{
		printf("����δ�������%s! %3.1f %ud\n",role->roleName,distance, m_info.View_distense);
		return false;//����
	}
}

//����ڹ��﹥����Χ��?
bool MyMonster::CharacterIsInMonsterAttackRect()
{
	if(role==0) return false;
	//�жϺͽ�ɫ����,�ڹ�����Χ��,�͹���,������ƶ�
	core::vector3df v2= role->pos;
	vector3df current_2dpos=prop.pos;
	float distance =v2.getDistanceFrom(current_2dpos);//�õ�����ͽ�ɫ�ľ���
	//printf("����%3.1f\n",distance);
	if(distance<m_info.Attack_distense){
		printf("���%s�ڹ��﹥����Χ�� %3.1f %ud!\n",role->roleName,distance,m_info.Attack_distense);
		return true; //����Ұ��
	}else{
		printf("���%s���ڹ��﹥����Χ�� %3.1f %ud!\n",role->roleName,distance,m_info.Attack_distense);
		return false;//����	
	}
}


void MyMonster::RandMove()
{
	struct _timeb timebuffer;//����һ���ṹ
	_ftime(&timebuffer);//��ʼ��timebuffer
	srand(timebuffer.millitm);//timebuffer.millitm��ú���


	//����Ѿ��ﵽĿ����ټ����ƶ���
	if(isArrived)
	{

		int p=rand()%4; // 1/4�ļ��ʵ���1 
		if(p!=1)
		{ //վ�� 3/4���� 
			SetState(cIdle);
			// lasttime=timer.GetElapsedTime();
			return;
		}


		int RANGE_MIN = 0;
		int RANGE_MAX = 10;

		float x;
		int rand100 = (((double) rand() /(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
		x=rand100; 

		float z;
		rand100 = (((double) rand()/(double) RAND_MAX) * RANGE_MAX + RANGE_MIN);
		z=rand100;


		//printf( "%x %3.1f %3.1f \n",this, x,z);
		vector3df current_pos=prop.pos;


			//������µ�����
		 prop.target=current_pos+vector3df(10-x,1,10-z);

		isArrived=false;
	}

	if(MoveTarget(prop.target)){
		//����Ŀ��,
		//printf("����Ŀ��,��ʼ������\n");
		isArrived=true;
	}
}

//����true��ʾ���ﳬ�������������ƶ���Χ
bool MyMonster::CheckMax_move_distense()
{
	bool ret=false;
	//��ǰ����͹�������ĵ��жϾ���.�Ƿ񳬹���������ƶ��뾶
	if( prop.pos.getDistanceFrom(center_pos)>m_info.MaxMove_distense){
		//����������ƶ���Χ.�������ĵ��ƶ�
		prop.target=center_pos;
		isArrived=false;
		//MessageBox(0,"���ﳬ�������������ƶ���Χ","",0);
		ret=true;
	}else{
		ret=false;
	}
	return ret;
}

//��������
void MyMonster::reborn()
{
	//hp�ָ����
   prop.HP=prop.MAX_HP;

   //δ��
   prop.isDeath=false;

   prop.killedByUser=0;

   prop.target=vector3df(0,0,0);

   //��ʼ�������ĵ�
   prop.pos=center_pos;

   //������־�ر�
   m_breborn=false;
}


void MyMonster::Update(){

	if(prop.isDeath )
	{
		//printf("��������!\n");
		SetState(cDeath);
		return ;
	}

	dwEnd=GetTickCount();//�õ�����ʱ��
	DWORD usetime=dwEnd-dwStart;

	if(usetime<500)
	{		

		return;
	}
	dwStart=dwEnd;

	//��ҹ���,
	if(role==0 || role->HP<=0)
	{
		RandMove();
		
		//������ﳬ��������ƶ���Χ,�������ƶ���ԭ�������ĵ�
		if(CheckMax_move_distense()){
			MoveTarget(prop.target);
			return;
		}


		return;
	}

	//������ﳬ��������ƶ���Χ,�������ƶ���ԭ�������ĵ�
	if(CheckMax_move_distense()){
		MoveTarget(prop.target);
		return;
	}


	//����ڹ�����Ұ��?
	if(CharacterIsInMonsterView()==false){
		//������Ұ��,û����,����ƶ�
		RandMove();
	}else{
		//����Ұ��,

		//����ڹ��﹥����Χ��?
		if(CharacterIsInMonsterAttackRect()){
			FaceToTarget(role->pos);
			attack();
		}else{
			//���ڹ�����Χ��,�ƶ���ȥ
			printf("��Ҳ��ڹ��﹥����Χ��!����������ƶ�\n");
			MoveTarget(role->pos);
		}
	}


}
