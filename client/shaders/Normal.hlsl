

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

sampler2D screenSampler : register(s0); 



float4 pixelMain(float2 TexCoord : TEXCOORD0 ) : COLOR0 
{ 
   

   
     float4   Color = tex2D(screenSampler, TexCoord); 


    
    return Color; 
}