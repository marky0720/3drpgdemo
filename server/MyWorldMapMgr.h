#pragma once

class MyWorldMapMgr
{
public:
	MyWorldMapMgr(void);
	~MyWorldMapMgr(void);


	//��list���ñ���.��Ϊ�Ժ�ֻ����
	irr::core::list<STeleport*> worldtelport_list;  //�����������ͼ�Ĵ��͵�


	//��ȡ�����ͼ�����д��͵�
	void ReadWorldMapTeleport();


};
