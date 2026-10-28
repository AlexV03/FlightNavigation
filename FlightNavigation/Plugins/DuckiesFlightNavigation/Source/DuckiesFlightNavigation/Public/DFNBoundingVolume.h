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

	unsigned int RoundUp2Pow2(unsigned int _value);
	bool GetNodePosition(float _nodeSize, uint_fast64_t _mCode, FVector& _position) const;
	uint64 GetParentCode(uint64 _childCode);
	uint64 GetChildCode(uint64 _parentCode, unsigned int _index);
	int32 GetNodeAmountInLayer(uint8 _layer) const;
	float GetVoxelSize(uint8 _layer) const;
	bool GetIndexFromCode(uint8 _layer, uint64 _mCode, int32& _cIndex) const;
	bool CheckIfNodeIsBlocked(uint8 _layer, uint64 _mCode);
	FVector GetFaceDirection(int _faceIdx);
	int32 GetNodeAmountOfSide(uint8 _layer) const;
	void FindNeighborInParents(uint8 _layer, int32 _nodeIndex, uint8 _faceIndex, uint8& _parentLayer, int32& _neighborNodeIndex);
	void CollectTraversableLinks(const FDFNLink& _link, uint8 _faceIndex, TArray<FDFNLink>& _outLinks) const;
	uint8 SubnodeCoordToIndex(uint8 _x, uint8 _y, uint8 _z) const;
	FIntVector SubnodeIndexToCoord(uint8 _subIndex) const;
	bool IsLeafWithSubnodes(const FDFNLink& _link) const;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RasterizeLayer(uint8 _layer);
	void RasterizeLeafNode();
	void RasterizeFirstLayer();
	void BuildNeighborLinks(uint8 _layer);
	TArray<FDFNLink> GetNeighborLinks(const FDFNLink& _link) const;
	bool GetSubnodeNeighbor(const FDFNLink& _link, uint8 _faceIndex, FDFNLink& _outNeighbor) const;
	bool CheckCollisionOverlap(const FVector& _position, ECollisionChannel _colChannel, const float _voxelSize);
	UFUNCTION(CallInEditor, Category = "Debug")
	void Generate();
	UFUNCTION(CallInEditor, Category = "Debug")
	void ClearVolume();

	bool CalculatePositionFromLink(const FDFNLink& _link, FVector& _position) const;
	bool CalculateLinkFromPosition(const FVector& _position, FDFNLink& _link) const;

	bool IsGenerated() const { return octD.NumberLayers > 0 && octD.layers.Num() == octD.NumberLayers; }

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

	const FColor& GetColorAt(int32 _index) const;
	void CreateLayerMortonCodes();
};
