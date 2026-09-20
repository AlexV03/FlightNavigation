// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OctreeData.h"
#include "DFNBoundingVolume.generated.h"

UCLASS()
class DUCKIESFLIGHTNAVIGATION_API ADFNBoundingVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADFNBoundingVolume();

	UPROPERTY(EditAnywhere)
	FVector volumeSize;

	//Function to calculate the navmesh

	int voxelResolution = 100; //Value is in centimeters

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FDFNOctreeData OctD; 

	unsigned int RoundUp2Pow2(unsigned int value);
	bool GetNodePosition(float nodeSize, uint_fast64_t mCode, FVector& position) const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RasterizeLayer(uint8 layer);
	void RasterizeLeafNode(FVector& origin, int32 layer);
	void RasterizeFirstLayer();
	bool CheckCollisionOverlap(const FVector& position, ECollisionChannel colChannel, const float voxelSize);

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif
};
