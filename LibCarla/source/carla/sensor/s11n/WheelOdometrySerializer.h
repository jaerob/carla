#pragma once

#include "carla/Memory.h"
#include "carla/sensor/RawData.h"
#include <cstring>
#include <cstdint>

namespace carla {
    namespace sensor {
        class SensorData;
        namespace s11n {
            class WheelOdometrySerializer {
            public:
                template <typename SensorT, typename RotationListT>
                static Buffer Serialize(
                    const SensorT&,
                    const RotationListT& rotations) {
                    const uint32_t size_in_bytes = sizeof(int32_t) * rotations.Num();
                    Buffer buffer{ size_in_bytes };
                    unsigned char* it = buffer.data();
                    for (int32_t const rotation : rotations) {
                        std::memcpy(it, &rotation, sizeof(int32_t));
                        it += sizeof(int32_t);
                    }
                    return buffer;
                }
                static SharedPtr<SensorData> Deserialize(RawData&& data);
            };
        } // namespace s11n
    } // namespace sensor
