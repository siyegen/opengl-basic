// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <string>

#include "shader.h"
#include "fpscamera.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void doMovement();

void drawCube(glm::vec3 cube, Shader* modelShader, Shader* lineShader, glm::mat4 view, glm::mat4 projection, GLuint containerVAO);

const GLuint WIDTH = 1280, HEIGHT = 1024;

// to get top down perspective
//glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, -1.0f, -1.0f);

const int levelWidth = 24;
const int numCubes = 30 * levelWidth;

FPSCamera camera(glm::vec3(((GLfloat)levelWidth/2 - 0.5f), 12.5f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f);
// Used for fps camera
bool keys[1024];
GLfloat lastX = WIDTH/2.0f, lastY = HEIGHT/2.0f;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame


GLfloat rotateCube = 0.0f;

// Light Position
glm::vec3 lightPos(1.2f, 15.0f, 2.0f);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (!window) {
		std::cout << "Couldn't create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Couldn't init glew" << std::endl;
		return -1;
	}

	char *GL_version = (char *)glGetString(GL_VERSION);
	char *GL_vendor = (char *)glGetString(GL_VENDOR);
	char *GL_renderer = (char *)glGetString(GL_RENDERER);
	std::cout << "Version" << GL_version << std::endl;
	std::cout << "Vendor" << GL_vendor << std::endl;
	std::cout << "Remderer" << GL_renderer << std::endl;

	// Setup mouse for fps style camera, will not need for game
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);

	// Load our shaders
	Shader lightedModel("shaders/simple3d.vs", "shaders/difuse_only.frag");
	Shader outlineShader("shaders/outline.vs", "shaders/outline.frag", "shaders/outline_geometry.gs");
	Shader lampShader("shaders/simple3d.vs", "shaders/lamp.frag");

	// Vertices and setting them up to draw
	GLfloat vertices[] = {
		// Position					//Normal
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	
	glm::vec3 cubePositions[numCubes];
	for (int i = 0, j = 0; i < numCubes; i++) {
		GLfloat x = 1.0f * (i%levelWidth);
		if (i != 0 && i % levelWidth == 0) {
			j++;
		}
		GLfloat z = -1.0f * j;
		cubePositions[i] = glm::vec3(x, 0.0f, z);
	}

	GLuint VBO, containerVAO;
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0); // Unbind VAO

	// Light source VAO
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	glm::mat4 projection;
	projection = glm::perspective(camera.Zoom, (GLfloat)(WIDTH / HEIGHT), 0.1f, 100.0f);

	while (!glfwWindowShouldClose(window)) {
		// timeDelta for different framerates
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents(); // input
		doMovement();

		// Color buffer clear
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLint modelLoc;
		GLint viewLoc;
		GLint projectionLoc;


		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();

		for (glm::vec3 cube : cubePositions) {
			drawCube(cube, &lightedModel, &outlineShader, view, projection, containerVAO);
		}
		
		// lamp object
		lampShader.Use();
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projectionLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set mat4 for shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw using light vertex attributes object (VAO)
		glBindVertexArray(lightVAO); // tells us what VAO to use, then unbind
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &containerVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}


void doMovement() {
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

	if (keys[GLFW_KEY_SPACE])
		rotateCube += 2.5f;

	GLfloat lightMove = 0.1f;

	if (keys[GLFW_KEY_LEFT])
		lightPos += glm::vec3(lightMove, 0.0f, lightMove);
	if (keys[GLFW_KEY_RIGHT])
		lightPos -= glm::vec3(lightMove, 0.0f, lightMove);
	if (keys[GLFW_KEY_UP])
		lightPos += glm::vec3(0.0f, lightMove, 0.0f);
	if (keys[GLFW_KEY_DOWN])
		lightPos -= glm::vec3(0.0f, lightMove, 0.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void drawCube(glm::vec3 cube, Shader* modelShader, Shader* lineShader, glm::mat4 view, glm::mat4 projection, GLuint containerVAO) {
	modelShader->Use();
	glm::mat4 model;
	// setup lighting uniform values
	GLint objectColorLoc = glGetUniformLocation(modelShader->Program, "objectColor");
	GLint lightColorLoc = glGetUniformLocation(modelShader->Program, "lightColor");
	GLint lightPosLoc = glGetUniformLocation(modelShader->Program, "lightPos");
	glUniform3f(objectColorLoc, 0.31f, 1.0f, 0.31f);
	glUniform3f(lightColorLoc, 0.8f, 0.5f, 1.0f);
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	
	// uniform locations for vertex
	GLint modelLoc = glGetUniformLocation(modelShader->Program, "model");
	GLint viewLoc = glGetUniformLocation(modelShader->Program, "view");
	GLint projectionLoc = glGetUniformLocation(modelShader->Program, "projection");
	// pass mat4 to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// Draw container
	glBindVertexArray(containerVAO);
	model = glm::translate(model, cube);

	model = glm::rotate(model, glm::radians(rotateCube), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	// Draw normals
	lineShader->Use();
	glBindVertexArray(containerVAO);
	modelLoc = glGetUniformLocation(lineShader->Program, "model");
	viewLoc = glGetUniformLocation(lineShader->Program, "view");
	projectionLoc = glGetUniformLocation(lineShader->Program, "projection");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
