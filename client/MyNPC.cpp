#include ".\mynpc.h"

MyNPC::MyNPC( NPCProperty p,Socket::CNetClient* client)
{
	dlg=0;

	this->dev=getIrrDevice();
	this->client=client;
	
	smgr=getIrrSmgr();

	//npc属性
	memset((char*)&npc_prop,0,sizeof(NPCProperty));
	memcpy((char*)&npc_prop,(char*)&p,sizeof(NPCProperty));

	//绘制

	node=smgr->addAnimatedMeshSceneNode(smgr->getMesh("npc/npc1.ms3d"));

	node->setPosition(p.pos);
	node->setRotation(p.rot);
	node->setID(p.ID);

	node->getMaterial(0).SpecularColor.set(0,0,0,0);//黑色
	node->setMaterialFlag(video::EMF_LIGHTING, false);//不照明
	node->setScale(core::vector3df(0.80f,0.80f,0.80f));//模型缩放
	node->setMaterialFlag(video::EMF_ANTI_ALIASING,true);//开启抗锯齿
	//	node->setDebugDataVisible(scene::EDS_BBOX);
	node->setAnimationSpeed(10);
	node->getMaterial(0).NormalizeNormals=true;
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	//node->setPosition(npc_prop.pos);

	node->setAnimationSpeed(1);
	node->setFrameLoop(1, 10);

	//给模型创建了三角形选择器后才能在场景碰撞管理器里拾取模型
	scene::ITriangleSelector* selector;		//碰撞选择器
	selector= smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);//设置选择器


	mouseCurNPC_Dlg=false;


	//------------------------------------------------------------------------------------------------------------
	//  显示npc名称的
	//char tmp[512];
	//GetWindowsDirectory(tmp,511);
	//strcat(tmp,"\\fonts\\SIMSUN.TTC");  //SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys

	gui::IGUIFont *font;
	//SIMSUN.TTC宋体,新宋体         SIMKAI.TTF楷体     \STXIHEI.TTF   Fixedsys
	//font = dev->getGUIEnvironment()->getFont(tmp,17, true,false);

	WCHAR * pwStr = ConvertLPCSTRToLPWSTR( npc_prop.strNPCName ); 

	font = dev->getGUIEnvironment()->getDXFont("宋体",true,15,0,800);

		char_textnode= smgr->addTextSceneNode(font,	pwStr, 	video::SColor(255,255,255,0), node);
	
	char_textnode->setPosition(vector3df(0,-5,0));

	delete pwStr;

}

MyNPC::~MyNPC(void)
{
}

//弹出npc的对话框.
void MyNPC::ShowTalkDlg()
{
	//对话框还没初始化,表示第一次点击npc
	if( 0 ==dlg){
		//npc的对话框.需要知道npc的属性
		dlg=new MyGUIDlgNpcTalk(&npc_prop,client);
		dlg->LoadGUI();
	}else{
		
		//再次点击npc.对话框直接显示
		dlg->ShowDlg(true);
	}
}

void MyNPC::OnEvent(const SEvent& e)
{
	if(dlg){
		dlg->OnEvent(e);
		if(dlg->mouseInGui){
			mouseCurNPC_Dlg=true;
		}else{
			mouseCurNPC_Dlg=false;
		}

	}
	
}

void MyNPC::UIRender()
{
	if(dlg) dlg->drawAll();


	
}

void MyNPC::update()
{
	dlg->update();


}

//卸载
void MyNPC::drop()
{
	if(dlg){
		dlg->drop();
	}
	scene::ISceneNodeAnimator* anim;
	anim = smgr->createDeleteAnimator(1);  //删除
	node->addAnimator(anim);
	anim->drop();	
}