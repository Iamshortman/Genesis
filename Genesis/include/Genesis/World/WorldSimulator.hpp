#pragma once

namespace Genesis
{
	class Entity;
	class World;

	typedef void(*WorldUpdateFunction)(const TimeStep, World*);
	typedef void(*RootEntityUpdateFunction)(const TimeStep, World*, Entity*);
	typedef void(*EntityUpdateFunction)(const TimeStep, World*, Entity*, const TransformD&, Entity*);

	struct WorldSystem
	{
		WorldUpdateFunction pre_update = nullptr;
		RootEntityUpdateFunction root_update = nullptr;
		EntityUpdateFunction entity_update = nullptr;
		WorldUpdateFunction post_update = nullptr;
	};

	class WorldSimulator
	{
	public:
		void addWorldSystem(const WorldSystem& system);

		void simulateWorld(const TimeStep time_step, World* world);

	protected:
		void updateRootEntity(const TimeStep time_step, World* world, Entity* root);
		void updateEntity(const TimeStep time_step, World* world, Entity* root, const TransformD& parent_transform, Entity* entity);

		vector<WorldUpdateFunction> preWorldUpdateFunctions;
		vector<WorldUpdateFunction> postWorldUpdateFunctions;

		vector<RootEntityUpdateFunction> rootEntityUpdateFunctions;
		vector<EntityUpdateFunction> entityUpdateFunctions;

	};
}