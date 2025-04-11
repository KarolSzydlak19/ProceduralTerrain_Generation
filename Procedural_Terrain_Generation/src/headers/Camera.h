#include <glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "Shader.h"
#include <Map.h>
#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H


class Camera
{
public:
	glm::vec3 position;
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	bool &rotateObject;

	bool firstClick = true;

	// width and height of window
	int windowWidth;
	int windowHeight;

	float speed = 0.1f;
	float sensitivity = 100.0f;

	Camera(int windowWidth, int windowHeight, glm::vec3 position, bool &rotateObject);

	void Matrix(Shader& shader, const char* uniform);

	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

	void InputHandler(GLFWwindow* window, bool mouseCapture);

	glm::mat4 getProjectionMatrix(float FOVdeg, float nearPlane, float farPlane);

};
#endif // !CAMERA_CLASS_H
