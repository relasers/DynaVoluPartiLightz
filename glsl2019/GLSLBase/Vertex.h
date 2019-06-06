#pragma once

class Vertex {
public:
	Vertex();
	float& operator[](int idx);
	Vertex& operator=(const Vertex& other);

public:
	union {
		struct {
			glm::vec3 pos;
			glm::vec3 dir;
			float speed;
			float collide_time;
		};
		struct {
			glm::vec3 pos;
			glm::vec3 norm;
			glm::vec2 tex_coord;
		};
		float f[8];
	};
};