#version 420 core                                                              
                                           
layout(binding = 0)uniform sampler2D tex;                                       
layout(binding = 1)uniform sampler2D noise_tex;
layout(binding = 2)uniform sampler2D control_tex;
layout(binding = 3)uniform sampler2D groupTeam_tex;
uniform int mapFlag;                
uniform vec2 mainPosition;
uniform vec2 mapSize;             
uniform float screenWidth;
uniform float screenHeight; 
uniform int isPressButton[6]; 
uniform float buttonListY[6];
uniform float buttonX; 
uniform float colorFactor;
uniform int switchRbo;
uniform int switchToon;
uniform int switchsmallmap;
uniform int screenGroupTeam;
out vec4 color;                                                                
                                                                              
in VS_OUT                                                                      
{                                                                              
    vec2 texcoord;                                                             
} fs_in;        

vec4 rbs(sampler2D finalTex,float colorFactor,float aa,float cc,float bb)
{
	float offset = 0.005;
	vec4 left_tex = texture(finalTex,vec2((fs_in.texcoord.x-aa)/bb,(fs_in.texcoord.y-cc)/bb)-vec2(offset,0));
	vec4 right_tex = texture(finalTex,vec2((fs_in.texcoord.x-aa)/bb,(fs_in.texcoord.y-cc)/bb)+vec2(offset,0));
	float r = left_tex.r*0.299+left_tex.g*0.587+left_tex.b*0.114;
	float g = right_tex.g;
	float b = right_tex.b;
	return vec4(r*colorFactor,g*colorFactor,b*colorFactor,1);
}

vec4 blur_qual_dog(sampler2D finalTex,float aa,float cc,float bb)
{
	float sigma_e = 2.0f;
	float sigma_r = 2.8f;
	float phi = 3.4f;
	float tau = 0.99f;
	float twoSigmaESquared = 2.0 * sigma_e * sigma_e;
	float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;
	int halfWidth = int(ceil( 2.0 * sigma_r ));

	vec4 color1 = vec4(0);
	int n =0;
	int half_size = 3; //filter_half_size
	for ( int i = -half_size; i <= half_size ; ++i ) 
	{
		for ( int j = -half_size; j <= half_size ; ++j ) 
		{
			vec4 c= texture(finalTex,vec2((fs_in.texcoord.x-aa)/bb,(fs_in.texcoord.y-cc)/bb) +vec2(i,j)/vec2(screenWidth,screenHeight));
			color1+= c;
			n++;
		}
	}
	color1 /=n;

	int nbins = 8;
	vec4 tex_color = texture(finalTex,vec2((fs_in.texcoord.x-aa)/bb,(fs_in.texcoord.y-cc)/bb));
	float r= floor(tex_color.r * float(nbins)) / float(nbins);
	float g= floor(tex_color.g * float(nbins)) / float(nbins);
	float b = floor(tex_color.b * float(nbins)) / float(nbins);
	vec4 color2 = vec4(r,g,b,tex_color.a);

	vec2 sum = vec2(0.0);
	vec2 norm = vec2(0.0);
	for ( int i= -halfWidth; i<= halfWidth; ++i) {
	for ( int j = -halfWidth; j <= halfWidth; ++j ) {
	float d = length(vec2(i,j));
	vec2 kernel= vec2( exp( -d * d / twoSigmaESquared),
	exp( -d * d / twoSigmaRSquared));
	vec4 c= texture(finalTex,vec2((fs_in.texcoord.x-aa)/bb,(fs_in.texcoord.y-cc)/bb)+vec2(i,j)/vec2(screenWidth,screenHeight));
	vec2 L= vec2(0.299 * c.r+ 0.587 * c.g+ 0.114 * c.b);
	norm += 2.0 * kernel;
	sum += kernel * L;}
	}
	sum /= norm;
	float H = 100.0 * (sum.x-tau * sum.y);
	float edge =( H > 0.0 )?1.0:2.0 *smoothstep(-2.0, 2.0, phi * H );
	vec4 color3 = vec4(edge,edge,edge,1.0 );

	return (color1+color2)/2*color3;
}

void main(void)                                                                
{
	vec2 s = vec2(mainPosition.x/mapSize.x+0.5,mainPosition.y/mapSize.y+0.5);
	
	switch(mapFlag)
	{
		case 1:
			if(distance(s,fs_in.texcoord)<0.005&&mapFlag!=0)color = vec4(1,0,0,1);
			else color = texture(noise_tex,fs_in.texcoord);
		break;
		case 2:
			if(fs_in.texcoord.y>0.1&&fs_in.texcoord.y<0.6&&fs_in.texcoord.x>0.4&&fs_in.texcoord.x<0.9)
			{
				if(switchRbo==1)color = rbs(screenGroupTeam==1?groupTeam_tex:tex,colorFactor,0.4,0.1,0.5);
				else if(switchToon==1)color = blur_qual_dog(screenGroupTeam==1?groupTeam_tex:tex,0.4,0.1,0.5);
				else color = vec4(texture(screenGroupTeam==1?groupTeam_tex:tex,vec2((fs_in.texcoord.x-0.4)/0.5,(fs_in.texcoord.y-0.1)/0.5)).rgb*colorFactor,1.0);
			}
			else
				color = texture(control_tex,fs_in.texcoord);
			for(int i=0;i<6;i++)
			{
				if(isPressButton[i]==1)
				{
					float realY = gl_FragCoord.y;
					float realX = gl_FragCoord.x;
					float dy = (buttonListY[i]+12)*screenHeight/720;
					float uy = (buttonListY[i]+61)*screenHeight/720;
					float dx = buttonX*screenWidth/1280;
					float ux = (buttonX+53)*screenWidth/1280;
					if(dy<realY&&realY<uy&&dx<realX&&realX<ux)
					{
						color = vec4(1,0,0,1);
					}
				}
			}
		break;
		default:
			vec4 smallmapcolor = vec4(0);
			if(switchRbo==1)color = rbs(screenGroupTeam==1?groupTeam_tex:tex,colorFactor,0,0,1);
			else if(switchToon==1)color = blur_qual_dog(screenGroupTeam==1?groupTeam_tex:tex,0,0,1);
			else color = vec4(texture(screenGroupTeam==1?groupTeam_tex:tex,fs_in.texcoord).rgb*colorFactor,1.0);

			if(switchsmallmap==1&&fs_in.texcoord.y>0.1&&fs_in.texcoord.y<0.4&&fs_in.texcoord.x>0.65&&fs_in.texcoord.x<0.95)
			{
				vec2 centerpos = vec2((fs_in.texcoord.x-0.65)/0.3,(fs_in.texcoord.y-0.1)/0.3)+s-vec2(0.5,0.5);
				centerpos = centerpos/5 + s*4/5;
				smallmapcolor = texture(noise_tex,centerpos);
				vec2 centersmallmap = vec2(0.8,0.25);
				if(distance(fs_in.texcoord,centersmallmap)<0.005)smallmapcolor=vec4(1,0,0,1);
				color = smallmapcolor*0.5+color*0.5;
			}
		break;
	}
}                        