#include "StdAfx.h"
#include "myitem.h"

MyItem::MyItem(  char* meshPath,SceneItem item)
{

	dev=getIrrDevice();
	smgr=getIrrSmgr();

	modelPath=meshPath;
	
	node=0;

	memcpy((char*)&this->item,(char*)&item,sizeof(SceneItem));

}

MyItem::~MyItem(void)
{
}


void MyItem::Draw()
{

	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh(modelPath)); 
	//	node->setMaterialTexture(0,device->getVideoDriver()->getTexture("a.bmp"));
	//node->setScale(vector3df(20,20,20));
	node->setPosition(item.pos);
	node->setMaterialFlag(video::EMF_LIGHTING, false);//照明
	//node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);// 加了这个就透明了..
	//	selecter= smgr->createOctTreeTriangleSelector(node->getMesh(), node , 128);
	//		selecter= smgr->createTriangleSelector(node->getMesh(), node );
	//	node->setTriangleSelector(selecter);//设置选择器
	//	node->setDebugDataVisible(scene::EDS_BBOX ); //设置显示包围盒
	//	node->setMaterialFlag(video::EMF_ZBUFFER, true); //可选
	//node->setAutomaticCulling( EAC_BOX  );

	if(item.item_Type==ITEMType_Money){
		node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);// 加了这个就透明了.	
	}

}

void MyItem::UIRender()
{
	if(node==0 ) return ;

    TextShowPos= smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(item.pos);
	
	
	//显示物品名称.名称是server传递来的.直接显示吧
	if(node && node->isTrulyVisible()  )
	{
		TextCenter(item.itemName,TextShowPos,SColor(1,255,255,0),14,600);
	}

}

void MyItem::drop()
{
	//if(node==0) return ;

scene::ISceneNodeAnimator* anim = 0;
node->addAnimator(anim=smgr->createDeleteAnimator(1));//创建删除动作,删除自己,场景节点
anim->drop();



}