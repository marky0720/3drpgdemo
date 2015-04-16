#include ".\mycharmagiceffect.h"


namespace Magic
{


MyCharMagicEffect1::MyCharMagicEffect1( )
{
	this->dev=getIrrDevice();

	parentNode=0;
	node=0;

	pos=vector3df(0,0,0);
	speed=0;

	drv=dev->getVideoDriver();
	ui=dev->getGUIEnvironment();
	smgr=dev->getSceneManager();

	mAlpha=1.0f;
	num=0.1f;
}

MyCharMagicEffect1::~MyCharMagicEffect1(void)
{
}

void MyCharMagicEffect1::setParentNode(ISceneNode* parentNode)
{
	this->parentNode=parentNode;
}

void MyCharMagicEffect1::update()
{
	if( parentNode && node) node->setPosition(parentNode->getAbsolutePosition()+pos);

}

bool MyCharMagicEffect1::init(ISceneNode* parentNode,vector3df pos ,float speed, ITexture* tex)
{
	this->parentNode=parentNode;
	this->pos=pos;
	this->speed=speed;

	if(tex) this->tex=tex;

	int weaponType=1;
	//	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("model/plane/plane.ms3d"));
	
		node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("model/circle/circle.ms3d"));

		scene::ISceneNode* pJointNode =node->getMS3DJointNode("joint4");  //weapon是手 
   	    if(pJointNode)
		{
	

			//移除武器节点
			pJointNode->removeAll();


			////  attach billboard to light
			IBillboardSceneNode*		billnode = smgr->addBillboardSceneNode( 0 , core::dimension2d<f32>(1.5, 1.5));
			//billnode->setPosition(vector3df(0,50,0));
			billnode->setMaterialFlag(video::EMF_LIGHTING, false);
			billnode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
			billnode->setMaterialTexture(0, drv->getTexture("textures//fire.bmp"));
			billnode->setMaterialFlag(EMF_ZWRITE_ENABLE ,false); //关闭背面剔除
			pJointNode->addChild(billnode);

			
			video::SColor startcolor,endcolor;

			if(weaponType==1){
				startcolor=video::SColor(0,0,0,0);
				endcolor=SColor(0,20,20,60);
			}else if(weaponType==2){
				startcolor=video::SColor(0,0,0,0);
				endcolor=SColor(0,80,0,0);
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



			////创建盒子发射器
			scene::IParticleEmitter* em;
			
			ps = getIrrDevice()->getSceneManager()->addParticleSystemSceneNode(false); //创建一个粒子系统,并使用自定义的发射器
			//ps->setPosition(c->getPosition()+vector3df(0,50,0));//位置
			//ps->setScale(core::vector3df(0.5f,0.5f,0.5f));//缩放
			//ps->setParticleSize(core::dimension2d<f32>(0.250f, 0.250f));//设置粒子大小
			ps->setParticlesAreGlobal(true);

			em=ps->createPointEmitter(vector3df(0,0.0f,0),300,300,
				startcolor, endcolor,//开始颜色和结束颜色
				2000,2000   //存活时间,800-2000毫秒
				);
			em->setMaxStartSize(core::dimension2d<f32>(2.0f, 2.0f));
			em->setMinStartSize(core::dimension2d<f32>(2.0f, 2.0f));
			////创建盒子发射器
			//em = ps->createBoxEmitter(
			//	core::aabbox3d<f32>(-4,-4,-4,4,4,4),//设置盒子的6个顶点
			//	core::vector3df(0.0f,0.0f,0.0f),//设置发射方向和速度
			//	500,700,//每秒最小粒子数80,最大100
			//	video::SColor(0,0,0,0), video::SColor(0,255,255,255),//开始颜色和结束颜色
			//	500,1000);//存活时间,800-2000毫秒
			ps->setEmitter(em);//粒子系统设置发射器
			em->drop();

			scene::IParticleAffector* paf =ps->createFadeOutParticleAffector();
			ps->addAffector(paf);
			paf->drop();

			ps->setMaterialFlag(video::EMF_LIGHTING, false);
			ps->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
			ps->setMaterialTexture(0, getIrrDevice()->getVideoDriver()->getTexture("textures/flare.bmp"));
			ps->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
			ps->setMaterialType((video::E_MATERIAL_TYPE)get());

			pJointNode->addChild(ps);

		} 

		node->setPosition(pos);
		node->setScale(vector3df(8,0,8));
		//node->setMaterialFlag(video::EMF_LIGHTING, false);
		//node->setMaterialFlag(video::EMF_ZBUFFER, false); //不用深度,即透明显示,不会被遮挡
		//node->setMaterialFlag(EMF_BACK_FACE_CULLING ,false); //关闭背面剔除
		//node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
		
		//node->setRotation(vector3df(90,0,0));
	
		scene::ISceneNodeAnimator* anim;
		anim = smgr->createRotationAnimator(core::vector3df(0,speed,0));  //绕Y轴旋转 速度0.3
		node->addAnimator(anim);
		anim->drop();	


		node->setMaterialType((video::E_MATERIAL_TYPE)get());




	return true;
}

void MyCharMagicEffect1::drop()
{
	if(node) {
		//scene::ISceneNodeAnimator* anim = 0;

		node->removeAnimators();
		node->removeAll();
		node->remove();



		//node->addAnimator(anim=smgr->createDeleteAnimator(1));//创建删除动作,删除自己

		//anim->drop();
	}
}



void MyCharMagicEffect1::OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
{

	// set inverted world matrix(设置世界的逆矩)
	// if we are using highlevel shaders (the user can select this when(如果我们要用高级着色渲染,我们必须设置constants的名字)
	// starting the program), we must set the constants by name.


	// set clip matrix 设置剪裁矩阵
	core::matrix4 worldViewProj;
	worldViewProj = drv->getTransform(video::ETS_PROJECTION);			
	worldViewProj *= drv->getTransform(video::ETS_VIEW);
	worldViewProj *= drv->getTransform(video::ETS_WORLD);


	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);


	num=RandNum2(0.1f,0.90f,0.0025);
	video::SColorf col(0,1-num,num,0.0f);

	//		num=RandNum2(0.1f,2.0f,0.0025);
	//		video::SColorf col(num,1.0f-num,num,0.0f);
	//if(num>1.0f){
	//            col=SColorf(2-num,2.0f-num,num-1,0.0f);
	//}else{
	//	col=SColorf(num,1.0f-num,num,0.0f);
	//}

	//video::SColorf col(1,1,1,0.0f);
	//printf("%3.1f ",num);
	services->setVertexShaderConstant("mColor", reinterpret_cast<f32*>(&col), 4);


	//
	//mAlpha=0.30f;
	mAlpha=num;
	

	services->setPixelShaderConstant("fTime", reinterpret_cast<f32*>(&mAlpha), 1);



}

//在begin和end之间来回的函数
float MyCharMagicEffect1::RandNum2(float  begin,float end,float vlaue)
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

s32 MyCharMagicEffect1::get(){


	c8* vsFileName = 0; // filename for the vertex shader
	c8* psFileName = 0; // filename for the pixel shader

	video::E_DRIVER_TYPE driverType ;
	driverType=drv->getDriverType();

	switch(driverType)
	{
	case video::EDT_DIRECT3D9:
		psFileName = "model/plane/plane.hlsl";
		vsFileName = psFileName; // both shaders are in the same file
		break;
	default:
		return 0;
		break;
	}

	//检查硬件是否支持着色
	if (!drv->queryFeature(video::EVDF_PIXEL_SHADER_1_1) &&
		!drv->queryFeature(video::EVDF_ARB_FRAGMENT_PROGRAM_1))
	{
		dev->getLogger()->log("WARNING: Pixel shaders disabled "\
			"because of missing driver/hardware support.");
		psFileName = 0;
	}

	if (!drv->queryFeature(video::EVDF_VERTEX_SHADER_1_1) &&
		!drv->queryFeature(video::EVDF_ARB_VERTEX_PROGRAM_1))
	{
		dev->getLogger()->log("WARNING: Vertex shaders disabled "\
			"because of missing driver/hardware support.");
		vsFileName = 0;
	}


	s32 newMaterialType1;

	// create materials

	video::IGPUProgrammingServices* gpu = drv->getGPUProgrammingServices();
	if (gpu)
	{

		newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
			psFileName, "vertexMain", video::EVST_VS_2_0,
			psFileName, "pixelMain", video::EPST_PS_2_0,
			this, video::EMT_TRANSPARENT_ADD_COLOR);//EMT_TRANSPARENT_ADD_COLOR EMT_SOLID

	}


	return newMaterialType1;
}


} // namespace Magic