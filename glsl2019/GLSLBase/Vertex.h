#pragma once

class Vertex {
public:
	Vertex();

	float& operator[](int idx);

public:
	union {
		struct {
			glm::vec3 pos;
			glm::vec3 dir;
			float speed;
			float collide_time;
		};
		float f[8];
	};
};