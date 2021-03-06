#pragma once

#include "engine/core/util/Array.hpp"
#include "engine/core/scene/node.h"
#include "engine/core/resource/Res.h"
#include "texture.h"
#include "texture_render.h"

namespace Echo
{
    class FrameBuffer : public Res
    {
        ECHO_CLASS(FrameBuffer, Res)

    public:
		// begin|end render
		virtual bool begin(const Color& bgColor, float depthValue, bool isClearStencil, ui8 stencilValue) { return false; }
		virtual bool end() { return false; }

		// on resize
		virtual void onSize(ui32 width, ui32 height) {}

	public:
		// clear color
		bool isClearColor() const { return m_isClearColor; }
		void setClearColor(bool isClearColor) { m_isClearColor = isClearColor; }

		// clear depth
		bool isClearDepth() const { return m_isClearDepth; }
		void setClearDepth(bool isClearDepth) { m_isClearDepth = isClearDepth; }

    protected:
		bool	m_isClearColor = true;
		bool	m_isClearDepth = true;
    };
    typedef ResRef<FrameBuffer> FrameBufferPtr;

	class FrameBufferOffScreen : public FrameBuffer
	{
        ECHO_RES(FrameBufferOffScreen, FrameBuffer, ".fbos", FrameBufferOffScreen::create, Res::load)

	public:
        // Type
        enum Attachment : ui8
        {
            Color0 = 0,
            Color1,
            Color2,
            Color3,
            Color4,
            Color5,
            Color6,
            Color7,
            DepthStencil
        };

	public:
        FrameBufferOffScreen();
        FrameBufferOffScreen(ui32 width, ui32 height);
		virtual ~FrameBufferOffScreen();

		// create fun
		static Res* create();

    public:
        // attachment color0
        ResourcePath getColor0();
        void setColor0(const ResourcePath& path);

        // attachment depth
        ResourcePath getDepth();
        void setDepth(const ResourcePath& path);

        // has depth attachment | color
        bool hasColorAttachment() { return m_views[int(Attachment::Color0)]; }
        bool hasDepthAttachment() { return m_views[int(Attachment::DepthStencil)]; }

	protected:
        array<TextureRenderPtr, 9>  m_views;
	};


    class FrameBufferWindow : public FrameBuffer
    {
        ECHO_RES(FrameBufferWindow, FrameBuffer, ".fbws", FrameBufferWindow::create, Res::load)

    public:
        FrameBufferWindow() {}
        virtual ~FrameBufferWindow() {}

		// create fun
		static Res* create();
    };
}
