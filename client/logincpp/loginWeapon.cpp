#include "../StdAfx.h"
#include "loginweapon.h"


namespace Login
{


class MyShaderCallBack : public video::IShaderConstantSetCallBack
{
public:

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		IVideoDriver* drv=device->getVideoDriver();
		// set inverted world matrix(������������)
		// if we are using highlevel shaders (the user can select this when(�������Ҫ�ø߼���ɫ��Ⱦ,���Ǳ�������constants������)
		// starting the program), we must set the constants by name.

		//�õ��������
		core::matrix4 invWorld = drv->getTransform(video::ETS_WORLD);
		invWorld.makeInverse();//���

	
		services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);
		
		// set clip matrix ���ü��þ���
		core::matrix4 worldViewProj;
		worldViewProj = drv->getTransform(video::ETS_PROJECTION);			
		worldViewProj *= drv->getTransform(video::ETS_VIEW);
		worldViewProj *= drv->getTransform(video::ETS_WORLD);

		
		services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

		// set camera position
		core::vector3df pos = device->getSceneManager()->getActiveCamera()->getAbsolutePosition();

		services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);


		// set light color 
	
		num=RandNum2(0.1f,0.90f,0.0025);
		video::SColorf col(1-num,1-num,num,0.0f);
	
//		num=RandNum2(0.1f,2.0f,0.0025);
//		video::SColorf col(num,1.0f-num,num,0.0f);
		//if(num>1.0f){
  //            col=SColorf(2-num,2.0f-num,num-1,0.0f);
		//}else{
		//	col=SColorf(num,1.0f-num,num,0.0f);
		//}

		//video::SColorf col(1,1,1,0.0f);
		//printf("%3.1f ",num);
		services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(&col), 4);




		//͸����
		mAlpha=0.90f;
		/*if(mAlpha>=1.0f) mAlpha=0.1f;
		printf("mAlpha:%3.1f \n",mAlpha);*/
		services->setVertexShaderConstant("mAlpha", reinterpret_cast<f32*>(&mAlpha), 1);
		
		
		
		// set transposed world matrix
		core::matrix4 world = drv->getTransform(video::ETS_WORLD);
		world = world.getTransposed();
		services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

		
		
	}
	MyShaderCallBack(irr::IrrlichtDevice * dev){
		device=dev;
		mAlpha=1.0f;
		 num=0.1f;
	}


	//��begin��end֮�����صĺ���
	float RandNum2(float  begin,float end,float vlaue)
	{
		static float yrot=begin;
		static bool a=true;
		if(a==true)  yrot+=vlaue;
		if(a==false) yrot-=vlaue;
		if(yrot>=end) 
		{
			a=false;
		}
		else if(yrot<=begin)
		{
			a=true;
		}

		return yrot;
	} 


private:
	float num;
	float mAlpha;
	IrrlichtDevice* device;
};

MyWeapon2::MyWeapon2(irr::IrrlichtDevice * dev)
{
	device=dev;
	 smgr = device->getSceneManager();	// ����������
	 drv=device->getVideoDriver();


	 //Ĭ�ϲ���hlsl
	 usehlsl=false;

	 ps=0;

}

MyWeapon2::~MyWeapon2(void)
{
}

void MyWeapon2::drop()
{
   //��������ж��.��Ϊ���ҽ��ڽ�ɫ�����ڵ���.��ɫɾ��.�������Զ�ɾ����
}


void MyWeapon2::useParticleEffect()
{

	 video::SColor startcolor,endcolor;

	 if(weaponType==1){
		 startcolor=video::SColor(0,0,0,0);
		endcolor=SColor(0,20,20,60);
	 }else if(weaponType==2){
		  startcolor=video::SColor(0,0,0,0);
		endcolor=SColor(0,50,0,0);
	 }else if(weaponType==3){
		 endcolor=SColor(0,40,40,45);
		 startcolor=video::SColor(0,0,0,0);
	 }else if(weaponType==4){
		  startcolor=video::SColor(0,0,0,0);
		 endcolor=SColor( 0,0,40,50);
	 }else if(weaponType==5){
		startcolor=video::SColor(0,50,0,50);
			endcolor=SColor(0,0,40,50);
	 }else if(weaponType==6){
		 startcolor=video::SColor(0,0,0,50);
		 endcolor=SColor(0,0,40,60);
	 }
	

	// ���� ����ϵͳ	

	scene::ISceneNode* pJointNode =weaponNode->getMS3DJointNode("joint2");  //joint4�ǽ���
	if(pJointNode)
	{
		if(ps) pJointNode->removeChild(ps);

		vector3df pos1=pJointNode->getAbsolutePosition();
		vector3df pos2=weaponNode->getMS3DJointNode("joint4")->getAbsolutePosition();

		vector3df dir=pos1-pos2;

		dir.normalize();


		ps = device->getSceneManager()->addParticleSystemSceneNode(false); //����һ������ϵͳ,��ʹ���Զ���ķ�����
		//ps->setPosition(c->getPosition()+vector3df(0,50,0));//λ��
		//ps->setScale(core::vector3df(0.5f,0.5f,0.5f));//����
		//ps->setParticleSize(core::dimension2d<f32>(0.250f, 0.250f));//�������Ӵ�С
	    
	    ps->setParticlesAreGlobal(true);
		
		em=ps->createPointEmitter(dir,20,20,
			startcolor, endcolor,//��ʼ��ɫ�ͽ�����ɫ
			2000,2000   //���ʱ��,800-2000����
			);
		em->setMaxStartSize(core::dimension2d<f32>(4.0f, 4.0f));
		em->setMinStartSize(core::dimension2d<f32>(4.0f, 4.0f));
		////�������ӷ�����
		//em = ps->createBoxEmitter(
		//	core::aabbox3d<f32>(-4,-4,-4,4,4,4),//���ú��ӵ�6������
		//	core::vector3df(0.0f,0.0f,0.0f),//���÷��䷽����ٶ�
		//	500,700,//ÿ����С������80,���100
		//	video::SColor(0,0,0,0), video::SColor(0,255,255,255),//��ʼ��ɫ�ͽ�����ɫ
		//	500,1000);//���ʱ��,800-2000����
		ps->setEmitter(em);//����ϵͳ���÷�����
		em->drop();

		scene::IParticleAffector* paf =ps->createFadeOutParticleAffector();
		ps->addAffector(paf);
		paf->drop();

		ps->setMaterialFlag(video::EMF_LIGHTING, false);
		ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		ps->setMaterialTexture(0, device->getVideoDriver()->getTexture("textures/flare.bmp"));
		ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

		pJointNode->addChild(ps);
	}
}


void MyWeapon2::ChangeWeapon(int id)
{



	//��������meshģ��
	char temp[255]="";
	sprintf(temp,"weapon/sword%d%d.u3d",id,id);
	weaponmesh = smgr->getMesh(temp);

	//������������ڵ�
	weaponNode = smgr->addAnimatedMeshSceneNode( weaponmesh );
	//weaponNode->setScale(core::vector3df(5,5,5));
	weaponNode->setMaterialFlag(video::EMF_LIGHTING, false);//����
	
	weaponNode->setID(101);

	weaponNode->addShadowVolumeSceneNode(0,0,false);
	smgr->setShadowColor(video::SColor(80,0,0,0));

	//weaponNode->setRotation(vector3df(0,180,0));
	//	 core::vector3df jointPos = weaponNode->getAbsolutePosition();
	//weaponNode->setPosition(vector3df(0,-0.5,0));
	
	//����ɲ���,���ں�hlsl����ʵ���л�
	oldMaterialType=weaponNode->getMaterial(0).MaterialType;


//----------------------------------------------------------------------------------------------
//�����ϵĹ���
	//ISceneNode*node = smgr->addBillboardSceneNode(weaponNode, core::dimension2d<f32>(3, 3));
	////node->setPosition(vector3df(0.1,0.10,1.5));
	//node->setMaterialFlag(video::EMF_LIGHTING, false);
	//node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL );
	//node->setMaterialTexture(0, drv->getTexture("weapon/flare.tga"));





}

//
void MyWeapon2::useBillboard(void)
{
	/*
	scene::ISceneNode* pJointNode =weaponNode->getMS3DJointNode("joint2");  //joint4�ǽ���
	if(pJointNode)
	{

		vector3df pos1=pJointNode->getAbsolutePosition();
		vector3df pos2=weaponNode->getMS3DJointNode("joint4")->getAbsolutePosition();

		vector3df start(0,0,5);
		vector3df end(0,0,-15);

		//printf("%3.1f %3.1f %3.1f\n%3.1f %3.1f %3.1f\n ",pos1.X,pos1.Y,pos1.Z,pos2.X,pos2.Y,pos2.Z);
		Plane =  smgr->addPlaneSceneNode(start,end,6,pJointNode)  ;//smgr->addBillboardSceneNode(pJointNode, core::dimension2d<f32>(8, 22));
		//Plane->setPosition(vector3df(0,0,0));
		Plane->setMaterialFlag(EMF_LIGHTING, false);
		Plane->setMaterialFlag(EMF_BACK_FACE_CULLING,false); 
		//node->setRotation(vector3df(90,90,90));
		Plane->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR ); //EMT_TRANSPARENT_ALPHA_CHANNEL
		Plane->setMaterialTexture(0, drv->getTexture("textures/����ҵ�.bmp"));
		

		// load textures for animation
		core::array<video::ITexture*> textures;
		for (s32 g=7; g > 0; --g)
		{
			core::stringc tmp;
			tmp = "textures/scar/scar";
			tmp += g;
			tmp += ".tga";
			video::ITexture* t = drv->getTexture( tmp.c_str() );
			
			textures.push_back(t);
		}

		// create texture animator
		scene::ISceneNodeAnimator* glow = smgr->createTextureAnimator(textures, 100);

		// add the animator
		Plane->addAnimator(glow);

		// drop the animator because it was created with a create() function
		glow->drop();
	
		//1219e2c0.tga

	}*/

}


void MyWeapon2::setUseHLSL(bool b,bool transparentMeta,int weaponType)
{
  usehlsl=b;
  if(b){
	//HLSL();

		//weaponNode->setMaterialTexture(0,device->getVideoDriver()->getTexture("weapon/de8b4a0.tga"));
		 ///rfrr_8.tga ��ɫ��  kdb108_tx.png����Ļ�   ice.png   fire.png    abb113.png kdd108_tx.png��������ı�  kde108_tx.png�����Ƕ�  kdc108_tx.png�����ǵ�

		this->weaponType=weaponType;

	////����hlslЧ��

		//��ɫ
		if(weaponType==1){
			weaponNode->setMaterialTexture(1,device->getVideoDriver()->getTexture("hlsleffect/rfrr_8.tbj"));
		}else if(weaponType==2){
			weaponNode->setMaterialTexture(1,device->getVideoDriver()->getTexture("hlsleffect/kdb108_tx.tbj")); //����Ļ�
		}else if(weaponType==3){
			weaponNode->setMaterialTexture(1,device->getVideoDriver()->getTexture("hlsleffect/kdd108_tx.tbj"));//����ı� 
		}else if(weaponType==4){
			weaponNode->setMaterialTexture(1,device->getVideoDriver()->getTexture("hlsleffect/gg.tbj"));//kde108_tx.tbj ����Ķ�
		}else if(weaponType==5){
			weaponNode->setMaterialTexture(1,device->getVideoDriver()->getTexture("hlsleffect/colorful.tbj"));//7��
		}else if(weaponType==6){
			weaponNode->setMaterialTexture(1,device->getVideoDriver()->getTexture("hlsleffect/a.tbj"));//�� kdc108_tx.png
		}
		
	//	weaponNode->setMaterialTexture(2,device->getVideoDriver()->getTexture("hlsleffect/sword_07_s.tga"));//
  
//weaponNode->setMaterialTexture(3,device->getVideoDriver()->getTexture("hlsleffect/tex.jpg"));//
			
	effect=new LoginHLSLEffect(device);
	weaponNode->setMaterialType((irr::video::E_MATERIAL_TYPE)effect->getMaterialType(transparentMeta));
	
	//����������Ч��
	useParticleEffect();

	//
	 useBillboard();

  }else{
	 weaponNode->setMaterialType((irr::video::E_MATERIAL_TYPE)oldMaterialType);
		  
  }
}


void MyWeapon2::update()
{
	if(weaponType>0 && ps){

		vector3df pos1=weaponNode->getMS3DJointNode("joint4")->getAbsolutePosition();
		vector3df pos2=weaponNode->getMS3DJointNode("joint2")->getAbsolutePosition();
		
		vector3df dir=pos1-pos2;

		dir.normalize();
		dir*=0.007f;
		em->setDirection(dir);

	}

}


}