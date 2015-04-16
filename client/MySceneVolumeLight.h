#pragma once


/************************************************************************/
// 
//		Με»ύΉβ
//		
//
/************************************************************************/


class MySceneVolumeLight
{
public:
	MySceneVolumeLight( char* meshPath);
	~MySceneVolumeLight(void);
	void drop();
	void setPosition(core::vector3df pos){node->setPosition(pos);}; 
	vector3df getPosition(){return node->getPosition();};
	void setRotation(core::vector3df rot){node->setRotation(rot);}; 
	vector3df getRotation(){return node->getRotation();};
	void setScale(vector3df scale){node->setScale(scale);};
	scene::IVolumeLightSceneNode* getNode(){return node;};

private:
	scene::IVolumeLightSceneNode *node;
	IrrlichtDevice* device;
	vector3df pos;
};
