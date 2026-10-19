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
#include "coffee.h"

#include <Wire.h>
#include <SPI.h>
#include <SPIFFS.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "async.h"
#include "steptimer.h"

namespace Coffee
{
    template<typename Subclass>
    struct WebServer
    {
        using self = Subclass;

    private: /*TODO: Update settings via json file*/
        static constexpr const char* SSID_NAME = "";
        static constexpr const char* SSID_PASS = "";
        static constexpr const char* SSID_AP_NAME = "ESP32_EXP_AP";
        static constexpr const char* SSID_AP_PASS = "";
        static constexpr const char* MDNS_NAME = "esp32-experiment";

        static constexpr const char* DEVICE_NAME = "ExperimentESP32";
        static constexpr const char* FIRMWARE_VERSION = "Version 0.1";

    public:
        WebServer() = default;

    private:
        ServerState m_serverState = ServerState::Stopped;
        AsyncWebServer m_webServer = AsyncWebServer(80);
        AsyncEventSource m_webEvents = AsyncEventSource("/event");
        StepTimer m_serverTimer;

    public:
        void setup() {  
            if(!SPIFFS.begin()) {
                static_cast<self*>(this)->println("FATAL ERROR: SPIFFS mount failed...");
                static_cast<Subclass*>(this)->setSystemState(SystemState::Error);
                delay(500);
            }

            m_webServer.on("/", HTTP_GET, [this](AsyncWebServerRequest* request){ this->onWebServerRoot(request); });
            m_webServer.on("/status", HTTP_GET, [this](AsyncWebServerRequest* request){ this->onWebServerAbout(request); });
            m_webServer.onNotFound([this](AsyncWebServerRequest* request){ this->onWebServerRequests(request); });
            m_webEvents.onConnect([this](AsyncEventSourceClient *client) {
                if(client->lastId()) {
                    static_cast<self*>(this)->println("Client connected! The last message ID received was: ", client->lastId());
                }
                client->send("Hello!", NULL, millis(), 10000);
            });

            static_cast<self*>(this)->serverSetup(m_webServer, m_webEvents);

            m_webServer.addHandler(&m_webEvents);          
            setServerState(ServerState::Stopped);
        }

        void loop() {
            switch (m_serverState) {
            case ServerState::Initializing:
                m_serverTimer.begin(500000); // 500 ms step time
                setServerState(ServerState::InitAP);
                break;
            case ServerState::InitAP:
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP(SSID_AP_NAME, SSID_AP_PASS);

                static_cast<self*>(this)->println(String("AP:") + SSID_AP_NAME);
                static_cast<self*>(this)->println(String("APIP:") + WiFi.softAPIP().toString());
                setServerState(ServerState::InitMDNS);
                break;
            case ServerState::InitMDNS:
                if (MDNS.begin(MDNS_NAME)) {
                    static_cast<self*>(this)->println(String("MDNS:") + MDNS_NAME);
                    setServerState(ServerState::InitServer);
                }
                break;
            case ServerState::InitServer:
                m_serverTimer.tick([&]()
                {
                    m_webServer.end();
                    m_webServer.begin();
                    setServerState(ServerState::Listening);
                });
                break;
            case ServerState::InitSTA:
                m_serverTimer.tick([&]()
                {
                    WiFi.begin(SSID_NAME, SSID_PASS);
                    setServerState(ServerState::WaitSTA);
                });
                break;
            case ServerState::WaitSTA:
                m_serverTimer.tick([&]()
                {
                    if(WiFi.status() == WL_CONNECTED) {
                        static_cast<self*>(this)->println(String("Local IP:") + WiFi.localIP().toString());
                        setServerState(ServerState::Listening);
                    }
                });
                break;
            case ServerState::Listening:
                if (WiFi.status() != WL_CONNECTED) {
                    setServerState(ServerState::InitSTA);
                }
                break;

            case ServerState::Stopped:
                //m_serverTimer.tick([&]()
                //{ 
                    m_webServer.end();
                //});
                break;
            }
        }

        ServerState getServerState() const {
            return m_serverState;
        }

        void setServerState(ServerState value) {
            if(m_serverState != value) {
                ServerState oldValue = m_serverState;
                m_serverState = value;
                // Optionally, handle state change here
            }
        }

        void webEventSend(const char *message, const char *event=NULL, uint32_t id=0, uint32_t reconnect=0) {
            m_webEvents.send(message, event, id, reconnect);
        }
    protected:
        void onWebServerNotFound(AsyncWebServerRequest* request) {
            request->send(404, "text/plain", "Not Found");
        }

        void onWebServerRoot(AsyncWebServerRequest* request) {
            request->redirect("/index.html"); // Redirect to our html web page
            //request->send(302, "text/plain", "");
        }

        void onWebServerRequests(AsyncWebServerRequest* request) {
            String dataType = "text/plain";
            String path = request->url();
            if (path.endsWith("/"))
                path += "index.html";

            if (path.endsWith(".src"))
                path = path.substring(0, path.lastIndexOf("."));
            else if (path.endsWith(".html"))
                dataType = "text/html";
            else if (path.endsWith(".htm"))
                dataType = "text/html";
            else if (path.endsWith(".css"))
                dataType = "text/css";
            else if (path.endsWith(".json"))
                dataType = "application/json";
            else if (path.endsWith(".txt"))
                dataType = "text/plain";
            else if (path.endsWith(".js"))
                dataType = "application/javascript";
            else if (path.endsWith(".png"))
                dataType = "image/png";
            else if (path.endsWith(".gif"))
                dataType = "image/gif";
            else if (path.endsWith(".jpg"))
                dataType = "image/jpeg";
            else if (path.endsWith(".ico"))
                dataType = "image/x-icon";
            else if (path.endsWith(".xml"))
                dataType = "text/xml";
            else if (path.endsWith(".pdf"))
                dataType = "application/pdf";
            else if (path.endsWith(".zip"))
                dataType = "application/zip";
   
            static_cast<self*>(this)->println(path);
            request->send(SPIFFS, path, dataType);
        }

        void onWebServerAbout(AsyncWebServerRequest* request) {
            String json;
            JsonDocument doc;
            JsonVariant root = doc.to<JsonVariant>();
           
            /*TODO: All info*/
            root["DeviceName"] = DEVICE_NAME;
            byte mac[6];
            WiFi.macAddress(mac);
            root["MacAddress"] = String(mac[5], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[0], HEX);
            root["Firmware"] = FIRMWARE_VERSION;
            root["SSID"] = WiFi.SSID();
            IPAddress ip = WiFi.localIP();
            IPAddress mask = WiFi.subnetMask();
            root["IPAddress"] = String() + ip[0] + "." + ip[1] + "." + ip[2] + "." + ip[3];
            root["SubnetMask"] = String() + mask[0] + "." + mask[1] + "." + mask[2] + "." + mask[3];
 
            serializeJson(root, json);
            request->send(200, "application/json", json);
        }

    public:
        
    };
}