// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "DFNPathFinder.h"//temp
//#include "DFNBoundingVolume.h"//temp
#include "DFNNavAgent.generated.h"

class ADFNBoundingVolume;
class DFNPathFinder;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUCKIESFLIGHTNAVIGATION_API UDFNNavAgent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDFNNavAgent();

	UPROPERTY(EditAnywhere)
	ADFNBoundingVolume* bv = nullptr;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "My Component")
	void FindPath();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TUniquePtr<DFNPathFinder> pathFinder;
};
