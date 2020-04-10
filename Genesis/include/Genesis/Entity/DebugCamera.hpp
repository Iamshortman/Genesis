#pragma once

#include "Genesis/Core/VectorTypes.hpp"
#include "Genesis/Core/Transform.hpp"

#include "Genesis/Input/InputManager.hpp"

namespace Genesis
{
	struct DebugCamera
	{
		DebugCamera(double linear, double angular) : linear_speed(linear), angular_speed(angular) {};

		//meter per second
		double linear_speed;

		//rad per second
		double angular_speed;

		static void update(InputManager* input_manager, DebugCamera& debug_camera, TransformD& transform, TimeStep time_step)
		{
			vector3D position = transform.getPosition();
			position += (transform.getForward() * input_manager->getButtonAxisCombo("Debug_ForwardBackward", "Debug_Forward", "Debug_Backward")) * debug_camera.linear_speed * time_step;
			position += (transform.getUp() * input_manager->getButtonAxisCombo("Debug_UpDown", "Debug_Up", "Debug_Down")) * debug_camera.linear_speed * time_step;
			position += (transform.getLeft() * input_manager->getButtonAxisCombo("Debug_LeftRight", "Debug_Left", "Debug_Right")) * debug_camera.linear_speed * time_step;
			transform.setPosition(position);

			quaternionD orientation = transform.getOrientation();
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Pitch", "Debug_PitchUp", "Debug_PitchDown", false) * debug_camera.angular_speed * (PI_D * 2.0) * time_step, transform.getLeft()) * orientation;
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Yaw", "Debug_YawLeft", "Debug_YawRight", false) * debug_camera.angular_speed * (PI_D * 2.0) * time_step, transform.getUp()) * orientation;
			orientation = glm::angleAxis(input_manager->getButtonAxisCombo("Debug_Roll", "Debug_RollRight", "Debug_RollLeft", false) * debug_camera.angular_speed * (PI_D) * time_step, transform.getForward()) * orientation;
			transform.setOrientation(orientation);
		};
	};
}