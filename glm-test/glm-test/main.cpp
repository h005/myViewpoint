#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
	glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::pi<float>() / 2, glm::vec3(0.f, 0.f, 1.f));
	glm::vec4 point = glm::vec4(glm::vec3(1.f, 0.f, 0.f), 1.f);
	std::cout << glm::to_string(rotate * point) << std::endl;
	std::cout << glm::to_string(rotate) << std::endl;

	float v[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9, 10, 11,
		12, 13, 14, 15
	};
	glm::mat4 m = glm::make_mat4(v);
	glm::mat3 mm = glm::mat3(m);
	glm::vec4 zz = glm::vec4(m[3]);
	std::cout << glm::to_string(mm) << std::endl;
	std::cout << glm::to_string(zz) << std::endl;

	return 0;
}