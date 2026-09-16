// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavVolume.generated.h"

UCLASS()
class DUCKIESFLIGHTNAVIGATION_API ANavVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavVolume();

	UPROPERTY(EditAnywhere)
	FVector extents;

	//Function to calculate the navmesh

	int voxelResolution = 2; //Values are in meters. 2 is 2 meter per voxel in size. leaf node will then be 4 x 2.(2 being the resolution number)

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif
};
