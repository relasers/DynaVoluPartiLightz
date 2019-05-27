#version 450



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




uniform sampler3D u_WorldIntensityTexture;
uniform DirectionalLight dirLight;
uniform Material u_material;

#define NR_POINT_LIGHTS 64  
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform bool u_bInterPolate;
uniform bool u_bTricubicInterPolate;

in vec4 v_WorldPosition;


const int shininess = 64;


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



layout(location=0) out vec4 FragColor;
in vec4 v_Color;
in vec3 v_Normal;
in vec3 v_viewDir;

// t, ( i-1, i , i+1, i+2 )
float CubicInterPolation(float t, vec4 value)
{

	//http://graphics.cs.cmu.edu/nsp/course/15-462/Fall04/assts/catmullRom.pdf
	float ten =  0.5; // tension
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

vec4 TrilinearInterpolation(sampler3D _tex, vec3 _coord , float _unit_per_dim)
{
	// calculate t value for interpolation
	vec3 delta;
	delta = ( v_WorldPosition.xyz - floor(v_WorldPosition.xyz) ) 
	/ ( floor(v_WorldPosition.xyz + vec3(1,1,1) ) 
	- floor(v_WorldPosition.xyz)); // ( x - x0 ) / ( x1 - x0 )

	vec4 VFI[2][2][2];

	for(int i =0;i<2;++i)
	{
		for(int j =0;j<2;++j)
		{
			for(int k =0;k<2;++k)
			{
				VFI[i][j][k] = texture (_tex,_coord 
				+ vec3(i*_unit_per_dim,j*_unit_per_dim,k*_unit_per_dim)
				);
			}
		}
	}

	// Trilinear interpolation
	// https://en.wikipedia.org/wiki/Trilinear_interpolations
	vec4 c00, c01, c10, c11;
	

	c00 = mix(VFI[0][0][0], VFI[1][0][0],delta.x );
	c01 = mix(VFI[0][0][1], VFI[1][0][1],delta.x );
	c10 = mix(VFI[0][1][0], VFI[1][1][0],delta.x );
	c11 = mix(VFI[0][1][1], VFI[1][1][1],delta.x );
	
	vec4 c0 , c1;
	c0 = mix(c00,c10,delta.y);
	c1 = mix(c01,c11,delta.y);

	return mix(c0,c1, delta.z);
}

// TriCubic Interpolation using cat-mullrom spline interpolation
//http://www.cds.caltech.edu/~marsden/bib/2005/08-LeMa2005/LeMa2005.pdf
vec4 TriCubicInterpolation(sampler3D _tex, vec3 _coord , float _unit_per_dim)
{
	// calculate t value for interpolation
	vec3 delta;
	delta = ( v_WorldPosition.xyz - floor(v_WorldPosition.xyz) ) 
	/ ( floor(v_WorldPosition.xyz + vec3(1,1,1) ) 
	- floor(v_WorldPosition.xyz)); // ( x - x0 ) / ( x1 - x0 )

	//delta = vec3(0,0,0);

	vec4 s[4][4][4];
	vec4 t[4][4];
	vec4 u[4];
	for(int i =0;i<4;++i)
	{
		for(int j =0;j<4;++j)
		{
			for(int k =0;k<4;++k)
			{
			s[i][j][k] = vec4(1);
				s[i][j][k] = texture (_tex,_coord 
				+ vec3((i-1)*_unit_per_dim,(j-1)*_unit_per_dim,(k-1)*_unit_per_dim)
				);
			}
		}
	}

	for(int i =0;i<4;++i)
	{
		for(int j =0;j<4;++j)
		{
			t[i][j].x = CubicInterPolation(delta.z,
			vec4(s[i][j][0].x, s[i][j][1].x, s[i][j][2].x, s[i][j][3].x) );
			t[i][j].y = CubicInterPolation(delta.z,
			vec4(s[i][j][0].y, s[i][j][1].y, s[i][j][2].y, s[i][j][3].y) );
			t[i][j].z = CubicInterPolation(delta.z,
			vec4(s[i][j][0].z, s[i][j][1].z, s[i][j][2].z, s[i][j][3].z) );
			t[i][j].w = CubicInterPolation(delta.z,
			vec4(s[i][j][0].w, s[i][j][1].w, s[i][j][2].w, s[i][j][3].w) );
		}
	}

	for(int i =0;i<4;++i)
	{
		u[i].x = CubicInterPolation(delta.y, vec4(t[i][0].x,t[i][1].x,t[i][2].x,t[i][3].x) );
		u[i].y = CubicInterPolation(delta.y, vec4(t[i][0].y,t[i][1].y,t[i][2].y,t[i][3].y) );
		u[i].z = CubicInterPolation(delta.y, vec4(t[i][0].z,t[i][1].z,t[i][2].z,t[i][3].z) );
		u[i].w = CubicInterPolation(delta.y, vec4(t[i][0].w,t[i][1].w,t[i][2].w,t[i][3].w) );
	}
	vec4 result = vec4(1);

	result.x = CubicInterPolation(delta.x, vec4(u[0].x,u[1].x,u[2].x,u[3].x));
	result.y = CubicInterPolation(delta.x, vec4(u[0].y,u[1].y,u[2].y,u[3].y));
	result.z = CubicInterPolation(delta.x, vec4(u[0].z,u[1].z,u[2].z,u[3].z));
	result.w = CubicInterPolation(delta.x, vec4(u[0].w,u[1].w,u[2].w,u[3].w));
	return result;
}

void main()
{
	//FragColor = v_Color;
	//FragColor = vec4(0.2f,0.2f,0.2f,1.0f);

	FragColor = vec4(0.05f,0.05f,0.05f,1.0f);
	FragColor.xyz += CalcDirLight(dirLight,u_material,v_Normal,v_viewDir).xyz;

	vec3 new_coord = v_WorldPosition.xyz;

	vec3 newCoord0 = floor(v_WorldPosition.xyz);
	vec3 newCoord1 = floor(v_WorldPosition.xyz+vec3(1,1,1));

	float dim_of_texture = 128.0f;
	float unit_per_dim = 1.0f/128.0f;

	new_coord /= dim_of_texture;
	newCoord0 /= dim_of_texture;
	newCoord1 /= dim_of_texture;


	vec4 interpolatedValue;
	if(u_bTricubicInterPolate)
	{
		interpolatedValue = TriCubicInterpolation(u_WorldIntensityTexture,newCoord0,unit_per_dim);
	}
	else
	{
		interpolatedValue = TrilinearInterpolation(u_WorldIntensityTexture,newCoord0,unit_per_dim);
	}


	float ParticleIntensity;
	vec3 ParticleLightVec;
	// Calculate Particle Light Diffuse
	if(u_bInterPolate)
	{
		ParticleIntensity = interpolatedValue.a;
		ParticleLightVec = normalize(interpolatedValue.xyz);
	}
	else
	{
		ParticleIntensity = texture (u_WorldIntensityTexture,new_coord).a;
		ParticleLightVec = normalize(texture (u_WorldIntensityTexture,new_coord).xyz);
	}
	
	FragColor.xyz += ParticleIntensity;
	// calc Half vector
	vec3 HalfVec = normalize(ParticleLightVec + v_viewDir);
	// N dot H
	float NdotH = dot(HalfVec,v_Normal);
	float SpecIntensity = pow( max( NdotH, 0 ), u_material.mSpecularPower );

	FragColor.xyz += ParticleIntensity * SpecIntensity * vec3(1,1,1);

	//FragColor.x = ;
	//FragColor = vec4(1,1,1,1);
}
