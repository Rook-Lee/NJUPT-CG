#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  //需要什么变换，就导入什么文件，具体可以去官网看
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"


const GLint WIDTH = 800, HEIGHT = 600;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
glm::mat4 RotateArbitraryLine(glm::vec3 v1, glm::vec3 v2, float theta);

void DoMovement();

GLfloat lastX = 0.0f;
GLfloat lastY = 0.0f;
bool firstMouse = true;

bool keys[1024];

Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//for mac
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", nullptr, nullptr);

	// retina display
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, ScrollCallback);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Shader shader = Shader("core.vs", "core.frag");
	Shader lightShader = Shader("light.vs", "light.frag");

	GLfloat vertices[] = {
		//position              //color
		//back
		-0.5f,-0.5f,-0.5f,      1.0f,0.0f,0.0f,
		0.5f,-0.5f,-0.5f,       1.0f,0.0f,0.0f,
		0.5f,0.5f,-0.5f,        1.0f,0.0f,0.0f,
		0.5f,0.5f,-0.5f,        1.0f,0.0f,0.0f,
		-0.5f,0.5f,-0.5f,       1.0f,0.0f,0.0f,
		-0.5f,-0.5f,-0.5f,      1.0f,0.0f,0.0f,
		//front
		-0.5f,-0.5f,0.5f,       0.0f,1.0f,0.0f,
		0.5f,-0.5f,0.5f,        0.0f,1.0f,0.0f,
		0.5f,0.5f,0.5f,         0.0f,1.0f,0.0f,
		0.5f,0.5f,0.5f,         0.0f,1.0f,0.0f,
		-0.5f,0.5f,0.5f,        0.0f,1.0f,0.0f,
		-0.5f,-0.5f,0.5f,       0.0f,1.0f,0.0f,
		//left
		-0.5f,0.5f,0.5f,        0.0f,0.0f,1.0f,
		-0.5f,0.5f,-0.5f,       0.0f,0.0f,1.0f,
		-0.5f,-0.5f,-0.5f,      0.0f,0.0f,1.0f,
		-0.5f,-0.5f,-0.5f,      0.0f,0.0f,1.0f,
		-0.5f,-0.5f,0.5f,       0.0f,0.0f,1.0f,
		-0.5f,0.5f,0.5f,        0.0f,0.0f,1.0f,
		//right
		0.5f,0.5f,0.5f,         0.0f,1.0f,1.0f,
		0.5f,0.5f,-0.5f,        0.0f,1.0f,1.0f,
		0.5f,-0.5f,-0.5f,       0.0f,1.0f,1.0f,
		0.5f,-0.5f,-0.5f,       0.0f,1.0f,1.0f,
		0.5f,-0.5f,0.5f,        0.0f,1.0f,1.0f,
		0.5f,0.5f,0.5f,         0.0f,1.0f,1.0f,
		//bottom
		-0.5f,-0.5f,-0.5f,      1.0f,0.0f,1.0f,
		0.5f,-0.5f,-0.5f,		1.0f,0.0f,1.0f,
		0.5f,-0.5f,0.5f,		1.0f,0.0f,1.0f,
		0.5f,-0.5f,0.5f,		1.0f,0.0f,1.0f,
		-0.5f,-0.5f,0.5f,		1.0f,0.0f,1.0f,
		-0.5f,-0.5f,-0.5f,		1.0f,0.0f,1.0f,
		//top
		-0.5f,0.5f,-0.5f,       1.0f,1.0f,0.0f,
		0.5f,0.5f,-0.5f,		1.0f,1.0f,0.0f,
		0.5f,0.5f,0.5f,		    1.0f,1.0f,0.0f,
		0.5f,0.5f,0.5f,		    1.0f,1.0f,0.0f,
		-0.5f,0.5f,0.5f,		1.0f,1.0f,0.0f,
		-0.5f,0.5f,-0.5f,		1.0f,1.0f,0.0f,
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/*GLuint VAO1, VBO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);

	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);*/

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glViewport(0, 0, screenWidth, screenHeight);
		glfwPollEvents();
		DoMovement();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		glm::mat4 view = camera.GetViewMatrix();

		glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
			static_cast<GLfloat>(screenWidth) / static_cast<GLfloat>(screenHeight), 0.1f, 100.0f);
		GLuint viewLoc = glGetUniformLocation(shader.Program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		GLuint projectionLoc = glGetUniformLocation(shader.Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 transform = glm::mat4(1.0f);
		//transform = glm::translate(transform, glm::vec3(0.0f, 0.4f, 0.0f));
		//转换弧度
		transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(1.0f, 0, 0));
		transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0, 0, 1.0f));
		transform = glm::rotate(transform, glm::radians(90.0f) * static_cast<GLfloat>(glfwGetTime()), glm::vec3(1.0f, 1.0f, -1.0f));
		//transform = glm::translate(transform, glm::vec3(0.0f, 0.4f, 0.0f));//谁后进去谁先来
		//缩放
		transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));
		GLuint transLoc = glGetUniformLocation(shader.Program, "transform");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		lightShader.Use();
		viewLoc = glGetUniformLocation(lightShader.Program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		projectionLoc = glGetUniformLocation(lightShader.Program, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		/*transform = glm::mat4(1.0f);
		//transform = glm::translate(transform, glm::vec3(0.0f, 0.4f, 0.0f));
		//转换弧度
		transform = glm::rotate(transform, glm::radians(20.0f) * static_cast<GLfloat>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
		//平移
		transform = glm::translate(transform, glm::vec3(0.0f, 2.0f, 0.0f));//谁后进去谁先来
		//缩放
		transform = glm::scale(transform, glm::vec3(0.1f, 0.1f, 0.1f));
		transLoc = glGetUniformLocation(lightShader.Program, "transform");
		glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));*/

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}
void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}
	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	//
	camera.ProcessMouseMovement(xOffset, yOffset);
}
void DoMovement() {
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
		camera.ProcessKeyboard(FOWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessScroll(yOffset);
}
glm::mat4 RotateArbitraryLine(glm::vec3 v1, glm::vec3 v2, float theta) {
	glm::mat4 rmatrix;
	float a = v1.x;
	float b = v1.y;
	float c = v1.z;

	glm::vec3 p1 = v2 - v1;
	glm::vec3 p = glm::normalize(p1);

	float u = p.x;
	float v = p.y;
	float w = p.z;

	float uu = u * u;
	float uv = u * v;
	float uw = u * w;
	float vv = v * v;
	float vw = v * w;
	float ww = w * w;
	float au = a * u;
	float av = a * v;
	float aw = a * w;
	float bu = b * u;
	float bv = b * v;
	float bw = b * w;
	float cu = c * u;
	float cv = c * v;
	float cw = c * w;

	float costheta = glm::cos(theta);
	float sintheta = glm::sin(theta);

	rmatrix[0][0] = uu + (vv + ww) * costheta;
	rmatrix[0][1] = uv * (1 - costheta) + w * sintheta;
	rmatrix[0][2] = uw * (1 - costheta) - v * sintheta;
	rmatrix[0][3] = 0;

	rmatrix[1][0] = uv * (1 - costheta) - w * sintheta;
	rmatrix[1][1] = vv + (uu + ww) * costheta;
	rmatrix[1][2] = vw * (1 - costheta) + u * sintheta;
	rmatrix[1][3] = 0;

	rmatrix[2][0] = uw * (1 - costheta) + v * sintheta;
	rmatrix[2][1] = vw * (1 - costheta) - u * sintheta;
	rmatrix[2][2] = ww + (uu + vv) * costheta;
	rmatrix[2][3] = 0;

	rmatrix[3][0] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
	rmatrix[3][1] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
	rmatrix[3][2] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
	rmatrix[3][3] = 1;

	return rmatrix;
}