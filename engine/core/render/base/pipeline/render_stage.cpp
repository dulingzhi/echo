#include "render_stage.h"
#include "render_pipeline.h"
#include "render_queue.h"
#include "image_filter.h"
#include "engine/core/main/Engine.h"
#include <thirdparty/pugixml/pugixml.hpp>

namespace Echo
{
	RenderStage::RenderStage(RenderPipeline* pipeline)
		: m_pipeline(pipeline)
	{
	}

	RenderStage::~RenderStage()
	{
        EchoSafeDeleteContainer(m_renderQueues, IRenderQueue);
	}

	void RenderStage::bindMethods()
	{
		CLASS_BIND_METHOD(RenderStage, getName, DEF_METHOD("getName"));
		CLASS_BIND_METHOD(RenderStage, setName, DEF_METHOD("setName"));

		CLASS_REGISTER_PROPERTY(RenderStage, "Name", Variant::Type::String, "getName", "setName");
	}

	ImageFilter* RenderStage::addImageFilter(const String& name)
	{
		ImageFilter* queue = EchoNew(ImageFilter(this));
		queue->setName(name);
		m_renderQueues.emplace_back(queue);

		return queue;
	}

	void RenderStage::parseXml(void* pugiNode)
	{
		pugi::xml_node* stageNode = (pugi::xml_node*)pugiNode;
		if (stageNode)
		{
			// queues
			for (pugi::xml_node queueNode = stageNode->child("queue"); queueNode; queueNode = queueNode.next_sibling("queue"))
			{
				String type = queueNode.attribute("type").as_string();
				if (type == "queue")
				{
					RenderQueue* queue = EchoNew(RenderQueue( this));
					queue->setName(queueNode.attribute("name").as_string("Opaque"));
					queue->setSort(queueNode.attribute("sort").as_bool(false));
					m_renderQueues.emplace_back(queue);
				}
				else if (type == "filter")
				{
					addImageFilter(queueNode.attribute("name").as_string());
				}
			}

			// frame buffer
			pugi::xml_node framebufferNode = stageNode->child("framebuffer");
			if (framebufferNode)
			{
				m_frameBufferId = framebufferNode.attribute("id").as_uint();
			}
		}
	}

	void RenderStage::addRenderable(const String& name, RenderableID id)
	{
		for (IRenderQueue* iqueue : m_renderQueues)
		{
			RenderQueue* queue = dynamic_cast<RenderQueue*>(iqueue);
			if (queue)
			{
				if (queue->getName() == name)
					queue->addRenderable(id);
			}
		}
	}

	void RenderStage::render()
	{
		if (m_frameBufferId != -1)
		{
			RenderPipeline::current()->beginFramebuffer(m_frameBufferId);

			for (IRenderQueue* iqueue : m_renderQueues)
			{
				iqueue->render();
			}

			RenderPipeline::current()->endFramebuffer(m_frameBufferId);
		}
	}
}
