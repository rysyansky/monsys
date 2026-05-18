#include "server/WebsocketManager.hpp"

void WebsocketManager::Add(crow::websocket::connection* conn) {
    if(!conn) {
        CROW_LOG_ERROR <<"Attempted to add null connection to WebsocketManager";
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    clients_.insert(conn);
}

void WebsocketManager::Remove(crow::websocket::connection* conn) {
    if(!conn) {
        CROW_LOG_ERROR <<"Attempted to remove null connection from WebsocketManager";
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    clients_.erase(conn);
}

void WebsocketManager::Broadcast(const nlohmann::json& msg) {
    if(msg.is_null()) {
        CROW_LOG_ERROR <<"Attempted to broadcast null message in WebsocketManager";
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto client : clients_) {
        if(!client) {
            CROW_LOG_WARNING << "Encountered null client connection in WebsocketManager during broadcast";
            continue;
        }
        
        try {
            client->send_text(msg.dump());
        }
        catch (const std::exception& e) {
            CROW_LOG_ERROR <<
                "WebSocket send failed: " <<
                e.what();
        }
    }
}