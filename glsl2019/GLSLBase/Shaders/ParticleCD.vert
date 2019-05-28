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
const int texel_collide_range = 30;
const vec3 g_gravity = vec3(0, -0.00005, 0);

void main() {

	gl_Position = u_ProjView * vec4(a_Pos, 1);
	vec2 TexCoord = (gl_Position.xy + vec2(1)) * 0.5f;

	ivec2 texel_base = ivec2(TexCoord.x * u_ClientWidth, TexCoord.y * u_ClientHeight);
	
	vec4 curr_pos = texelFetch(u_CDTexTure, texel_base, 0);
	vec4 collide_pos;
	bool Collide = false;
	vec3 temp;

	if (curr_pos.w > 0.5f){
		float min_length = 10000.0f;

		if (a_CollideTime <= 0)
		{
			for (int ty = -texel_collide_range; ty <= texel_collide_range; ++ty){
			for (int tx = -texel_collide_range; tx <= texel_collide_range; ++tx){
				if (0 == tx && 0 == ty) continue;
				vec4 pos = texelFetch(u_CDTexTure, texel_base + ivec2(tx, ty), 0);

				float len = length(curr_pos.xyz - pos.xyz);
				//if (pos.w > 0.5f && 0.00095f > len) {
				if (pos.w > 0.3f && 0.002f > len) {
					if (len < min_length) {
						min_length = len;
						collide_pos = pos;
					}
					Collide = true;
				}
			}
		}
		}
	}
	
	if (Collide)
	{
		f_Speed = a_Speed * 0.8f;
		v_Dir = normalize(curr_pos.xyz - collide_pos.xyz);
		//v_Dir = reflect(a_Dir, vec3(0,1,0));
		f_CollideTime = 0.1f;
		if (collide_pos.w < 0.5f) {
			//f_Speed = 0.0f;
		}
		else {
			v_Pos = a_Pos + (v_Dir * f_Speed) * 0.1f;
		}
	}
	else
	{
		temp = a_Dir * a_Speed + g_gravity;
		v_Dir = normalize(temp);
		f_Speed = length(temp);
		f_CollideTime = a_CollideTime - g_tick;
		v_Pos = a_Pos + (v_Dir * f_Speed) * g_tick;
	}
}
