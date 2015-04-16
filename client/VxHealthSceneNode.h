#pragma once


/******************************************************************* 
* Copyright (C) 2006 Brett R. Jones issued to public domain 
*********************************************************************/ 
#ifndef VX_HEALTH_SCENE_NODE_H 
#define VX_HEALTH_SCENE_NODE_H 

#include "ISceneNode.h" 
#include "ISceneCollisionManager.h" 

namespace irr 
{ 
	namespace scene 
	{ 

		class VxHealthSceneNode : public ISceneNode 
		{ 
		public: 

			//! constructor 
			VxHealthSceneNode( ISceneNode* parent, 
				ISceneManager* mgr, 
				s32 id, 
				scene::ISceneCollisionManager* coll, 
				s32 width = 100, 
				s32 height = 10, 
				const core::vector3df& position = core::vector3df(0,0,0), 
				video::SColor BarColor=video::SColor(150,0,200,0), 
				video::SColor BkgColor=video::SColor(150,200,0,0), 
				video::SColor BoarderColor=video::SColor(200,220,220,220) ); 

			//! destructor 
			virtual ~VxHealthSceneNode(); 

			virtual void OnPreRender(); 

			//! renders the node. 
			virtual void render(); 

			//! returns the axis aligned bounding box of this node 
			virtual const core::aabbox3d<f32>& getBoundingBox() const; 

			//! returns amount of materials used by this scene node. 
			virtual s32 getMaterialCount(); 

			//! sets progress percentage ( or health ) 
			virtual void setProgress( s32 s32Percent ); 

			//! sets the color of the progress bar 
			virtual void setProgressColor(video::SColor color); 

			//! sets the color of the progress bar background 
			virtual void setBackgroundColor(video::SColor color); 

			//! sets the color of the progress bar border 
			virtual void setBorderColor(video::SColor color); 


		private: 

			core::aabbox3d< f32 > Box; 
			video::SColor BarColor; 
			video::SColor BorderColor; 
			video::SColor BkgColor; 
			scene::ISceneCollisionManager* Coll; 
			core::dimension2d<s32> m_gDim; //dimension width, height 
			s32 m_bBorder; 
			s32 MaxNum; 
			s32 isVisible; 
			s32 m_s32Percent; 
		}; 

	} // end namespace scene 
} // end namespace irr 

#endif // VX_HEALTH_SCENE_NODE_H 



/*

³õÊ¼»¯
/// use somthing like this 
m_pgHealthBar =new irr::scene::VxHealthSceneNode( 
this->node, // parent node 
smgr, // scene manager 
-1, // id 
smgr->getSceneCollisionManager(), // collision manager 
50, // width 
6, // height 
core::vector3df(0,14,0), // position 
video::SColor(150,0,200,0), // bar color 
video::SColor(150,220,0,0), // background color 
video::SColor(255,255,255,255) ); // boarder color 
//set percent of health 
m_pgHealthBar->setProgress( 60 );


ÔÚgui->drawall();
	m_pgHealthBar->render();


*/