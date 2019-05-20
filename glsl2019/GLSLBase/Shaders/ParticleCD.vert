#version 450

in vec3 a_Pos;
in vec3 a_Dir;
in float a_Speed;
in float a_CollideTime;

out vec3 v_Pos;
out vec3 v_Dir;
out float f_Speed;
out float f_CollideTime;

uniform mat4 u_ProjView;
uniform int u_ClientWidth;
uniform int u_ClientHeight;
uniform sampler2D u_CDTexTure;

const float g_tick = 0.0166666666666667f;
const float g_cube_size = 0.5f;
const int eNONE = -1;
const vec3 g_cube_normal[] = {
	  vec3(1.0f, 0.0f, 0.0f)	// LEFT
	, vec3(-1.0f, 0.0f, 0.0f)	// RIGHT
	, vec3(0.0f, -1.0f, 0.0f)	// TOP
	, vec3(0.0f, 1.0f, 0.0f)	// BOTTOM
	, vec3(0.0f, 0.0f, 1.0f)	// FRONT
	, vec3(0.0f, 0.0f, -1.0f)	// BACK
};
const int eLEFT = 0, eRIGHT = 1, eTOP = 2, eBOTTOM = 3, eFRONT = 4, eBACK = 5;
int collide_check(vec3 pos) {
	if (-g_cube_size > pos.x) return eLEFT; 
	if (g_cube_size < pos.x) return eRIGHT; 
	if (-g_cube_size > pos.y) return eBOTTOM; 
	if (g_cube_size < pos.y) return eTOP; 
	if (-g_cube_size > pos.z) return eFRONT; 
	if (g_cube_size < pos.z) return eBACK; 
	return eNONE;
}
const int texel_offset_size = 24;
ivec2 texel_offset[] = {
	  ivec2(-2, -2), ivec2(-1, -2), ivec2(0, -2), ivec2(1, -2), ivec2(2, -2)
	, ivec2(-2, -1), ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1), ivec2(2, -1)
	, ivec2(-2, 0), ivec2(-1, 0), /*ivec2(0, 0),*/ ivec2(1, 0), ivec2(2, 0)
	, ivec2(-2, +1), ivec2(-1, +1), ivec2(0, +1), ivec2(1, +1), ivec2(2, +1)
	, ivec2(-2, +2), ivec2(-1, +2), ivec2(0, +2), ivec2(1, +2), ivec2(2, +2)
};
void main()
{
	f_CollideTime = a_CollideTime - g_tick;
	gl_Position = u_ProjView * vec4(a_Pos, 1);
	vec2 TexCoord = (gl_Position.xy + vec2(1)) * 0.5f;
	ivec2 texel_base = ivec2(TexCoord.x * u_ClientWidth, TexCoord.y * u_ClientHeight);
	for(int i = 0; i < texel_offset_size; ++i){
		vec4 pos = texelFetch(u_CDTexTure, texel_base + texel_offset[i], 0);
		if (0.5f < pos.x && 0.01f > length(a_Pos - pos.yzw)){
			f_CollideTime = 1.0f;
			break;
		}
	}

	v_Pos = a_Pos + a_Dir * a_Speed * g_tick;
	
	int ret = collide_check(v_Pos);
	if (eNONE == ret) {
		v_Dir = a_Dir;
		//f_CollideTime = a_CollideTime - g_tick;
	}
	else {
		if (eLEFT == ret)			v_Pos.x = -g_cube_size;
		else if (eRIGHT == ret)		v_Pos.x = g_cube_size;
		else if (eBOTTOM == ret)	v_Pos.y = -g_cube_size;
		else if (eTOP == ret)		v_Pos.y = g_cube_size;
		else if (eFRONT == ret)		v_Pos.z = -g_cube_size;
		else if (eBACK == ret)		v_Pos.z = g_cube_size;
		v_Dir = reflect(a_Dir, g_cube_normal[ret]);
		//f_CollideTime = 1.0f;
	}
	
	f_Speed = a_Speed;
}
