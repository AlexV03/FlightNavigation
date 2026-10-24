// Fill out your copyright notice in the Description page of Project Settings.


#include "DFNNavAgent.h"
#include "DFNBoundingVolume.h"
#include "DFNPathFinder.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UDFNNavAgent::UDFNNavAgent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	bTickInEditor = false;
}

void UDFNNavAgent::FindPath()
{
	AActor* currentActor = owningActor.Get();

	if (!currentActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO ACCTOORRR"));
	}

	if (!EnsurePathFinder())
	{
		UE_LOG(LogTemp, Warning, TEXT("NO MORE PARTHHHSS"));
	}

	if (!bv->IsGenerated())
	{
		UE_LOG(LogTemp, Warning, TEXT("NO VANIGATION BOUNDSSS"));
	}

	if (!currentActor || !EnsurePathFinder() || !bv->IsGenerated())
	{
		FinishMove();
		return;
	}

	/*if (targetActor.IsValid())
		destination = targetActor->GetActorLocation();*/

	const FVector actorPos = owningActor->GetActorLocation();

	if (!pathFinder->FindPath(actorPos, destination, agentSize))
	{
		UE_LOG(LogTemp, Warning, TEXT("DFNNavAgent: no path to %s"), *destination.ToString());
		FinishMove();
		return;
	}

	//lastTarget = destination;
	//lastGenerationId = bv->GetGenerationId();
	//forceRecalculation = false;

	followPath = pathFinder->GetPath();
	followPath.Add(destination);//Add destination to make agent go to final position
	waypointIndex = 0;
}

// Called when the game starts
void UDFNNavAgent::BeginPlay()
{
	Super::BeginPlay();
	FindVolume();
	EnsurePathFinder();
}

void UDFNNavAgent::StopMovement()
{
	if (isMoving)
		FinishMove();
}

// Called every frame
void UDFNNavAgent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!isMoving)
		return;

	//Should only update when MoveTowards has been called, and then stops updating path when pawn/actor has reached
	//its destination or targetActor

	AActor* currentActor = owningActor.Get();
	if (!currentActor)
	{
		FinishMove();
		return;
	}



	//needs to change: should recalculate when either target or destination changes or when target is moving
	//. not when this actor/pawn moves
	//No need to recalculate path when this actor/pawn is moving.
	//Should only findpath when it needs recalculation and when it has a path stop recalculating it and start
	//path moving for the pawn/actor, so the pawn/actor follows the path
	/*if (!bv->IsGenerated())
		return;*/
		//else make pawn/actor follow the path, path is stored as finalpath in the pathfinder

		/*if (NeedsRecalculation())
		{
			FindPath();
			if (!isMoving)
				return;
		}*/

	FollowPath(DeltaTime);

	const bool reachedEnd = !followPath.IsValidIndex(waypointIndex);
	const bool withinRadius = FVector::DistSquared(currentActor->GetActorLocation(), destination) <= FMath::Square(acceptanceRadius);

	if (reachedEnd || withinRadius)
	{
		FinishMove();
		return;
	}

	if (drawPath)
	{
		if (followPath.Num() > 0)
			pathFinder->DrawPath();
		else
			DrawDebugLine(GetWorld(), currentActor->GetActorLocation(), destination,
				FColor::Red, false, -1.0f, 0, 2.0f);
	}
}

//Everything should be inplace now and working.
//TO TEST!!! call MoveTowards function in a blueprint actor once and then also test every update.
//PARTITALLY WORKS:!!!: movement works, SHoudl use smoothing, but there are edge cases were it doesnt work.

void UDFNNavAgent::MoveTowards(APawn* pawn, const FVector _destination, const AActor* _targetActor, const float _acceptanceRadius)
{
	AActor* currentActor = pawn ? static_cast<AActor*>(pawn) : GetOwner();
	const FVector newDestination = _targetActor ? _targetActor->GetActorLocation() : _destination;

	const bool sameSettings = isMoving /*&& owningActor.Get() == currentActor*/
		/*&& targetActor.Get() == _targetActor*/ && /*(_targetActor ||*/ !HasMoved(newDestination, destination/*)*/);

	acceptanceRadius = _acceptanceRadius;

	if (sameSettings)
		return;

	owningActor = currentActor;
	//targetActor = _targetActor;
	destination = newDestination;
	isMoving = true;
	//forceRecalculation = true;

	FindPath();

	//FindVolume();??
}

bool UDFNNavAgent::FindVolume()
{
	if (IsValid(bv))
		return true;

	UWorld* world = GetWorld();
	AActor* owner = GetOwner();

	if (!world || !owner)
		return false;

	for (TActorIterator<ADFNBoundingVolume> it(world); it; ++it)
	{
		if (it->IsInsideNavVolume(owner->GetActorLocation()))
		{
			bv = *it;
			return true;
		}
	}

	return false;
}

bool UDFNNavAgent::EnsurePathFinder()
{
	if (!IsValid(bv))
	{
		pathFinder.Reset();
		pathFinderVolume = nullptr;
		//pathFound = false;
		return false;
	}

	if (!pathFinder || pathFinderVolume != bv)
	{
		pathFinder = MakeUnique<DFNPathFinder>(*bv);
		pathFinderVolume = bv;
		//lastGenerationId = MAX_uint32;   // force recalculation
	}

	return true;
}
//bool UDFNNavAgent::NeedsRecalculation() const
//{
//	if (forceRecalculation)
//		return true;
//
//	if (bv->GetGenerationId() != lastGenerationId)
//		return true;
//
//	return targetActor.IsValid() && HasMoved(targetActor->GetActorLocation(), lastTarget);
//}

bool UDFNNavAgent::HasMoved(const FVector& current, const FVector& last) const
{
	if (recalcDistance <= 0.0f)
		return !current.Equals(last);

	return FVector::DistSquared(current, last) > FMath::Square(recalcDistance);
}

void UDFNNavAgent::FollowPath(float deltaTime)
{
	AActor* currentActor = owningActor.Get();
	/*if (!currentActor || !followPath.IsValidIndex(waypointIndex))
		return;*/

	const FVector oldLocation = currentActor->GetActorLocation();
	FVector currentLocation = oldLocation;

	float remaining = agentSpeed * deltaTime;

	while (remaining > 0.0f && followPath.IsValidIndex(waypointIndex))
	{
		const FVector waypoint = followPath[waypointIndex];
		const float dist = FVector::Dist(currentLocation, waypoint);

		if (dist <= remaining)
		{
			currentLocation = waypoint;
			remaining -= dist;
			++waypointIndex;
		}
		else
		{
			currentLocation += (waypoint - currentLocation) / dist * remaining;
			remaining = 0.0f;
		}
	}

	const FVector moveDelta = currentLocation - oldLocation;
	if (moveDelta.IsNearlyZero())
		return;

	currentActor->SetActorLocationAndRotation(currentLocation, moveDelta.Rotation());
}

void UDFNNavAgent::FinishMove()
{
	isMoving = false;
	//pathFound = false;
	//forceRecalculation = false;
	followPath.Empty();
	waypointIndex = 0;
}

void UDFNNavAgent::SmoothPath()
{
	//Pre pruning with line of sight, to reduce path points that are going in straight line.
	//Chaikin
	//string pulling
	//catmull-rom spline/smoothing path points
}
