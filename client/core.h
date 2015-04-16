#ifndef CORE_H 
#define CORE_H 


#include "stdafx.h"



#include "shadercpp/effectwrapper.h"

//#include "effectHandler.h"

#include "myscenemanager.h"


#include "realisticwater.h" 



#include "NetClient.h"

class GameMain: public irr::IEventReceiver { 
public: 
	GameMain (SLogin*); 
	~GameMain (); 
	virtual      bool OnEvent (const irr::SEvent& e); 
	void Run (); 

	void Login(); // 登入模块


	SLogin* slogin;

	bool InitNetWork();

	//向服务器发送登陆请求
	void SendLoginRequest();

	//等待服务器响应登陆,阻塞的
	bool WaitForServerLoginResponse();
	HANDLE m_h_LoginEvent; //等待登陆的响应事件对象

	MySceneManager* myScene_Mgr;
private: 
	irr::IrrlichtDevice *device; 
	irr::video::IVideoDriver *drv; 
	irr::gui::IGUIEnvironment* ui;
	irr::scene::ISceneManager *smgr; 
	irr::u32 last_time; 

	Socket::CNetClient client;  //网络

	char temp[255];



	RealisticWaterSceneNode* water;

	scene::IVolumeLightSceneNode * n;









	screenEffect::effectHandler* effect;

	gui::IGUIMeshViewer* v;


	bool	uesBlur;

	bool HideOther;

}; 

#endif // CORE_H