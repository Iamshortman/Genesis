#pragma once

#include "genesis_engine/scene/Ecs.hpp"
#include "genesis_engine/scene/Entity.hpp"

namespace Genesis
{
	class ResourceManager;

	class EntityPropertiesWindow
	{
	protected:
		ResourceManager* resource_manager = nullptr;

	public:
		EntityPropertiesWindow(ResourceManager* resource_manager);

		void draw(Entity entity);
	};
}