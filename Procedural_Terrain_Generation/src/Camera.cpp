#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 cameraPos) {
	position = cameraPos;
	windowWidth = width;
	windowHeight = height;
}

glm::mat4 Camera::getProjectionMatrix(float FOVdeg, float nearPlane, float farPlane) {
	return glm::perspective(glm::radians(FOVdeg), (float) windowWidth / windowHeight, nearPlane, farPlane);
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(position, position + orientation, up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)(windowWidth / windowHeight), nearPlane, farPlane);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::InputHandler(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += speed * orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position += speed * -glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position += speed * -orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += speed * glm::normalize(glm::cross(orientation, up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += speed * up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		position += speed * -up;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		speed *= 2;
	}

	// mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, (windowWidth / 2), (windowHeight / 2));
			firstClick = false;
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (windowHeight / 2)) / windowHeight;
		float rotY = sensitivity * (float)(mouseX - (windowWidth / 2)) / windowWidth;

		// Vertical change
		glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotX), glm::normalize(glm::cross(orientation, up)));

		// Prevents "barrel roll"
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			orientation = newOrientation;
		}

		// Horizontal change
		orientation = glm::rotate(orientation, glm::radians(-rotY), up);

		// Sets mouse coursor to the middle of the screen
		glfwSetCursorPos(window, (windowWidth / 2), (windowHeight / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}
