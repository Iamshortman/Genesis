#include "Genesis/Entity/World.hpp"

#include "Genesis/Debug/Assert.hpp"
#include "Genesis/Debug/Profiler.hpp"

#include "Genesis/Rendering/Camera.hpp"
#include "Genesis/Core/Transform.hpp"
#include "Genesis/Entity/DebugCamera.hpp"

#include "Genesis/Physics/RigidBody.hpp"

#include "Genesis/Entity/MeshComponent.hpp"

using namespace Genesis;
using namespace Genesis::Physics;

struct CharacterController
{
	CharacterController(double force)
	{
		this->forward_force = force;
		this->sideways_force = force;
	};

	double forward_force;
	double sideways_force;

	double forward_input;
	double sideways_input;
};

#include "Genesis/Rendering/ResourceLoaders.hpp"

World::World(MeshPool* mesh_pool, MaterialPool* material_pool)
{
	this->mesh_pool = mesh_pool;
	this->material_pool = material_pool;

	this->entity_registry = new EntityRegistry();
	this->physics_world = new PhysicsWorld(vector3D(0.0, -9.8, 0.0));

	{
		this->main_camera = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(this->main_camera, vector3D(0.0, 0.0, -20.0));
		this->entity_registry->assign<Camera>(this->main_camera, 77.0f);
		this->entity_registry->assign<DebugCamera>(this->main_camera, 5.0, 0.3);
	}

	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, 0.0, 0.0));
		this->entity_registry->assign<MeshComponent>(entity, this->mesh_pool->getResource("res/sphere.obj"), this->material_pool->getResource("res/materials/red.csv"));
		this->entity_registry->assign<RigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry->get<TransformD>(entity)));
		this->entity_registry->assign<ProxyShape>(entity, this->entity_registry->get<RigidBody>(entity).addCollisionShape(new reactphysics3d::SphereShape(1.0f), TransformD(), 1.0f));

		//this->entity_registry->assign<CharacterController>(entity, 5.0);

		this->entity_registry->get<RigidBody>(entity).get()->setAngularDamping(1.0);
	}

	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, -5.0, 0.0));
		this->entity_registry->assign<MeshComponent>(entity, this->mesh_pool->getResource("res/ground.obj"), this->material_pool->getResource("res/materials/grid.csv"));
		this->entity_registry->assign<StaticRigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry->get<TransformD>(entity)));
		this->entity_registry->assign<ProxyShape>(entity, this->entity_registry->get<StaticRigidBody>(entity).addCollisionShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(16.0, 1.0, 16.0)), TransformD(), 0.0f));
	}

	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, -5.0, 10.0));
		this->entity_registry->assign<MeshComponent>(entity, this->mesh_pool->getResource("res/portal.obj"), this->material_pool->getResource("res/materials/blue.csv"));
		this->entity_registry->assign<StaticRigidBody>(entity, this->physics_world->addRigidBody(this->entity_registry->get<TransformD>(entity)));
		this->entity_registry->assign<ProxyShape>(entity, this->entity_registry->get<StaticRigidBody>(entity).addCollisionShape(new reactphysics3d::BoxShape(reactphysics3d::Vector3(2.0, 3.0, 0.5)), TransformD(vector3D(0.0, 1.5, 0.0)), 0.0f));
	}
	{
		EntityHandle entity = this->entity_registry->create();
		this->entity_registry->assign<TransformD>(entity, vector3D(0.0, -5.0, 10.0));
		this->entity_registry->assign<MeshComponent>(entity, this->mesh_pool->getResource("res/portal_inside.obj"), this->material_pool->getResource("res/materials/white.csv"));
	}

}

World::~World()
{
	delete this->entity_registry;
	delete this->physics_world;
}

void World::runSimulation(Application* application, TimeStep time_step)
{
	GENESIS_PROFILE_FUNCTION("World::runSimulation");
	physics_world->simulate(time_step);

	//Physics System
	{
		auto& view = this->entity_registry->view<RigidBody, TransformD>();
		for (EntityHandle entity : view)
		{
			RigidBody& rigid_body = view.get<RigidBody>(entity);
			TransformD& transform = view.get<TransformD>(entity);
			TransformD rigid_body_transform = rigid_body.getTransform();
			transform.setPosition(rigid_body_transform.getPosition());
			transform.setOrientation(rigid_body_transform.getOrientation());
		}
	}

	//Character Systems
	{
		auto& view = this->entity_registry->view<CharacterController>();
		for (EntityHandle entity : view)
		{
			CharacterController& character = view.get<CharacterController>(entity);
			character.forward_input = application->input_manager.getButtonAxisCombo("Debug_ForwardBackward", "Debug_Forward", "Debug_Backward");
			character.sideways_input = application->input_manager.getButtonAxisCombo("Debug_LeftRight", "Debug_Left", "Debug_Right");
		}
	}
	{
		auto& view = this->entity_registry->view<RigidBody, CharacterController>();
		for (EntityHandle entity : view)
		{
			RigidBody& rigid_body = view.get<RigidBody>(entity);
			CharacterController& character = view.get<CharacterController>(entity);
			TransformD transform = rigid_body.getTransform();

			vector3D old_velocity = rigid_body.getLinearVelocity();

			vector3D velocity((transform.getForward() * character.forward_force * character.forward_input) + (transform.getLeft() * character.sideways_force * character.sideways_input));
			velocity.y = old_velocity.y;
			rigid_body.setLinearVelocity(velocity);
		}
	}

	DebugCamera::update(&application->input_manager, this->entity_registry->get<DebugCamera>(this->main_camera), this->entity_registry->get<TransformD>(this->main_camera), time_step);

}