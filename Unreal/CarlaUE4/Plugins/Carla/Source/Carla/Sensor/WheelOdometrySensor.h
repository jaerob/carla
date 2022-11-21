#pragma once

#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"
#include <cstdint>


#include "WheelOdometrySensor.generated.h"

UCLASS()
class CARLA_API AWheelOdometrySensor : public ASensor
{
	GENERATED_BODY()

public:

	AWheelOdometrySensor(const FObjectInitializer& ObjectInitializer);

	static FActorDefinition GetSensorDefinition();

	void Set(const FActorDescription& ActorDescription) override;

	void SetOwner(AActor* Owner) override;

	void PostPhysTick(UWorld* World, ELevelTick TickType, float DeltaSeconds) override;

private:

	int32 slots = 30;
	float sensorResolution = 0;
	float lastWheelRotation[4];
	float lastRemainder[4];
};