#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp> 
#include <glm/gtc/constants.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

int main() {
	glm::mat4 rotate = glm::rotate(glm::mat4(1.f), glm::pi<float>() / 2, glm::vec3(0.f, 0.f, 1.f));
	glm::vec4 point = glm::vec4(glm::vec3(1.f, 0.f, 0.f), 1.f);
	std::cout << glm::to_string(rotate * point) << std::endl;
	std::cout << glm::to_string(rotate) << std::endl;

	return 0;
}