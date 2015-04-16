float4x4 mWorldViewProj;





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

VS_OUTPUT vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   Output.Position = mul( Input.Position, mWorldViewProj );

   Output.Texcoord = Input.Texcoord;

   return( Output );
   
}




sampler2D baseMap;
sampler2D base2Map;

float fTime;

struct PS_INPUT 
{
   float2 Texcoord : TEXCOORD0;

};

struct PS_OUTPUT 
{
 vector color : COLOR0;
};


PS_OUTPUT ps_main( PS_INPUT Input ) 
{
   PS_OUTPUT output;
   output.color=float4(0,0,0,0);
  
 //float4 a=tex2D( base2Map,Input.Texcoord  );
   Input.Texcoord.y = Input.Texcoord.y-fTime*0.2; 
//   Input.Texcoord.x = Input.Texcoord.x-fTime; 
   float4 c=tex2D( baseMap,Input.Texcoord  );

output.color=c;

//  output.color*=0.80;

   return output;
}
