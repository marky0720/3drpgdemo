// part of the Irrlicht Engine Shader example.
// These simple Direct3D9 pixel and vertex shaders will be loaded by the shaders
// example. Please note that these example shaders don't do anything really useful. 
// They only demonstrate that shaders can be used in Irrlicht.

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 mWorldViewProj;  // World * View * Projection transformation
float4 mColor;       // Light color

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
float4 Diffuse    : COLOR0;     // vertex diffuse color
};


VS_OUTPUT vertexMain( in float4 vPosition : POSITION,
                   
                      float2 texCoord     : TEXCOORD0 )
{
VS_OUTPUT    Output;
   Output.Position = mul(vPosition, mWorldViewProj );

Output.Diffuse=mColor;
   Output.Texcoord = texCoord ;

   return( Output );
}



// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


sampler2D basemap;

float fTime;
PS_OUTPUT pixelMain( float2 TexCoord : TEXCOORD0 ,  float4 Diffuse  : COLOR0 ) 
{ 
	PS_OUTPUT Output;

	float4 col = tex2D( basemap, TexCoord );  // sample color map


	Output.RGBColor =   (col*Diffuse)*fTime;
	
	Output.RGBColor.a=1;

	return Output;
}

