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



	dwStart = GetTickCount();  //得到开始时间
	attackStart=dwStart;

	//设置网络
	server=netser;


	yrot=90.0f;	//默认旋转-90度

	movespeed=sm->MoveSpeed;//移动速度

	//怪物的中心点
	center_pos=pp.pos;

	//------------------------------------------------------------------------------------------------------
	memset((char*)&prop,0,sizeof(MonsterProperty));
	memcpy((char*)&prop,(char*)&pp,sizeof(MonsterProperty));

	memcpy((char*)&m_info,(char*)sm,sizeof(SMonsterInfo));

	//prop.MAX_HP=1500;			//最大hp
	//prop.HP=1000;				//怪物当前hp
	//prop.Attack=20;				//攻击力
	//prop.Defense=5;				//防御力
	//prop.isDeath=false;			//是否死亡
	//prop.pos=prop.pos;				//初始化坐标
	//prop.rot=prop.rot; //旋转
	//prop.currState=prop.currState;	//默认角色是走
	//prop.target=vector3df(0,0,0);
	//prop.worldMap_id=worldmap; //怪物初始化在哪个世界地图

	isArrived=false;	//未到达目的地

	//不重生
	m_breborn=false;
	//-----------------------------------------------------------------------------------------------------




}
void MyMonster::SetCharacter(RoleProperty* role){
	this->role=role;
}
//设置模型旋转度,用于矫正模型不对应
void MyMonster::SetInitRotValue(float rot)
{
	yrot=rot;
}



//设置动作
void MyMonster::SetState(int s){

	//走
	if(s==cWalk && prop.currState!=cWalk){
		if(prop.currState==cWalk){
			return;
		}
		prop.currState=cWalk;

		//死亡
	}else if(s==cDeath && prop.currState!=cDeath){
		if(prop.currState==cDeath){
			return;
		}
		prop.currState=cDeath;

		//脚攻击
	}else if(s==cAttack && prop.currState!=cAttack){
		if(prop.currState==cAttack){
			return;
		}
		prop.currState=cAttack;

		//头攻击
	}else if(s==cAttack2 && prop.currState!=cAttack2){
		if(prop.currState==cAttack2){
			return;
		}
		prop.currState=cAttack2;

		//站
	}else if(s==cIdle && prop.currState!=cIdle){
		if(prop.currState==cIdle){
			return;
		}
		prop.currState=cIdle;

	}


}


void MyMonster::characterUnderAttack(int underattack)
{
	//无目标角色
	if(role==0) return;

	//角色已挂就不攻击他了
	if(role->isDeath) return;

	//角色还活着
	if(role->HP>0){
		//		
		//防御力大于攻击力,miss
		if(underattack<=role->Defense){
			//所有在线角色广播,该玩家被攻击但miss.
			role->cmd=CMD_S2C_USERUNDERATTACK;
			server->SendMsgToAllInWorld((char*)role,sizeof(RoleProperty),prop.worldMap_id);
			return;
		}
		int damage=underattack-role->Defense;
		//如果hp小于收到的伤害值.就直接hp为0.
		if(role->HP<damage){
			damage=role->HP;
			role->HP=0;
		}
		else{
			role->HP=role->HP-damage;	//hp减少
		}

		//所有在线角色广播,该玩家被攻击.更新hp显示.
		role->cmd=CMD_S2C_USERUNDERATTACK;

		printf("怪物,characterUnderAttack里,准备SendMsgToAll玩家被攻击\n");
		server->SendMsgToAllInWorld((char*)role,sizeof(RoleProperty), prop.worldMap_id);

		//统计hp百分比
		s32 per=100*((float)role->HP/(float)role->MAX_HP);
		printf("角色%s被攻击! hp:%d sock:%d per:%d\n",role->roleName,role->HP,role->sock,per);

	}

}

void MyMonster::attack()
{
	printf("attack\n");
	//自己已挂就不能动了
	if(prop.isDeath) return;


	attackdwEnd=GetTickCount();//得到结束时间
	DWORD usetime=attackdwEnd-attackStart;

	if(usetime>=1000)
	{
		attackStart=attackdwEnd;


		if(role->isDeath==false)
		{
			printf("怪物攻击角色:%s  HP:%d  sock:%d\n",role->roleName,role->HP,role->sock);
			//设置动作
			struct _timeb timebuffer;//定义一个结构
			_ftime(&timebuffer);//初始化timebuffer
			srand(timebuffer.millitm);//timebuffer.millitm获得毫秒
			int p=rand()%2;
			if(p==1)	SetState(cAttack);
			else  SetState(cAttack2);

			//角色被怪物攻击
			characterUnderAttack(prop.Attack);

		}else{
			printf("role %s HP:%d  sock:%d \n",role->roleName,role->HP,role->sock);
		}
	}

}


//怪物遭受攻击,参数是攻击值 socket是攻击者
int MyMonster::UnderAttack( int attack,SOCKET attacker)
{


	//攻击力小于防御力...
	if(attack<=prop.Defense){
		//怪物无受到伤害
		return 0;
	}

	int damage=attack-prop.Defense;
	//如果hp小于收到的伤害值.就直接hp为0.
	if(prop.HP<damage){
		damage=prop.HP;
		prop.HP=0;
	}
	else{
		prop.HP=prop.HP-damage;	//hp减少
	}

	//记录下攻击者..
	prop.killedByUser=attacker;
	prop.damage=damage;

	printf("怪物id:%d 受攻击%d 剩HP:%d\n",prop.ID,attack,prop.HP);


	if(prop.HP<=0){
		prop.isDeath=true;
		printf("怪物死亡isDeath:%d\n",prop.isDeath);
		SetState(cDeath);	//怪物死亡就设置标记和动作
	}

	return prop.damage;
}


void MyMonster::FaceToTarget(vector3df target_pos)
{
	//让怪物面向角色
	vector2df orient1;//怪物的目的向量
	float theta1 , theta2;
	//target_pos=m_puser->getPosition();//得到角色坐标
	core::vector2df v2,current_2dpos;//怪物的目的坐标和当前坐标
	v2.X=target_pos.X;	//怪物要面向的2d坐标
	v2.Y=target_pos.Z;	//Y高度不需要

	current_2dpos.X=prop.pos.X;//当前怪物2d坐标
	current_2dpos.Y=prop.pos.Z;

	//面向角色的向量
	orient1=v2-current_2dpos;
	orient1.normalize();	//向量归一化
	theta1 = getradian(this->orient.X,this->orient.Y);//分别计算出向量的夹角
	theta2 = getradian(orient1.X,orient1.Y);
	//当前怪物的旋转
	this->yrot += theta1 - theta2;//方法1
	//	this->yrot +=orient1.getAngleWith(orient);//方法2.使用getAngleWith也可以计算出两个向量的夹角,但不准确
	if(this->yrot >=360.0f)	this->yrot -= 360.0f;
	if(this->yrot <0.0f)	this->yrot += 360.0f;
	this->orient = orient1;

	prop.rot=vector3df(vector3df(0,yrot+180,0));
	//printf("度:%3.1f\r\n",theta1);
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
	//已到达目标点
	float distance =target_pos.getDistanceFrom(prop.pos);//得到怪物和角色的距离
	//printf("距离%3.1f\n",distance);

	//再判断距离是否在怪物的攻击距离(范围)内
	if(distance<m_info.Attack_distense ){
		ret=true;
	}else{
		ret=false;
	}

	SetState(cWalk);			

	//调整角度,面向target
	this->FaceToTarget(target_pos);

	return ret;
}

//玩家在怪物视野内?
bool MyMonster::CharacterIsInMonsterView()
{
	if(role==0) return false;
	//判断和角色距离,在攻击范围内,就攻击,否则就移动
	core::vector3df v2=role->pos;
	vector3df current_2dpos=prop.pos;
	float distance =v2.getDistanceFrom(current_2dpos);//得到怪物和角色的距离
	//printf("距离%3.1f\n",distance);

	//玩家在怪物视野内?
	if(distance< m_info.View_distense){
		printf("怪物发现玩家%s! %3.1f %ud\n",role->roleName,distance , m_info.View_distense);
		return true; //在视野内
	}else{
		printf("怪物未发现玩家%s! %3.1f %ud\n",role->roleName,distance, m_info.View_distense);
		return false;//不在
	}
}

//玩家在怪物攻击范围内?
bool MyMonster::CharacterIsInMonsterAttackRect()
{
	if(role==0) return false;
	//判断和角色距离,在攻击范围内,就攻击,否则就移动
	core::vector3df v2= role->pos;
	vector3df current_2dpos=prop.pos;
	float distance =v2.getDistanceFrom(current_2dpos);//得到怪物和角色的距离
	//printf("距离%3.1f\n",distance);
	if(distance<m_info.Attack_distense){
		printf("玩家%s在怪物攻击范围内 %3.1f %ud!\n",role->roleName,distance,m_info.Attack_distense);
		return true; //在视野内
	}else{
		printf("玩家%s不在怪物攻击范围内 %3.1f %ud!\n",role->roleName,distance,m_info.Attack_distense);
		return false;//不在	
	}
}


void MyMonster::RandMove()
{
	struct _timeb timebuffer;//定义一个结构
	_ftime(&timebuffer);//初始化timebuffer
	srand(timebuffer.millitm);//timebuffer.millitm获得毫秒


	//如果已经达到目标就再计算移动点
	if(isArrived)
	{

		int p=rand()%4; // 1/4的几率等于1 
		if(p!=1)
		{ //站立 3/4几率 
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


			//保存更新的坐标
		 prop.target=current_pos+vector3df(10-x,1,10-z);

		isArrived=false;
	}

	if(MoveTarget(prop.target)){
		//到达目标,
		//printf("到达目标,开始新坐标\n");
		isArrived=true;
	}
}

//返回true表示怪物超过了允许的最大移动范围
bool MyMonster::CheckMax_move_distense()
{
	bool ret=false;
	//当前坐标和怪物的中心点判断距离.是否超过怪物最大移动半径
	if( prop.pos.getDistanceFrom(center_pos)>m_info.MaxMove_distense){
		//超过了最大移动范围.就向中心点移动
		prop.target=center_pos;
		isArrived=false;
		//MessageBox(0,"怪物超过了允许的最大移动范围","",0);
		ret=true;
	}else{
		ret=false;
	}
	return ret;
}

//怪物重生
void MyMonster::reborn()
{
	//hp恢复最大
   prop.HP=prop.MAX_HP;

   //未死
   prop.isDeath=false;

   prop.killedByUser=0;

   prop.target=vector3df(0,0,0);

   //初始化到中心点
   prop.pos=center_pos;

   //重生标志关闭
   m_breborn=false;
}


void MyMonster::Update(){

	if(prop.isDeath )
	{
		//printf("怪物死亡!\n");
		SetState(cDeath);
		return ;
	}

	dwEnd=GetTickCount();//得到结束时间
	DWORD usetime=dwEnd-dwStart;

	if(usetime<500)
	{		

		return;
	}
	dwStart=dwEnd;

	//玩家挂了,
	if(role==0 || role->HP<=0)
	{
		RandMove();
		
		//如果怪物超过了最大移动范围,就让他移动到原来的中心点
		if(CheckMax_move_distense()){
			MoveTarget(prop.target);
			return;
		}


		return;
	}

	//如果怪物超过了最大移动范围,就让他移动到原来的中心点
	if(CheckMax_move_distense()){
		MoveTarget(prop.target);
		return;
	}


	//玩家在怪物视野内?
	if(CharacterIsInMonsterView()==false){
		//不在视野内,没发现,随机移动
		RandMove();
	}else{
		//在视野内,

		//玩家在怪物攻击范围内?
		if(CharacterIsInMonsterAttackRect()){
			FaceToTarget(role->pos);
			attack();
		}else{
			//不在攻击范围内,移动过去
			printf("玩家不在怪物攻击范围内!怪物向玩家移动\n");
			MoveTarget(role->pos);
		}
	}


}
