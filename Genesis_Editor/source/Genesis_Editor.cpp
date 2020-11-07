#include "Genesis_Editor/EditorApplication.hpp"

int main(int argc, char** argv)
{
	GENESIS_PROFILE_START(); 
	Genesis::Logging::inti_engine_logging();
	Genesis::Logging::inti_client_logging("Genesis_Editor");

	Genesis::EditorApplication* editor = new Genesis::EditorApplication();
	GENESIS_INFO("Genesis_Editor Started");

	GENESIS_PROFILE_BLOCK_START("Genesis_Editor Loop");
	editor->run();
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_PROFILE_BLOCK_START("Genesis_Editor Exit");
	delete editor;
	GENESIS_PROFILE_BLOCK_END();

	GENESIS_INFO("Genesis_Editor Closed");

	GENESIS_PROFILE_WRITE_TO_FILE("Genesis_profile.prof");

	//Wait till enter
	//getchar();

	return 0;
}

//Hardcoded for now
#include "SDL2_Platform.hpp" 
#include "SDL2_Window.hpp" 
#include "OpenglBackend.hpp"
#include "Genesis/LegacyRendering/LegacyImGui.hpp"
#include "Genesis/Platform/FileSystem.hpp"

#include "imgui.h"

//Components
#include "Genesis/Component/ModelComponent.hpp"
#include "Genesis/Component/NameComponent.hpp"
#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Rendering/Lights.hpp"
#include "Genesis/Physics/RigidBody.hpp"
#include "Genesis/Physics/CollisionShape.hpp"
#include "Genesis/Physics/ReactPhyscis.hpp"

#include "Genesis/Resource/ObjLoader.hpp"

#include "Genesis/PhysicsTest/PhysicsTestSystems.hpp"
#include "Genesis/PhysicsTest/CollisionShape.hpp"

#include "Genesis/World/Entity.hpp"

#include <jsoncons/json.hpp>
#include <fstream>
using namespace jsoncons;

namespace Genesis
{
	void loadSceneTemp(EntityWorld& world, const string& json_file, MeshPool* mesh_pool, MaterialPool* material_pool);

	EditorApplication::EditorApplication()
	{
		this->console_window = new ConsoleWindow();
		Logging::console_sink->setConsoleWindow(this->console_window);

		this->platform = new SDL2_Platform(this);
		this->window = new SDL2_Window(vector2U(1600, 900), "Genesis Editor");

		this->legacy_backend = new Opengl::OpenglBackend((SDL2_Window*) window);
		this->ui_renderer = new LegacyImGui(this->legacy_backend, this->input_manager, this->window);

		this->mesh_pool = new MeshPool(this->legacy_backend);
		this->texture_pool = new TexturePool(this->legacy_backend);
		this->material_pool = new MaterialPool(this->texture_pool);

		this->temp_material = this->material_pool->getResource("res/materials/grid.mat");

		this->entity_hierarchy_window = new EntityHierarchyWindow();
		this->entity_properties_window = new EntityPropertiesWindow(this->mesh_pool, this->material_pool);
		this->scene_window = new SceneWindow(this->input_manager, this->legacy_backend);
		this->asset_browser_window = new AssetBrowserWindow(this->legacy_backend);
		this->material_editor_window = new MaterialEditorWindow(this->material_pool, this->texture_pool);
		this->material_editor_window->setActiveMaterial(this->temp_material);

		this->editor_world = new EntityWorld();

		{
			Entity entity = this->editor_world->createEntity("Camera_Entity");
			entity.addComponent<TransformD>().setPosition(vector3D(0.0, 0.0, -3.0));
			entity.addComponent<Camera>();
			entity.addComponent<DirectionalLight>(vector3F(1.0f), 1.0f, true);
		}

		{
			Experimental::Entity* entity = new Experimental::Entity(0, "Camera_Entity");
			entity->addComponent<Camera>();
			entity->addComponent<DirectionalLight>(vector3F(1.0f), 1.0f, true);
			this->test_editor_world.addEntity(entity);
		}

		{
			Entity sphere = this->editor_world->createEntity("Sphere");
			sphere.addComponent<TransformD>();
			sphere.addComponent<ModelComponent>(this->mesh_pool->getResource("res/meshes/sphere.obj"), this->material_pool->getResource("res/materials/red.mat"));
		}

		loadSceneTemp(*this->editor_world, "res/ground.entity", this->mesh_pool, this->material_pool);
	}

	EditorApplication::~EditorApplication()
	{
		this->temp_material.reset();

		delete this->editor_world;

		delete this->mesh_pool;
		delete this->texture_pool;
		delete this->material_pool;

		delete this->console_window;
		delete this->entity_hierarchy_window;
		delete this->entity_properties_window;
		delete this->scene_window;
		delete this->asset_browser_window;
		delete this->material_editor_window;

		delete this->legacy_backend;

		Logging::console_sink->setConsoleWindow(nullptr);
	}

	void EditorApplication::update(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::update");
		Application::update(time_step);

		this->scene_window->update(time_step);

		if (this->scene_window->isSceneRunning())
		{
			this->editor_world->runSimulation(time_step);
		}

		this->editor_world->resolveTransforms();

		this->test_world_simulator.simulateWorld(time_step, &this->test_editor_world);
	}

	void EditorApplication::render(TimeStep time_step)
	{
		GENESIS_PROFILE_FUNCTION("EditorApplication::render");

		Application::render(time_step);

		this->legacy_backend->startFrame();
		vector4F clear_color = vector4F(0.0f, 0.0f, 0.0f, 1.0f);
		float clear_depth = 1.0f;
		this->legacy_backend->clearFramebuffer(true, true, &clear_color, &clear_depth);

		this->ui_renderer->beginFrame();
		this->ui_renderer->beginDocking();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Scene", ""))
				{

				}

				if (ImGui::MenuItem("Exit", "")) { this->close(); };
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		this->ui_renderer->endDocking();

		//ImGui::ShowDemoWindow();

		{
			FrameStats stats = this->legacy_backend->getLastFrameStats();
			ImGui::Begin("Stats");
			ImGui::Text("Frame Time (ms): %.2f", time_step * 1000.0);
			ImGui::Text("Draw Calls     : %u", stats.draw_calls);
			ImGui::Text("Tris count     : %u", stats.triangles_count);
			ImGui::End();
		}

		this->console_window->draw();
		this->entity_hierarchy_window->draw(this->editor_world, this->mesh_pool, this->material_pool);
		this->entity_properties_window->draw(this->entity_hierarchy_window->getSelected());
		this->scene_window->draw(*this->editor_world);
		this->asset_browser_window->draw("res/");
		this->material_editor_window->draw();

		this->ui_renderer->endFrame();

		this->legacy_backend->endFrame();
	}

	void loadEntity(json& json_entity, Entity entity, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		if (json_entity.contains("Name"))
		{
			entity.addComponent<NameComponent>(json_entity["Name"].as_string().c_str());
		}

		if (json_entity.contains("Transform"))
		{
			json& json_transform = json_entity["Transform"];
			TransformD& transform = entity.addComponent<TransformD>();
			
			vector<double> position = json_transform["position"].as<vector<double>>();
			transform.setPosition(vector3D(position[0], position[1], position[2]));

			vector<double> orientation = json_transform["orientation"].as<vector<double>>();
			transform.setOrientation(quaternionD(orientation[0], orientation[1], orientation[2], orientation[3]));

			vector<double> scale = json_transform["scale"].as<vector<double>>();
			transform.setScale(vector3D(scale[0], scale[1], scale[2]));
		}

		if (json_entity.contains("Model"))
		{
			json& json_model = json_entity["Model"];
			ModelComponent& model = entity.addComponent<ModelComponent>();
			model.mesh = mesh_pool->getResource(json_model["mesh"].as_string());
			model.material = material_pool->getResource(json_model["material"].as_string());
		}

		if (json_entity.contains("RigidBody"))
		{
			json& json_rigid_body = json_entity["RigidBody"];
			RigidBody& rigid_body = entity.addComponent<RigidBody>();
			rigid_body.setType((RigidBodyType)json_rigid_body["type"].as<int>());
			rigid_body.setMass(json_rigid_body["mass"].as_double());
			rigid_body.setGravityEnabled(json_rigid_body["gravity_enabled"].as_bool());
			rigid_body.setIsAllowedToSleep(json_rigid_body["is_allowed_to_sleep"].as_bool());

			vector<double> linear_velocity = json_rigid_body["linear_velocity"].as<vector<double>>();
			rigid_body.setLinearVelocity(vector3D(linear_velocity[0], linear_velocity[1], linear_velocity[2]));

			vector<double> angular_velocity = json_rigid_body["angular_velocity"].as<vector<double>>();
			rigid_body.setAngularVelocity(vector3D(angular_velocity[0], angular_velocity[1], angular_velocity[2]));
		}

	}

	void loadSceneTemp(EntityWorld& world, const string& json_file, MeshPool* mesh_pool, MaterialPool* material_pool)
	{
		std::ifstream in_stream(json_file);

		if (!in_stream.is_open())
		{
			GENESIS_ENGINE_ASSERT("Failed to open scene file {}", json_file);
			return;
		}

		jsoncons::json scene_file = json::parse(in_stream);

		for (std::size_t i = 0; i < scene_file.size(); ++i)
		{
			loadEntity(scene_file[i], world.createEntity(), mesh_pool, material_pool);
		}
	}
}