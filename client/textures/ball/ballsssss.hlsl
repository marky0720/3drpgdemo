// part of the Irrlicht Engine Shader example.
// These simple Direct3D9 pixel and vertex shaders will be loaded by the shaders
// example. Please note that these example shaders don't do anything really useful. 
// They only demonstrate that shaders can be used in Irrlicht.

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 mWorldViewProj;  // World * View * Projection transformation


// Vertex shader output structure

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0;

};

struct VS_OUTPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord : TEXCOORD0;

};


VS_OUTPUT vertexMain( in float4 vPosition : POSITION,
                    in float3 vNormal   : NORMAL,
                      float2 texCoord     : TEXCOORD0 )
{
VS_OUTPUT    Output;
   Output.Position = mul(vPosition, mWorldViewProj );

   Output.Texcoord = texCoord ;

   return( Output );
}



// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


sampler2D basemap;
sampler2D flameMap;
sampler2D flame2Map;
float fTime;
PS_OUTPUT pixelMain( float2 TexCoord : TEXCOORD0) 
{ 
	PS_OUTPUT Output;
TexCoord.y =TexCoord.y+fTime; 
TexCoord.x =TexCoord.x-fTime; 
float4 col = tex2D( basemap, TexCoord );  // sample color map
TexCoord.x =TexCoord.x-fTime; 
TexCoord.y =TexCoord.y+fTime; 
	float4 col2 = tex2D( flameMap, TexCoord );  // sample color map


   
  // TexCoord.x = TexCoord.x-fTime; 

	//float4 glow=tex2D( flameMap,TexCoord  );

	//TexCoord.y =TexCoord.y+2*fTime; 
	// TexCoord.x = TexCoord.x-fTime; 

	//float4 glow2=tex2D( flame2Map,TexCoord  );
	// multiply with diffuse and do other senseless operations
	//Output.RGBColor =   col+glow+glow2*0.5;
	Output.RGBColor = col+col2;
	Output.RGBColor*=1.2;
	Output.RGBColor.a=0.1;

	return Output;
}

