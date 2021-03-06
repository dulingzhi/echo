#pragma once

#include "build_settings.h"

namespace Echo
{
    class AndroidBuildSettings : public BuildSettings
    {
        ECHO_SINGLETON_CLASS(AndroidBuildSettings, BuildSettings)

    public:
		struct AppIconItem
		{
			i32     m_size;
			String  m_folder;
		};
        
    public:
        AndroidBuildSettings();
        virtual ~AndroidBuildSettings();
        
        // instance
        static AndroidBuildSettings* instance();

		// get name
		virtual const char* getPlatformName() const override { return "Android"; }

		// platform thumbnail
        virtual ImagePtr getPlatformThumbnail() const override;

        // set output directory
        virtual void setOutputDir(const String& outputDir) override;

        // build
        virtual void build() override;

		// app name
		void setAppName(const String& appName) { m_appName = appName; }
		String getAppName() const;

		// get final result path
		virtual String getFinalResultPath() override;
        
        // icon res path
        void setIconBackgroundRes(const ResourcePath& path);
        const ResourcePath& getIconBackgroundRes() { return m_iconBackground; }

		void setIconForegroundRes(const ResourcePath& path);
		const ResourcePath& getIconForegroundRes() { return m_iconForeground; }

        // Fullscreen
        bool isFullscreen() const { return m_isFullScreen; }
        void setFullscreen(bool isFullscreen) { m_isFullScreen = isFullscreen; }

    private:
        // output directory
        bool prepare();

        // copy
        void copySrc();
        void copyRes();

		// replace
		void replaceIcon();

    private:
        // write settings
        void writeStringsXml();
        void writeStylesXml();

        // write config
        void writeModuleConfig();

    private:
		// utils function
		bool rescaleIcon(const char* iFilePath, const char* oFilePath, ui32 targetWidth, ui32 targetHeight);
        
    private:
        String                  m_rootDir;
        String                  m_projectDir;
        String                  m_outputDir;
        String                  m_appName;
        ResourcePath            m_iconBackground = ResourcePath("Res://icon.png", ".png");
        ResourcePath            m_iconForeground = ResourcePath("Res://icon.png", ".png");
        bool                    m_isFullScreen = true;
    };
}
