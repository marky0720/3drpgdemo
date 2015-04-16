#pragma once

#include "stdafx.h"



namespace Magic
{


class IMyCharMagicEffect  : public video::IShaderConstantSetCallBack
{
public:

    virtual bool init( ISceneNode* parentNode=0, //效果跟随的父节点.
					   vector3df pos=vector3df(0,0,0), //设置坐标.
					   float speed=0, //速度
					   ITexture* tex=0  //设置特效的纹理
					
		) =0;
    virtual void update() =0;
    virtual void drop() =0;
	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)=0;

	virtual void setParentNode(scene::ISceneNode* parentNode=0)=0;

};


} // end namespace