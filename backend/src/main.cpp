#include "server/WebsocketManager.hpp"
#include "server/routes.hpp"
#include <crow.h>
#include <crow/app.h>
#include <nlohmann/json.hpp>
#include <cxxopts.hpp>
#include "StatCollector.hpp"

using json = nlohmann::json;

std::string read_file(const std::string& path) {

    std::ifstream file(path);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void broadcast_loop(WebsocketManager& ws_manager) {
    StatCollector collector;
    while (true) {
        collector.Snapshot();
        nlohmann::json data = collector.GetSystemData();
        ws_manager.Broadcast(data);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    cxxopts::Options options("app", "HTTP Server");

    options.add_options()
        ("p,port", "Port",
         cxxopts::value<int>()->default_value("8080"));
    options.add_options()
        ("h,help", "Print help");
    options.add_options()
        ("d,debug", "Enable debug output",
         cxxopts::value<bool>()->default_value("false")->implicit_value("true"));

    auto result = options.parse(argc, argv);

    if(result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    int port = result["port"].as<int>();
    if(result["debug"].as<bool>()) {
        crow::logger::setLogLevel(crow::LogLevel::Debug);
    }
    else {
        crow::logger::setLogLevel(crow::LogLevel::Info);
    }

    CROW_LOG_INFO << "Starting server on port " << port;

    crow::SimpleApp app;

    WebsocketManager ws_manager;
    
    RegisterRoutes(app);

    #ifdef ENABLE_DEBUG_ROUTES
    RegisterDebugRoutes(app);
    #endif

    RegisterWebsocketRoutes(app, ws_manager);

    std::thread broadcast_thread(broadcast_loop, std::ref(ws_manager));
    broadcast_thread.detach();

    app.port(port).multithreaded().run();
}