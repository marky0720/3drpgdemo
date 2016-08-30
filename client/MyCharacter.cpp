#include "StdAfx.h"
#include ".\mycharacter.h"

#include "MycharAttackNumTextShow.h"



MyCharacter::MyCharacter( RoleProperty p,bool isMaincharacter,MySceneLoader* sceneloader )
{
	//���������� �˽�ɫ�����õ����θ߶�
	this->sceneloader=sceneloader;
	
	//����
	cnet=0;

	ball=0;


	m_pMagic_Mgr=0;

	//�Ƿ�������ɫ
	isMain=isMaincharacter; 


	
	//�������
	dev=getIrrDevice();
	smgr=getIrrSmgr();

	//-----------------------------------------------------------------------------------------------------
	//��ɫ����
	memset((char*)&property,0,sizeof(RoleProperty));
	memcpy((char*)&property,(char*)&p,sizeof(RoleProperty));


	if(property.charSex==1) printf("====character ���н�ɫ isDeath:%d sock:%d \n",property.isDeath,property.sock);
	if(property.charSex==2) printf("====character ��Ů��ɫ isDeath:%d sock:%d \n",property.isDeath,property.sock);

	//-----------------------------------------------------------------------------------------------------
	//��������
	for(int i=0;i<4;i++){
		for(int j=0;j<6;j++){
			package.itembox[i][j]=0; //����Ʒ
			package.itemParam1[i][j]=0; //��Ʒ����1����
			package.itemParam2[i][j]=0; //��Ʒ����2����
			package.itemParam3[i][j]=0; //��Ʒ����3����
			package.itemParam4[i][j]=0; //��Ʒ����4����
			package.itemParam5[i][j]=0; //��Ʒ����5����
			package.itemParam6[i][j]=0; //��Ʒ����6����
		}
	}

	//----------------------------------------------------------------------------------------------------------
	//��ɫ������ʼ��

	monster=0; //ָ��Ĺ���

	isfollow=false; //δ�й���Ŀ��

	isDieAnimation=false; 


	//������ɫ��Ҫ������
	runstate=false;
	//������ɫ��Ҫ������
	lastRunmode=true;

	//��������
	curAttackType = MyCharacter::eAttacktype::MagicAttack;  

	//��ʼ��Ŀ�ĵص���
	isArrived=true;



	//-----------------------------------------------------------------------------------------------------
	//��ʼ����ʱ��
	timer=dev->getTimer();
	lasttime=timer->getRealTime();
	//------------------------------------------------------------------------------------------------------------


}

//�����ļ���ģ��
void MyCharacter::Draw()
{


	//----------------------------------------------------------------------------------------------------------
	//��ɫģ�ͼ���
	
	m_charClothMgr=new MyCharClothMgr(property.sock,property.charSex);
	if(property.charSex==2){  //Ů
		m_charClothMgr->LoadBody(1); 
	}else if(property.charSex==1){ //��
		
		m_charClothMgr->LoadBody(3);
	 
	}

	m_charClothMgr->setPosition(property.pos);


	m_pMagic_Mgr=new Magic::MyCharMagicEffectMgr;
 	 
 
	//��ʼ��վ�� false��ʾ�����״̬ ǿ������վ��
	idle(false);

	//��ʼ��������
	m_weapon_mgr=new MyWeaponMgr(m_charClothMgr->getRootNode());

	

	//���������,����ʼ����ɫ���ϲ�������
	this->setCurWeapon(0,0);

	//----------------------------------------------------------------------------------------------------------
	//��ʼ��Ѫ��
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


//�û�����
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


	 ////�жϽ�ɫ�Ƿ���ƶ���pos
	 if(isMain){
		 if(this->sceneloader->canmovehere(movepos) ){ 
			 movepos.Y=this->sceneloader->getHeight(movepos.X,movepos.Z);
			 node->setPosition(movepos); 
			 node->updateAbsolutePosition(); 
		 }else{
			 //ֹͣ�ƶ�
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
	//���ߵ�������ɫ��ʾѪ�� ǰ�������ָ���˽�ɫģ��
	if(!isMain && m_pgHealthBar && mouseCurCharacter) m_pgHealthBar->render(); 

	//if(char_textnode)
	{

	

		 vector2d<s32>	TextShowPos;
		 //����ɫ��3d����ת��Ϊ2d��Ļ���� .����Ҫ�ڽ�ɫͷ��һ��.
		 TextShowPos= smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(getPosition()+vector3df(0,19,0));

		  vector2d<s32>	 centerpos;
		centerpos=TextShowPos;
	
		centerpos.X-=40; 
		centerpos.Y-=3; //ͼƬ�Ͽ�һ��

		 //��ɫͼƬ
		getIrrDriver()->draw2DImage(getIrrDriver()->getTexture("ui/black.tbj"),
			centerpos, //���������ָ�����Ƶ����� ����λ���Ͻ�λ�û���ͼƬ
			rect<s32>(0,0,90,17),0, //�������Դͼ���ȡͼƬҪ��ʾ������ ���س�ͼƬ�Ĵ�С
			SColor(100,255,255,255),true);

		
		//��ʾ����
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
		isArrived=true; //��׼�ƶ�
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

//ִֻ��һ����������..
void MyCharacter::OneTimeDie()
{
	if(isDieAnimation==false) {

		m_charClothMgr->Animate(ANIMATION_SPEED,91,99,false);
		
	}

	isDieAnimation=true;
}


//�����Ƿ���Ҫ�ȼ��״̬
void MyCharacter::run(bool checkState) 
{ 
	 if(checkState){
		 if(getState()== cRun ) return;

	 }
	 setState(cRun); 
		 
		 //��?��
		 if(property.RunMode){
			     fSpeed = 0.7f; //�ƶ��ٶ�
				 m_charClothMgr->Animate(ANIMATION_SPEED+2,61,79,true);

		 }else{
			     fSpeed = 0.350f; //�ٶ�
				  m_charClothMgr->Animate(ANIMATION_SPEED+1,1,20,true);
		 }

} 
  

irr::scene::IAnimatedMeshSceneNode* MyCharacter::getNode() 
{ 
         return m_charClothMgr->getRootNode();     
} 

 //����Ԥ����վ������ �л�
void MyCharacter::ChangeState()
{
	
    if(getState()==  cIdle )
	{
	  	readyAttack(false);
	}else if(getState()==  cReadyAttack ){
	    idle(false);
	}

	//�ս�ɫ�����ݷ��͸�������
	 ReoprtToServer();
}

void MyCharacter::ChangeRunMode()
{

    property.RunMode=!property.RunMode; //ģʽ�任

	//�Ѿ����ܻ�������
	if(getState() == cRun){
	  	run(false);//�����״ֱ̬���л�����
	}

}

 //�����Ƿ��ڽ�ɫ������Χ��
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
		//printf("����:%3.1f\n",length);
		return true; //��20����,���ڷ�Χ��,������ 
	}
	return false;
}

void MyCharacter::ShowAttackTextNum(int num)
{
	 MyCharAttackNumTextShow shownumtext(m_charClothMgr->getPosition(),num);

}

//��ɫ���ܹ���,���������ܹ���ֵ
void MyCharacter::UnderAttack( int damage)	
{
    //�ѹҾͲ��ܶ���
    if(property.isDeath) return;

	
		//��ʾ�������˶���Ѫ,0�Ļ�����ʾmiss
		MyCharAttackNumTextShow shownumtext(m_charClothMgr->getPosition(),damage);
		

		property.HP=property.HP-(damage);	//hp����
		//ͳ��hp�ٷֱ�
		s32 per=100*((float)property.HP/(float)property.MAX_HP);
		printf("role������! hp:%d per:%d\n",property.HP,per);

		
		//����Ѫ������
		m_pgHealthBar->setProgress(per);
		if(property.HP<=0){
			property.HP=0;
			property.isDeath=true;
			printf("role����!\n");
			
		}

}


 //���ٹ��ﲢ����
void MyCharacter::FollowAndAttackTarget()
{
     //�ѹҾͲ��ܶ���
	  if(property.isDeath) return;
 
	  //ָ��Ĺ���ָ���,�򲻹���
	  if(monster==0) {
		  isfollow=false;
		  idle(true);
		  return;
	  }
	  //ָ��Ĺ�������,�򲻹���
	  if(monster->IsDead()){

		  //��־�Ĺ���ָ��ָ��0
		  isfollow=false;
		  idle(true);
		  monster=0;
		  return ;
	  }

   //���жϾ���,���ھ͹���,����ͼ����ƶ�
	 if(getDistenceFromMonster(monster->GetNode()))
	{
		isArrived=true;
		//printf("������,����!\n");
		attack(monster->GetNode()->getPosition() , curAttackType);
		isfollow=false;
	}else{
		//printf("���벻��,�����ƶ�\n");
	    Beginmove(monster->GetNode()->getAbsolutePosition());
		move();
		isfollow=true;
	}

	//�������������Ƿ�����.
	if(monster->IsDead()){
		
		//��־�Ĺ���ָ��ָ��0
		monster=0;

		isfollow=false;

		//printf("����ֹͣ!��������\n");
		
		idle(true);
		//��ɫ״̬�ı�,�����������
		ReoprtToServer();
	}
}


void MyCharacter::attack(vector3df attackpos,eAttacktype attackType) 
{ 
	  //�ѹҾͲ��ܶ���
	  if(property.isDeath) return;

	  if(monster==0) {
		 readyAttack(true);
		  isfollow=false;
		  return;
	  }

	  setState( cAttack);

	  	 //��ʱ
		u32 usetime=timer->getRealTime()-lastattacktime;
		if(usetime<800) {
			return;
		}
		else lastattacktime=timer->getRealTime();
        

		 //����Ŀ��
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

		 

		 //��ʱ����״̬��������   
		 ReoprtToServer();


		 //�������ܽ�ɫ����, property.Attack�ǽ�ɫ������
		if(monster) monster->UnderAttack( property.Attack+package.weaponparam2);

}



void MyCharacter::Onlyupdate() 
{

	if(ball) ball->update();
	if(m_pMagic_Mgr) m_pMagic_Mgr->update();


	 //��ɫ��״̬����������ʾ  ����վ��,�Ͱ����ϵ���������,���ϵ���ʾ
	 m_weapon_mgr->update(getState());
	//UpdateByProperty();

	 if(property.target.X==0 &&property.target.Y==0 &&property.target.Z==0)
	  isArrived=true;

	if(!isArrived){
		//�ѹҾͲ��ܶ���
		if(property.isDeath) return;

		property.pos=m_charClothMgr->getPosition();

		vector2df p1;
		p1.X=m_charClothMgr->getPosition().X;
		p1.Y=m_charClothMgr->getPosition().Z;

		vector2df p2;
		p2.X=property.target.X;
		p2.Y=property.target.Z;

		//�ѵ���
		//f32 len=m_charClothMgr->getPosition().getDistanceFrom(	property.target) ;
		f32 len=p1.getDistanceFrom(p2);

		if( len< 4){
			m_charClothMgr->setPosition(property.target);
			property.pos=property.target;

			isArrived=true;

			property.target=vector3df(0,0,0);
			idle(false); 
			if(isMain==false) printf("������ɫ�ѵ���!\n");
		} 
		else 
		{ 
			// printf("����:%3.1f\n",len);
			//δ����
			isArrived=false;
			faceTarget(	property.target); 
			if(isMain==false) printf("������ɫrun!\n");
			run(true);
			moveto(m_charClothMgr->getRootNode(), core::vector3df(0,0,fSpeed)); 
		} 

		return;
	}



}


void MyCharacter::update() 
{

	  //��ɫ��״̬����������ʾ  ����վ��,�Ͱ����ϵ���������,���ϵ���ʾ
     m_weapon_mgr->update(getState());
	

	 if(ball) ball->update();
	 if(m_pMagic_Mgr) m_pMagic_Mgr->update();

	 

	 //��ɫ�Ƿ��ѹ�
	 if( property.isDeath || property.HP<=0){
		// printf("ִֻ��һ���������ض���\n");
		 OneTimeDie();
		 return ;
	 }


	 //����ɫ����.�������������.����Ŀ��.ֹͣ����
	 if(isMain && monster==NULL){
			int s=getState();		  
			if(s==cAttack){
				idle(true);
			}
		 
	 }
		       //Do nothing 
	 //δ����Ŀ�ĵ�,�����ƶ�
	  if(!isArrived){
			//printf("char�ƶ�!\n");
		    move();
			
			return;
	  }
	
	  if(monster==0) {
		 // //MessageBox(0,"monster000","",0);
		  return;
	  }

	 //��׷����.. 
		if(isfollow){ 
			//printf("follow\n");
			FollowAndAttackTarget(); return;
		}
			 //�ڹ�����
	    if(getState() ==  cAttack){
			//printf("getState Attack\n");
	     FollowAndAttackTarget();
		 return;
	   }



	
} 


void MyCharacter::CancelAttack()
{
	 //�ر�׷��
     isfollow=false;
}


void MyCharacter::Beginmove(core::vector3df pos)
{
	 isArrived=false;
     property.target=pos;

	 //ÿһ�α���ܲ��ͷ���һ����Ϣ��������
	 ReoprtToServer();
}


void MyCharacter::move() 
{ 

	 //�ѹҾͲ��ܶ���
	  if(property.isDeath) return;

	  property.pos=m_charClothMgr->getPosition();

  
		 //�ѵ���
	 //    f32 len=m_charClothMgr->getPosition().getDistanceFromSQ(	property.target) ;
		 vector2df p1;
		 p1.X=m_charClothMgr->getPosition().X;
		 p1.Y=m_charClothMgr->getPosition().Z;

		 vector2df p2;
		 p2.X=property.target.X;
		 p2.Y=property.target.Z;

		 //�ѵ���
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
			// printf("����:%3.1f\n",len);
			    //δ����
				isArrived=false;
                faceTarget(	property.target); 

				run(true);
                moveto(m_charClothMgr->getRootNode(), core::vector3df(0,0,fSpeed)); 
         } 
   
} 


void MyCharacter::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{
	 //��ɫ�ѹ�?
	 if(property.isDeath){
		//printf("�ѹ�!@");
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


//��ɫ����������������
void MyCharacter::UpdateByProperty()
{

	//����hp��ʾ
	//ͳ��hp�ٷֱ�
	s32 per=100*((float)property.HP/(float)property.MAX_HP);
	//����Ѫ������
	m_pgHealthBar->setProgress(per);

	if(property.HP<=0 || property.isDeath){
		//ִֻ��һ���������ض���
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
		// printf("��ɫֹͣ�ƶ�,state:%d\n",property.currState);
		property.currState=cRun;
		idle(true); 
	}else if(property.currState== cReadyAttack ){
		laststate=cReadyAttack;
		runstate=false;
		//  printf("��ɫֹͣ�ƶ�,state:%d\n",property.currState);
		readyAttack(false); 
	}else if(property.currState== cAttack){

		runstate=false;
		//  printf("��ɫ����,state:%d\n",property.currState);
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


//������ϵ�����Ʒ,����
void MyCharacter::CollectDropItem()
{
	RoleProperty r;
	r.cmd=CMD_C2S_USERCollectItem; //�������Ʒ

	cnet->SendMsg((char*)&r,sizeof(RoleProperty));
}



//��ɫ�����Ե�.�����������
void MyCharacter::AddPoint(int proptype)
{
	if(cnet==0 ){
		 
		return ;
	}

	RoleProperty p;
	memset((char*)&p,'\0',sizeof(RoleProperty));
	memcpy((char*)&p,(char*)&property,sizeof(RoleProperty));
	p.cmd=CMD_C2S_Request_AddPoint;        //����ӵ�


	//û�������Ե�,����
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



//ֻ���½�ɫ�����������.
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
	
	//�����¼�
	if (e.EventType == EET_KEY_INPUT_EVENT) {
 
		    
		   //��X �л� վ�� ����׼��ģʽ
			if (e.KeyInput.Key == KEY_KEY_X && e.KeyInput.PressedDown==false) { 
			   ChangeState();
		    }
		   
			//��R �л��ܲ���· ģʽ
			if (e.KeyInput.Key == KEY_KEY_R && e.KeyInput.PressedDown==false) { 
				ChangeRunMode();
		    }

			//���ո�,����Ʒ
			if (e.KeyInput.Key == KEY_SPACE && e.KeyInput.PressedDown==false) { 
				CollectDropItem();
			}


			//��Q ʹ�ð������MP�ָ�ҩ
			if (e.KeyInput.Key == KEY_KEY_Q && e.KeyInput.PressedDown==false) { 

				//����ʹ��װ�������������Ʒ
				property.cmd=CMD_C2S_Request_UsePackageHPItem;
				
				cnet->SendMsg((char*)&property,sizeof(RoleProperty));
			}


	}
	
}


//���������ͼ
void MyCharacter::SetWorldMap(int worldMap)
{
  property.worldMap_id=worldMap;
}

