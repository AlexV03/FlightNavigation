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
	int32 GetNodeAmountOfSide(uint8 layer) const;
	void FindNeighborInParents(uint8 layer, int32 nodeIndex, uint8 faceIndex, uint8& parentLayer, int32& neighborNodeIndex);
	void CollectTraversableLinks(const FDFNLink& link, uint8 faceIndex, TArray<FDFNLink>& outLinks) const;
	uint8 SubnodeCoordToIndex(uint8 x, uint8 y, uint8 z) const;
	FIntVector SubnodeIndexToCoord(uint8 subIndex) const;
	bool IsLeafWithSubnodes(const FDFNLink& link) const;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RasterizeLayer(uint8 layer);
	void RasterizeLeafNode();
	void RasterizeFirstLayer();
	void BuildNeighborLinks(uint8 layer);
	TArray<FDFNLink> GetNeighborLinks(const FDFNLink& link) const;
	bool GetSubnodeNeighbor(const FDFNLink& link, uint8 faceIndex, FDFNLink& outNeighbor) const;
	bool CheckCollisionOverlap(const FVector& position, ECollisionChannel colChannel, const float voxelSize);
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();
	UFUNCTION(CallInEditor, Category = "Debug")
	void ClearVolume();

	bool CalculatePositionFromLink(const FDFNLink& link, FVector& position) const;
	bool CalculateLinkFromPosition(const FVector& position, FDFNLink& link) const;

	bool IsGenerated() const { return octD.NumberLayers > 0 && octD.layers.Num() == octD.NumberLayers; }
	//uint32 GetGenerationId() const { return bvgId; }

	bool IsInsideNavVolume(const FVector& _position) const;

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif

private:
	int32 newVolumeSize = 0;
	FVector origin;
	static const TArray<FColor> layerColors;

	//uint32 bvgId = 0;

	const FColor& GetColorAt(int32 index) const;
	void CreateLayerMortonCodes();
};
