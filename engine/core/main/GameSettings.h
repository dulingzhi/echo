#pragma once

#include "engine/core/base/variant.h"
#include "engine/core/Util/StringUtil.h"
#include <thirdparty/pugixml/pugixml.hpp>
#include "engine/core/resource/Res.h"

namespace Echo
{
	/**
	 * ��Ŀ�ļ�
	 */
	class GameSettings : public Object
	{
		ECHO_SINGLETON_CLASS(GameSettings, Object);

	public:
		GameSettings();
		~GameSettings();

		// instance
		static GameSettings* instance();

		// set window width
		void setWindowWidth(i32 width) { m_windowWidth = width; }

		// get winodw width
		i32 getWindowWidth() const { return m_windowWidth; }

		// set window height
		void setWindowHeight(i32 height) { m_windowHeight = height; }

		// get window height
		i32 getWindowHeight() const { return m_windowHeight; }

		// set launch scene
		void setLaunchScene(const ResourcePath& path) { m_launchScene.setPath(path.getPath()); }

		// get launch scene
		const ResourcePath& getLaunchScene() const { return m_launchScene; }

	private:
		i32				m_windowWidth;
		i32				m_windowHeight;
		ResourcePath	m_launchScene;
	};
}