#pragma once

#include "engine/core/scene/Node.h"
#include "engine/core/render/mesh/Mesh.h"
#include "engine/core/render/Material.h"
#include "engine/core/render/render/Renderable.h"
#include "gltf_res.h"

namespace Echo
{
	class GltfMesh : public Node
	{
		ECHO_CLASS(GltfMesh, Node)

	public:
		GltfMesh();
		virtual ~GltfMesh();

		// set gltf resource
		const ResourcePath& getGltfRes() { return m_assetPath; }
		void setGltfRes(const ResourcePath& path);

		// set mesh index
		int getMeshIdx() const { return m_meshIdx; }
		void setMeshIdx(int meshIdx);

		// set primitive index
		int getPrimitiveIdx() { return m_primitiveIdx; }
		void setPrimitiveIdx(int primitiveIdx);

		// material
		Material* getMaterial() const { return m_material; }
		void setMaterial(const Object* material){}

	protected:
		// build drawable
		void buildRenderable();

		// update
		virtual void update();

		// get global uniforms
		virtual void* getGlobalUniformValue(const String& name);

		// clear
		void clear();
		void clearRenderable();

	private:
		Renderable*				m_renderable;
		Matrix4					m_matWVP;
		ResourcePath			m_assetPath;
		GltfResPtr				m_asset;		// gltf asset ptr
		int						m_meshIdx;		// mesh index in the asset
		int						m_primitiveIdx;	// sub mesh index
		Material*				m_material;
	};
}