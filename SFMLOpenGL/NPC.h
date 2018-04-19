#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class NPC
{
public:
	NPC();

	void changePosition(glm::vec3 t_changeInPosition);
	void setPosition(glm::vec3 t_newPosition);
	glm::vec3 getPosition();

private:
	glm::vec3 positions;


};

