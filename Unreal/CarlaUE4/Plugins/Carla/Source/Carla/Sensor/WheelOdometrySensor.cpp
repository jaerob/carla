#include "Carla.h"
#include "Carla/Sensor/WheelOdometrySensor.h"

#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"

#include "GenericPlatform/GenericPlatform.h"
#include "Kismet/KismetMathLibrary.h"

AWheelOdometrySensor::AWheelOdometrySensor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	lastWheelRotation[0] = 0;
	lastWheelRotation[1] = 0;
	lastWheelRotation[2] = 0;
	lastWheelRotation[3] = 0;

	lastRemainder[0] = 0;
	lastRemainder[1] = 0;
	lastRemainder[2] = 0;
	lastRemainder[3] = 0;

	sensorResolution = 360.0f / (slots * 2);
}

FActorDefinition AWheelOdometrySensor::GetSensorDefinition()
{
	auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
		TEXT("other"),
		TEXT("wheel_odometry")
	);


	FActorVariation Slots;
	Slots.Id = TEXT("wheel_odometry_slots");
	Slots.Type = EActorAttributeType::Int;
	Slots.RecommendedValues = { TEXT("30") };
	Slots.bRestrictToRecommended = false;

	Definition.Variations.Append({ Slots });

	return Definition;
}

void AWheelOdometrySensor::Set(const FActorDescription& Description)
{
	Super::Set(Description);

	int32 Slots = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToInt(
		"wheel_odometry_slots",
		Description.Variations,
		30);

	slots = Slots;
}

void AWheelOdometrySensor::SetOwner(AActor* Owner)
{
	Super::SetOwner(Owner);
}

void AWheelOdometrySensor::PostPhysTick(UWorld* World, ELevelTick TickType, float DeltaSeconds) {
	// We expect that the owner is a Carla Vehicle [ACarlaWheeledVehicle] which inherits from AWheeledVehicle.
	// AWheeledVehicle has a UWheeledVehicleMovementComponent which has a WheelSetup for each wheel.
	AWheeledVehicle const* vehicle = Cast<AWheeledVehicle>(GetOwner());
	if (vehicle == nullptr) {
		return;
	}

	TArray<int32> Rotations;
	// Represent the timestamp in mikroseconds and as an integer so that we can simply use the int array
	// to transfer the data to the python api.
	Rotations.Add(UKismetMathLibrary::Round(DeltaSeconds * 1000000));
	UWheeledVehicleMovementComponent* movementComponent = vehicle->GetVehicleMovement();

	// We expect exactly 4 wheels so this sensor will not work for other vehicles like bikes.
	if (movementComponent->Wheels.Num() == 4) {

		for (int32 index = 0; index < movementComponent->Wheels.Num(); index++) {
			UVehicleWheel const* wheel = movementComponent->Wheels[index];
			// For some reason the rotation that the movementComponent returns is always negative.
			float currentRotation = -wheel->GetRotationAngle();
			float lastRotation = lastWheelRotation[index];

			float difference = currentRotation - lastRotation;
			if (difference < -900.0f) {
				// The wheel's rotation is stored in a range between 0 and 1800 degree. Once the rotation 
				// is higher than 1800, it begins from 0 again. This if statement corrects this jump.
				// Of course this calculation will fail if the wheel is rotating too fast or the physics
				// tick rate is too small.
				difference = 1800 + difference;
			}

			float summedRemainder = lastRemainder[index] + difference;

			float remainder = 0.0f;
			int32 numTicks = UKismetMathLibrary::FMod(summedRemainder, sensorResolution, remainder);

			Rotations.Add(numTicks);
			lastWheelRotation[index] = currentRotation;
			lastRemainder[index] = remainder;
		}

		auto Stream = GetDataStream(*this);
		Stream.Send(*this, Rotations);
	}
}