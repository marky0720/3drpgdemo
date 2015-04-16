#include ".\mycharattacknumtextshow.h"

MyCharAttackNumTextShow::MyCharAttackNumTextShow( vector3df pos,int num)
{

	
	char text[8]="";
	if(num==0) sprintf(text,"MISS");
		else sprintf(text,"%d",num);

	WCHAR * pwStr = ConvertLPCSTRToLPWSTR((char*)text); 

	SColor color;
	
	if(num==0){
		color.setAlpha(255);
		color.setRed(0);
		color.setBlue(205);
		color.setGreen(0);
	}else{
		color.setAlpha(255);
		color.setRed(200);
		color.setBlue(0);
		color.setGreen(0);
	}

	char tmp[512];
	GetWindowsDirectory(tmp,511);
	strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys

//	IGUIFont* font = dev->getGUIEnvironment()->getFont(tmp,18,true,0);
	IGUIFont* font = getIrrUI()->getDXFont("宋体",true,20,0,800);
	ITextSceneNode* textnode= getIrrSmgr()->addTextSceneNode(font,	
			pwStr, 
			color,	0);
	delete pwStr;
	textnode->setPosition(pos);

	scene::ISceneNodeAnimator* a = 0;
	vector3df p=pos;
	p+=vector3df(0,5,0);
	//上飞
	a = getIrrSmgr()->createFlyStraightAnimator (p,p+vector3df(0,10,0),400);
	textnode->addAnimator(a); //字体节点飞动画
	//a->drop();
	
	//4.5秒删除
	a=getIrrSmgr()->createDeleteAnimator(450);
	textnode->addAnimator(a);//字体节点删除

	a->drop();



}

MyCharAttackNumTextShow::~MyCharAttackNumTextShow(void)
{
}
