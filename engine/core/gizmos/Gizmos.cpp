#include "Gizmos.h"
#include "engine/core/memory/MemAllocDef.h"
#include "engine/core/scene/node_tree.h"
#include "engine/core/render/base/renderer.h"

// material for vulkan or metal or opengles
static const char* g_gizmoVsCode = R"(#version 450

// uniforms
layout(binding = 0) uniform UBO
{
    mat4 u_WorldMatrix;
    mat4 u_ViewProjMatrix;
} vs_ubo;

// inputs
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_UV;

// outputs
layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec4 v_Color;
layout(location = 2) out vec2 v_UV;

void main(void)
{
    vec4 position = vs_ubo.u_WorldMatrix * vec4(a_Position, 1.0);
    
    v_Position  = position.xyz;
    gl_Position = vs_ubo.u_ViewProjMatrix * position;
    
    v_Color = a_Color;
	v_UV = a_UV;
}
)";

static const char* g_gizmoPsCode = R"(#version 450

precision mediump float;

// uniforms
layout(binding = 0) uniform UBO
{
    vec3  u_CameraPosition;
    float u_CameraFar;
} fs_ubo;

// uniforms
#ifdef ENABLE_ALBEDO_TEXTURE
layout(binding = 3) uniform sampler2D BaseColor;
#endif

// inputs
layout(location = 0) in vec3  v_Position;
layout(location = 1) in vec4  v_Color;
layout(location = 2) in vec2  v_UV;

// outputs
layout(location = 0) out vec4 o_FragColor;

void main(void)
{
	vec4 finalColor = v_Color;

#ifdef ENABLE_ALBEDO_TEXTURE
    finalColor = texture(BaseColor, v_UV) * finalColor;
#endif

    o_FragColor = finalColor;
}
)";

namespace Echo
{
	Gizmos::Batch::Batch(Material* material, Gizmos* gizmos)
	{
		m_gizmos = gizmos;
		m_material = material;
		m_mesh = Mesh::create(true, true);
		m_renderable = nullptr;
		m_meshDirty = true;
	}

	Gizmos::Batch::~Batch()
	{
		if (m_renderable)
		{
			m_renderable->release();
			m_renderable = nullptr;
		}

		m_mesh.reset();
	}

	void Gizmos::Batch::addVertex(const Gizmos::VertexFormat& vert)
	{
		m_vertexs.push_back(vert);
		m_meshDirty = true;
	}

	void Gizmos::Batch::addIndex(Word idx)
	{
		m_indices.push_back(idx);
		m_meshDirty = true;
	}

	void Gizmos::Batch::clear()
	{
		m_vertexs.clear();
		m_indices.clear();

		m_meshDirty = true;
	}

	void Gizmos::Batch::update()
	{
		if (m_meshDirty)
		{
			MeshVertexFormat define;
			define.m_isUseVertexColor = true;
			define.m_isUseUV = true;

			m_mesh->updateIndices((ui32)m_indices.size(), sizeof(Word), m_indices.data());
			m_mesh->updateVertexs(define, (ui32)m_vertexs.size(), (const Byte*)m_vertexs.data());

			m_meshDirty = false;
		}

		if (m_albedo && m_material->isMacroUsed("ENABLE_ALBEDO_TEXTURE"))
		{
			m_material->getUniform("BaseColor")->setTexture(m_albedo);
		}

		// render
		if (m_renderable)
		{
			m_renderable->submitToRenderQueue();
		}
		else if(m_vertexs.size())
		{
			m_renderable = Renderable::create(m_mesh, m_material, m_gizmos);
			m_renderable->submitToRenderQueue();
		}

		// auto clear data
		if (m_gizmos->isAutoClear())
		{
			clear();
		}
	}

	Gizmos::Gizmos()
	{
        m_shader = initDefaultShader(false);
		m_shaderSprite = initDefaultShader(true);
        
		m_material = ECHO_CREATE_RES(Material);
		m_material->setShaderPath( m_shader->getPath());

		m_lineBatch = EchoNew(Batch(m_material, this));
		m_lineBatch->m_mesh->setTopologyType(Mesh::TT_LINELIST);

		m_triangleBatch = EchoNew(Batch(m_material, this));
		m_triangleBatch->m_mesh->setTopologyType(Mesh::TT_TRIANGLELIST);
	}

	Gizmos::~Gizmos()
	{
		m_material.reset();

		EchoSafeDelete(m_lineBatch, Batch);
		EchoSafeDelete(m_triangleBatch, Batch);
		EchoSafeDeleteContainer(m_spriteBatchs, Batch);
	}

	void Gizmos::bindMethods()
	{
	}

    ShaderProgramPtr Gizmos::initDefaultShader(bool enableAlbedoTexture)
    {
        ResourcePath shaderVirtualPath = ResourcePath(enableAlbedoTexture ? "echo_gizmo_default_shader_sprite" : "echo_gizmo_default_shader");
        ShaderProgramPtr shader = ECHO_DOWN_CAST<ShaderProgram*>(ShaderProgram::get(shaderVirtualPath));
        if(!shader)
        {
			shader = ECHO_CREATE_RES(ShaderProgram);

			// macros
			if (enableAlbedoTexture)
				shader->setMacros({ "ENABLE_ALBEDO_TEXTURE" });

			// render state
			shader->setBlendMode("Transparent");
			shader->setCullMode("CULL_NONE");

			// set code
			shader->setPath(shaderVirtualPath.getPath());
			shader->setType("glsl");
			shader->setVsCode(g_gizmoVsCode);
			shader->setPsCode(g_gizmoPsCode);
        }

		return shader;
    }

	void Gizmos::adjustPointSize(float& radius, const Vector3& position, int flags)
	{
		Camera* camera = getCamera();
		if (camera)
		{
			if (camera->getProjectionMode() == Camera::ProjMode::PM_PERSPECTIVE && (flags & RenderFlags::FixedPixel))
			{
				float halfHeight = camera->getHeight() * 0.5f;
				float ratio = radius / halfHeight;
				float halfNearPlaneWidth = camera->getNear() * tan(camera->getFov() * 0.5f);
				float pointDistance = (camera->getPosition() - position).len();

				radius = (ratio * halfNearPlaneWidth) * (pointDistance / camera->getNear());
			}
		}
	}

	void Gizmos::drawPoint(const Vector3& position, const Color& color, float pixels, int segments, int flags)
	{
		Camera* camera = getCamera();
		if (camera)
		{
			float radius = pixels / 2.f;
			adjustPointSize(radius, position, flags);

			float deltaDegree = 2.f * Math::PI / segments;
			for (int i = 0; i < segments; i++)
			{
				Vector3 v0 = position - camera->getDirection() * radius;
				Vector3 v1 = v0 + Quaternion::fromAxisAngle(camera->getDirection(), i * deltaDegree + Math::PI_DIV4).rotateVec3(camera->getRight()) * radius;
				Vector3 v2 = v0 + Quaternion::fromAxisAngle(camera->getDirection(), (i + 1) * deltaDegree + Math::PI_DIV4).rotateVec3(camera->getRight()) * radius;

				drawTriangle(v0, v1, v2, color);
			}
		}
	}

	void Gizmos::drawLine(const Vector3& from, const Vector3& to, const Color& color)
	{
		m_lineBatch->addIndex((Word)m_lineBatch->m_vertexs.size());
		m_lineBatch->addIndex((Word)m_lineBatch->m_vertexs.size() + 1);

		m_lineBatch->addVertex(VertexFormat(from, color));
		m_lineBatch->addVertex(VertexFormat(to, color));

		m_localAABB.addPoint(from);
		m_localAABB.addPoint(to);
	}

	void Gizmos::drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Color& color)
	{
		m_triangleBatch->addIndex((Word)m_triangleBatch->m_vertexs.size());
		m_triangleBatch->addIndex((Word)m_triangleBatch->m_vertexs.size() + 1);
		m_triangleBatch->addIndex((Word)m_triangleBatch->m_vertexs.size() + 2);

		m_triangleBatch->addVertex(VertexFormat(v0, color));
		m_triangleBatch->addVertex(VertexFormat(v1, color));
		m_triangleBatch->addVertex(VertexFormat(v2, color));

		m_localAABB.addPoint(v0);
		m_localAABB.addPoint(v1);
		m_localAABB.addPoint(v2);
	}

	void Gizmos::drawSprite(const Vector3& position, const Color& color, float pixels, TexturePtr texture, int flags)
	{
		Camera* camera = getCamera();
		if (camera)
		{
			float radius = pixels / 2.f;
			adjustPointSize(radius, position, flags);

			Vector3 center = position;
			Vector3 v0 = center + Quaternion::fromAxisAngle(camera->getDirection(), Math::PI_DIV2 * 0.f + Math::PI_DIV4).rotateVec3(camera->getRight()) * radius;
			Vector3 v1 = center + Quaternion::fromAxisAngle(camera->getDirection(), Math::PI_DIV2 * 1.f + Math::PI_DIV4).rotateVec3(camera->getRight()) * radius;
			Vector3 v2 = center + Quaternion::fromAxisAngle(camera->getDirection(), Math::PI_DIV2 * 2.f + Math::PI_DIV4).rotateVec3(camera->getRight()) * radius;
			Vector3 v3 = center + Quaternion::fromAxisAngle(camera->getDirection(), Math::PI_DIV2 * 3.f + Math::PI_DIV4).rotateVec3(camera->getRight()) * radius;

			Batch* spriteBatch = getSpriteBatch(texture);
			if (spriteBatch)
			{
				spriteBatch->addIndex((Word)spriteBatch->m_vertexs.size() + 0);
				spriteBatch->addIndex((Word)spriteBatch->m_vertexs.size() + 1);
				spriteBatch->addIndex((Word)spriteBatch->m_vertexs.size() + 2);
				spriteBatch->addIndex((Word)spriteBatch->m_vertexs.size() + 0);
				spriteBatch->addIndex((Word)spriteBatch->m_vertexs.size() + 2);
				spriteBatch->addIndex((Word)spriteBatch->m_vertexs.size() + 3);

				spriteBatch->addVertex(VertexFormat(v0, color, Vector2(1.f, 1.f)));
				spriteBatch->addVertex(VertexFormat(v1, color, Vector2(0.f, 1.f)));
				spriteBatch->addVertex(VertexFormat(v2, color, Vector2(0.f, 0.f)));
				spriteBatch->addVertex(VertexFormat(v3, color, Vector2(1.f, 0.f)));
			}

			m_localAABB.addPoint(v0);
			m_localAABB.addPoint(v1);
			m_localAABB.addPoint(v2);
		}
	}

	Gizmos::Batch* Gizmos::getSpriteBatch(TexturePtr texture)
	{
		if (texture)
		{
			for (Batch* batch : m_spriteBatchs)
			{
				if (batch->m_albedo == texture)
					return batch;
			}

			Material* material = ECHO_CREATE_RES(Material);
			material->setMacro("ENABLE_ALBEDO_TEXTURE", true);
			material->setShaderPath(m_shaderSprite->getPath());

			Batch* batch = EchoNew(Batch(material, this));
			batch->m_mesh->setTopologyType(Mesh::TT_TRIANGLELIST);
			batch->m_albedo = texture;
			m_spriteBatchs.insert(batch);

			return batch;
		}

		return nullptr;
	}

	void Gizmos::clear()
	{
		m_localAABB.reset();
		m_lineBatch->clear();
		m_triangleBatch->clear();

		for (Batch* batch : m_spriteBatchs)
			batch->clear();
	}

	void Gizmos::update_self()
	{
		if (isNeedRender())
		{
			m_lineBatch->update();
			m_triangleBatch->update();
			
			for (Batch* batch : m_spriteBatchs)
				batch->update();
		}
	}
}
