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

MyMonster::MyMonster( MonsterProperty pp,const c8* filename )
{

	device=getIrrDevice();
	smgr=getIrrSmgr();

	file=filename;


	yrot=90.0f;	//默认旋转-90度

	movespeed=0.35;
	 //定时器
	 timer=device->getTimer();
	 lasttime=timer->getRealTime();
lastattacktime=timer->getRealTime();
//------------------------------------------------------------------------------------------------------
	setMonsterProperty(pp);	 
		//prop.ID=pp.ID;
	 // prop.pos=pp.pos; //位置
	 // prop.worldMap_id=pp.worldMap_id;
	 // prop.rot=pp.rot;
	 // prop.currState=pp.currState;	//默认角色是走
	 // prop.MAX_HP=pp.MAX_HP;
	 // prop.HP=pp.HP;	    //怪物当前hp
	 // prop.Attack=pp.Attack;	//攻击力
	 // prop.Defense=pp.Defense;		//防御力
	 // prop.isDeath=pp.isDeath;;		//是否死亡

	 // lastState=prop.currState;
	
//-----------------------------------------------------------------------------------------------------

	  isdrop=false;

	  node=0;
	  m_pgHealthBar=0;

}

//设置模型旋转度,用于矫正模型不对应
void MyMonster::SetInitRotValue(float rot)
{
	yrot=rot;
}

void MyMonster::Draw()
{	
	   //绘制
	
		node=smgr->addAnimatedMeshSceneNode(smgr->getMesh(file));
		//if(picpath) node->setMaterialTexture(0,device->getVideoDriver()->getTexture(picpath));
		node->getMaterial(0).SpecularColor.set(0,0,0,0);//黑色
		node->setMaterialFlag(video::EMF_LIGHTING, false);//不照明
		node->setScale(core::vector3df(0.80f,0.80f,0.80f));//模型缩放
		node->setMaterialFlag(video::EMF_ANTI_ALIASING,true);//开启抗锯齿
	//	node->setDebugDataVisible(scene::EDS_BBOX);
		node->setAnimationSpeed(10);
		node->getMaterial(0).NormalizeNormals=true;
		node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
		node->setPosition(prop.pos);



		//给模型创建了三角形选择器后才能在场景碰撞管理器里拾取模型
		scene::ITriangleSelector* selector;		//碰撞选择器
		selector= smgr->createTriangleSelector(node);
		node->setTriangleSelector(selector);//设置选择器


		node->setFrameLoop(1, 19);


//初始化血条
m_pgHealthBar =new irr::scene::VxHealthSceneNode( 
this->node, // parent node 
smgr, // scene manager 
-1, // id 
smgr->getSceneCollisionManager(), // collision manager 
50, // width 
6, // height 
core::vector3df(0,20,0), // position 
video::SColor(255,0,200,0), // bar color 
video::SColor(255,220,0,0), // background color 
video::SColor(255,255,255,255) ); // boarder color 
//set percent of health 
s32 per=100*(int)prop.HP/(int)prop.MAX_HP;
//printf("怪物per%d\n",per);
m_pgHealthBar->setProgress( per );
//-----------------------------------------------------------------------------------------------------



}

//模型缩放
void MyMonster::SetScale(float scale)
{
	node->setScale(vector3df(scale,scale,scale));
}

//设置动作
void MyMonster::SetState(int s){

	//走
	if(s==cWalk && prop.currState!=cWalk){
		if(lastState==cWalk){
			return;
		}
		lastState=cWalk;
		node->setLoopMode(true);
		node->setAnimationSpeed(10);//关键帧速度
		node->setFrameLoop(2, 20);
	//死亡
	}else if(s==cDeath && prop.currState!=cDeath){
		if(lastState==cDeath){
			return;
		}
		lastState=cDeath;
		node->setAnimationSpeed(10);//关键帧速度
		node->setFrameLoop(91, 103);
		node->setLoopMode(false);
		
	//脚攻击
	}else if(s==cAttack && prop.currState!=cAttack){
		if(lastState==cAttack){
			return;
		}
		lastState=cAttack;
		node->setAnimationSpeed(7);//关键帧速度
		node->setFrameLoop(106, 115);
		node->setLoopMode(true);
    //头攻击
	}else if(s==cAttack2 && prop.currState!=cAttack2){
		if(lastState==cAttack2){
			return;
		}
		lastState=cAttack2;
		node->setAnimationSpeed(7);//关键帧速度
		node->setFrameLoop(117, 128);
		node->setLoopMode(true);
	//站
	}else if(s==cIdle && prop.currState!=cIdle){
		if(lastState==cIdle){
			return;
		}
		lastState=cIdle;
		node->setLoopMode(true);
		node->setAnimationSpeed(10);//关键帧速度
		node->setFrameLoop(137, 169);
	}


}



//怪物被攻击显示的数字
void MyMonster::ShowAttackTextNum(int num)
{
	//显示被攻击了多少血,0的话就显示miss
	MyCharAttackNumTextShow shownumtext(node->getPosition(),num);

}

//怪物遭受攻击,参数是攻击值
void MyMonster::UnderAttack(unsigned int attack)
{

	//攻击力小于防御力...
	if(attack<=prop.Defense){
		//显示被攻击了多少血,0的话就显示miss
		MyCharAttackNumTextShow shownumtext(node->getPosition(),0);

		return;
	}

	//不计算伤害值,只发送给服务器告诉他某人砍了怪物.

	prop.cmd=MonsterCMD_UnderAttack;
	printf("怪物%d受攻击\n",prop.ID);
	cnet->SendMsg((char*)&prop,sizeof(MonsterProperty));


	if(prop.HP<=0){
	//		m_puser->GainExp(50);//玩家获得经验
		prop.isDeath=true;
		SetState(cDeath);	//怪物死亡就设置标记和动作
	
	}



}

void MyMonster::UIRender()
{
	if(m_pgHealthBar && node->isVisible()) this->m_pgHealthBar->render();
}



void MyMonster::Update()
{

	if(node==0) return;

		if(prop.isDeath)
		{
			//printf("怪物死亡!\n");
			SetState(cDeath);
			return ;
		}

		//角色根据属性立即更新

		  
			node->setPosition(prop.pos);
			node->setRotation(prop.rot);


			//更新血条
			//统计hp百分比
			s32 per=100*((float)prop.HP/(float)prop.MAX_HP);
			this->m_pgHealthBar->setProgress(per);

			if(prop.currState==cWalk){
				prop.currState=-1;
				SetState(cWalk);
			}else if(prop.currState== cIdle ){
				prop.currState=-1;
				SetState(cIdle);
			}else if(prop.currState== cAttack ){
				prop.currState=-1;
				SetState(cAttack);
			}else if(prop.currState== cAttack2 ){
				prop.currState=-1;
				SetState(cAttack2);
			}


}
void MyMonster::OnAnimationEnd(IAnimatedMeshSceneNode* node)
{

}

void MyMonster::drop()
{
	isdrop=true;

	if(node==0) return;
	
	scene::ISceneNodeAnimator* anim;		
	node->addAnimator(anim=smgr->createDeleteAnimator(1));//创建删除动作,删除自己,场景节点
	anim->drop();

}

void MyMonster::setMonsterProperty(MonsterProperty p){
	  memcpy(   (char*)&prop,(char*)&p,sizeof(MonsterProperty));

}

/*
  死后飞出鬼魂效果
 */
void MyMonster::AddGhostFlyEffect()
{
  	IBillboardSceneNode* ghost = smgr->addBillboardSceneNode(0, core::dimension2d<f32>(10, 40));
	ghost->setMaterialFlag(video::EMF_LIGHTING, false);
	ghost->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);//EMT_TRANSPARENT_ALPHA_CHANNEL  EMT_TRANSPARENT_ADD_COLOR
	ghost->setMaterialTexture(0, smgr->getVideoDriver()->getTexture("ghost/sc_ghost.tbj"));

	scene::ISceneNodeAnimator* a = 0;
	vector3df p=node->getPosition();
	//上飞
	a = smgr->createFlyStraightAnimator (p,p+vector3df(0,250,0),4000);
	ghost->addAnimator(a);
	
	//4.5秒删除
	a=smgr->createDeleteAnimator(4500);
	ghost->addAnimator(a);

	a->drop();
}
