#pragma once

#include "stdafx.h"


/************************************************************************/
// 
//		用于显示角色喊话,头上显示的
//		
//
/************************************************************************/


class MyCharHeaderMsg
{
public:
	MyCharHeaderMsg(ISceneNode* parent,const wchar_t* msg);
	~MyCharHeaderMsg(void);
};
