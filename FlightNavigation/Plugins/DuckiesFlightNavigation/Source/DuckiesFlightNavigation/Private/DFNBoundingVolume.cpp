// Fill out your copyright notice in the Description page of Project Settings.


#include "DFNBoundingVolume.h"
#include "DrawDebugHelpers.h"
#include "libmorton/morton.h"
#include "Algo/Unique.h"
#include "Math/Color.h"

const TArray<FColor> ADFNBoundingVolume::layerColors = {
		FColor(230, 25, 75),   // Red
		FColor(60, 180, 75),   // Green
		FColor(255, 225, 25),  // Yellow
		FColor(0, 130, 200),   // Blue
		FColor(245, 130, 48),  // Orange
		FColor(145, 30, 180),  // Purple
		FColor(70, 240, 240),  // Cyan
		FColor(240, 50, 230),  // Magenta
		FColor(210, 245, 60),  // Lime
		FColor(250, 190, 212), // Pink
		FColor(0, 128, 128),   // Teal
		FColor(220, 190, 255), // Lavender
		FColor(170, 110, 40),  // Brown
		FColor(128, 0, 0),     // Maroon
		FColor(170, 255, 195), // Mint
		FColor(0, 0, 128)      // Navy
};

// Sets default values
ADFNBoundingVolume::ADFNBoundingVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADFNBoundingVolume::BeginPlay()
{
	Super::BeginPlay();
	Generate();
}

//Formula found in the link below.
//https://stackoverflow.com/questions/1322510/given-an-integer-how-do-i-find-the-next-largest-power-of-two-using-bit-twiddlin/1322548#1322548
unsigned int ADFNBoundingVolume::RoundUp2Pow2(unsigned int value)
{
	unsigned int v;

	v = value;

	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	return v;
}

bool ADFNBoundingVolume::GetNodePosition(float nodeSize, uint_fast64_t mCode, FVector& position) const
{
	uint_fast32_t X, Y, Z;
	morton3D_64_decode(mCode, X, Y, Z);
	//position = origin - (newVolumeSize * 0.5f) + (FVector(X, Y, Z) * nodeSize) + FVector(nodeSize * 0.5f);
	position = GetActorLocation() - (newVolumeSize * 0.5f) + (FVector(X, Y, Z) * nodeSize) + FVector(nodeSize * 0.5f);

	return true;
}

uint64 ADFNBoundingVolume::GetParentCode(uint64 childCode)
{
	return childCode >> 3;
}

uint64 ADFNBoundingVolume::GetChildCode(uint64 parentCode, unsigned int index)
{
	return (parentCode << 3) | index;
}

int32 ADFNBoundingVolume::GetNodeAmountInLayer(uint8 layer) const
{
	return FMath::Pow(FMath::Pow(2.0f, ((octD.NumberLayers - 1) - layer)), 3);
}

float ADFNBoundingVolume::GetVoxelSize(uint8 layer) const
{
	return ((newVolumeSize * 0.5) / FMath::Pow(2.f, (octD.NumberLayers - 1))) * (FMath::Pow(2.0f, layer + 1));
}

bool ADFNBoundingVolume::GetIndexFromCode(uint8 layer, uint64 mCode, int32& cIndex) const
{
	const TArray<FDFNNode>& nLayer = octD.layers[layer];

	int low = 0;
	int high = nLayer.Num() - 1;

	while (low <= high)
	{
		int mid = low + (high - low) / 2;
		uint64 midCode = nLayer[mid].mortonCode;

		if (midCode == mCode)
		{
			cIndex = mid;
			return true;
		}

		if (midCode < mCode)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return false;
}

// Called every frame
void ADFNBoundingVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	UWorld* world = GetWorld();
	if (world && !world->IsGameWorld())
	{
		FVector origin2 = GetActorLocation();
		FVector extent = volumeSize * 0.5f * GetActorScale3D();
		DrawDebugBox(world, origin2, extent, FQuat::Identity, GetColorAt(0), false, 0.0f, 0, 2.0f);
	}
#endif
}

void ADFNBoundingVolume::RasterizeLayer(uint8 layer)
{
	//check if layer 0
		//if layer 0
		//forloop through each node in this layer to create the leafnodes
			//create leaf node
			//RasterrizeLeafNode(aka, collision checking for the 64 subnodes)
	//else
		//forloop through all nodes in this layer
			//Create node
			//fill node, firstChild(filled with proper values), parent(filled with proper values), mortoncode
	UWorld* world = GetWorld();
	int32 nodeAmount = GetNodeAmountInLayer(layer);
	float nodeSize = GetVoxelSize(layer);

	/*UE_LOG(LogTemp, Warning, TEXT("!Node amount: %d"), nodeAmount);
	UE_LOG(LogTemp, Warning, TEXT("!Node size: %f"), nodeSize);
	UE_LOG(LogTemp, Warning, TEXT("!Current layer: %d"), layer);
	UE_LOG(LogTemp, Warning, TEXT("!Max layers in oct: %d"), octD.NumberLayers);*/

	//if (layer == 6)
	//{
	//	for (int32 i = 0; i < nodeAmount; i++)
	//	{
	//		//check if node is in morton code
	//		//if true:
	//			//create leafnode
	//			//go trough leafnode subnodes(rasterize leafnode)
	//		if (world)
	//		{
	//			FVector position;
	//			GetNodePosition(nodeSize, i, position);
	//			DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, GetColorAt(layer), true, -1.0f, layer, 2.0f);
	//		}
	//	}
	//}

	if (layer == 0)
	{
		//Create octD layer 0
		//octD.layers.Emplace();

		for (int32 i = 0; i < nodeAmount; i++)
		{
			//check if node is in morton code
			//if true:
				//create leafnode
				//go trough leafnode subnodes(rasterize leafnode)
			if (octD.mortonCodes[0].Contains(GetParentCode(i)))
			{
				//Create a new node
				//Fill node data
				//Add node to layer 0 nodes array.
				FDFNNode node;
				FDFNLink link;
				link.layer = 0;
				link.nodeIndex = i;
				link.subNodeIndex = 0;
				node.firstChild = link;
				node.mortonCode = i;

				//UE_LOG(LogTemp, Warning, TEXT("!Max layers in oct: %d"), nodeAmount);
				FVector position;
				GetNodePosition(nodeSize, i, position);

				if (CheckCollisionOverlap(position, ECC_WorldStatic, nodeSize))
				{
					RasterizeLeafNode(position, layer);
				}

				octD.layers[0].Add(node);
			}
		}
	}
	else if (layer < octD.NumberLayers - 1)
	{
		//octD.layers.Emplace();
		int32 nodeCount = 0;

		for (int32 i = 0; i < nodeAmount; i++)
		{
			//If statement to check if this node is within a child node that has morton code(collision)
			if (octD.mortonCodes[layer].Contains(GetParentCode(i)))
			{

				//do the layer caluclations
				FDFNNode node;
				FDFNLink link;
				int32 childIndex;
				//Parent -> child. Giving parent node its child node
				node.mortonCode = i;

				//If GetChildNode
				if (GetIndexFromCode(layer, node.mortonCode << 3, childIndex))
				{


					link.layer = layer - 1;
					link.nodeIndex = childIndex;
					link.subNodeIndex = 0;
					node.firstChild = link;

					nodeCount++;


					//Child -> parent. Go through 8 children and setting there parent link
					for (int ci = 0; ci < 8; ci++)
					{
						octD.layers[node.firstChild.layer][node.firstChild.nodeIndex + ci].parent.layer = layer;
						octD.layers[node.firstChild.layer][node.firstChild.nodeIndex + ci].parent.nodeIndex = nodeCount;
					}

					octD.layers[layer].Add(node);

					if (world)
					{
						FVector position;
						GetNodePosition(nodeSize, i, position);
						DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, GetColorAt(layer), true, -1.0f, layer, 2.0f);
					}
				}
			}
		}
	}
	else if (layer == octD.NumberLayers - 1)
	{
		//Create root node

		if (world)
		{
			if (showRootNode && layer == octD.NumberLayers - 1)
			{
				FVector position;
				GetNodePosition(nodeSize, 0, position);
				DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, GetColorAt(layer), true, -1.0f, layer, 2.0f);
			}
		}
	}
}

//Step by step.
//1: make all the nodes, make array with colors for each layer.*
//2: check for morton code if node has collision.
//3: add those nodes to the array.

void ADFNBoundingVolume::RasterizeLeafNode(FVector& _origin, uint8 layer)
{
	//loop through 64 voxels
	//Check collision
	//If true: set voxel int bit to 1
	//If false: set voxel int bit to 0
}

//Idea: Box needs to be power of 2, could make it that the bounding box is still the real navmesh bounds. so everything outside the
//bounding box will be "blocked" and everything inside will be calculated normally.


void ADFNBoundingVolume::RasterizeFirstLayer()
{
	octD.mortonCodes.Emplace();

	float leafSize = 4 * voxelResolution;
	float nodeSize = leafSize * 2;

	FVector layerOneNodes = volumeSize / nodeSize;

	//TArray<uint64> mortonCodes; //Storing voxels that have collision
	//mortonCodes.Emplace();

	UWorld* world = GetWorld();
	//Modify to make it cover the whole octree eventually
	//if (world)
	//{
	//	FVector origin = GetActorLocation();
	//	FVector extent = nodeSize * GetActorScale3D();
	//	DrawDebugBox(world, origin, extent, FQuat::Identity, FColor::Blue, true, -1.0f, 0, 2.0f);
	//}

	unsigned int X = RoundUp2Pow2(ceil(layerOneNodes.X));
	unsigned int Y = RoundUp2Pow2(ceil(layerOneNodes.Y));
	unsigned int Z = RoundUp2Pow2(ceil(layerOneNodes.Z));

	int32 layerOneDim = FMath::Max3(X, Y, Z);
	//int32 nodeAmount = layerOneDim * 3;

	newVolumeSize = nodeSize * layerOneDim;
	origin = GetActorLocation() - FVector(newVolumeSize * 0.5f);

	octD.NumberLayers = 2 + log2(layerOneDim);
	//UE_LOG(LogTemp, Warning, TEXT("My integer value idddddddddds: %d"), newVolumeSize);
	UE_LOG(LogTemp, Warning, TEXT("!VOLUME size: %d"), newVolumeSize);
	//int32 nodeAmount = 0;

	int32 na = GetNodeAmountInLayer(1);
	for (int i = 0; i < na; i++)
	{
		FVector position;
		//int32 numNode = xi + yi + zi;
		GetNodePosition(nodeSize, i, position);

		//UE_LOG(LogTemp, Warning, TEXT("My integer value is: %d"), nodeAmount);
		//UE_LOG(LogTemp, Warning, TEXT("My POSITION is: %s"), *position.ToString());
		if (CheckCollisionOverlap(position, ECC_WorldStatic, nodeSize))
		{
			//uint64 mCode = morton3D_64_encode(xi, yi, zi);
			//uint64 mCode = morton3D_64_encode(position.X, position.Y, position.Z);
			//uint32_t mCode = morton3D_32_encode(xi, yi, zi);
			//uint32_t mCode = morton3D_64_encode(xi, yi, zi);

			//octD.mortonCodes.Add(mCode);
			octD.mortonCodes[0].Add(i);
			//UE_LOG(LogTemp, Warning, TEXT("My POSITION is: %s"), *position.ToString());
			//UE_LOG(LogTemp, Warning, TEXT("My codes amount are: %d"), octD.mortonCodes.Num());
			//if (world)
			//{
			//	//DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, GetColorAt(1), true, -1.0f, 0, 2.0f);
			//	DrawDebugBox(world, position, FVector(nodeSize * 0.5f), FQuat::Identity, FColor::Black, true, -1.0f, 0, 2.0f);
			//}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("FINAL: My codes amount are: %d"), octD.mortonCodes.Num());
}

bool ADFNBoundingVolume::CheckCollisionOverlap(const FVector& position, ECollisionChannel colChannel, const float voxelSize)
{
	bool occupied = GetWorld()->OverlapAnyTestByChannel(position, FQuat::Identity, colChannel, FCollisionShape::MakeBox(FVector(voxelSize * 0.5f)));
	return occupied;
}

void ADFNBoundingVolume::Generate()
{
	octD.Reset();//Clear data upon generation

	//first pass rasterizer.
	RasterizeFirstLayer();

	//Creates for each layer, morton codes
	CreateLayerMortonCodes();

	//clear leafnodes
	//allocate leafnodes
	//octD.leafNodes.Empty();

	octD.leafNodes.AddDefaulted(octD.mortonCodes.Num() * 8);

	for (int i = 0; i < octD.NumberLayers; i++)
	{
		//allocate amount of layers into octree
		octD.layers.Emplace();
	}

	//for (int i = 0; i < octD.NumberLayers - 1; i++)
	for (int i = 0; i < octD.NumberLayers; i++)
	{
		//Rasterrize all layers(down to up)
		RasterizeLayer(i);
	}

	for (int i = octD.NumberLayers - 2; i >= 0; i--)
	{
		//creating neighbore links(up to down)
	}
}

const FColor& ADFNBoundingVolume::GetColorAt(int32 index) const
{
	if (layerColors.IsValidIndex(index))
	{
		return layerColors[index];
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Index out of range : layerColors"));
		return FColor::Black;
	}
}

void ADFNBoundingVolume::CreateLayerMortonCodes()
{
	int index = 0;

	while (octD.mortonCodes[index].Num() > 1)
	{
		// Add a new morton code layer
		octD.mortonCodes.Emplace();
		// Add any parent morton codes to the new layer
		for (uint64& code : octD.mortonCodes[index])
		{
			octD.mortonCodes[index + 1].Add(code >> 3);
		}
		index++;
	}
}
