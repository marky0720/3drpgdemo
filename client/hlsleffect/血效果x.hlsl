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
sampler2D flameMap;
//sampler2D glowMap;


float fTime;
float bright;

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

  PS_OUTPUT blue;
    blue.color=float4(0,0,220,0);

   float4 base=tex2D( baseMap, Input.Texcoord );


  // float4 glow=tex2D( glowMap,Input.Texcoord  );
 
   Input.Texcoord.y = Input.Texcoord.y-fTime; 
   Input.Texcoord.x = Input.Texcoord.x-fTime; 
   float4 color2=tex2D( flameMap,Input.Texcoord  );

		//float4 temp;
   	 	// output.color=base ;//+glow;
		 //output.color.r*=2;
		//output.color*=glow; //相与
 		//output.color*=2.5; //变亮
		//temp=output.color; //temp现在是边缘颜色
		
	
		//output.color=base+color2+temp;

		
		//color2-=color2*glow;//*1.5;
		//base -=base*glow ;//*1.5;

		//	temp=output;
		

		
		//output.color=base + color2  + base*glow*0.2;//+glow;
		
		output.color=base * color2;

   if(output.color.x<150 && output.color.y<150 && output.color.z<150){
   	  
	// output.color+=glow;
   	// output.color.r+=color2;
	//output.color.r=255;
	//output.color.g=0;
	//output.color.b=0;

	//output.color+=color2 ;
	

   }else{
     // Input.Texcoord.y = Input.Texcoord.y+fTime*2; 
  	// Input.Texcoord.x = Input.Texcoord.x-fTime*3; 

    	//output.color+=blue ;
		
   }
  output.color*=0.70;
  output.color*=bright;
   return output;
}
