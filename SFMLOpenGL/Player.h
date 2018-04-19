#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Player
{
public:
	Player();
	void changePosition(glm::vec3 t_changeInPosition);
	void setPosition(glm::vec3 t_newPosition);
	glm::vec3 getPosition();

private:
	glm::vec3 position;

};

