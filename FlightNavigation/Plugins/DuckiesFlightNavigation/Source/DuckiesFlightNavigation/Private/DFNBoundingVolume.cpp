// Fill out your copyright notice in the Description page of Project Settings.


#include "DFNBoundingVolume.h"
#include "DrawDebugHelpers.h"
#include "libmorton/morton.h"
#include "Algo/Unique.h"

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
	RasterizeFirstLayer();
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

bool ADFNBoundingVolume::GetNodePosition(float nodeSize, uint_fast64_t mCode, FVector& position) const
{
	uint_fast32_t X, Y, Z;
	morton3D_64_decode(mCode, X, Y, Z);
	position = GetActorLocation() - (volumeSize * 0.5f) + FVector(X, Y, Z) * nodeSize + FVector(nodeSize * 0.5f);

	return true;
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
	
}

void ADFNBoundingVolume::RasterizeLeafNode(FVector& origin, int32 layer)
{

}

void ADFNBoundingVolume::RasterizeFirstLayer()
{
	float leafSize = 4 * voxelResolution;
	float nodeSize = leafSize * 2;

	FVector layerOneNodes = volumeSize / nodeSize;

	TArray<uint64> mortonCodes; //Storing voxels that have collision
	//mortonCodes.Emplace();

	UWorld* world = GetWorld();
	if (world)
	{
		FVector origin = GetActorLocation();
		FVector extent = nodeSize * GetActorScale3D();
		DrawDebugBox(world, origin, extent, FQuat::Identity, FColor::Red, true, -1.0f, 0, 2.0f);
	}

	unsigned int X = RoundUp2Pow2(ceil(layerOneNodes.X));
	unsigned int Y = RoundUp2Pow2(ceil(layerOneNodes.Y));
	unsigned int Z = RoundUp2Pow2(ceil(layerOneNodes.Z));

	int32 layerOneDim = FMath::Max3(X, Y, Z);
	//int32 nodeAmount = layerOneDim * 3;

	//UE_LOG(LogTemp, Warning, TEXT("My integer value is: %d"), layerOneDim);

	int32 nodeAmount = 0;

	for (int32 xi = 0; xi < layerOneDim; xi++)
	{
		for (int32 yi = 0; yi < layerOneDim; yi++)
		{
			for (int32 zi = 0; zi < layerOneDim; zi++)
			{
				FVector position;
				//int32 numNode = xi + yi + zi;
				GetNodePosition(nodeSize, nodeAmount, position);

				UE_LOG(LogTemp, Warning, TEXT("My integer value is: %d"), nodeAmount);

				if (CheckCollisionOverlap(position, ECC_WorldStatic, nodeSize))
				{
					uint64 mCode = morton3D_64_encode(xi, yi, zi);
					mortonCodes.Add(mCode);
					UE_LOG(LogTemp, Warning, TEXT("My POSITION is: %s"), *position.ToString());
				}

				if (world)
				{
					DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, FColor::Emerald, true, -1.0f, 0, 2.0f);
				}

				nodeAmount++;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("My codes amount are: %d"), mortonCodes.Num());
	//UE_LOG(LogTemp, Warning, TEXT("My integer value is: %d"), numNode);


	mortonCodes.Sort();
	mortonCodes.SetNum(Algo::Unique(mortonCodes));
}

bool ADFNBoundingVolume::CheckCollisionOverlap(const FVector& position, ECollisionChannel colChannel, const float voxelSize)
{
	bool occupied = GetWorld()->OverlapAnyTestByChannel(position, FQuat::Identity, colChannel, FCollisionShape::MakeBox(FVector(voxelSize * 0.5f)));
	return occupied;
}
