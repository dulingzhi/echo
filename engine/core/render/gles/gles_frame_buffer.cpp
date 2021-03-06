#include "engine/core/util/PathUtil.h"
#include "base/Renderer.h"
#include "gles_render_base.h"
#include "gles_mapping.h"
#include "gles_frame_buffer.h"
#include "gles_texture_2d.h"
#include "gles_texture_render.h"

namespace Echo
{
    GLESFrameBufferOffScreen::GLESFrameBufferOffScreen(ui32 width, ui32 height)
		: FrameBufferOffScreen(width, height)
        , m_fbo(0)
	{
        OGLESDebug(glGenFramebuffers(1, &m_fbo));
		m_esTextures.assign(0);
	}

    GLESFrameBufferOffScreen::~GLESFrameBufferOffScreen()
	{
		OGLESDebug(glDeleteFramebuffers(1, &m_fbo));
	}

	bool GLESFrameBufferOffScreen::bind(i32& width, i32& height)
	{
		if (!hasColorAttachment()) return false;
		if (!hasDepthAttachment()) return false;

		width = m_views[i32(Attachment::DepthStencil)]->getWidth();
		height = m_views[i32(Attachment::DepthStencil)]->getHeight();
		if (!width || !height) return false;

		for (i32 i = i32(Attachment::Color0); i <= i32(Attachment::DepthStencil); i++)
		{
			if (m_views[i])
			{
				if (width != m_views[i]->getWidth()) return false;
				if (height != m_views[i]->getHeight()) return false;
			}
		}

		OGLESDebug(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
		attach();

#ifdef ECHO_EDITOR_MODE
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		return status == GL_FRAMEBUFFER_COMPLETE ? true : false;
#else
		return true;
#endif
	}

    void GLESFrameBufferOffScreen::attach()
    {
		for (i32 i = i32(Attachment::Color0); i <= i32(Attachment::DepthStencil); i++)
		{
			GLESTextureRender* texture = dynamic_cast<GLESTextureRender*>(m_views[i].ptr());
			GLuint esTexture = texture ? texture->getGlesTexture() : 0;
			if (esTexture != m_esTextures[i])
			{
				if (i != Attachment::DepthStencil)
				{
					OGLESDebug(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, esTexture, 0));
				}
				else
				{
					OGLESDebug(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, texture->getGlesTexture()));
				}

				m_esTextures[i] = esTexture;
			}
		}
    }

	bool GLESFrameBufferOffScreen::begin(const Color& bgColor, float depthValue, bool isClearStencil, ui8 stencilValue)
	{
		i32 width = 0;
		i32 height = 0;
		if (bind(width, height))
		{
			OGLESDebug(glViewport(0, 0, width, height));

			clear(m_isClearColor, bgColor, m_isClearDepth, depthValue, isClearStencil, stencilValue);

			return true;
		}

		return false;
	}

	bool GLESFrameBufferOffScreen::end()
	{
		return true;
	}

	void GLESFrameBufferOffScreen::clear(bool clearColor, const Color& color, bool clearDepth, float depth_value, bool clear_stencil, ui8 stencil_value)
	{
		GLbitfield mask = 0;
		if (clearColor)
		{
			OGLESDebug(glClearColor(color.r, color.g, color.b, color.a));
			mask |= GL_COLOR_BUFFER_BIT;
		}

		if (clearDepth)
		{
			OGLESDebug(glClearDepthf(depth_value));
			mask |= GL_DEPTH_BUFFER_BIT;
		}

		OGLESDebug(glDepthMask(clearColor));

		if (clear_stencil)
		{
			OGLESDebug(glClearStencil(stencil_value));
			mask |= GL_STENCIL_BUFFER_BIT;
		}

		OGLESDebug(glStencilMask(clear_stencil));

		if (mask != 0)
		{
			OGLESDebug(glClear(mask));
		}
	}

	void GLESFrameBufferOffScreen::onSize( ui32 width, ui32 height )
	{
        for (TextureRender* colorView : m_views)
        {
            if (colorView)
                colorView->onSize(width, height);
        }
	}

	GLESFramebufferWindow::GLESFramebufferWindow()
		: FrameBufferWindow()
	{
	}

	GLESFramebufferWindow::~GLESFramebufferWindow()
	{
	}

	bool GLESFramebufferWindow::begin(const Color& backgroundColor, float depthValue, bool clearStencil, ui8 stencilValue)
	{
		// bind frame buffer
		OGLESDebug(glBindFramebuffer(GL_FRAMEBUFFER, 0));

		ui32 width = Renderer::instance()->getWindowWidth();
		ui32 height = Renderer::instance()->getWindowHeight();
		OGLESDebug(glViewport(0, 0, width, height));

		// clear
		GLESFrameBufferOffScreen::clear(m_isClearColor, backgroundColor, m_isClearDepth, depthValue, clearStencil, stencilValue);

		return true;
	}

	bool GLESFramebufferWindow::end()
	{
		return true;
	}

	void GLESFramebufferWindow::onSize(ui32 width, ui32 height)
	{
	}
}
