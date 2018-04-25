#ifndef GAME_H
#define GAME_H

#include <string>
#include <sstream>

#include <iostream>
#include <GL/glew.h>
#include <GL/wglew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

//#include <SFML/>

#include <Debug.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Player.h"
#include "NPC.h"
static const int NUMBER_OF_CUBES = 2;

class Game
{
public:
	Game();
	Game(sf::ContextSettings settings);
	~Game();
	void run();
private:

	sf::RenderWindow window;
	sf::Clock clock;
	sf::Time time;
	bool animate = false;
	glm::vec3 animation = glm::vec3(0.0f);
	float rotation = 0.0f;
	bool isRunning = false;
	void initialize();
	void update();
	void render();
	void unload();

	Player m_player;

	NPC m_NPC;

	void drawCube(int t_index);

	sf::View sfViewport[NUMBER_OF_CUBES];

	glm::vec4 cameraPositions[NUMBER_OF_CUBES];

	static const int LEFT = 0;
	static const int RIGHT = 1;
	glm::mat4 rotationMatrix[2];

	float rotationAngle;

	void setSfViews();
	void intialiseRotationMatrix();

	bool gyroscopeActive{ false };
	sf::Vector3f currentGyroscopeVector;

};

#endif  // ! GAME_H