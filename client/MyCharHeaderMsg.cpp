#include ".\mycharheadermsg.h"

MyCharHeaderMsg::MyCharHeaderMsg( ISceneNode* parent,const wchar_t* msg)
{

	irr::core::list<ISceneNode*> msglist=parent->getChildren();
	irr::core::list<ISceneNode*>::Iterator oper;
	for(oper =msglist.begin(); oper !=msglist.end();++oper )
	{
		//������msgҪ��ʾ,���ϴε�msg����������,����..
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
	strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC����,������         SIMKAI.TTF����     \STXIHEI.TTF   Fixedsys

	IGUIFont* font = getIrrUI()->getFont(tmp,18,true,0);
//	IGUIFont* font = getIrrUI()->getDXFont("����",true,18,0,500);
	ITextSceneNode* textnode= getIrrSmgr()->addTextSceneNode(font,	
			msg, 
			color,	parent,
			vector3df(0,90,0), //text�������
			33);   //ID



	scene::ISceneNodeAnimator* a = 0;
	//4.5��ɾ��
	a= getIrrSmgr()->createDeleteAnimator(4500);
	textnode->addAnimator(a);//����ڵ�ɾ��
	a->drop();
}

MyCharHeaderMsg::~MyCharHeaderMsg(void)
{
}
