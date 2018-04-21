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

#include <Debug.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Player.h"
#include "NPC.h"

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

	static const int NUMBER_OF_VIEWPORTS = 2;
	sf::View viewport[NUMBER_OF_VIEWPORTS];

	void setViews();

};

#endif  // ! GAME_H