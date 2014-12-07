#include "trackball.h"
#include <iostream>

#define R 0.5f

static void computeNormVec3(const glm::vec2 &in, glm::vec3 &out) {
	float d2 = (in.x * in.x + in.y * in.y);
	if (d2 <= R * R - 0.0001) {
		out.x = in.x;
		out.y = in.y;
		out.z = sqrt(R * R - d2);
	}
	else {
		out.x = in.x;
		out.y = in.y;
		out.z = 0;
	}
	
	out = glm::normalize(out);
}

void computeRotation(const glm::vec2 &start, const glm::vec2 &end, glm::vec3 &N, float &angle) {
	// Reference: http://www.lubanren.net/weblog/post/283.html
	glm::vec3 a;
	computeNormVec3(start, a);

	glm::vec3 t(end - start, 0.f);

	N = glm::cross(a, t);
	angle = glm::length(t) / R;
	printf("%f %f %f angle %f\n", N.x, N.y, N.z, angle);
}