// Fill out your copyright notice in the Description page of Project Settings.


#include "DFNNavAgent.h"
#include "DFNBoundingVolume.h"
#include "DFNPathFinder.h"

// Sets default values for this component's properties
UDFNNavAgent::UDFNNavAgent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UDFNNavAgent::FindPath()
{
	pathFinder->FindPath(GetOwner()->GetActorLocation(), FVector(330, 290, 1010));
}

// Called when the game starts
void UDFNNavAgent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (bv)
	{
		pathFinder = MakeUnique<DFNPathFinder>(*bv);

		// test call
		
		//pathFinder->FindPath(FVector(10, -480, 1750), FVector(-660, 380, 1000));
	}
}


// Called every frame
void UDFNNavAgent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	pathFinder->DrawPath();
}

