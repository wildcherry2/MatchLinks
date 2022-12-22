#pragma once

class ServerListener {
    public:
        using Server = SimpleWeb::Server<SimpleWeb::HTTP>;

        static ServerListener& Instance(const uint16_t& new_port = 2525);
        void SetPort(const uint16_t& new_port);
        uint16_t GetPort() { return port; }
        void StartServer();
        void StopServer();
        bool IsListening() { return listening; }

    private:
        ServerListener(const uint16_t& port);
        ~ServerListener();

        bool listening = false;
        uint16_t port = 2525;
        Server server;
        std::thread server_thread;
        CustomMatchTeamSettings blue, orange;
};