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
uniform sampler2D u_ObjectCDpos;
uniform sampler2D u_ObjectCDnorm;

const float g_tick = 0.0166666666666667f;
const int texel_collide_range = 50;
const vec3 g_gravity = vec3(0, -0.01f, 0);
const float g_collision_range = 0.01f;

void main() {

	gl_Position = u_ProjView * vec4(a_Pos, 1);
	vec2 TexCoord = (gl_Position.xy + vec2(1)) * 0.5f;

	ivec2 texel_base = ivec2(TexCoord.x * u_ClientWidth, TexCoord.y * u_ClientHeight);
	
	vec4 curr_pos = texelFetch(u_CDTexTure, texel_base, 0);
	vec4 particle_collide_pos;
	bool particle_collision = false;
	vec4 object_collide_pos;
	bool object_collision = false;
	ivec2 object_collide_texel;

	float particle_min_length = 10000.0f;
	float object_min_length = 10000.0f;

	if (a_CollideTime < 0) {
		for (int ty = -texel_collide_range; ty <= texel_collide_range; ++ty){
			for (int tx = -texel_collide_range; tx <= texel_collide_range; ++tx){
				if (0 == tx && 0 == ty) continue;
				ivec2 texel = texel_base + ivec2(tx, ty);

				vec4 particle_pos = texelFetch(u_CDTexTure, texel, 0);
				if (curr_pos.w > 0.5f && particle_pos.w > 0.5f) {
					float len = length(curr_pos.xyz - particle_pos.xyz);
					if (g_collision_range > len && len < particle_min_length) {
						particle_min_length = len;
						particle_collide_pos = particle_pos;
						particle_collision = true;
					}
				}

				vec4 object_pos = texelFetch(u_ObjectCDpos, texel, 0);
				if (object_pos.w > 0.5f) {
					float len = length(a_Pos - object_pos.xyz);
					if (g_collision_range > len && len < object_min_length) {
						object_min_length = len;
						object_collide_pos = object_pos;
						object_collision = true;
						object_collide_texel = texel;
					}
				}
			}
		}
	}
	
	if (true == particle_collision) {
		f_Speed = a_Speed * 0.2f;
		v_Dir = normalize(curr_pos.xyz - particle_collide_pos.xyz);
		f_CollideTime = 0.005f;
		v_Pos = a_Pos + (v_Dir * g_collision_range);
	}
	else {
		vec3 temp = (a_Dir * a_Speed) + (g_gravity * g_tick);
		v_Dir = normalize(temp);
		f_Speed = length(temp);
		f_CollideTime = a_CollideTime - g_tick;
		v_Pos = a_Pos + (v_Dir * f_Speed) * g_tick;
	}

	if (true == object_collision) {
		f_Speed = a_Speed * 0.2f;
		v_Dir = reflect(a_Dir, texelFetch(u_ObjectCDnorm, object_collide_texel, 0).xyz);
		f_CollideTime = 0.005f;
		v_Pos = a_Pos + (v_Dir * g_collision_range);
	}
}
