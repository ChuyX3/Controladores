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
#include "entry.h"
#include "coffee/control/encoder.h"
#include "motor.h"

namespace Coffee
{
    struct Main : Entry<Main>
    {
        using super = Entry<Main>;
    private:
        enum GPIO {
            ENCODER_A = 32,
            ENCODER_B = 33,
            MOTOR_ENABLE = 25,
            MOTOR_LEFT_PWM = 26,
            MOTOR_RIGHT_PWM = 27,
        };
   
        StepTimer stepTimer;
        Encoder encoder;
        Motor motor;

        double lastPosition = 0.0;
        uint64_t leftTime = 0;
        uint64_t sampleCount = 0;
        std::pair<String, double> sampleData[5];
        std::queue<std::pair<uint32_t, double>> profileBack;
        std::queue<std::pair<uint32_t, double>> profileFront;

    public: 
        void setup() {

            sampleData[0].first = "t";
            sampleData[0].second = 0.0;
            sampleData[1].first = "u";
            sampleData[1].second = 0.0;
            sampleData[2].first = "y1";
            sampleData[2].second = 0.0;
            sampleData[3].first = "y2";
            sampleData[3].second = 0.0;
            sampleData[4].first = "y3";
            sampleData[4].second = 0.0;

            encoder.setScale(1.0);
            encoder.setOffset(0.0);
            encoder.begin(ENCODER_A, ENCODER_B);
            motor.setup(MOTOR_ENABLE, MOTOR_LEFT_PWM, MOTOR_RIGHT_PWM, -12.0, 12.0);
            stepTimer.begin(1000); // 1 ms step time
        }

        void loop() {
            
            switch(getControllerState())
            {
            case ControllerState::Loading:
                std::swap(profileFront, profileBack);
                stepTimer.reset();
                motor.set(0);
                motor.start();
                encoder.reset(0);
                sampleCount = 0;
                leftTime = 0;
                setControllerState(ControllerState::Running);
                break;
                
            case ControllerState::Running:
                stepTimer.tick([this](){
                    if(leftTime > 0) {
                        if(leftTime > stepTimer.deltaTimeUs())
                            leftTime = leftTime - stepTimer.deltaTimeUs();
                        else
                            leftTime = 0;
                    }
                    else if(!profileFront.empty()) {
                        auto step = profileFront.front();
                        profileFront.pop();
                        leftTime = step.first * 1000;
                        motor.set(step.second);
                    }
                    else {
                        setControllerState(ControllerState::Stopping);
                        return;
                    }
                    
                    //Fill sample data
                    sampleData[0].second = stepTimer.deltaTime() * sampleCount;
                    sampleData[1].second = motor.get();
                    sampleData[2].second = encoder.position();
                    /*comparative speed based on position change: (position change) / deltaTime*/
                    sampleData[3].second = encoder.speed(); /*first: position-change / time between changes */
                    sampleData[4].second = (encoder.position() - lastPosition) / stepTimer.deltaTime();   /*second: position change / fixed deltaTime */
                    lastPosition = encoder.position();
                    sampleCount++;

                    postNewSample(sampleData, 5);
                });
                break;

            case ControllerState::Stopping:
                motor.stop();
                leftTime = 0;
                sampleCount = 0;
                setControllerState(ControllerState::Waiting);
                break;

            case ControllerState::Waiting:
                delay(10);
                break;
            }
        }

        void pushProfileAsync(std::queue<std::pair<uint32_t, double>>&& profile) {
            postTask([=]() mutable -> void {
                if(getControllerState() == ControllerState::Waiting) {
                    std::swap(this->profileBack, profile);
                }
            });
        }

        void serverSetup(AsyncWebServer& webServer, AsyncEventSource& webEvents) {
            super::serverSetup(webServer, webEvents);
            webServer.on("/profile", HTTP_POST, [this](AsyncWebServerRequest*){ }, nullptr,
                [this](AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                this->onWebServerProfile(request, data, len, index, total);
            });
        }

        void onWebServerProfile(AsyncWebServerRequest* request, uint8_t *data, size_t len, size_t index, size_t total) {
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
                    pushProfileAsync(std::move(queue));
                    request->send(200);
                    return;
                }
            }
            request->send(415);
        }
    };
}