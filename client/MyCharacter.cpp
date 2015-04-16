#include "StdAfx.h"
#include ".\mycharacter.h"

#include "MycharAttackNumTextShow.h"



MyCharacter::MyCharacter( RoleProperty p,bool isMaincharacter,MySceneLoader* sceneloader )
{
	//场景加载器 此角色用他得到地形高度
	this->sceneloader=sceneloader;
	
	//网络
	cnet=0;

	ball=0;


	m_pMagic_Mgr=0;

	//是否是主角色
	isMain=isMaincharacter; 


	
	//引擎对象
	dev=getIrrDevice();
	smgr=getIrrSmgr();

	//-----------------------------------------------------------------------------------------------------
	//角色属性
	memset((char*)&property,0,sizeof(RoleProperty));
	memcpy((char*)&property,(char*)&p,sizeof(RoleProperty));


	if(property.charSex==1) printf("====character 是男角色 isDeath:%d sock:%d \n",property.isDeath,property.sock);
	if(property.charSex==2) printf("====character 是女角色 isDeath:%d sock:%d \n",property.isDeath,property.sock);

	//-----------------------------------------------------------------------------------------------------
	//包裹清零
	for(int i=0;i<4;i++){
		for(int j=0;j<6;j++){
			package.itembox[i][j]=0; //空物品
			package.itemParam1[i][j]=0; //物品属性1清零
			package.itemParam2[i][j]=0; //物品属性2清零
			package.itemParam3[i][j]=0; //物品属性3清零
			package.itemParam4[i][j]=0; //物品属性4清零
			package.itemParam5[i][j]=0; //物品属性5清零
			package.itemParam6[i][j]=0; //物品属性6清零
		}
	}

	//----------------------------------------------------------------------------------------------------------
	//角色参数初始化

	monster=0; //指向的怪物

	isfollow=false; //未有攻击目标

	isDieAnimation=false; 


	//非主角色需要的属性
	runstate=false;
	//非主角色需要的属性
	lastRunmode=true;

	//攻击类型
	curAttackType = MyCharacter::eAttacktype::MagicAttack;  

	//初始化目的地到达
	isArrived=true;



	//-----------------------------------------------------------------------------------------------------
	//初始化定时器
	timer=dev->getTimer();
	lasttime=timer->getRealTime();
	//------------------------------------------------------------------------------------------------------------


}

//真正的加载模型
void MyCharacter::Draw()
{


	//----------------------------------------------------------------------------------------------------------
	//角色模型加载
	
	m_charClothMgr=new MyCharClothMgr(property.sock,property.charSex);
	if(property.charSex==2){  //女
		m_charClothMgr->LoadBody(1); 
	}else if(property.charSex==1){ //男
		
		m_charClothMgr->LoadBody(3);
	 
	}

	m_charClothMgr->setPosition(property.pos);


	m_pMagic_Mgr=new Magic::MyCharMagicEffectMgr;
 	 
 
	//初始化站立 false表示不检查状态 强制设置站立
	idle(false);

	//初始化武器类
	m_weapon_mgr=new MyWeaponMgr(m_charClothMgr->getRootNode());

	

	//武器栏清空,即初始化角色手上不拿武器
	this->setCurWeapon(0,0);

	//----------------------------------------------------------------------------------------------------------
	//初始化血条
	m_pgHealthBar =new irr::scene::VxHealthSceneNode( 
		this->m_charClothMgr->getRootNode(), // parent node 
		smgr, // scene manager 
		-1, // id 
		smgr->getSceneCollisionManager(), // collision manager 
		50, // width 
		6, // height 
		core::vector3df(0,84,-5), // position 
		video::SColor(150,0,0,200), // bar color 
		video::SColor(150,220,0,0), // background color 
		video::SColor(255,255,255,255) ); // boarder color 
	//set percent of health 
	m_pgHealthBar->setProgress( 100 );

 
//------------------------------------------------------------------------------------------------------------

	mouseCurCharacter=false;

	
}


//用户喊话
void MyCharacter::PostMsg(const wchar_t* msg)
{
	MyCharHeaderMsg m(m_charClothMgr->getRootNode(),msg);
}

void MyCharacter::moveto(irr::scene::ISceneNode *node, //node to move 
             irr::core::vector3df vel) //velocity vector 
{ 
     irr::core::matrix4 m; 
	 vector3df rot=node->getRotation();
	 rot.Y+=180;
     m.setRotationDegrees(rot); 
     m.transformVect(vel); 
	 vector3df movepos=node->getPosition() + vel;


	 ////判断角色是否可移动到pos
	 if(isMain){
		 if(this->sceneloader->canmovehere(movepos) ){ 
			 movepos.Y=this->sceneloader->getHeight(movepos.X,movepos.Z);
			 node->setPosition(movepos); 
			 node->updateAbsolutePosition(); 
		 }else{
			 //停止移动
			stop();
		 }
	 }else{
		 movepos.Y=this->sceneloader->getHeight(movepos.X,movepos.Z);
		 node->setPosition(movepos); 
		 node->updateAbsolutePosition(); 
	 }
	
    
 }

//
core::vector3df MyCharacter::faceTarget(irr::core::vector3df targetpos) 
{ 
	vector3df nodepos=m_charClothMgr->getPosition();
	core::vector3df posDiff = targetpos - nodepos; 
	f32 degree = nodepos.Y; //keep current rotation if nothing to do 
	posDiff.normalize(); 
	  
	if (posDiff.X != 0.0f || posDiff.Z != 0.0f) 
		degree = atan2(posDiff.X,posDiff.Z) * core::RADTODEG; 

	m_charClothMgr->setRotation( core::vector3df(0,degree+180,0) ); 

	return core::vector3df(0,degree,0); 
}




void MyCharacter::UIRender(void)
{
	//在线的其他角色显示血条 前提是鼠标指向了角色模型
	if(!isMain && m_pgHealthBar && mouseCurCharacter) m_pgHealthBar->render(); 

	//if(char_textnode)
	{

	

		 vector2d<s32>	TextShowPos;
		 //将角色的3d坐标转换为2d屏幕坐标 .而且要在角色头上一点.
		 TextShowPos= smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(getPosition()+vector3df(0,19,0));

		  vector2d<s32>	 centerpos;
		centerpos=TextShowPos;
	
		centerpos.X-=40; 
		centerpos.Y-=3; //图片上靠一点

		 //黑色图片
		getIrrDriver()->draw2DImage(getIrrDriver()->getTexture("ui/black.tbj"),
			centerpos, //这个参数是指定绘制的坐标 即定位左上角位置绘制图片
			rect<s32>(0,0,90,17),0, //这个是在源图象截取图片要显示的区域 即截出图片的大小
			SColor(100,255,255,255),true);

		
		//显示名称
		TextCenter(property.roleName,TextShowPos,SColor(1,255,255,255),12,0);

	}

}


MyCharacter::~MyCharacter(void)
{
	
}
 
  
void MyCharacter::setState(int newState) 
{ 
         property.currState = newState; 
} 
 
void MyCharacter::setPosition(core::vector3df pos) 
{ 
		 m_charClothMgr->setPosition(pos);
} 
  
void MyCharacter::setRotate(core::vector3df rot) 
{ 
         m_charClothMgr->setRotation(rot); 
} 
  
core::vector3df MyCharacter::getPosition() 
{ 
         return m_charClothMgr->getPosition(); 
} 
  
core::vector3df MyCharacter::getRotate() 
{ 
         return m_charClothMgr->getRotation(); 
} 
  
int MyCharacter::getState() 
{ 
         return property.currState; 
} 

void MyCharacter::idle(bool checkState)
{  
	 if(checkState){
		 if(getState()== cIdle ) return;
	 }
	   setState(cIdle); 
		isArrived=true; //不准移动
	   m_charClothMgr->Animate(ANIMATION_SPEED-6,21,29,true);

}

void MyCharacter::readyAttack(bool checkState)
{  	 
	if(checkState){
		 if(getState()== cReadyAttack ) return;
		
	 } 
	setState(cReadyAttack); 

       m_charClothMgr->Animate(ANIMATION_SPEED-6,41,49,true);

}

//只执行一次死亡动作..
void MyCharacter::OneTimeDie()
{
	if(isDieAnimation==false) {

		m_charClothMgr->Animate(ANIMATION_SPEED,91,99,false);
		
	}

	isDieAnimation=true;
}


//参数是否需要先检查状态
void MyCharacter::run(bool checkState) 
{ 
	 if(checkState){
		 if(getState()== cRun ) return;

	 }
	 setState(cRun); 
		 
		 //跑?走
		 if(property.RunMode){
			     fSpeed = 0.7f; //移动速度
				 m_charClothMgr->Animate(ANIMATION_SPEED+2,61,79,true);

		 }else{
			     fSpeed = 0.350f; //速度
				  m_charClothMgr->Animate(ANIMATION_SPEED+1,1,20,true);
		 }

} 
  

irr::scene::IAnimatedMeshSceneNode* MyCharacter::getNode() 
{ 
         return m_charClothMgr->getRootNode();     
} 

 //攻击预备和站立动作 切换
void MyCharacter::ChangeState()
{
	
    if(getState()==  cIdle )
	{
	  	readyAttack(false);
	}else if(getState()==  cReadyAttack ){
	    idle(false);
	}

	//刚角色新数据发送给服务器
	 ReoprtToServer();
}

void MyCharacter::ChangeRunMode()
{

    property.RunMode=!property.RunMode; //模式变换

	//已经在跑或走中了
	if(getState() == cRun){
	  	run(false);//不检查状态直接切换动作
	}

}

 //怪物是否在角色攻击范围内
bool MyCharacter::getDistenceFromMonster(ISceneNode* monster)
{
	 if(monster==0) {
		 setState(cIdle);
		 isfollow=false;
		 return false;
	 }
   //ASSERT(monster);

	 vector3df player=m_charClothMgr->getAbsolutePosition();
	 vector3df target=monster->getAbsolutePosition();
	f32 length= player.getDistanceFrom(target);
	if(length<12) {
		//printf("距离:%3.1f\n",length);
		return true; //在20已内,即在范围内,返回真 
	}
	return false;
}

void MyCharacter::ShowAttackTextNum(int num)
{
	 MyCharAttackNumTextShow shownumtext(m_charClothMgr->getPosition(),num);

}

//角色遭受攻击,参数是遭受攻击值
void MyCharacter::UnderAttack( int damage)	
{
    //已挂就不能动了
    if(property.isDeath) return;

	
		//显示被攻击了多少血,0的话就显示miss
		MyCharAttackNumTextShow shownumtext(m_charClothMgr->getPosition(),damage);
		

		property.HP=property.HP-(damage);	//hp减少
		//统计hp百分比
		s32 per=100*((float)property.HP/(float)property.MAX_HP);
		printf("role被攻击! hp:%d per:%d\n",property.HP,per);

		
		//设置血条进度
		m_pgHealthBar->setProgress(per);
		if(property.HP<=0){
			property.HP=0;
			property.isDeath=true;
			printf("role死亡!\n");
			
		}

}


 //跟踪怪物并攻击
void MyCharacter::FollowAndAttackTarget()
{
     //已挂就不能动了
	  if(property.isDeath) return;
 
	  //指向的怪物指针空,则不攻击
	  if(monster==0) {
		  isfollow=false;
		  idle(true);
		  return;
	  }
	  //指向的怪物死了,则不攻击
	  if(monster->IsDead()){

		  //标志的怪物指针指向0
		  isfollow=false;
		  idle(true);
		  monster=0;
		  return ;
	  }

   //先判断距离,在内就攻击,否则就继续移动
	 if(getDistenceFromMonster(monster->GetNode()))
	{
		isArrived=true;
		//printf("距离内,攻击!\n");
		attack(monster->GetNode()->getPosition() , curAttackType);
		isfollow=false;
	}else{
		//printf("距离不够,继续移动\n");
	    Beginmove(monster->GetNode()->getAbsolutePosition());
		move();
		isfollow=true;
	}

	//攻击完后检查怪物是否死亡.
	if(monster->IsDead()){
		
		//标志的怪物指针指向0
		monster=0;

		isfollow=false;

		//printf("攻击停止!怪物已死\n");
		
		idle(true);
		//角色状态改变,报告给服务器
		ReoprtToServer();
	}
}


void MyCharacter::attack(vector3df attackpos,eAttacktype attackType) 
{ 
	  //已挂就不能动了
	  if(property.isDeath) return;

	  if(monster==0) {
		 readyAttack(true);
		  isfollow=false;
		  return;
	  }

	  setState( cAttack);

	  	 //用时
		u32 usetime=timer->getRealTime()-lastattacktime;
		if(usetime<800) {
			return;
		}
		else lastattacktime=timer->getRealTime();
        

		 //面向目标
		 faceTarget(attackpos); 

         switch (attackType) 
         { 
                 case eAttacktype::PowerAttack : 
                 {        
					 m_charClothMgr->Animate(ANIMATION_SPEED,102,120,true);
					 break;
                 }        
  
                 case eAttacktype::MagicAttack: 
                 {       
					 m_charClothMgr->Animate(ANIMATION_SPEED+2,120,128,true);
					 break;
                 }
  
         } 

		 

		 //及时更新状态到服务器   
		 ReoprtToServer();


		 //怪物遭受角色攻击, property.Attack是角色攻击力
		if(monster) monster->UnderAttack( property.Attack+package.weaponparam2);

}



void MyCharacter::Onlyupdate() 
{

	if(ball) ball->update();
	if(m_pMagic_Mgr) m_pMagic_Mgr->update();


	 //角色的状态更新武器显示  比如站着,就把手上的武器隐藏,背上的显示
	 m_weapon_mgr->update(getState());
	//UpdateByProperty();

	 if(property.target.X==0 &&property.target.Y==0 &&property.target.Z==0)
	  isArrived=true;

	if(!isArrived){
		//已挂就不能动了
		if(property.isDeath) return;

		property.pos=m_charClothMgr->getPosition();

		vector2df p1;
		p1.X=m_charClothMgr->getPosition().X;
		p1.Y=m_charClothMgr->getPosition().Z;

		vector2df p2;
		p2.X=property.target.X;
		p2.Y=property.target.Z;

		//已到达
		//f32 len=m_charClothMgr->getPosition().getDistanceFrom(	property.target) ;
		f32 len=p1.getDistanceFrom(p2);

		if( len< 4){
			m_charClothMgr->setPosition(property.target);
			property.pos=property.target;

			isArrived=true;

			property.target=vector3df(0,0,0);
			idle(false); 
			if(isMain==false) printf("其他角色已到达!\n");
		} 
		else 
		{ 
			// printf("距离:%3.1f\n",len);
			//未到达
			isArrived=false;
			faceTarget(	property.target); 
			if(isMain==false) printf("其他角色run!\n");
			run(true);
			moveto(m_charClothMgr->getRootNode(), core::vector3df(0,0,fSpeed)); 
		} 

		return;
	}



}


void MyCharacter::update() 
{

	  //角色的状态更新武器显示  比如站着,就把手上的武器隐藏,背上的显示
     m_weapon_mgr->update(getState());
	

	 if(ball) ball->update();
	 if(m_pMagic_Mgr) m_pMagic_Mgr->update();

	 

	 //角色是否已挂
	 if( property.isDeath || property.HP<=0){
		// printf("只执行一次死亡倒地动作\n");
		 OneTimeDie();
		 return ;
	 }


	 //主角色攻击.如果怪物已死亡.或无目标.停止攻击
	 if(isMain && monster==NULL){
			int s=getState();		  
			if(s==cAttack){
				idle(true);
			}
		 
	 }
		       //Do nothing 
	 //未到达目的地,继续移动
	  if(!isArrived){
			//printf("char移动!\n");
		    move();
			
			return;
	  }
	
	  if(monster==0) {
		 // //MessageBox(0,"monster000","",0);
		  return;
	  }

	 //在追踪中.. 
		if(isfollow){ 
			//printf("follow\n");
			FollowAndAttackTarget(); return;
		}
			 //在攻击中
	    if(getState() ==  cAttack){
			//printf("getState Attack\n");
	     FollowAndAttackTarget();
		 return;
	   }



	
} 


void MyCharacter::CancelAttack()
{
	 //关闭追踪
     isfollow=false;
}


void MyCharacter::Beginmove(core::vector3df pos)
{
	 isArrived=false;
     property.target=pos;

	 //每一次变更跑步就发送一个信息给服务器
	 ReoprtToServer();
}


void MyCharacter::move() 
{ 

	 //已挂就不能动了
	  if(property.isDeath) return;

	  property.pos=m_charClothMgr->getPosition();

  
		 //已到达
	 //    f32 len=m_charClothMgr->getPosition().getDistanceFromSQ(	property.target) ;
		 vector2df p1;
		 p1.X=m_charClothMgr->getPosition().X;
		 p1.Y=m_charClothMgr->getPosition().Z;

		 vector2df p2;
		 p2.X=property.target.X;
		 p2.Y=property.target.Z;

		 //已到达
		 //f32 len=m_charClothMgr->getPosition().getDistanceFrom(	property.target) ;
		 f32 len=p1.getDistanceFrom(p2);

		  if(isfollow && len< 12){
				
			    isArrived=true;
				
				property.target=vector3df(0,0,0);
                idle(false); 

		 }
		 else    if (len< 2.0 ) 
         { 
			 m_charClothMgr->setPosition(property.target);
			 property.pos=property.target;

			    isArrived=true;
				
				property.target=vector3df(0,0,0);
                idle(false); 
				setState(cIdle); 

         } 
         else 
         { 
			// printf("距离:%3.1f\n",len);
			    //未到达
				isArrived=false;
                faceTarget(	property.target); 

				run(true);
                moveto(m_charClothMgr->getRootNode(), core::vector3df(0,0,fSpeed)); 
         } 
   
} 


void MyCharacter::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{
	 //角色已挂?
	 if(property.isDeath){
		//printf("已挂!@");
	 }

}



void MyCharacter::drop()
{
	if(m_pMagic_Mgr){
		m_pMagic_Mgr->drop();
		delete m_pMagic_Mgr;
		m_pMagic_Mgr=0;
	}


	if(ball){
		ball->drop();
		delete ball;
		ball=0;
	}


	 m_charClothMgr->DropAll();

}


//角色根据属性立即更新
void MyCharacter::UpdateByProperty()
{

	//更新hp显示
	//统计hp百分比
	s32 per=100*((float)property.HP/(float)property.MAX_HP);
	//设置血条进度
	m_pgHealthBar->setProgress(per);

	if(property.HP<=0 || property.isDeath){
		//只执行一次死亡倒地动作
		OneTimeDie();
		return;
	}

	m_charClothMgr->setPosition(property.pos);
	m_charClothMgr->setRotation(property.rot);

	if(property.currState==cRun){
 
		isArrived=false;
		laststate=cRun;
		setRunAnimation();

	}else if(property.currState== cIdle ){
		laststate=cIdle;
		runstate=false;
		// printf("角色停止移动,state:%d\n",property.currState);
		property.currState=cRun;
		idle(true); 
	}else if(property.currState== cReadyAttack ){
		laststate=cReadyAttack;
		runstate=false;
		//  printf("角色停止移动,state:%d\n",property.currState);
		readyAttack(false); 
	}else if(property.currState== cAttack){

		runstate=false;
		//  printf("角色攻击,state:%d\n",property.currState);
		if(laststate==cAttack)return;

		m_charClothMgr->Animate(ANIMATION_SPEED,81,90,true);

		laststate=cAttack;
	}

}


void MyCharacter::ReoprtToServer( )
{
	if(cnet==0 ){
		 
		return ;
	}
	 

	 
	property.pos=getPosition();
	property.rot=m_charClothMgr->getRotation();
	 
	property.target=getTarget();
  

	property.cmd=CMD_UserUPDATE;
	//printf("sendsize:%d",sizeof(RoleProperty));
	cnet->SendMsg((char*)&property,sizeof(RoleProperty));

}


//捡地面上掉落物品,或金币
void MyCharacter::CollectDropItem()
{
	RoleProperty r;
	r.cmd=CMD_C2S_USERCollectItem; //请求捡物品

	cnet->SendMsg((char*)&r,sizeof(RoleProperty));
}



//角色加属性点.向服务器发送
void MyCharacter::AddPoint(int proptype)
{
	if(cnet==0 ){
		 
		return ;
	}

	RoleProperty p;
	memset((char*)&p,'\0',sizeof(RoleProperty));
	memcpy((char*)&p,(char*)&property,sizeof(RoleProperty));
	p.cmd=CMD_C2S_Request_AddPoint;        //请求加点


	//没可用属性点,返回
	if(p.usePoint<=0){
		return;
	}

	if(proptype==1){
		p.Power_Point++;
	}else if(proptype==2){
		p.Minjie_Point++;
	}else if(proptype==3){
		p.Tili_Point++;
	}

	cnet->SendMsg((char*)&p,sizeof(RoleProperty));
}



//只更新角色属性面版属性.
void MyCharacter::setPropPlane(RoleProperty* rr)
{
	property.Exp=rr->Exp;
	property.NextLevelExp=rr->NextLevelExp;
	property.Level=rr->Level;
	property.HP=rr->HP;
	property.MAX_HP=rr->MAX_HP;
	property.Minjie_Point=rr->Minjie_Point;
	property.Tili_Point=rr->Tili_Point;
	property.Power_Point=rr->Power_Point;
	property.usePoint=rr->usePoint;
	property.Attack=rr->Attack;
	property.Defense=rr->Defense;

}



void MyCharacter::OnEvent(const irr::SEvent & e)
{	
	
	//键盘事件
	if (e.EventType == EET_KEY_INPUT_EVENT) {
 
		   if ( 
		   //按X 切换 站立 攻击准备模式
			if (e.KeyInput.Key == KEY_KEY_X && e.KeyInput.PressedDown==false) { 
			   ChangeState();
		    }
		   
			//按R 切换跑步走路 模式
			if (e.KeyInput.Key == KEY_KEY_R && e.KeyInput.PressedDown==false) { 
				ChangeRunMode();
		    }

			//按空格,捡物品
			if (e.KeyInput.Key == KEY_SPACE && e.KeyInput.PressedDown==false) { 
				CollectDropItem();
			}


			//按Q 使用包裹里的MP恢复药
			if (e.KeyInput.Key == KEY_KEY_Q && e.KeyInput.PressedDown==false) { 

				//请求使用装备包裹里的消耗品
				property.cmd=CMD_C2S_Request_UsePackageHPItem;
				
				cnet->SendMsg((char*)&property,sizeof(RoleProperty));
			}


	}
	
}


//设置世界地图
void MyCharacter::SetWorldMap(int worldMap)
{
  property.worldMap_id=worldMap;
}

