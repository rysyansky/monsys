#pragma once

#include <crow.h>
#include <crow/app.h>
#include "WebsocketManager.hpp"

void RegisterRoutes(crow::SimpleApp& app);

#ifdef ENABLE_DEBUG_ROUTES
void RegisterDebugRoutes(crow::SimpleApp& app);
#endif

void RegisterWebsocketRoutes(crow::SimpleApp& app, WebsocketManager& ws_manager);