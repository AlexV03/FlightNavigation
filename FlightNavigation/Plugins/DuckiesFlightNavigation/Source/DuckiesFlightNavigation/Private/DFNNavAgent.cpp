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

	const FVector actorPos = owningActor->GetActorLocation();

	if (!pathFinder->FindPath(actorPos, destination, agentSize))
	{
		UE_LOG(LogTemp, Warning, TEXT("DFNNavAgent: no path to %s"), *destination.ToString());
		FinishMove();
		return;
	}


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

	AActor* currentActor = owningActor.Get();
	if (!currentActor)
	{
		FinishMove();
		return;
	}

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

void UDFNNavAgent::MoveTowards(APawn* _pawn, const FVector _destination, const AActor* _targetActor, const float _acceptanceRadius)
{
	AActor* currentActor = _pawn ? static_cast<AActor*>(_pawn) : GetOwner();
	const FVector newDestination = _targetActor ? _targetActor->GetActorLocation() : _destination;

	const bool sameSettings = isMoving && !HasMoved(newDestination, destination);

	acceptanceRadius = _acceptanceRadius;

	if (sameSettings)
		return;

	owningActor = currentActor;
	destination = newDestination;
	isMoving = true;

	FindPath();
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
		return false;
	}

	if (!pathFinder || pathFinderVolume != bv)
	{
		pathFinder = MakeUnique<DFNPathFinder>(*bv);
		pathFinderVolume = bv;
	}

	return true;
}

bool UDFNNavAgent::HasMoved(const FVector& _current, const FVector& _last) const
{
	if (recalcDistance <= 0.0f)
		return !_current.Equals(_last);

	return FVector::DistSquared(_current, _last) > FMath::Square(recalcDistance);
}

void UDFNNavAgent::FollowPath(float _deltaTime)
{
	AActor* currentActor = owningActor.Get();

	const FVector oldLocation = currentActor->GetActorLocation();
	FVector currentLocation = oldLocation;

	float remaining = agentSpeed * _deltaTime;

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
	followPath.Empty();
	waypointIndex = 0;
}

//void UDFNNavAgent::SmoothPath()
//{
	//Pre pruning with line of sight, to reduce path points that are going in straight line.
	//Chaikin
	//string pulling
	//catmull-rom spline/smoothing path points
//}
