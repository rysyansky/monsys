#include "server/routes.hpp"
#include <crow/app.h>
#include <crow/logging.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

static std::string GetExecutableDir() {
    char buf[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len == -1) return {};
    buf[len] = '\0';
    return std::filesystem::path(buf).parent_path().string();
}

static std::string GuessMimeType(const std::string& path) {
    if (path.ends_with(".js")) {
        return "application/javascript";
    }
    if (path.ends_with(".css")) {
        return "text/css";
    }
    if (path.ends_with(".html")) {
        return "text/html; charset=utf-8";
    }
    return "application/octet-stream";
}

void RegisterRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/")
    ([](){
        std::ifstream file(GetExecutableDir() + "/static/index.html");
        if(!file) {
            CROW_LOG_ERROR << "Failed to open index.html";
            return std::string("<h1>500 Internal Server Error</h1><p>Could not load page</p>");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::string html = buffer.str();
        return html;
    });

    CROW_ROUTE(app, "/assets/<path>")
    ([](const std::string& asset_path){
        if (asset_path.find("..") != std::string::npos) {
            CROW_LOG_WARNING << "Rejected suspicious asset path: " << asset_path;
            return crow::response(400, "Bad Request");
        }

        const auto full_path =
            GetExecutableDir() + "/static/assets/" + asset_path;

        std::ifstream file(full_path, std::ios::binary);
        if (!file) {
            CROW_LOG_WARNING << "Asset not found: " << full_path;
            return crow::response(404);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        crow::response resp(buffer.str());
        resp.add_header("Content-Type", GuessMimeType(asset_path));
        return resp;
    });
}

#ifdef ENABLE_DEBUG_ROUTES
void RegisterDebugRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/debug")
    ([](){
        CROW_LOG_INFO << "Received request to /debug";
        return std::string("<a href=\"/debug/testmsg\">Test Message</a><br><a href=\"/debug/ws\">WebSocket Debug</a>");
    });
    CROW_ROUTE(app, "/debug/testmsg")
    ([](){
        CROW_LOG_INFO << "Received request to /debug/testmsg";
        return crow::response("Debug message received");
    });

    CROW_ROUTE(app, "/debug/ws")
    ([](){

        return R"(
    <!DOCTYPE html>
    <html>
    <head>
        <title>WS Debug</title>

        <style>
            body {
                font-family: monospace;
                background: #111;
                color: #eee;
                padding: 20px;
            }

            #messages {
                display: flex;
                flex-direction: column;
                gap: 12px;
            }

            .msg {
                background: #1e1e1e;
                border: 1px solid #333;
                border-radius: 8px;
                padding: 12px;
                white-space: pre-wrap;
                overflow-x: auto;
            }

            .time {
                color: #888;
                margin-bottom: 8px;
            }
        </style>
    </head>
    <body>

    <h1>WebSocket Debug</h1>

    <div id="status">Connecting...</div>

    <hr>

    <div id="messages"></div>

    <script>

    const messages = document.getElementById("messages");
    const status = document.getElementById("status");

    const socket = new WebSocket(
        "ws://" + window.location.host + "/ws"
    );

    socket.onopen = () => {

        status.textContent = "Connected";

        socket.send(JSON.stringify({
            type: "hello"
        }));
    };

    socket.onmessage = (event) => {

        const wrapper = document.createElement("div");
        wrapper.className = "msg";

        const time = document.createElement("div");
        time.className = "time";
        time.textContent = new Date().toLocaleTimeString();

        const content = document.createElement("div");

        try {

            const parsed = JSON.parse(event.data);

            content.textContent = JSON.stringify(
                parsed,
                null,
                2
            );

        } catch {

            content.textContent = event.data;
        }

        wrapper.appendChild(time);
        wrapper.appendChild(content);

        messages.prepend(wrapper);
    };

    socket.onclose = () => {
        status.textContent = "Disconnected";
    };

    socket.onerror = (err) => {
        console.error(err);
    };

    </script>

    </body>
    </html>
    )";
    });
}
#endif

void RegisterWebsocketRoutes(crow::SimpleApp& app, WebsocketManager& ws_manager) {
    CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([&ws_manager](crow::websocket::connection& conn){
        CROW_LOG_INFO << "Client connected";
        ws_manager.Add(&conn);
    })
    .onclose([&ws_manager](crow::websocket::connection& conn, const std::string& reason, uint16_t close_code){
        ws_manager.Remove(&conn);

        CROW_LOG_INFO << "Client disconnected. reason='" << reason << "' code=" << close_code;
    })
    .onmessage([](crow::websocket::connection& conn,
                  const std::string& data,
                  bool) {

        CROW_LOG_INFO << "Received: " << data;
    });
}