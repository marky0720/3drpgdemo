#pragma  once


#include "../stdafx.h"


#include "MyloginCharClothMgr.h"
#include "loginweapon.h"
#include "MyuvanimationHLSLEffect.h"

#include <string.h>

namespace Login
{



class LoginCore: public irr::IEventReceiver { 
        public: 
                LoginCore (); 
                ~LoginCore (); 
            virtual      bool OnEvent (const irr::SEvent& e); 
             SLogin* Run (); 
			 //�����
			 void myTextLeft( const char* str,irr::core::vector2d<s32> pos , irr::video::SColor color,u32 fontsize,u32 weight);

			 char* LoginCore::WCharToMByte2(LPCWSTR lpcwszStr);
	
			
        private: 
irr::IrrlichtDevice * device;
irr::video::IVideoDriver* drv;
irr::gui::IGUIEnvironment* ui;
irr::scene::ISceneManager* smgr;

				ICameraSceneNode* cam;

				bool bquit;

				int ret;

				ISceneNode* node;
				MyLoginCharClothMgr* mgr;
				Login::MyWeapon2* backweapon;

				scene::IAnimatedMeshSceneNode* waterfall;


				Login::MyWeapon2* flyweapon; //�ɽ�
				scene::IParticleEmitter* flyem;//�ɽ����ӷ�����
				MyLoginCharClothMgr* man; //�ɽ��ϵ���

                irr::u32 last_time; 
             


				IGUIButton* btn_login;
				IGUIButton* btn_quit;
				IGUIEditBox* editbox;//����������
				
				//ISoundEngine* engine;

		SLogin* slogin;

		gui::IGUIWindow* win ;
		irr::gui::IGUICheckBox* mancheck;
			irr::gui::IGUICheckBox* girlcheck;

}; 

}