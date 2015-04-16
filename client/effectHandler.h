#ifndef __SCREEN_ALIGNED_QUAD_HEADER_INCLUDED__ 
#define __SCREEN_ALIGNED_QUAD_HEADER_INCLUDED__ 

#include <irrlicht.h> 

namespace screenEffect
{

	/*
	
	   屏幕效果类
	 
	 */

const s32 winWidth          = 800; 
const s32 winHeight         = 600; 

////在begin和end之间来回的函数


class CBlur : public video::IShaderConstantSetCallBack 
{ 
public: 
   virtual void OnSetConstants(video::IMaterialRendererServices* services, irr::s32 userData) 
   { 
	   //bloom亮度
	//	float time=RandNum(0.0f,0.20f,0.0025f);
	  // float time=0.005f;
	//   services->setPixelShaderConstant("fTime", &time, 1);
   } 

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
		//printf("%3.1f\r",yrot);
		return yrot;
	} 

}; 

class effectHandler : public irr::scene::ISceneNode 
{ 
private: 
   irr::core::aabbox3d<irr::f32>      m_BBox; 
   irr::video::S3DVertex              m_Vertices[4]; 
   irr::video::SMaterial              m_Material; 
	 irr::scene::ISceneManager* smgr;
public: 
   effectHandler(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* smgr, irr::s32 id) 
      : irr::scene::ISceneNode(parent, smgr, id) 
   { 
	   this->smgr=smgr;
      m_Material.Lighting        = false; 
      m_Material.ZWriteEnable    = false; 

      irr::video::SColor vertexColor(255, 0, 0, 255); 
      irr::core::vector3df nullNormalVector(0.0f, 0.0f, 0.0f);//no lightning, 0 normal 

    
      m_Vertices[0] = irr::video::S3DVertex( 
         irr::core::vector3df(-1.0f, 1.0f, 0.0f), 
         nullNormalVector, 
         vertexColor, 
         irr::core::vector2df(0.0f, 0.0f)); 
      m_Vertices[1] = irr::video::S3DVertex( 
         irr::core::vector3df(1.0f, 1.0f, 0.0f), 
         nullNormalVector, 
         vertexColor, 
         irr::core::vector2df(1.0f, 0.0f)); 
      m_Vertices[2] = irr::video::S3DVertex( 
         irr::core::vector3df(1.0f, -1.0f, 0.0f), 
         nullNormalVector, 
         vertexColor, 
         irr::core::vector2df(1.0f, 1.0f)); 
      m_Vertices[3] = irr::video::S3DVertex( 
         irr::core::vector3df(-1.0f, -1.0f, 0.0f), 
         nullNormalVector, 
         vertexColor, 
         irr::core::vector2df(0.0f, 1.0f)); 

      m_BBox.reset(m_Vertices[0].Pos); 
      for(irr::s32 i = 0; i < 4; i++) 
         m_BBox.addInternalPoint(m_Vertices[i].Pos); 
   } 

   void OnRegisterSceneNode() 
   { 
      if(IsVisible) 
         SceneManager->registerNodeForRendering(this); 
      irr::scene::ISceneNode::OnRegisterSceneNode(); 
   } 

   void render() 
   { 
      irr::u16 indices[6] = {0, 2, 3,   0, 1, 2}; 
      irr::video::IVideoDriver* driver = SceneManager->getVideoDriver(); 
      driver->setMaterial(m_Material); 
      driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation); 
      driver->drawIndexedTriangleList(&m_Vertices[0], 4, &indices[0], 2); 
   } 

   const irr::core::aabbox3d<irr::f32>& getBoundingBox() const 
   { 
      return m_BBox; 
   } 

   video::SMaterial& getMaterial(irr::u32 num) 
   { 
      return m_Material; 
   } 

   void initEffect(char* shaderBlur01Filename ,bool trans)
   {
	   if(shaderBlur01Filename){
		 video::IGPUProgrammingServices* gpu =  smgr->getVideoDriver()->getGPUProgrammingServices(); 

		 //blur模糊
		 blur = new CBlur(); 
		 if(trans){
			 // c8* shaderBlur01Filename = "shaders/Blur.hlsl"; 
			 shaderBlur01Material = gpu->addHighLevelShaderMaterialFromFiles( 
				 shaderBlur01Filename, "vertexMain", video::EVST_VS_1_1, 
				 shaderBlur01Filename, "pixelMain", video::EPST_PS_2_0, 
				 blur, video::EMT_TRANSPARENT_ADD_COLOR); 
			 blur->drop(); 
		 }else{
			 // c8* shaderBlur01Filename = "shaders/Blur.hlsl"; 
			 shaderBlur01Material = gpu->addHighLevelShaderMaterialFromFiles( 
				 shaderBlur01Filename, "vertexMain", video::EVST_VS_1_1, 
				 shaderBlur01Filename, "pixelMain", video::EPST_PS_2_0, 
				 blur, video::EMT_SOLID); 
			 blur->drop(); 
		}
		
	 this->getMaterial(0).MaterialType = (video::E_MATERIAL_TYPE)this->shaderBlur01Material; 
       
	   }

		//
		rt = smgr->getVideoDriver()->addRenderTargetTexture(core::dimension2d<u32>(winWidth, winHeight)); 


		//材质
	//	m_Material.MaterialType = (video::E_MATERIAL_TYPE)shaderBlur01Material; 
		m_Material.Wireframe = false;
		m_Material.Lighting = false;
		m_Material.TextureLayer[0].Texture=this->rt; 
	m_Material.TextureLayer[0].TextureWrap = video::ETC_CLAMP; 

		/*this->getMaterial(0).TextureLayer[0].Texture  = this->rt; */

		//smgr->getVideoDriver()->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true); 
   }

   CBlur* blur;
   	video::ITexture* rt ;
   s32		shaderBlur01Material;
}; 


} // namespace screenEffect


#endif