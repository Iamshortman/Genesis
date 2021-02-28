#include "genesis_editor/panels/scene_panel.hpp"

#include "imgui.h"

//Because Windows is stupid and stupid defines
#ifdef far
#undef far
#endif // far

#include "ImGuizmo.h"
#include "ImGuizmo.cpp"

namespace genesis
{
	SceneWindow::SceneWindow(InputManager* input_manager, LegacyBackend* legacy_backend)
	{
		this->input_manager = input_manager;

		this->legacy_backend = legacy_backend;
		this->world_renderer = new LegacySceneRenderer(this->legacy_backend);

		this->scene_camera_transform.setPosition(vec3d(0.0, 0.0, -5.0));
	}

	SceneWindow::~SceneWindow()
	{
		delete this->world_renderer;
		if (this->framebuffer != nullptr)
		{
			this->legacy_backend->destoryFramebuffer(this->framebuffer);
		}
	}

	constexpr fnv_hash32 debug_forward_axis = string_hash_32("Debug_ForwardBackward");
	constexpr fnv_hash32 debug_forward = string_hash_32("Debug_Forward");
	constexpr fnv_hash32 debug_backward = string_hash_32("Debug_Backward");

	constexpr fnv_hash32 debug_up_axis = string_hash_32("Debug_UpDown");
	constexpr fnv_hash32 debug_up = string_hash_32("Debug_Up");
	constexpr fnv_hash32 debug_down = string_hash_32("Debug_Down");

	constexpr fnv_hash32 debug_left_axis = string_hash_32("Debug_LeftRight");
	constexpr fnv_hash32 debug_left = string_hash_32("Debug_Left");
	constexpr fnv_hash32 debug_right = string_hash_32("Debug_Right");

	constexpr fnv_hash32 debug_pitch_axis = string_hash_32("Debug_Pitch");
	constexpr fnv_hash32 debug_pitch_up = string_hash_32("Debug_PitchUp");
	constexpr fnv_hash32 debug_pitch_down = string_hash_32("Debug_PitchDown");

	constexpr fnv_hash32 debug_yaw_axis = string_hash_32("Debug_Yaw");
	constexpr fnv_hash32 debug_yaw_left = string_hash_32("Debug_YawLeft");
	constexpr fnv_hash32 debug_yaw_right = string_hash_32("Debug_YawRight");

	constexpr fnv_hash32 debug_roll_axis = string_hash_32("Debug_Roll");
	constexpr fnv_hash32 debug_roll_left = string_hash_32("Debug_RollLeft");
	constexpr fnv_hash32 debug_roll_right = string_hash_32("Debug_RollRight");

	void SceneWindow::update(TimeStep time_step)
	{		
		if (this->window_active)
		{
			vec3d position = this->scene_camera_transform.getPosition();
			position += (this->scene_camera_transform.getForward() * (double)this->input_manager->getButtonAxis(debug_forward_axis, debug_forward, debug_backward)) * this->linear_speed * time_step;
			position += (this->scene_camera_transform.getUp() * (double)this->input_manager->getButtonAxis(debug_up_axis, debug_up, debug_down)) * this->linear_speed * time_step;
			position += (this->scene_camera_transform.getLeft() * (double)this->input_manager->getButtonAxis(debug_left_axis, debug_left, debug_right)) * this->linear_speed * time_step;
			this->scene_camera_transform.setPosition(position);

			quatd orientation = this->scene_camera_transform.getOrientation();
			orientation = glm::angleAxis((double)this->input_manager->getButtonAxis(debug_pitch_axis, debug_pitch_up, debug_pitch_down) * this->angular_speed * (PI_D * 2.0) * time_step, this->scene_camera_transform.getLeft()) * orientation;
			orientation = glm::angleAxis((double)this->input_manager->getButtonAxis(debug_yaw_axis, debug_yaw_left, debug_yaw_right) * this->angular_speed * (PI_D * 2.0) * time_step, this->scene_camera_transform.getUp()) * orientation;
			orientation = glm::angleAxis((double)this->input_manager->getButtonAxis(debug_roll_axis, debug_roll_left, debug_roll_right) * this->angular_speed * (PI_D) * time_step, -this->scene_camera_transform.getForward()) * orientation;
			this->scene_camera_transform.setOrientation(orientation);
		}
	}

	void decompose_model_matrix(const mat4f& model_matrix, vec3f& translation, quatf& rotation, vec3f& scale);

	void SceneWindow::draw(SceneRenderList& render_list, SceneLightingSettings& lighting, Entity selected_entity)
	{
		ImGui::Begin("Scene View", nullptr, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Graphics"))
			{
				ImGui::MenuItem("Lighting Enabled", nullptr, &this->settings.lighting);
				ImGui::MenuItem("Frustrum Culling", nullptr, &this->settings.frustrum_culling);
				ImGui::Separator();
				ImGui::Text("Gamma Correction:");
				ImGui::SliderFloat("##Gamma Correction:", &lighting.gamma_correction, 1.0f, 5.0f, "%.2f");
				ImGui::ColorEdit3("Ambient Light", &lighting.ambient_light.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Camera"))
			{
				if (ImGui::MenuItem("Default", nullptr, true))
				{
				}

				ImGui::Separator();
				if (ImGui::BeginMenu("Default Camera"))
				{
					ImGui::Text("Fov X:");
					if (ImGui::DragFloat("##Fov_X", &this->scene_camera.frame_of_view, 0.5f, 1.0f, 140.0f))
					{
						this->scene_camera.frame_of_view = std::clamp(this->scene_camera.frame_of_view, 1.0f, 140.0f);
					}

					ImGui::Text("Z Near:");
					if (ImGui::InputFloat("##Z_Near", &this->scene_camera.z_near))
					{
						this->scene_camera.z_near = std::max(this->scene_camera.z_near, 0.001f);
					}

					ImGui::Text("Z Far:");
					if (ImGui::InputFloat("##Z_Far", &this->scene_camera.z_far))
					{
						this->scene_camera.z_far = std::max(this->scene_camera.z_near + 1.0f, this->scene_camera.z_far);
					}
					ImGui::Separator();

					ImGui::Text("Position:");
					vec3d position = this->scene_camera_transform.getPosition();
					if (ImGui::InputScalarN("##Position", ImGuiDataType_::ImGuiDataType_Double, &position, 3))
					{
						this->scene_camera_transform.setPosition(position);
					};

					ImGui::Text("Rotation:");
					vec3d rotation = glm::degrees(glm::eulerAngles(this->scene_camera_transform.getOrientation()));
					if (ImGui::InputScalarN("##Rotation", ImGuiDataType_::ImGuiDataType_Double, &rotation, 3))
					{
						this->scene_camera_transform.setOrientation(quatd(glm::radians(rotation)));
					}
					
					ImGui::Separator();
					const double MIN_MOVE_SPEED = 0.001;
					const double MAX_MOVE_SPEED = 10000.0;
					ImGui::Text("Linear Speed:");
					if (ImGui::DragScalar("##Linear_Speed", ImGuiDataType_::ImGuiDataType_Double, &this->linear_speed, 0.1, &MIN_MOVE_SPEED, &MAX_MOVE_SPEED))
					{
						this->linear_speed = std::clamp(this->linear_speed, MIN_MOVE_SPEED, MAX_MOVE_SPEED);
					}

					const double MIN_ROTATION_SPEED = PI_D / 16;
					const double MAX_ROTATION_SPEED = PI_D / 4;
					ImGui::Text("Rotational Speed:");
					if (ImGui::DragScalar("##Rotation_Speed", ImGuiDataType_::ImGuiDataType_Double, &this->angular_speed, 0.01, &MIN_ROTATION_SPEED, &MAX_ROTATION_SPEED))
					{
						this->angular_speed = std::clamp(this->angular_speed, MIN_ROTATION_SPEED, MAX_ROTATION_SPEED);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImVec2 im_image_pos = ImGui::GetCursorScreenPos();
		ImVec2 im_image_size = ImGui::GetContentRegionAvail();
		vec2u image_size = vec2u(im_image_size.x, im_image_size.y);

		if (image_size != this->framebuffer_size)
		{
			//Rebuild Framebuffer
			if (this->framebuffer != nullptr)
			{
				this->legacy_backend->destoryFramebuffer(this->framebuffer);
			}

			this->framebuffer_size = image_size;
			FramebufferAttachmentInfo color_attachment = { LegacyImageFormat::RGBA_32_Float, MultisampleCount::Sample_1 };
			FramebufferDepthInfo depth_attachment = { LegacyDepthFormat::depth_24,  MultisampleCount::Sample_1 };
			FramebufferCreateInfo create_info = {};
			create_info.attachments = &color_attachment;
			create_info.attachment_count = 1;
			create_info.depth_attachment = &depth_attachment;
			create_info.size = this->framebuffer_size;

			this->framebuffer = this->legacy_backend->createFramebuffer(create_info);
		}

		CameraStruct active_camera = {};
		active_camera.camera = this->scene_camera;
		active_camera.transform = this->scene_camera_transform;

		this->world_renderer->draw_scene(this->framebuffer_size, this->framebuffer, render_list, lighting, this->settings, active_camera);

		ImGui::Image((ImTextureID)this->legacy_backend->getFramebufferColorAttachment(this->framebuffer, 0), im_image_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

		this->window_active = ImGui::IsWindowFocused();

		ImVec2 im_mouse_pos = ImGui::GetMousePos();
		ImVec2 im_min_pos = im_image_pos;

		//ImGuizmo
		if (selected_entity.valid() && selected_entity.has<TransformD>())
		{
			TransformD& transform = selected_entity.get<TransformD>();

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::BeginFrame();
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(im_image_pos.x, im_image_pos.y, im_image_size.x, im_image_size.y);

			mat4f projection_matrix = this->scene_camera.get_infinite_projection_no_flip(image_size);
			mat4f view_matrix = this->scene_camera_transform.getViewMatirx();

			mat4f model_matrix = transform.getModelMatrix();

			ImGuizmo::Manipulate(glm::value_ptr(view_matrix), glm::value_ptr(projection_matrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(model_matrix));

			//TODO use decompose that doesn't give skew and perspective
			vec3f translation;
			quatf rotation;
			vec3f scale;

			decompose_model_matrix(model_matrix, translation, rotation, scale);

			//vec3f skew;
			//vec4f perspective;
			//glm::decompose(model_matrix, scale, rotation, translation, skew, perspective);

			if (ImGuizmo::IsUsing())
			{
				transform.setPosition((vec3d)translation);
				transform.setOrientation((quatd)rotation);
				transform.setScale((vec3d)scale);
			}
		}

		ImGui::End();
	}

	void decompose_model_matrix(const mat4f& model_matrix, vec3f& translation, quatf& rotation, vec3f& scale)
	{
		//Get translation
		translation = vec3f(model_matrix[3]);

		//Get scale
		vec3f rows[3];
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				rows[i][j] = model_matrix[i][j];
			}
		}

		for (size_t i = 0; i < 3; i++)
		{
			scale[i] = glm::length(rows[i]);
			rows[i] = glm::normalize(rows[i]);
		}

		//Finally get rotation
		int i, j, k = 0;
		float root;
		float trace = rows[0].x + rows[1].y + rows[2].z;
		if (trace > 0.0f)
		{
			root = sqrt(trace + 1.0f);
			rotation.w = 0.5f * root;
			root = 0.5f / root;
			rotation.x = root * (rows[2].y - rows[1].z);
			rotation.y = root * (rows[0].z - rows[2].x);
			rotation.z = root * (rows[1].x - rows[0].y);
		} // End if > 0
		else
		{
			static int Next[3] = { 1, 2, 0 };
			i = 0;
			if (rows[1].y > rows[0].x) i = 1;
			if (rows[2].z > rows[i][i]) i = 2;
			j = Next[i];
			k = Next[j];

			root = sqrt(rows[i][i] - rows[j][j] - rows[k][k] + 1.0f);

			rotation[i] = 0.5f * root;
			root = 0.5f / root;
			rotation[j] = root * (rows[i][j] + rows[j][i]);
			rotation[k] = root * (rows[i][k] + rows[k][i]);
			rotation.w = root * (rows[j][k] - rows[k][j]);
		}
	}
}