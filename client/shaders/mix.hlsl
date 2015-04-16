

struct VS_OUTPUT 
{ 
    float4 Pos      : POSITION; 
    float2 TexCoord : TEXCOORD0; 
}; 

VS_OUTPUT vertexMain(in float4 pos      : POSITION, 
                     in float2 texCoord : TEXCOORD) 
{ 
    VS_OUTPUT Out; 
    
    Out.Pos      = pos; 
    Out.TexCoord = texCoord; 

    return Out; 
} 

sampler2D screenSampler ;
sampler2D g ;


float4 pixelMain(float2 TexCoord : TEXCOORD0 ) : COLOR0 
{ 
   

 
     float4   Color = tex2D(screenSampler, TexCoord); 
      float4   Color2 = tex2D(g, TexCoord); 
 //if(Color2.r!=0 && Color2.g!=0&& Color2.b!=0) 
 //   Color =Color+Color2 ;
 // Color=float4(255,255,0,0)+Color;


    return Color; 
}