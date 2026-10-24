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
#include <Arduino.h>
#include <functional>
#include <queue>
#include <vector>

#include <Wire.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "async.h"
#include "encoder.h"
#include "steptimer.h"
#include "motor.h"

namespace Coffee
{
    struct Main
    {
        Main() = default;

    private:
        static constexpr const char* SSID_NAME = "";
        static constexpr const char* SSID_PASS = "";
        static constexpr const char* SSID_AP_NAME = "ESP32_EXP_AP";
        static constexpr const char* SSID_AP_PASS = "";
        static constexpr const char* MDNS_NAME = "esp32-experiment";

        static constexpr const char* DEVICE_NAME = "ExperimentESP32";
        static constexpr const char* FIRMWARE_VERSION = "Version 0.1";

    private: /* Experiment */
        enum GPIO {
            ENCODER_A = 32,
            ENCODER_B = 33,
            MOTOR_ENABLE = 25,
            MOTOR_LEFT_PWM = 26,
            MOTOR_RIGHT_PWM = 27,
        };

        enum ExperimentState {
            Initializing,
            Running,
            Stopping,
            Waiting,
        };

        uint64_t leftTime = 0;
        uint64_t sampleCount = 0;
        StepTimer stepTimer;
        Encoder encoder;
        Motor motor;

        std::pair<String, double> sampleData[3];
        std::queue<std::pair<uint32_t, double>> profileBack;
        std::queue<std::pair<uint32_t, double>> profileFront;
        ExperimentState experimentState = ExperimentState::Waiting;

    public: /* Experiment */

        void setup() {
            init();

            sampleData[0].first = "t";
            sampleData[0].second = 0.0;
            sampleData[1].first = "u";
            sampleData[1].second = 0.0;
            sampleData[2].first = "y";
            sampleData[2].second = 0.0;

            encoder.begin(ENCODER_A, ENCODER_B);
            motor.setup(MOTOR_ENABLE, MOTOR_LEFT_PWM, MOTOR_RIGHT_PWM, -12.0, 12.0);
            stepTimer.begin(1000); // 1 ms step time
        }

        void loop() {
            /* Dispatch all queued tasks*/
            dispatchAll();

            switch(experimentState)
            {
            case ExperimentState::Initializing:
                // Handle initializing logic
                std::swap(profileFront, profileBack);
                stepTimer.reset();
                sampleCount = 0;
                leftTime = 0;
                motor.start();
                encoder.write(0);
                setState(ExperimentState::Running);
                break;

            case ExperimentState::Running:
                /* Execute step timer logic */
                stepTimer.tick([this](){
                    if(sampleCount > 0) {
                        if(sampleCount > stepTimer.deltaTimeUs())
                            sampleCount = sampleCount - stepTimer.deltaTimeUs();
                        else
                            sampleCount = 0;
                    }
                    else if(!profileFront.empty()) {
                        auto step = profileFront.front();
                        profileFront.pop();
                        sampleCount = step.first * 1000;
                        motor.set(step.second);
                        Serial.println("Second value: " + String(step.second));
                    }
                    else {
                        setState(ExperimentState::Stopping);
                        return;
                    }
                    
                    //Fill sample data


                    postNewSample(sampleData, 3);
                });
                break;

            case ExperimentState::Stopping:
                // Handle stopping logic
                motor.stop();
               setState(ExperimentState::Waiting);
                break;

            case ExperimentState::Waiting:
                // Handle waiting logic
                delay(100);
                break;
            }
        }

    private: /* General */
        void init() {
            Serial.begin(115200);
            if(!SPIFFS.begin()) {
                while(true) { 
                    Serial.println("FATAL ERROR: SPIFFS mount failed...");
                    delay(1000);
                }
            }
            xTaskCreatePinnedToCore(
                [](void *arg) {
                    reinterpret_cast<Main *>(arg)->onAsyncWorker();
                },"Main::onAsyncWorker"
                , ASYNCWORKER_STACK
                , this
                , ASYNCWORKER_PRIORITY
                , &asyncWorker
                , ASYNCWORKER_CORE);

            delay(500);
            yield();
        }

        void startAsync(std::queue<std::pair<uint32_t, double>> profile) {
            postTask([=]() mutable -> void {
                if(this->experimentState == ExperimentState::Waiting) {
                    setState(ExperimentState::Initializing);
                    this->profileBack = std::move(profile);
                }
            });
        }

        void stopAsync() {
            postTask([=]() mutable -> void {
                if(this->experimentState < ExperimentState::Stopping) {
                    setState(ExperimentState::Stopping);
                }
            });
        }

        void setState(ExperimentState value) {
            if(experimentState != value) {
                ExperimentState oldValue = experimentState;
                experimentState = value;
                stateChanged(oldValue, experimentState);
            }
        }

    private: /* Concurrency */
        using method_t = std::function<void()>;
        enum {
            ASYNCWORKER_CORE = 0,
            ASYNCWORKER_PRIORITY = 1,
            ASYNCWORKER_STACK = 30720,
        };
        TaskHandle_t asyncWorker = nullptr;
        Async::Mutex queueMutex;
        std::queue<method_t> taskQueue;

    private: /* Concurrency */
        void onAsyncWorker() {
            Serial.println("Async worker started...");
            delay(1000);
            yield();
            initServer();
            while (true) {
                updateServer();
                delay(10);
                yield();
            }
        }
        void dispatchAll() {
            std::function<void()> method;
            auto locker = queueMutex.ScopeLocker();
            while (taskQueue.size() > 0) {
                method = std::move(taskQueue.front());
                taskQueue.pop();
                queueMutex.Unlock();
                try {
                    method();
                }
                catch (...) {
                }
                queueMutex.Lock();
            }
        }

    public: /* Concurrency */
        void postTask(method_t method) {
            queueMutex.Sync([&]() {
                taskQueue.push(std::move(method));
            });
        }

    private: /*server*/
        enum class ServerState {
            Initializing,
            InitAP,
            InitServer,
            InitMDNS,
            InitSTA,
            WaitSTA,
            Listening,
        };
        
        AsyncWebServer webServer = AsyncWebServer(80);
        AsyncEventSource webEvents = AsyncEventSource("/sample");
        ServerState serverState = ServerState::Initializing;
        StepTimer serverTimer;

        void initServer() {
            webServer.on("/", HTTP_GET, [this](AsyncWebServerRequest* request){ this->onWebServerRoot(request); });
            webServer.on("/status", HTTP_GET, [this](AsyncWebServerRequest* request){ this->onWebServerStatus(request); });
            webServer.onNotFound([this](AsyncWebServerRequest* request){ this->onWebServerRequests(request); });
            webServer.on("/start", HTTP_POST, [this](AsyncWebServerRequest*){ }, nullptr,
                [this](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                this->onWebServerStart(request, data, len, index, total);
            });
            webServer.on("/stop", HTTP_GET, [this](AsyncWebServerRequest* request){ this->onWebServerStop(request); });
            webEvents.onConnect([](AsyncEventSourceClient *client)
            {
                if(client->lastId()) {
                    Serial.printf("Client connected! The last message ID received was: %u\n", client->lastId());
                }
                client->send("Hello!", NULL, millis(), 10000);
            });
            webServer.addHandler(&webEvents);
        }

        void updateServer() {
            switch (serverState) {
            case ServerState::Initializing:
                serverTimer.begin(500000); // 500 ms step time
                serverState = ServerState::InitAP;
                break;
            case ServerState::InitAP:
                WiFi.mode(WIFI_AP_STA);
                WiFi.softAP(SSID_AP_NAME, SSID_AP_PASS);
                Serial.println(String("AP:") + SSID_AP_NAME);
                Serial.println(String("APIP:") + WiFi.softAPIP().toString());
                serverState = ServerState::InitMDNS;
                break;
            case ServerState::InitMDNS:
                if (MDNS.begin(MDNS_NAME)) {
                    Serial.println(String("MDNS:") + MDNS_NAME);
                    serverState = ServerState::InitServer;
                }
                break;
            case ServerState::InitServer:
                serverTimer.tick([&]()
                {
                    webServer.end();
                    webServer.begin();
                    serverState =  ServerState::Listening;
                });
                break;
            case ServerState::InitSTA:
                serverTimer.tick([&]()
                {
                    WiFi.begin(SSID_NAME, SSID_PASS);
                    serverState =  ServerState::WaitSTA;
                });
                break;
            case ServerState::WaitSTA:
                serverTimer.tick([&]()
                {
                    if(WiFi.status() == WL_CONNECTED) {
                        Serial.println(String("Local IP:") + WiFi.localIP().toString());
                        serverState = ServerState::Listening;
                    }
                });
                break;
            case ServerState::Listening:
                if (WiFi.status() != WL_CONNECTED) {
                    serverState = ServerState::InitSTA;   
                }
                break;
            }
        }

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
            //else{
            //    onWebServerNotFound(request);
            //    return;
            //}

            Serial.println(path);
            request->send(SPIFFS, path, dataType);
        }

        void onWebServerStatus(AsyncWebServerRequest* request) {
            String json;
            JsonDocument doc;
            JsonVariant root = doc.to<JsonVariant>();
            if(request->args() == 0) {
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
                root["State"] = (uint32_t)experimentState;
            }
            else if(request->argName(0) == "state") {
                /*TODO: Experiment state*/
                root["State"] = (uint32_t)experimentState;
            }
            else if(request->argName(0) == "firmware") {
                /*TODO: Experiment state*/
                root["Firmware"] = FIRMWARE_VERSION;
            }
            else {
                onWebServerNotFound(request);
            }

            serializeJson(root, json);
            request->send(200, "application/json", json);
        }

        void onWebServerStop(AsyncWebServerRequest* request) {
            stopAsync();
            request->send(200);
        }

        void onWebServerStart(AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
            JsonDocument doc;
            std::queue<std::pair<uint32_t,double>> queue;
            if(request->contentType() == "application/json") {
                if(deserializeJson(doc, (const char*)data, len) != DeserializationError::Ok) {
                    request->send(400);
                    return;
                }
                JsonVariantConst var = doc;
                if(var.is<JsonArrayConst>()) {
                    auto array = var.as<JsonArrayConst>();
                    for(JsonObjectConst item : array) {
                        queue.push({item["time"].as<uint32_t>(), item["value"].as<double>()});
                    }
                    startAsync(queue);
                    request->send(200);
                    return;
                }
            }
            request->send(415);
        }

        void postNewSample(std::pair<String, double> data[], int count) {
            String json;
            JsonDocument doc;
            JsonVariant root = doc.to<JsonVariant>();
            for(int i = 0; i < count; i++){
                root[data[i].first] = data[i].second;
            }
            serializeJson(root, json);
            webEvents.send(json.c_str(), "newdata", millis());
        }

        void stateChanged(ExperimentState oldValue, ExperimentState newValue) {
            String json;
            JsonDocument doc;
            JsonVariant root = doc.to<JsonVariant>();
            root["State"] = newValue;
            root["OldState"] = oldValue;
            serializeJson(root, json);
            webEvents.send(json.c_str(), "statechanged", millis());
            //Serial.println("Event statechanged: " + json);
        }
    };
}