

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
    float blurOffset = 0.045100; 
    
    float2 TexelKernel[8] = 
    { 
       { -4, 0 }, 
       { -3, 0 }, 
       { -2, 0 }, 
       { -1, 0 }, 
       {  1, 0 }, 
       {  2, 0 }, 
       {  3, 0 }, 
       {  4, 0 }, 
    }; 

    float BlurWeights[8] = 
    { 
        0.002216, 
        0.008764, 
        0.026995, 
        0.064759, 
        0.064759, 
        0.026995, 
        0.008764, 
        0.002216, 
    }; 

    float4 Color = 0; 

    for (int i = 0; i < 8; i++) 
    {    
        Color += tex2D(screenSampler, TexCoord + (TexelKernel[i].yx * blurOffset) * BlurWeights[i]); 
        Color += tex2D(screenSampler, TexCoord + (TexelKernel[i].xy * blurOffset) * BlurWeights[i]); 
    } 
    
    Color /= 16; 
    
    Color.g *= 2;//i am bleeding :) 
    Color *= (1.2);//bloom? 
    
    return Color; 
}