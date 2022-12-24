#include "pch.h"
#include "MatchLinks.h"
#include "Menu.h"
#include "Settings.h"
#include "Server.h"

BAKKESMOD_PLUGIN(MatchLinks, "Match Links", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
MatchLinks* plugin = nullptr;

void MatchLinks::onLoad() {
	_globalCvarManager = cvarManager;
	plugin = this;
    Settings::Instance();
    if(auto enabled = cvarManager->getCvar("ml_enabled").getBoolValue()) {
	    Menu::Instance(); 
	    ServerListener::Instance().StartServer();
	}
}

void MatchLinks::onUnload() {
	if(auto enabled = cvarManager->getCvar("ml_enabled").getBoolValue())
        ServerListener::Instance().StopServer();
}

void MatchLinks::RenderSettings() {
	Settings::Instance().Render();
}

void MatchLinks::RenderWindow() {
	if(auto enabled = cvarManager->getCvar("ml_enabled").getBoolValue())
	    Menu::Instance().Render();
}
