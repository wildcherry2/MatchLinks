#include "pch.h"
#include "RemoteMatch.h"
#include "Menu.h"
#include "Settings.h"
#include "Server.h"

BAKKESMOD_PLUGIN(RemoteMatch, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
RemoteMatch* plugin = nullptr;

void RemoteMatch::onLoad() {
    /*while(!::IsDebuggerPresent())
		::Sleep(100);*/
	_globalCvarManager = cvarManager;
	plugin = this;
    Settings::Instance();
    if(auto enabled = cvarManager->getCvar("ml_enabled").getBoolValue()) {
	    Menu::Instance(); 
	    ServerListener::Instance().StartServer();
	}
}

void RemoteMatch::onUnload() {
	if(auto enabled = cvarManager->getCvar("ml_enabled").getBoolValue())
        ServerListener::Instance().StopServer();
}

void RemoteMatch::RenderSettings() {
	Settings::Instance().Render();
}

void RemoteMatch::RenderWindow() {
	if(auto enabled = cvarManager->getCvar("ml_enabled").getBoolValue())
	    Menu::Instance().Render();
}
