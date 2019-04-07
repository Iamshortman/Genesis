#include "Camera.hpp"

using namespace Genesis;

Camera::Camera()
{
	this->frame_of_view = 90.0;
}

matrix4F Camera::getProjectionMatrix(vector2U& screen_size)
{
	float aspect_ratio = ((float)screen_size.x) / ((float)screen_size.y);

	float f = 1.0f / tan(glm::radians(this->frame_of_view) / 2.0f);
	matrix4F projection = matrix4F
	(
		f / aspect_ratio, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, this->z_near, 0.0f
	);

	return projection;
}

matrix4F Camera::getOrthographicMatrix(float x_bounds, float y_bounds)
{
	return glm::ortho(-x_bounds, x_bounds, -y_bounds, y_bounds, this->z_near, this->z_far);
}
