/* 
 * Copyright (c) 2023 Jesus Angel Rocha Morales.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "pid.h"

#include <functional>
#include <queue>
#include <vector>
#include "coffee/collections/arrayview.h"

#include "webserver.h"
#include "async.h"

namespace Coffee
{
    template<typename Subclass, DebugLevel level = DebugLevel::Debug>
    struct Entry
        : SerieLogger<Subclass, level>
        , WebServer<Subclass>
        , AsyncWorker<Subclass>
    {
        using self = Subclass;
        using entry = Entry<self, level>;
        using logger = SerieLogger<self, level>;
        using webServer = WebServer<self>;
        using asyncWorker = AsyncWorker<self>;

        Entry() = default;

    private:
        SystemState m_systemState = SystemState::Loading;
        ControllerState m_controllerState = ControllerState::Waiting;
        Async::Dispatcher m_mainDispatcher;
    
    public:
        void init(){
            logger::setup();
            asyncWorker::setup();
            webServer::setup();
            static_cast<self*>(this)->setup();
        }

        void run() {
           m_mainDispatcher.update();
            switch (m_systemState)
            {
            case SystemState::Error:
                asyncWorker::stopTaskAsync();
                logger::println("System encountered an error.");
                delay(1000);
                break;

            case SystemState::Loading: /* Loading settings*/
                asyncWorker::reloadTaskAsync();
                setControllerState(ControllerState::Loading);
                setSystemState(SystemState::Running);
                break;
  
            case SystemState::Running:
                static_cast<self*>(this)->loop();
                break;
            default:
                break;
            }
        }

    public: /* Experiment */
        void reloadAsync() {
            postTask([=]() mutable -> void {
                if(this->m_systemState != SystemState::Loading) {
                    setSystemState(SystemState::Loading);
                }
            });
        }

        void startControllerAsync() {
            postTask([=]() mutable -> void {
                if(this->m_controllerState == ControllerState::Waiting) {
                    setControllerState(ControllerState::Loading);
                }
            });
        }

        void stopControllerAsync() {
            postTask([=]() mutable -> void {
                if(this->m_controllerState < ControllerState::Stopping) {
                    setControllerState(ControllerState::Stopping);
                }
            });
        }

        SystemState getSystemState() const {
            return m_systemState;
        }

        void setSystemState(SystemState value) {
            if(m_systemState != value) {
                SystemState oldValue = m_systemState;
                m_systemState = value;
                //stateChanged(oldValue, m_systemState);
            }
        }

        ControllerState getControllerState() const {
            return m_controllerState;
        }

        void setControllerState(ControllerState value) {
            if(m_controllerState != value) {
                ControllerState oldValue = m_controllerState;
                m_controllerState = value;
                stateChanged(oldValue, m_controllerState);
            }
        }

        void postTask(Async::Dispatcher::method_t method) {
            m_mainDispatcher.postTask(std::move(method));
        }

    public: /* Concurrency */
        void asyncTaskLoad() {
            WebServer<self>::setServerState(ServerState::Initializing);
        }

        void asyncTaskRun() {
            WebServer<self>::loop();
        }

        void asyncTaskError() {
            WebServer<self>::setServerState(ServerState::Stopped);
        }

        void serverSetup(AsyncWebServer& webServer, AsyncEventSource& webEvents) {
            webServer.on("/start", HTTP_GET, [this](AsyncWebServerRequest* request){ static_cast<self*>(this)->onWebServerStart(request); });
            webServer.on("/stop", HTTP_GET, [this](AsyncWebServerRequest* request){ static_cast<self*>(this)->onWebServerStop(request); });
        }

        template<typename Container>
        void postNewSample(Enumerator<std::pair<String, double>, Container>const& data) {
            String json;
            JsonDocument doc;
            JsonVariant root = doc.to<JsonVariant>();
            for(auto const& pair : data) {
                root[pair.first] = pair.second;
            }
            serializeJson(root, json);
            webServer::webEventSend(json.c_str(), "sample", millis());
        }

        void stateChanged(ControllerState oldValue, ControllerState newValue) {
            String json;
            JsonDocument doc;
            JsonVariant root = doc.to<JsonVariant>();
            root["State"] = coffee::underlying<ControllerState>(newValue);
            root["OldState"] = coffee::underlying<ControllerState>(oldValue);
            serializeJson(root, json);
            webServer::webEventSend(json.c_str(), "statechanged", millis());
        }

    protected: /*server*/
        void onWebServerStart(AsyncWebServerRequest* request) {
            startControllerAsync();
            request->send(200);
        }

        void onWebServerStop(AsyncWebServerRequest* request) {
            stopControllerAsync();
            request->send(200);
        }
        
    };
}