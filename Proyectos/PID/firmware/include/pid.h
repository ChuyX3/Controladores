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
#include "coffee.h"
#include "coffee/steptimer.h"

namespace Coffee
{
    enum class DebugLevel {
        None,
        Verbose,
        Debug,   
    };

    enum class SystemState {
        Loading,
        Running,
        Error,
    };

    enum class ControllerState {
        Loading,
        Waiting,
        Starting,
        Running,
        Stopping,
        Calibrating,
    };

    enum class ServerState {
        Initializing,
        InitAP,
        InitServer,
        InitMDNS,
        InitSTA,
        WaitSTA,
        Listening,
        Stopped
    };

    template<typename Subclass, DebugLevel level = DebugLevel::Debug>
    struct SerieLogger
    {
        SerieLogger() = default;

        void setup()const{
            coffee::constexpr_if<(level >= DebugLevel::Verbose)>([&](auto...){
                Serial.begin(115200);
            });
        }

        template<typename... Args>
        void print(Args... args)const {
            coffee::constexpr_if<(level >= DebugLevel::Verbose)>([&](auto...){
                (Serial.print(args), ...);
            });
        }

        template<typename... Args>
        void println(Args... args)const {
            coffee::constexpr_if<(level >= DebugLevel::Verbose)>([&](auto...){
                (Serial.print(args), ...);
                Serial.println();
            });
        }

    };


}