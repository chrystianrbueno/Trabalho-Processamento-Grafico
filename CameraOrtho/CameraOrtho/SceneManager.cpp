#include "SceneManager.h"
#include <Windows.h>
#include <chrono>

//static controllers for mouse and keyboard
#define CONSTPRESENT 10
static bool keys[1024];
static bool keyDir = false;
static bool keyEsq = false;
static bool resized;
static GLuint width, height;
typedef std::chrono::high_resolution_clock Clock;
int contador= 0, pontuacao = 0;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;

	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();

}

void SceneManager::initializeGraphics()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Sprites", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag");

	//setup the scene -- LEMBRANDO QUE A DESCRIÇÃO DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURAÇÃO
	setupScene();

	resized = true; //para entrar no setup da câmera na 1a vez

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}


//USO DO TECLADO
void SceneManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
	{
		keyEsq = true;
		keyDir = false;
	} else if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
	{
		keyDir = true;
		keyEsq = false;
	}
	else if(action == GLFW_RELEASE) {
		keyDir = false;
		keyEsq = false;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}

void SceneManager::update()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);

}

void SceneManager::render()
{
	// Clear the colorbuffer
	glClearColor(0.0f, 0.74f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}

	//atualiza e desenha os Sprites


	for (Sprite* imgCenario : objectCenario)
	{
		imgCenario->update();
		imgCenario->draw();
	}
	
	if ((objectCenario[0]->getPosY() < 350.0f)) {
		objectCenario[0]->setPosY(objectCenario[0]->getPosY() + 0.01f);
	}
	if ((objectCenario[1]->getPosX() < 350.0f)) {
		objectCenario[1]->setPosX(objectCenario[1]->getPosX() + 0.001f);
	}
	if ((objectCenario[2]->getPosX() < 550.0f)) {
		objectCenario[2]->setPosX(objectCenario[2]->getPosX() + 0.001f);
	}

	for (Sprite* imgPresents : objectPresents)
	{
		imgPresents->setPosY(imgPresents->getPosY() - imgPresents->getTempoQueda());
		imgPresents->update();
		imgPresents->draw();
		
		if (imgPresents->getPosY() <= 100.0f && imgPresents->getPosY() >= 90.0f)
		{
			if (imgPresents->getPosX() + 66.0f >= objectTreno[0]->getPosX() && objectTreno[0]->getPosX() + 200.0f >= imgPresents->getPosX())
			{
				pontuacao++;
				imgPresents->setPosY(620);
				imgPresents->setPosX(rand() % (700 - 0 + 1));
				imgPresents->setTempoQueda(imgPresents->getTempoQueda() + 0.02);

			}
		}
		else if (imgPresents->getPosY() <= 0.0f)
		{
			imgPresents->setPosY(620);
			contador++;
		}
		
	}

	objectTreno[0]->update();
	objectTreno[0]->draw();

	if (keyDir)
	{
		if (objectTreno[0]->getPosX() + 1.0f < 600) 
		{
			objectTreno[0]->setPosX(objectTreno[0]->getPosX() + 1.0f);
		}
	}
	if(keyEsq)
	{
		if (objectTreno[0]->getPosX() - 1.0f > 0)
		{
			objectTreno[0]->setPosX(objectTreno[0]->getPosX() - 1.0f);
		}
	}

}

//GAME LOOP
void SceneManager::run()
{	
	auto t1 = Clock::now();
	auto tInicial = Clock::now();
	float segCount = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		if (contador == 10)
		{
			finish();
			break;
		}
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		update();

		//Render scene
		render();

		// Swap the screen buffers
		glfwSwapBuffers(window);
		auto t2 = Clock::now();
		segCount = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - tInicial).count() / 1000000000;
	}
	cout << "Voce fez " << pontuacao << " pts em " << segCount << " segundos";
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


void SceneManager::setupScene()
{
	//Criação dos Sprites iniciais -- pode-se fazer métodos de criação posteriormente
	//Adicionando mais um
	
	Sprite* obj = new Sprite;
	unsigned int texID = loadTexture("../textures/sol.png");
	obj->setPosX(600.0f);
	obj->setPosY(220.0f);
	obj->setPosZ(0.0f);
	obj->setDimension(glm::vec3(210.0f, 200.0f, 1.0f));
	obj->setShader(shader);
	objectCenario.push_back(obj);
	objectCenario[0]->setTexture(texID);

	texID = loadTexture("../textures/nuvem.png");
	obj = new Sprite;
	obj->setPosX(10.0f);
	obj->setPosY(400.0f);
	obj->setPosZ(0.0f);
	obj->setDimension(glm::vec3(300.0f, 150.0f, 1.0f));
	obj->setShader(shader);
	objectCenario.push_back(obj);
	objectCenario[1]->setTexture(texID);

	obj = new Sprite;
	obj->setPosX(320.0f);
	obj->setPosY(400.0f);
	obj->setPosZ(0.0f);
	obj->setDimension(glm::vec3(300.0f, 150.0f, 1.0f));
	obj->setShader(shader);
	objectCenario.push_back(obj);
	objectCenario[2]->setTexture(texID);

	texID = loadTexture("../textures/montanha.png");
	obj = new Sprite;
	obj->setPosX(0.0f);
	obj->setPosY(0.0f);
	obj->setPosZ(0.0f);
	obj->setDimension(glm::vec3(800.0f, 600.0f, 1.0f));
	obj->setShader(shader);
	objectCenario.push_back(obj);
	objectCenario[3]->setTexture(texID);

	obj = new Sprite;
	texID = loadTexture("../textures/presente.png");
	setPresent(obj, texID);

	obj = new Sprite;
	texID = loadTexture("../textures/treno.png");
	setTreno(obj, texID);

	//Definindo a janela do mundo (ortho2D)
	ortho2D[0] = 0.0f; //xMin
	ortho2D[1] = 800.0f; //xMax
	ortho2D[2] = 0.0f; //yMin
	ortho2D[3] = 600.0f; //yMax


	//Habilita transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void SceneManager::setupCamera2D() //TO DO: parametrizar aqui
{
	float zNear = -1.0, zFar = 1.0; //estão fixos porque não precisamos mudar

	projection = glm::ortho(ortho2D[0], ortho2D[1], ortho2D[2], ortho2D[3], zNear, zFar);


	//Obtendo o identificador da matriz de projeção para enviar para o shader
	GLint projLoc = glGetUniformLocation(shader->ID, "projection");
	//Enviando a matriz de projeção para o shader
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

unsigned int SceneManager::loadTexture(string filename)
{
	unsigned int texture;

	// load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	
	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glActiveTexture(GL_TEXTURE0);

	return texture;
}

void SceneManager::setPresent(Sprite* obj, unsigned int texID)
{

	for (int i = 0; i < CONSTPRESENT; i++){

		obj = new Sprite;
		obj->setPosX(rand() % (700 - 0 + 1));
		obj->setPosY(620.0f);
		obj->setPosZ(0.0f);
		obj->setTempoQueda((float)(rand() % 99) / 1000);
		obj->setDimension(glm::vec3(66.0f, 60.0f, 1.0f));
		obj->setShader(shader);
		objectPresents.push_back(obj);

		objectPresents[i]->setTexture(texID);
	}

}

void SceneManager::setTreno(Sprite* obj, unsigned int texID)
{
	
	obj = new Sprite;
	obj->setPosX(300.0f);
	obj->setPosY(0.0f);
	obj->setPosZ(0.0f);
	obj->setDimension(glm::vec3(200.0f, 100.0f, 1.0f));
	obj->setShader(shader);
	objectTreno.push_back(obj);

	objectTreno[0]->setTexture(texID);
}