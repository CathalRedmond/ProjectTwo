#include <Game.h>
#include <Cube.h>
#include <Easing.h>

// Helper to convert Number to String for HUD
template <typename T>
std::string toString(T number)
{
	ostringstream oss;
	oss << number;
	return oss.str();
}
GLuint	vsid,		// Vertex Shader ID
		fsid,		// Fragment Shader ID
		progID,		// Program ID
		vao = 0,	// Vertex Array ID
		vbo,		// Vertex Buffer ID
		vib,		// Vertex Index Buffer
		to[1];		// Texture ID
GLint	positionID,	// Position ID
		colorID,	// Color ID
		textureID,	// Texture ID
		uvID,		// UV ID
		mvpID,		// Model View Projection ID
		x_offsetID, // X offset ID
		y_offsetID,	// Y offset ID
		z_offsetID;	// Z offset ID

GLenum	error;		// OpenGL Error Code


//Please see .//Assets//Textures// for more textures
const std::string filename = ".//Assets//Textures//cube_wip.tga";

int width;						// Width of texture
int height;						// Height of texture
int comp_count;					// Component of texture

unsigned char* img_data;		// image data

glm::mat4 mvp[NUMBER_OF_CUBES], projection[NUMBER_OF_CUBES],
		view[NUMBER_OF_CUBES], model[NUMBER_OF_CUBES];			// Model View Projection

sf::Font font;						// Game font

float x_offset, y_offset, z_offset; // offset on screen (Vertex Shader)

Game::Game() : 
	window(sf::VideoMode(800, 600),
	"Introduction to OpenGL Texturing")
{
}

Game::Game(sf::ContextSettings settings) : 
	window(sf::VideoMode::getDesktopMode(),
	"Introduction to OpenGL Texturing", 
	sf::Style::Fullscreen, 
	settings)
{
}

Game::~Game(){}


void Game::run()
{

	initialize();

	sf::Event event;

	float rotation = 0.01f;
	float start_value = 0.0f;
	float end_value = 1.0f;

	while (isRunning) {

#if (DEBUG >= 2)
		DEBUG_MSG("Game running...");
#endif

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				isRunning = false;
			}
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Left:
					// only works if no gyrpscope detected
					if (!gyroscopeActive)
					{
						cameraPositions[0] = rotationMatrix[LEFT] * cameraPositions[0];
						cameraPositions[1] = rotationMatrix[LEFT] * cameraPositions[1];
					}
					break;
				case sf::Keyboard::Right:
					// only works if no gyrpscope detected
					if (!gyroscopeActive)
					{
						cameraPositions[0] = rotationMatrix[RIGHT] * cameraPositions[0];
						cameraPositions[1] = rotationMatrix[RIGHT] * cameraPositions[1];
					}
					break;
				case sf::Keyboard::Return:
					
					DEBUG_MSG(model[0][0].x);
					DEBUG_MSG(model[0][0].y);
					DEBUG_MSG(model[0][0].z);

					DEBUG_MSG(model[1][0].x);
					DEBUG_MSG(model[1][0].y);
					DEBUG_MSG(model[1][0].z);
					break;
				case sf::Keyboard::Escape:
					isRunning = false;
					break;
				default:
					break;
				}
			}

			update();
			render();
		}
	}
#if (DEBUG >= 2)
	DEBUG_MSG("Calling Cleanup...");
#endif
	unload();

}

void Game::initialize()
{
		// checks if gyroscope is detected
		if (sf::Sensor::isAvailable(sf::Sensor::Gyroscope))
		{
			gyroscopeActive = true;
		}

		if (gyroscopeActive)
		{
			// sets up sensor for the gyroscope if it is detected
			sf::Sensor::setEnabled(sf::Sensor::Gyroscope, true);

			currentGyroscopeVector = sf::Sensor::getValue(sf::Sensor::Gyroscope);
		}

		rotationAngle = 0.05;

		intialiseRotationMatrix();
		setSfViews();

		isRunning = true;
		GLint isCompiled = 0;
		GLint isLinked = 0;

		if (!(!glewInit())) { DEBUG_MSG("glewInit() failed"); }

		// Copy UV's to all faces


		DEBUG_MSG(glGetString(GL_VENDOR));
		DEBUG_MSG(glGetString(GL_RENDERER));
		DEBUG_MSG(glGetString(GL_VERSION));

		// Vertex Array Buffer
		glGenBuffers(1, &vbo);		// Generate Vertex Buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Vertices (3) x,y,z , Colors (4) RGBA, UV/ST (2)
		glBufferData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS) + (2 * UVS)) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

		glGenBuffers(1, &vib); //Generate Vertex Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

		// Indices to be drawn
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * INDICES * sizeof(GLuint), indices, GL_STATIC_DRAW);

		// NOTE: uniforms values must be used within Shader so that they 
		// can be retreived
		const char* vs_src =
			"#version 400\n\r"
			""
			"in vec3 sv_position;"
			"in vec4 sv_color;"
			"in vec2 sv_uv;"
			""
			"out vec4 color;"
			"out vec2 uv;"
			""
			"uniform mat4 sv_mvp;"
			"uniform mat4 sv_mvp2;"
			"uniform float sv_x_offset;"
			"uniform float sv_y_offset;"
			"uniform float sv_z_offset;"
			""
			"void main() {"
			"	color = sv_color;"
			"	uv = sv_uv;"
			//"	gl_Position = vec4(sv_position, 1);"
			"	gl_Position = sv_mvp * vec4(sv_position.x + sv_x_offset, sv_position.y + sv_y_offset, sv_position.z + sv_z_offset, 1 );"
			"}"; //Vertex Shader Src


		DEBUG_MSG("Setting Up Vertex Shader");

		vsid = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vsid, 1, (const GLchar**)&vs_src, NULL);
		glCompileShader(vsid);

		// Check is Shader Compiled
		glGetShaderiv(vsid, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_TRUE) {
			DEBUG_MSG("Vertex Shader Compiled");
			isCompiled = GL_FALSE;
		}
		else
		{
			DEBUG_MSG("ERROR: Vertex Shader Compilation Error");
		}

		const char* fs_src =
			"#version 400\n\r"
			""
			"uniform sampler2D f_texture;"
			""
			"in vec4 color;"
			"in vec2 uv;"
			""
			"out vec4 fColor;"
			""
			"void main() {"
			"	fColor = texture2D(f_texture, uv);"
			""
			"}"; //Fragment Shader Src

		DEBUG_MSG("Setting Up Fragment Shader");

		fsid = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fsid, 1, (const GLchar**)&fs_src, NULL);
		glCompileShader(fsid);

		// Check is Shader Compiled
		glGetShaderiv(fsid, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_TRUE) {
			DEBUG_MSG("Fragment Shader Compiled");
			isCompiled = GL_FALSE;
		}
		else
		{
			DEBUG_MSG("ERROR: Fragment Shader Compilation Error");
		}

		DEBUG_MSG("Setting Up and Linking Shader");
		progID = glCreateProgram();
		glAttachShader(progID, vsid);
		glAttachShader(progID, fsid);
		glLinkProgram(progID);

		// Check is Shader Linked
		glGetProgramiv(progID, GL_LINK_STATUS, &isLinked);

		if (isLinked == 1) {
			DEBUG_MSG("Shader Linked");
		}
		else
		{
			DEBUG_MSG("ERROR: Shader Link Error");
		}

		// Set image data
		// https://github.com/nothings/stb/blob/master/stb_image.h
		img_data = stbi_load(filename.c_str(), &width, &height, &comp_count, 4);

		if (img_data == NULL)
		{
			DEBUG_MSG("ERROR: Texture not loaded");
		}

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &to[0]);
		glBindTexture(GL_TEXTURE_2D, to[0]);

		// Wrap around
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		// Filtering
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Bind to OpenGL
		// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexImage2D.xml
		glTexImage2D(
			GL_TEXTURE_2D,			// 2D Texture Image
			0,						// Mipmapping Level 
			GL_RGBA,				// GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_BGR, GL_RGBA 
			width,					// Width
			height,					// Height
			0,						// Border
			GL_RGBA,				// Bitmap
			GL_UNSIGNED_BYTE,		// Specifies Data type of image data
			img_data				// Image Data
		);

		cameraPositions[0] = glm::vec4(1.32f, -4.0f, 10.0f, 1.0f);
		cameraPositions[1] = glm::vec4(-1.32f, -4.0f, 10.0f, 1.0f);

		// Projection Matrix 
		projection[0] = glm::perspective(
			45.0f,					// Field of View 45 degrees
			4.0f / 3.0f,			// Aspect ratio
			5.0f,					// Display Range Min : 0.1f unit
			100.0f					// Display Range Max : 100.0f unit
		);
		
		// Camera Matrix
		view[0] = lookAt(
			glm::vec3(cameraPositions[0]),	// Camera (x,y,z), in World Space
			glm::vec3(0.0f, 0.0f, 0.0f),		// Camera looking at origin
			glm::vec3(0.0f, 0.0f, 0.0f)		// 0.0f, 0.0f, 0.0f Look Down and 0.0f, -1.0f, 0.0f Look Up
		);

		// Model matrix
		model[0] = glm::mat4(
			1.0f					// Identity Matrix
		);

		


		projection[1] = glm::perspective(
			45.0f,
			4.0f / 3.0f,
			5.0f,
			100.0f);

		view[1] = lookAt(
			glm::vec3(cameraPositions[1]),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f)
		);

		model[1] = glm::mat4(
			1.0f
		);

		// Enable Depth Test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		// Load Font
		font.loadFromFile(".//Assets//Fonts//BBrick.ttf");
}

void Game::update()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Updating...");
#endif
	// Update Model View Projection
	// For mutiple objects (cubes) create multiple models
	// To alter Camera modify view & projection

	// updates mvp and views
	for (int index = 0; index < NUMBER_OF_CUBES; index++)
	{
		mvp[index] = projection[index] * view[index] * model[index];
		view[index] = lookAt(
			glm::vec3(cameraPositions[index]),
			glm::vec3(0.0f,0.0f,0.0f),
			glm::vec3(0.0f,-1.0f,0.0f)
		);
	}

	if (gyroscopeActive)
	{

		if (currentGyroscopeVector != sf::Sensor::getValue(sf::Sensor::Gyroscope))
		{
			sf::Vector3f tempGyroscopeVector = sf::Sensor::getValue(sf::Sensor::Gyroscope);
			if (currentGyroscopeVector.y < tempGyroscopeVector.y)
			{
				cameraPositions[0] = rotationMatrix[LEFT] * cameraPositions[0];
				cameraPositions[1] = rotationMatrix[LEFT] * cameraPositions[1];
			}
			else if(currentGyroscopeVector.y > tempGyroscopeVector.y)
			{
				cameraPositions[0] = rotationMatrix[RIGHT] * cameraPositions[0];
				cameraPositions[1] = rotationMatrix[RIGHT] * cameraPositions[1];

			}
		}
		currentGyroscopeVector = sf::Sensor::getValue(sf::Sensor::Gyroscope);
	}

}

void Game::render()
{

#if (DEBUG >= 2)
	DEBUG_MSG("Render Loop...");
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save current OpenGL render states
	// https://www.sfml-dev.org/documentation/2.0/classsf_1_1RenderTarget.php#a8d1998464ccc54e789aaf990242b47f7
	window.pushGLStates();

	// Find mouse position using sf::Mouse
	int x = sf::Mouse::getPosition(window).x;
	int y = sf::Mouse::getPosition(window).y;

	std::string hud = "Position: [ " + std::to_string(x) + " ] [ " + std::to_string(y) + " ] ";

	sf::Text text(hud, font);

	text.setCharacterSize(60);
	text.setFillColor(sf::Color(0, 255, 0, 170));
	text.setPosition(50.f, 50.f);

	

	window.setView(sfViewport[0]);
	window.draw(text);
	window.setView(sfViewport[1]);
	window.draw(text);
	// Restore OpenGL render states
	// https://www.sfml-dev.org/documentation/2.0/classsf_1_1RenderTarget.php#a8d1998464ccc54e789aaf990242b47f7

	window.popGLStates();
	

	
	for (int index = 0; index < 2; index++)
	{
		drawCube(index);
	}
	



	// Check for OpenGL Error code
	error = glGetError();
	if (error != GL_NO_ERROR) {
		DEBUG_MSG(error);
	}
}

void Game::unload()
{
#if (DEBUG >= 2)
	DEBUG_MSG("Cleaning up...");
#endif
	glDetachShader(progID, vsid);	// Shader could be used with more than one progID
	glDetachShader(progID, fsid);	// ..
	glDeleteShader(vsid);			// Delete Vertex Shader
	glDeleteShader(fsid);			// Delete Fragment Shader
	glDeleteProgram(progID);		// Delete Shader
	glDeleteBuffers(1, &vbo);		// Delete Vertex Buffer
	glDeleteBuffers(1, &vib);		// Delete Vertex Index Buffer
	stbi_image_free(img_data);		// Free image stbi_image_free(..)
}



void Game::drawCube(int t_index)
{
	// Rebind Buffers and then set SubData
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);

	// Use Progam on GPU
	glUseProgram(progID);

	// Find variables within the shader
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glGetAttribLocation.xml
	positionID = glGetAttribLocation(progID, "sv_position");
	if (positionID < 0) { DEBUG_MSG("positionID not found"); }

	colorID = glGetAttribLocation(progID, "sv_color");
	if (colorID < 0) { DEBUG_MSG("colorID not found"); }

	uvID = glGetAttribLocation(progID, "sv_uv");
	if (uvID < 0) { DEBUG_MSG("uvID not found"); }

	textureID = glGetUniformLocation(progID, "f_texture");
	if (textureID < 0) { DEBUG_MSG("textureID not found"); }

	mvpID = glGetUniformLocation(progID, "sv_mvp");
	if (mvpID < 0) { DEBUG_MSG("mvpID not found"); }



	x_offsetID = glGetUniformLocation(progID, "sv_x_offset");
	if (x_offsetID < 0) { DEBUG_MSG("x_offsetID not found"); }

	y_offsetID = glGetUniformLocation(progID, "sv_y_offset");
	if (y_offsetID < 0) { DEBUG_MSG("y_offsetID not found"); }

	z_offsetID = glGetUniformLocation(progID, "sv_z_offset");
	if (z_offsetID < 0) { DEBUG_MSG("z_offsetID not found"); };

	// VBO Data....vertices, colors and UV's appended
	
	glBufferSubData(GL_ARRAY_BUFFER, 0 * VERTICES * sizeof(GLfloat), 3 * VERTICES * sizeof(GLfloat), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * VERTICES * sizeof(GLfloat), 4 * COLORS * sizeof(GLfloat), colors);
	glBufferSubData(GL_ARRAY_BUFFER, ((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat), 2 * UVS * sizeof(GLfloat), uvs);
		
	
	//Send transformation to shader mvp uniform [0][0] is start of array
	if (t_index == 0 )
	{
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0][0]);
	}
	else if(t_index == 1)
	{
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[1][0][0]);
	}

	// Set Active Texture .... 32 GL_TEXTURE0 .... GL_TEXTURE31
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(textureID, 0); // 0 .... 31

	// Set the X, Y and Z offset (this allows for multiple cubes via different shaders)
	// Experiment with these values to change screen positions
	if (t_index == 0)
	{
		glUniform1f(x_offsetID, 0.00f);
	}
	else if (t_index == 1)
	{
		glUniform1f(x_offsetID, 0.00f);
	}
	glUniform1f(y_offsetID, 0.00f);
	glUniform1f(z_offsetID, 0.00f);
	
	
	// Set pointers for each parameter (with appropriate starting positions)
	// https://www.khronos.org/opengles/sdk/docs/man/xhtml/glVertexAttribPointer.xml
	glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, (VOID*)(3 * VERTICES * sizeof(GLfloat)));
	glVertexAttribPointer(uvID, 2, GL_FLOAT, GL_FALSE, 0, (VOID*)(((3 * VERTICES) + (4 * COLORS)) * sizeof(GLfloat)));

	// Enable Arrays
	glEnableVertexAttribArray(positionID);
	glEnableVertexAttribArray(colorID);
	glEnableVertexAttribArray(uvID);

	if (t_index == 0)
	{
		window.setView(sfViewport[0]);
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}
	else if (t_index == 1)
	{
		window.setView(sfViewport[1]);
		glDrawElements(GL_TRIANGLES, 3 * INDICES, GL_UNSIGNED_INT, NULL);
	}
	
	if (t_index == 0)
	{
		glViewport(0, 0, sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height);
	}
	else if (t_index == 1)
	{
		glViewport(sf::VideoMode::getDesktopMode().width / 2, 0, sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height);
	}

	if (t_index == 1)
	{
		window.display();
	}
	

	// Disable Arrays
	glDisableVertexAttribArray(positionID);
	glDisableVertexAttribArray(colorID);
	glDisableVertexAttribArray(uvID);

	// Unbind Buffers with 0 (Resets OpenGL States...important step)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Reset the Shader Program to Use
	glUseProgram(0);
}

// sets up the sf::views used for the sf::text object
void Game::setSfViews()
{
	sfViewport[0].setCenter(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);
	sfViewport[1].setCenter(sf::VideoMode::getDesktopMode().width / 2, sf::VideoMode::getDesktopMode().height / 2);

	sfViewport[0].setSize(window.getView().getSize());
	sfViewport[1].setSize(window.getView().getSize());

	sfViewport[0].setViewport(sf::FloatRect(0,0,0.5,1));
	sfViewport[1].setViewport(sf::FloatRect(0.5, 0, 0.5, 1));

}

// intialise the values of the rotation matrix used.
void Game::intialiseRotationMatrix()
{
	rotationMatrix[LEFT] = glm::mat4x4(glm::cos(-rotationAngle), 0.0f, glm::sin(-rotationAngle), 0.0f,
									   0.0f, 1.0f, 0.0f, 0.0f,
									  -glm::sin(-rotationAngle), 0.0f, glm::cos(-rotationAngle), 0.0f,
									   0.0f, 0.0f, 0.0f, 1.0f);


	rotationMatrix[RIGHT] = glm::mat4x4(glm::cos(rotationAngle), 0.0f, glm::sin(rotationAngle), 0.0f,
									    0.0f, 1.0f, 0.0f, 0.0f,
									   -glm::sin(rotationAngle), 0.0f, glm::cos(rotationAngle), 0.0f,
									    0.0f, 0.0f, 0.0f, 1.0f);;

}



