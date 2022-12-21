#pragma once
#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class RemoteMatch: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase, public PluginWindowBase {
	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override; // Uncomment and implement if you need a unload method

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	void RenderWindow() override; // Uncomment if you want to render your own plugin window
};

extern RemoteMatch* plugin;
