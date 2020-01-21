#pragma once

#include "Genesis/Rendering/Renderer.hpp"
#include "Genesis/Rendering/ImGuiRenderer.hpp"
#include "Genesis/Rendering/ResourceLoaders.hpp"

#include "Genesis/Entity.hpp"

namespace Genesis
{
	struct TempComponent
	{
		TransformF transform;
		matrix4F model_matrix;
	};


	//Prototype
	class Application;

	//TODO switch this class to a "World class"
	class GameScene
	{
	public:
		GameScene(Application* app);
		~GameScene();

		virtual void runSimulation(double delta_time);

		virtual void drawWorld(double delta_time);

	protected:
		Application* application;

		//Physics
		//PhysicsSystem physics_system;

		//EnTT
		EntityRegistry entity_registry;

		//Rendering
		Renderer* renderer = nullptr;
		ImGuiRenderer* ui_renderer = nullptr;

		Shader screen_shader = nullptr;
		VertexBuffer screen_vertex;
		IndexBuffer screen_index;
		Sampler screen_sampler;

		//Game Systems
			//Pre-Frame Update
			//Simulation Update
			//Pre-Frame Update
			//Render
	};
}