#include "genesis_engine/resource/mesh_pool.hpp"

#include "genesis_engine/resource/obj_loader.hpp"

namespace genesis
{
	MeshPool::MeshPool(LegacyBackend* backend)
	{
		this->backend = backend;
	}

	shared_ptr<Mesh> MeshPool::loadResource(const string& key)
	{
		MeshStruct mesh = ObjLoader::loadMesh(this->backend, key);
		return std::make_shared<Mesh>(key, this->backend, mesh);
	}
}