#include "NPC.h"



NPC::NPC()
{
}

void NPC::changePosition(glm::vec3 t_changeInPosition)
{
	positions += t_changeInPosition;
}

void NPC::setPosition(glm::vec3 t_newPosition)
{
	positions = t_newPosition;
}

glm::vec3 NPC::getPosition()
{
	return positions;
}



