#pragma once

class MyWorldMapMgr
{
public:
	MyWorldMapMgr(void);
	~MyWorldMapMgr(void);


	//该list不用保护.因为以后都只读他
	irr::core::list<STeleport*> worldtelport_list;  //存所有世界地图的传送点


	//读取世界地图的所有传送点
	void ReadWorldMapTeleport();


};
