#version 430


struct PointLight
{
	vec4 mLightColor;
	vec3 mPosition;
	float mRange;
	vec3 mAtten;
};

struct DirectionalLight
{
	vec4 mLightColor;
	vec3 mDirection;
};

struct SpotLight
{
	vec4 mLightColor;
	vec3 mPosition;
	float mRange;
	vec3 mDirection;
	float mSpot;
	vec3 mAtten;
	float mPadding;
};

struct Material
{
	vec3 mDiffuseColor;
	float mSpecularFactor;
	float mSpecularPower;
};

uniform bool u_ActiveOverSampling;
uniform bool u_bInterPolate;
uniform bool u_bTricubicInterPolate;
uniform bool u_OnlyApplyLightIntensity;
uniform bool u_ApplyParticleSpecular;
uniform bool u_CheckLightDirection;

uniform float u_Tension;

in vec2 v_TexPos;

layout(location=0) out vec4 FragColor;

uniform sampler2D gFragColor;
uniform sampler2D gBrightColor;
uniform sampler2D gWorldPosition;
uniform sampler2D gWorldNormal;
uniform sampler2D gViewDir;
uniform sampler2D gSpecData;
uniform DirectionalLight dirLight;
uniform sampler3D u_WorldParticleLightDirection;
uniform sampler3D u_WorldParticleLightColor;

vec3 CalcDirLight(DirectionalLight light,Material mate, vec3 normal, vec3 viewDir)
{
	// Direction Light의 경우 조명이 향하는 방향을 정의하고 있다.
	// 따라서 빛으로 향하는 벡터를 만들기 위해 역수로 설정한다.
    vec3 lightDir = -light.mDirection; 
    // diffuse shading
    float diffFactor = max(dot(normal, lightDir), 0.0);
    // specular shading
	vec3 Hvector = normalize(lightDir.xyz + viewDir.xyz).xyz;
	float NdotH = max(dot(Hvector,normal ) ,0);

    float specFactor =  pow(NdotH, mate.mSpecularPower);
    
	// combine results
	vec3 diffColor = mate.mDiffuseColor.xyz * diffFactor;
	vec3 specColor = vec3(1,1,1) * specFactor;
	vec3 resultColor = diffColor + specColor;

    return max(resultColor,vec3(0,0,0));
}

vec3 CalcPointLight(PointLight light,Material mate, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.mPosition - fragPos);
    // diffuse shading
    float diffFactor = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float specFactor = mate.mSpecularFactor*pow(max(dot(viewDir, reflectDir), 0.0), mate.mSpecularPower);
    // attenuation
    float dist = length(light.mPosition - fragPos);
    float attenuation = 1.0 / (light.mAtten.x + light.mAtten.y * dist + light.mAtten.z * (dist * dist));    
    // combine results
    diffFactor *= attenuation;
    specFactor *= attenuation;

    vec3 diffColor = mate.mDiffuseColor * diffFactor;
	vec3 specColor = vec3(mate.mSpecularFactor) * specFactor;

    return (diffColor + specColor);
}

vec4 OverSampling3DTexture(sampler3D _tex, vec3 _coord, float _texel_size)
{
	vec4 result;

	for(int i = -1 ; i<=1 ;++i )
	{
		for(int j =-1 ; j<=1;++j)
		{
			for(int k =-1 ; k<=1;++k)
			{
				result += texture (_tex,_coord + vec3(i,j,k)*_texel_size);
			}
		}
	}
	result *= 0.037; // 1/27

	return result;
}

// t, ( i-1, i , i+1, i+2 )
float CubicInterPolation(float t, vec4 value)
{

	//http://graphics.cs.cmu.edu/nsp/course/15-462/Fall04/assts/catmullRom.pdf
	float ten =  u_Tension; // tension
	//mat4 catMullMatrix = mat4(
	//0,			1,				0,				0,
	//-ten,			0,				ten,			0,
	//2*ten,		ten - 3,		3- 2*ten,		-ten,
	//-ten,			2 - ten,		ten - 2,		ten
	//);

	vec4 delta = vec4(1, t ,t * t , t * t * t);

	float result = (-.5f * ten * delta.w + ten * delta.z - .5f * ten * delta.y) * value.x +
    (1 + .5f * delta.z * (ten - 6) + .5f * delta.w * (4 - ten)) * value.y +
    (.5f * delta.w * (ten - 4) + .5f * ten * delta.y - (ten - 3) * delta.z) * value.z +
    (-.5f * ten * delta.z + .5f * ten * delta.w) * value.w;
 

	return result;
}

vec4 TrilinearInterpolation(sampler3D _tex, vec3 _texCoord ,vec3 _leftBottomCoord, float _texelSize, vec3 _t)
{
	// calculate t value for interpolation

	vec4 VFI[2][2][2];

	for(int i =0;i<2;++i)
	{
		for(int j =0;j<2;++j)
		{
			for(int k =0;k<2;++k)
			{
				vec3 new_coord = _leftBottomCoord+vec3((i)*_texelSize,(j)*_texelSize,(k)*_texelSize);
				VFI[i][j][k] = (u_ActiveOverSampling) ?
				OverSampling3DTexture(_tex,new_coord,_texelSize) : 
				texture (_tex,new_coord);
			}
		}
	}

	// Trilinear interpolation
	// https://en.wikipedia.org/wiki/Trilinear_interpolations
	vec4 c00, c01, c10, c11;
	

	c00 = mix(VFI[0][0][0], VFI[1][0][0],_t.x );
	c01 = mix(VFI[0][0][1], VFI[1][0][1],_t.x );
	c10 = mix(VFI[0][1][0], VFI[1][1][0],_t.x );
	c11 = mix(VFI[0][1][1], VFI[1][1][1],_t.x );
	
	vec4 c0 , c1;
	c0 = mix(c00,c10,_t.y);
	c1 = mix(c01,c11,_t.y);

	return mix(c0,c1, _t.z);
}

// TriCubic Interpolation using cat-mullrom spline interpolation
//http://www.cds.caltech.edu/~marsden/bib/2005/08-LeMa2005/LeMa2005.pdf
vec4 TriCubicInterpolation(sampler3D _tex, vec3 _texCoord ,vec3 _leftBottomCoord, float _texelSize, vec3 _t)
{

	vec4 s[4][4][4];
	vec4 t[4][4];
	vec4 u[4];
	for(int i =0;i<4;++i)
	{
		for(int j =0;j<4;++j)
		{
			for(int k =0;k<4;++k)
			{
				vec3 new_coord = _leftBottomCoord +  vec3((i-1)*_texelSize,(j-1)*_texelSize,(k-1)*_texelSize);
				s[i][j][k] = (u_ActiveOverSampling) ? 
				OverSampling3DTexture(_tex,new_coord,_texelSize): 
				texture (_tex,new_coord);
			}
		}
	}

	for(int i =0;i<4;++i)
	{
		for(int j =0;j<4;++j)
		{
			t[i][j].x = CubicInterPolation(_t.z,
			vec4(s[i][j][0].x, s[i][j][1].x, s[i][j][2].x, s[i][j][3].x) );
			t[i][j].y = CubicInterPolation(_t.z,
			vec4(s[i][j][0].y, s[i][j][1].y, s[i][j][2].y, s[i][j][3].y) );
			t[i][j].z = CubicInterPolation(_t.z,
			vec4(s[i][j][0].z, s[i][j][1].z, s[i][j][2].z, s[i][j][3].z) );
			t[i][j].w = CubicInterPolation(_t.z,
			vec4(s[i][j][0].w, s[i][j][1].w, s[i][j][2].w, s[i][j][3].w) );
		}
	}

	for(int i =0;i<4;++i)
	{
		u[i].x = CubicInterPolation(_t.y, vec4(t[i][0].x,t[i][1].x,t[i][2].x,t[i][3].x) );
		u[i].y = CubicInterPolation(_t.y, vec4(t[i][0].y,t[i][1].y,t[i][2].y,t[i][3].y) );
		u[i].z = CubicInterPolation(_t.y, vec4(t[i][0].z,t[i][1].z,t[i][2].z,t[i][3].z) );
		u[i].w = CubicInterPolation(_t.y, vec4(t[i][0].w,t[i][1].w,t[i][2].w,t[i][3].w) );
	}
	vec4 result = vec4(1);

	result.x = CubicInterPolation(_t.x, vec4(u[0].x,u[1].x,u[2].x,u[3].x));
	result.y = CubicInterPolation(_t.x, vec4(u[0].y,u[1].y,u[2].y,u[3].y));
	result.z = CubicInterPolation(_t.x, vec4(u[0].z,u[1].z,u[2].z,u[3].z));
	result.w = CubicInterPolation(_t.x, vec4(u[0].w,u[1].w,u[2].w,u[3].w));
	return result;
}


void main()
{
	FragColor = vec4(0,0,0,0);
	vec3 WorldNormal = texture(gWorldNormal,v_TexPos).xyz;
	vec3 viewDir = texture(gViewDir,v_TexPos).xyz;
	Material material;
	material.mDiffuseColor = texture(gFragColor,v_TexPos).xyz;
	material.mSpecularFactor = texture(gSpecData,v_TexPos).x;
	material.mSpecularPower = texture(gSpecData,v_TexPos).y;
	FragColor.xyz += CalcDirLight(dirLight,material,WorldNormal,viewDir).xyz;
	
	vec3 newCoord = texture(gWorldPosition,v_TexPos).xyz;
	vec3 leftBottomCoord = floor(newCoord);
	float sizeOfTex = 128.0f;
	float texelSize = 1.0f/128.0f;
	vec3 t = newCoord - leftBottomCoord;

	newCoord *= texelSize;
	leftBottomCoord *= texelSize;

	vec4 interpolatedDirection;
	vec4 interpolatedColor;

	vec4 ParticleLightColor;
	vec3 ParticleLightVec;
	// Calculate Particle Light Diffuse
	if(u_bInterPolate)
	{
		
		// Process Interpolation
		if(u_bTricubicInterPolate)
		{
			interpolatedDirection = TriCubicInterpolation(u_WorldParticleLightDirection,newCoord,leftBottomCoord,texelSize,t);
			interpolatedColor = TriCubicInterpolation(u_WorldParticleLightColor,newCoord,leftBottomCoord,texelSize,t);
		}
		else
		{
			interpolatedDirection = TrilinearInterpolation(u_WorldParticleLightDirection,newCoord,leftBottomCoord,texelSize,t);
			interpolatedColor = TrilinearInterpolation(u_WorldParticleLightColor,newCoord,leftBottomCoord,texelSize,t);
		}

		
		ParticleLightVec = interpolatedDirection.xyz;
		// we can't normalize zero vector
		// 노멀라이즈 하면 끔찍한 계단현상을 보게될 것.
		if (length(ParticleLightVec.xyz) >= 0.001)
			ParticleLightVec = normalize(ParticleLightVec);

		ParticleLightColor = interpolatedColor.xyzw;
	}
	else
	{
		ParticleLightVec = (u_ActiveOverSampling) ? 
		OverSampling3DTexture(u_WorldParticleLightDirection,newCoord,texelSize).xyz
		: (texture (u_WorldParticleLightDirection,newCoord).xyz);
		if (length(ParticleLightVec.xyz) >= 0.001)
			ParticleLightVec = normalize(ParticleLightVec);

		ParticleLightColor = (u_ActiveOverSampling) ? 
		OverSampling3DTexture(u_WorldParticleLightColor,newCoord,texelSize)
		: texture (u_WorldParticleLightColor,newCoord).xyzw;
	}
	
	FragColor.xyz += (u_OnlyApplyLightIntensity) ? 
	vec3(ParticleLightColor.a) : vec3(ParticleLightColor.rgb) * ParticleLightColor.a;
	// calc Half vector

	if(u_ApplyParticleSpecular)
	{
		vec3 Hvector = normalize(ParticleLightVec.xyz + viewDir.xyz).xyz;
		// N dot H
		float NdotH = max(dot(Hvector,WorldNormal ) ,0);
		float SpecIntensity = pow( NdotH, material.mSpecularPower ) * ParticleLightColor.a;
		FragColor.xyz += SpecIntensity * vec3(1,1,1);
	}

	if(u_CheckLightDirection)
		FragColor.xyz = (ParticleLightVec+1)*0.5f;
	FragColor.xyz = texture(gWorldPosition,v_TexPos).xyz;
	//FragColor.xyz = vec3(v_TexPos.xy,1);
}
