#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 cameraPos, bool &rotateObject): rotateObject(rotateObject) {
	position = cameraPos;
	windowWidth = width;
	windowHeight = height;
}

glm::mat4 Camera::getProjectionMatrix(float FOVdeg, float nearPlane, float farPlane) {
	return glm::perspective(glm::radians(FOVdeg), (float) windowWidth / windowHeight, nearPlane, farPlane);
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(position, position + orientation, up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)windowWidth / windowHeight, nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::InputHandler(GLFWwindow* window, bool mouseCapture)
{
	if (!rotateObject) {
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
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseCapture)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (mouseCapture) {
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
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && mouseCapture)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			firstClick = true;
		}
	}
	else {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			//objectPosition += speed * glm::vec3(0.0f, 0.0f, -1.0f); // Move object forward
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			//objectPosition += speed * glm::vec3(-1.0f, 0.0f, 0.0f); // Move object left
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			//objectPosition += speed * glm::vec3(0.0f, 0.0f, 1.0f);  // Move object backward
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			//objectPosition += speed * glm::vec3(1.0f, 0.0f, 0.0f);  // Move object right
		}

		// Mouse movement for object rotation
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && mouseCapture) {
			// Use mouse to rotate the object instead of the camera
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float rotX = sensitivity * (float)(mouseY - (windowHeight / 2)) / windowHeight;
			float rotY = sensitivity * (float)(mouseX - (windowWidth / 2)) / windowWidth;

			//objectRotation.x += rotX;  // Rotate object on X-axis
			//objectRotation.y += rotY;  // Rotate object on Y-axis

			// Reset cursor position
			glfwSetCursorPos(window, (windowWidth / 2), (windowHeight / 2));
		}
	}
	
}
