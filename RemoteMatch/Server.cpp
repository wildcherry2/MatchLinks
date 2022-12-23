#include "pch.h"
#include "Server.h"
#include "Settings.h"
#include "RemoteMatch.h"

ServerListener& ServerListener::Instance(const uint16_t& new_port) {
    static ServerListener server_listener(new_port);
    //if(server_listener.listening) server_listener.SetPort(new_port);
    return server_listener;
}

void ServerListener::SetPort(const uint16_t& new_port) {
    port = new_port;
    if(listening) {
        StopServer();
        StartServer();
    }
}

#define CHECKFIELD if(current_field == fields.end()) {\
    LOG("Bad URL received!");\
    response->close_connection_after_response = true;\
    response->write(SimpleWeb::StatusCode::server_error_internal_server_error, "Bad URL syntax!");\
    response->send();\
    return;\
}\

void ServerListener::StartServer() {
    if(listening) {
        LOG("Tried to start internal server, but server is already started.");
        return;
    }

    try {
        server_thread.join();
    }
    catch(...) {}

    server_thread = std::thread([this] {
        try {
            LOG("Starting server...");
            listening = true;
            server.config.port = port;
            server.on_error = [this](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request, const SimpleWeb::error_code& ec) {
                LOG("Internal server reported code {}: {}", ec.value(), ec.what());
            };

            server.resource["^/match$"]["GET"] = [this](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response,
                                                        std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
                if(plugin->gameWrapper && !(plugin->gameWrapper->IsInOnlineGame() || plugin->gameWrapper->IsInGame())) {
                    plugin->gameWrapper->Execute([this, response, request](GameWrapper* gw) {
                        auto fields = request->parse_query_string();
                        auto current_field = fields.begin();

                        CHECKFIELD
                        const bool join = current_field->second == "join";
                        ++current_field;

                        CHECKFIELD
                        const std::string name = current_field->second;
                        ++current_field;

                        CHECKFIELD
                        const std::string password = current_field->second;
                        ++current_field;

                        std::string region;
                        if(!join) {
                            CHECKFIELD
                            region = current_field->second;
                        }
                    
                        auto mmwrapper = plugin->gameWrapper->GetMatchmakingWrapper();
                        if(!mmwrapper.IsNull()) {
                            if(name.empty()) {
                                LOG("Link with empty name field received, but a name is required to create a private match!");
                            }
                            else if(!join) {
                                CustomMatchSettings cms;
                                cms.GameTags = "BotsNone";
                                cms.GameMode = 0;
                                cms.ServerName = name;
                                cms.Password = password;
                                cms.bClubServer = false;
                                cms.bPartyMembersOnly = false;
                                cms.MapName = Settings::Instance().GetSettingsData().map;
                                LOG("Creating lobby with name = {}, password = {}, region = {}, map = {}", cms.ServerName, cms.Password, Settings::Instance().GetSettingsData().region, Settings::Instance().GetSettingsData().map);
                                mmwrapper.CreatePrivateMatch(static_cast<Region>(std::stoi(region)), cms);

                            }
                            else {
                                LOG("Joining lobby with name = {} and password = {}", name, password);
                                mmwrapper.JoinPrivateMatch(name, password);
                            }
                        }
                        else LOG("Tried to {} match with null MatchmakingWrapper!", join ? "join" : "create");

                        response->close_connection_after_response = true;
                        response->write(SimpleWeb::StatusCode::success_accepted, "alright");
                        response->send();
                    });
                }

                else LOG("Tried to join/create match by URL with null GameWrapper or while in game!");
            };

            server.start();
            listening = false;
        } 
        catch(const std::exception& ex) {
            LOG("Caught exception in server thread: {}", ex.what());
        }
    });
}

#undef CHECKFIELD

void ServerListener::StopServer() {
    if(!listening) {
        LOG("Tried to stop internal server, but it's not running!");
        return;
    }

    LOG("Stopping internal server...");
    
    try {
        server.stop(); 
        server.io_service->stop();
        server_thread.join();
        listening = false; // insurance
    }
    catch(...) {}
}

ServerListener::ServerListener(const uint16_t& port) : port(port){}
ServerListener::~ServerListener() {
    StopServer();
}
