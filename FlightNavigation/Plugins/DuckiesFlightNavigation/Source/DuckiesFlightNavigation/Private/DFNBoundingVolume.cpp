// Fill out your copyright notice in the Description page of Project Settings.


#include "DFNBoundingVolume.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADFNBoundingVolume::ADFNBoundingVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADFNBoundingVolume::BeginPlay()
{
	Super::BeginPlay();
	RasterizeLayer(0);
}

//Formula found in the link below.
//https://stackoverflow.com/questions/1322510/given-an-integer-how-do-i-find-the-next-largest-power-of-two-using-bit-twiddlin/1322548#1322548
unsigned int ADFNBoundingVolume::RoundUp2Pow2(unsigned int value)
{
	unsigned int v;

	v = value;

	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

// Called every frame
void ADFNBoundingVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	UWorld* world = GetWorld();
	if (world && !world->IsGameWorld())
	{
		FVector origin = GetActorLocation();
		FVector extent = volumeSize * 0.5f * GetActorScale3D();
		DrawDebugBox(world, origin, extent, FQuat::Identity, FColor::Red, false, 0.0f, 0, 2.0f);
	}
#endif
}

void ADFNBoundingVolume::RasterizeLayer(uint8 layer)
{
	float leafSize = 4 * voxelResolution;
	float nodeSize = leafSize * 2;

	FVector layerOneNodes = volumeSize / nodeSize;

	unsigned int X = RoundUp2Pow2(ceil(layerOneNodes.X));
	unsigned int Y = RoundUp2Pow2(ceil(layerOneNodes.Y));
	unsigned int Z = RoundUp2Pow2(ceil(layerOneNodes.Z));

	unsigned int layerOneDim = FMath::Max3(X, Y, Z);

	//UE_LOG(LogTemp, Warning, TEXT("My integer value is: %d"), layerOneDim);

	//loop through each Axis
		//Check collision
		//If collision: add morton code
}

void ADFNBoundingVolume::RasterizeLeafNode(FVector& origin, int32 layer)
{

}

bool ADFNBoundingVolume::CheckCollisionOverlap(FVector bound)
{
	return false;
}

