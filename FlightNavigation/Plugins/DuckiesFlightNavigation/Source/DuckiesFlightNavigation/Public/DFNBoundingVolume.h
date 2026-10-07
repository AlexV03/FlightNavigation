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
	FVector volumeSize = { 1000.f, 1000.f, 1000.f };
	UPROPERTY(EditAnywhere)
	int voxelResolution = 50; //Value is in centimeters
	UPROPERTY(EditAnywhere)
	bool showRootNode = false;
	UPROPERTY(EditAnywhere)
	bool showSubNodes = false;
	UPROPERTY(EditAnywhere)
	bool showNeighborLinks = false;
	UPROPERTY(EditAnywhere)
	bool autoRebuild = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FDFNOctreeData octD;

	unsigned int RoundUp2Pow2(unsigned int value);
	bool GetNodePosition(float nodeSize, uint_fast64_t mCode, FVector& position) const;
	uint64 GetParentCode(uint64 childCode);
	uint64 GetChildCode(uint64 parentCode, unsigned int index);
	int32 GetNodeAmountInLayer(uint8 layer) const;
	float GetVoxelSize(uint8 layer) const;
	bool GetIndexFromCode(uint8 layer, uint64 mCode, int32& cIndex) const;
	bool CheckIfNodeIsBlocked(uint8 layer, uint64 mCode);
	FVector GetFaceDirection(int faceIdx);
	int32 GetNodeAmountOfSide(uint8 layer);
	void FindNeighborInParents(uint8 layer, int32 nodeIndex, uint8 faceIndex, uint8& parentLayer, int32& neighborNodeIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RasterizeLayer(uint8 layer);
	void RasterizeLeafNode();
	void RasterizeFirstLayer();
	void BuildNeighborLinks(uint8 layer);
	bool CheckCollisionOverlap(const FVector& position, ECollisionChannel colChannel, const float voxelSize);
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();
	UFUNCTION(CallInEditor, Category = "Debug")
	void ClearVolume();

#if WITH_EDITOR
	//virtual void PostEditMove(bool bFinished) override;
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif

private:
	int32 newVolumeSize = 0;
	FVector origin;
	static const TArray<FColor> layerColors;

	const FColor& GetColorAt(int32 index) const;
	void CreateLayerMortonCodes();
};
