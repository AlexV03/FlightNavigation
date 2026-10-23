// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DFNNavAgent.generated.h"

class ADFNBoundingVolume;
class DFNPathFinder;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUCKIESFLIGHTNAVIGATION_API UDFNNavAgent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDFNNavAgent();

	UPROPERTY(EditAnywhere, Category = "Navigation")
	TObjectPtr<ADFNBoundingVolume> bv = nullptr;//Make private and get it on beginplay from the scene

	//How much a target actor has to move before path is recalculated
	UPROPERTY(EditAnywhere, Category = "Navigation", meta = (ClampMin = "0.0"))
	float recalcDistance = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float agentSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	float agentSize = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	bool drawPath = true;

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void MoveTowards(APawn* pawn, const FVector _destination, const AActor* _targetActor, const float _acceptanceRadius);

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void StopMovement();

	UFUNCTION(BlueprintPure, Category = "Navigation")
	bool IsMoving() const { return isMoving; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	bool FindVolume();
	bool EnsurePathFinder();
	void FindPath();
	//bool NeedsRecalculation() const;
	bool HasMoved(const FVector& current, const FVector& last) const;
	void FollowPath(float deltaTime);
	void FinishMove();
	void SmoothPath();

	TUniquePtr<DFNPathFinder> pathFinder;
	const ADFNBoundingVolume* pathFinderVolume = nullptr;

	TWeakObjectPtr<AActor> owningActor;
	//TWeakObjectPtr<const AActor> targetActor;
	FVector destination = FVector::ZeroVector;
	//FVector lastTarget = FVector::ZeroVector;
	//uint32 lastGenerationId = MAX_uint32;
	float acceptanceRadius = 0.0f;

	TArray<FVector> followPath;
	int32 waypointIndex = 0;

	bool isMoving = false;
	//bool pathFound = false;
	//bool forceRecalculation = false;
};
