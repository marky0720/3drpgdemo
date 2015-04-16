#pragma once

#include "../stdafx.h"

/************************************************************************/
// 
//		 hlsl效果.用于武器光效的
//		
//
/************************************************************************/
namespace Login
{



class LoginHLSLEffect : public video::IShaderConstantSetCallBack
{
public:
	
	~LoginHLSLEffect(void);

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData)
	{
		IVideoDriver* drv=device->getVideoDriver();


		// set clip matrix 设置剪裁矩阵
		core::matrix4 mWorldViewProj;
		mWorldViewProj = drv->getTransform(video::ETS_PROJECTION);			
		mWorldViewProj *= drv->getTransform(video::ETS_VIEW);
		mWorldViewProj *= drv->getTransform(video::ETS_WORLD);
		services->setVertexShaderConstant("mWorldViewProj", mWorldViewProj.pointer(), 16);




		if (drv->getDriverType() == video::EDT_OPENGL) 
		{
	//		int d[] = {  0,1  }; // sampler2d IDs
	//		services->setPixelShaderConstant("baseMap", (float*)&d[0], 1);
	//		services->setPixelShaderConstant("flameMap", (float*)&d[1], 1);
		}


		float b=1.0;
		b=RandNum(1.0f,1.25f,0.0025f);
	    services->setPixelShaderConstant("bright", &b, 1);
	    
		services->setPixelShaderConstant("fTime", &time, 1);
		time+=0.001f;

	}
	LoginHLSLEffect(irr::IrrlichtDevice * dev){
		device=dev;
		 drv=dev->getVideoDriver();

		  num=0.1f;
		 time = 0.0f;
		
	}

	//
	s32 getMaterialType(bool istransparent){
	
		device->getVideoDriver();
		c8* vsFileName = 0; // filename for the vertex shader
		c8* psFileName = 0; // filename for the pixel shader

		video::E_DRIVER_TYPE driverType ;
		driverType=drv->getDriverType();

		switch(driverType)
		{
		case video::EDT_DIRECT3D9:
			psFileName = "hlsleffect/weapon.hlsl";
			vsFileName = psFileName; // both shaders are in the same file
			break;
		default:
			return NULL;
			break;
		}

		//检查硬件是否支持着色
		if (!drv->queryFeature(video::EVDF_PIXEL_SHADER_2_0) &&
			!drv->queryFeature(video::EVDF_ARB_FRAGMENT_PROGRAM_1))
		{
			device->getLogger()->log("WARNING: Pixel shaders disabled "\
				"because of missing driver/hardware support.");
			psFileName = 0;
		}

		if (!drv->queryFeature(video::EVDF_VERTEX_SHADER_1_1) &&
			!drv->queryFeature(video::EVDF_ARB_VERTEX_PROGRAM_1))
		{
			device->getLogger()->log("WARNING: Vertex shaders disabled "\
				"because of missing driver/hardware support.");
			vsFileName = 0;
		}

		// create materials

		video::IGPUProgrammingServices* gpu = drv->getGPUProgrammingServices();
		if (gpu)
		{
			
			if(istransparent)
			{
				newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
					getWeaponHLSLFile(device->getFileSystem()), "vs_main", video::EVST_VS_1_1,
					getWeaponHLSLFile(device->getFileSystem()), "ps_main", video::EPST_PS_2_0,
					this, video::EMT_TRANSPARENT_ADD_COLOR   );  //EMT_SOLID EMT_TRANSPARENT_ADD_COLOR
			}else
			{
				newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
					getWeaponHLSLFile(device->getFileSystem()), "vs_main", video::EVST_VS_1_1,
					getWeaponHLSLFile(device->getFileSystem()), "ps_main", video::EPST_PS_2_0,
					this, video::EMT_SOLID   );  //EMT_SOLID EMT_TRANSPARENT_ADD_COLOR
			}
			

			this->drop();
		}

		return newMaterialType1;
		//给武器加上材质
		//weaponNode->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);
	}




private:

	//在begin和end之间来回的函数
	float RandNum(float  begin,float end,float vlaue)
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


 float num;
   irr::video::IVideoDriver *drv; 
	IrrlichtDevice* device;
	 f32 time;
	s32 newMaterialType1;
};

}
