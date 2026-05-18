#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <mutex>

class WebsocketManager {
public:
    void Add(crow::websocket::connection* conn);
    void Remove(crow::websocket::connection* conn);

    void Broadcast(const nlohmann::json& msg);

private:
    std::unordered_set<crow::websocket::connection*> clients_;
    std::mutex mutex_;
};