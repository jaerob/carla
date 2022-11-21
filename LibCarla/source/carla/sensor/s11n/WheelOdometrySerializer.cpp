#include "carla/sensor/s11n/WheelOdometrySerializer.h"
#include "carla/sensor/data/WheelOdometryEvent.h"

namespace carla {
    namespace sensor {
        namespace s11n {
            SharedPtr<SensorData> WheelOdometrySerializer::Deserialize(RawData&& data) {
                return SharedPtr<SensorData>(new data::WheelOdometryEvent(std::move(data)));
            }
        } // namespace s11n
    } // namespace sensor
} // namespace carla