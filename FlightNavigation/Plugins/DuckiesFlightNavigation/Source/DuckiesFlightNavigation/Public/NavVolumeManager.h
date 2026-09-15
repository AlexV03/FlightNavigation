// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavVolumeManager.generated.h"

UCLASS()
class DUCKIESFLIGHTNAVIGATION_API ANavVolumeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavVolumeManager();

	//List of all the NavVolumes
		//For each navVolume it adds a button to rebuild that volume
		//When a navvolume is place in the level this should automatically be added to the list.
		//When navvolume is removed reorder the list.

	//Button to rebuild ALL the navvolumes

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
