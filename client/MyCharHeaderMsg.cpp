#include ".\mycharheadermsg.h"

MyCharHeaderMsg::MyCharHeaderMsg( ISceneNode* parent,const wchar_t* msg)
{

	irr::core::list<ISceneNode*> msglist=parent->getChildren();
	irr::core::list<ISceneNode*>::Iterator oper;
	for(oper =msglist.begin(); oper !=msglist.end();++oper )
	{
		//又有新msg要显示,把上次的msg都遍历出来,隐藏..
		if((*oper)->getID()==33){
			(*oper)->setVisible(false);
			//oper=msglist.erase(oper);
		}
	}


		SColor color;
		color.setAlpha(255);
		color.setRed(255);
		color.setBlue(250);
		color.setGreen(0);

	char tmp[512];
	GetWindowsDirectory(tmp,511);
	strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys

	IGUIFont* font = getIrrUI()->getFont(tmp,18,true,0);
//	IGUIFont* font = getIrrUI()->getDXFont("宋体",true,18,0,500);
	ITextSceneNode* textnode= getIrrSmgr()->addTextSceneNode(font,	
			msg, 
			color,	parent,
			vector3df(0,90,0), //text相对坐标
			33);   //ID



	scene::ISceneNodeAnimator* a = 0;
	//4.5秒删除
	a= getIrrSmgr()->createDeleteAnimator(4500);
	textnode->addAnimator(a);//字体节点删除
	a->drop();
}

MyCharHeaderMsg::~MyCharHeaderMsg(void)
{
}
