#include ".\myweaponmgr.h"

MyWeaponMgr::MyWeaponMgr(IAnimatedMeshSceneNode* parent)
{
	device=getIrrDevice();

	smgr=getIrrSmgr();

	parentNode=parent;


	//初始化武器类,分别是手上和背上的
	handweapon=new MyWeapon();
	backweapon=new MyWeapon();

	//必须先换武器
	ChangeWeapon(1,0);

	handweapon->getNode()->setVisible(false);
	backweapon->getNode()->setVisible(false);


}

void MyWeaponMgr::setWeaponVisible(bool v)
{
	updateUseLastState();
}

void MyWeaponMgr::OpenHlslEffect(bool use,int weaponType) //weaponType 是武器流动的纹理
{
	isusehlsleffect=use;

	//玄冰剑效果特殊些..
	if(currentWeaponid==XBJ_Weapon){
		
		handweapon->setUseHLSL(use,true,weaponType);
		backweapon->setUseHLSL(use,true,weaponType);
	}else{
		handweapon->setUseHLSL(use,false,weaponType);
		backweapon->setUseHLSL(use,false,weaponType);
		
	}


	
}

//weaponparam1表示效果类型
void MyWeaponMgr::ChangeWeapon(int id,int weaponparam1)
{
	currentWeaponid=id;
	currparam1=weaponparam1;

	//表示要卸下武器...我们这里只把他隐藏
	if(id==0){
		if(handweapon->getNode()) handweapon->getNode()->setVisible(false);
		if(backweapon->getNode()) backweapon->getNode()->setVisible(false);
		return;
	}

	bool usehlsl=false;
	if(weaponparam1==0){
		//普通武器效果
	}else{
		usehlsl=true;
	}

	//得到拿武器的关节,手			//角色->服装管理器->root骨骼
	scene::ISceneNode* pJointNode =parentNode->getMS3DJointNode("weapon");  //weapon是手 
	if(pJointNode)
	{
		handweapon->ps=0;

		//移除手上的武器节点
		pJointNode->removeAll();
		//加载武器
		handweapon->ChangeWeapon(abs(id));
		handweapon->getNode()->setID(101);
		// handweapon->setUseHLSL(true);
		handweapon->getNode()->setScale(vector3df(6,6,6));
		handweapon->getNode()->setRotation(vector3df(180,0,90));
		handweapon->setPosition(vector3df(0,0,0));

		//绑定到手上
		pJointNode->addChild( handweapon->getNode() );
	} 


	pJointNode = parentNode->getMS3DJointNode("joint23");  //   joint23 是背
	if(pJointNode)
	{
		backweapon->ps=0;

		//移除背上的武器节点
		pJointNode->removeAll();
		//加载武器
		backweapon->ChangeWeapon(abs(id));
		backweapon->getNode()->setID(101);
		//backweapon->setUseHLSL(true);
		backweapon->getNode()->setScale(vector3df(6,6,6));
		backweapon->getNode()->setRotation(vector3df(180,-5,110));
		backweapon->setPosition(vector3df(0,-2,0));
		//绑定到背上
		pJointNode->addChild( backweapon->getNode() );
	}

	if(currentWeaponid==XBJ_Weapon){
		//handweapon->setUseHLSL(true,true);
		//backweapon->setUseHLSL(true,true); //始终开启hlsl,透明效果
		
		handweapon->HLSL();
		backweapon->HLSL();

		////设置冰纹理
		//handweapon->getNode()->setMaterialTexture(1,device->getVideoDriver()->getTexture("weapon/w52.tga"));
		//backweapon->getNode()->setMaterialTexture(1,device->getVideoDriver()->getTexture("weapon/w52.tga"));
	}

	
	if(usehlsl && XBJ_Weapon!=id){
		//表示光武
		handweapon->setUseHLSL(true,false,weaponparam1);
		backweapon->setUseHLSL(true,false,weaponparam1);
	}

	

}
void MyWeaponMgr::updateUseLastState(  )
{
	//得到角色状态


	//表示卸下了武器,将武器都隐藏
	if(currentWeaponid==0){
		if(handweapon->getNode()) handweapon->getNode()->setVisible(false);
		if(backweapon->getNode()) backweapon->getNode()->setVisible(false);
		return;
	}

	//站着,就把手上的武器隐藏,背上的显示
	if(state == cIdle) //站
	{
		handweapon->getNode()->setVisible(false);
		backweapon->getNode()->setVisible(true);
	}else if(state==  cRun){//走
		handweapon->getNode()->setVisible(false);
		backweapon->getNode()->setVisible(true);
	}else if(state ==  cReadyAttack){//攻击预备
		handweapon->getNode()->setVisible(true);
		backweapon->getNode()->setVisible(false);
	}else if(state ==  cAttack){//攻击
		handweapon->getNode()->setVisible(true);
		backweapon->getNode()->setVisible(false);
	}else if(state ==  cDeath){
		handweapon->getNode()->setVisible(false);
		backweapon->getNode()->setVisible(true);
	}

}

void MyWeaponMgr::update(int state )
{
//	if(handweapon->getNode()==0 || backweapon->getNode()==0) return;

	//得到角色状态
	this->state=state;


	//表示卸下了武器,将武器都隐藏
	if(currentWeaponid==0){
		if(handweapon->getNode()) handweapon->getNode()->setVisible(false);
		if(backweapon->getNode()) backweapon->getNode()->setVisible(false);
		return;
	}

	//站着,就把手上的武器隐藏,背上的显示
	if(state == cIdle) //站
	{
		handweapon->getNode()->setVisible(false);
		backweapon->getNode()->setVisible(true);
	}else if(state==  cRun){//走
		handweapon->getNode()->setVisible(false);
		backweapon->getNode()->setVisible(true);
	}else if(state ==  cReadyAttack){//攻击预备
		handweapon->getNode()->setVisible(true);
		backweapon->getNode()->setVisible(false);
	}else if(state ==  cAttack){//攻击
		handweapon->getNode()->setVisible(true);
		backweapon->getNode()->setVisible(false);
	}else if(state ==  cDeath){
		handweapon->getNode()->setVisible(false);
		backweapon->getNode()->setVisible(true);
	}

	handweapon->update();
	backweapon->update();

}

MyWeaponMgr::~MyWeaponMgr(void)
{
}

void MyWeaponMgr::setParentNode(IAnimatedMeshSceneNode* p)
{

	DropAll();

	this->parentNode=p;

	ChangeWeapon(currentWeaponid,currparam1);
}

//卸载所有武器
void MyWeaponMgr::DropAll()
{
//	if(parentNode==0){
//		return;
//	}

	scene::ISceneNode* pJointNode = parentNode->getMS3DJointNode("weapon");  //weapon是手 
	if(pJointNode)
	{
		//移除手上的武器节点
		pJointNode->removeAll();
		handweapon->setNode(0);
	}

	
	pJointNode = parentNode->getMS3DJointNode("joint23");  //   joint23 是背
	if(pJointNode)
	{
			//移除背上的武器节点
			pJointNode->removeAll();
			backweapon->setNode(0);
	}

	
	


}