// Fill out your copyright notice in the Description page of Project Settings.


#include "NavVolume.h"
#include "DrawDebugHelpers.h"

// Sets default values
ANavVolume::ANavVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANavVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANavVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#if WITH_EDITOR
	UWorld* world = GetWorld();
	if (world && !world->IsGameWorld())
	{
		FVector origin = GetActorLocation();
		FVector extent = extents * GetActorScale3D();
		DrawDebugBox(world, origin, extent, FQuat::Identity, FColor::Red, false, 0.0f, 0, 2.0f);
	}
#endif
}

