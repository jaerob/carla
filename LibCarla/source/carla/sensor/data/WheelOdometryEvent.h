#pragma once

#include "carla/sensor/data/Array.h"

namespace carla {
    namespace sensor {
        namespace data {
            class WheelOdometryEvent : public Array<int> {
            public:
                explicit WheelOdometryEvent(RawData&& data)
                    : Array<int>(0u, std::move(data)) {}
            };
        } // namespace data
    } // namespace sensor
} // namespace carla