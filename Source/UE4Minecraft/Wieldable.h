// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Wieldable.generated.h"

UENUM(BlueprintType)
enum class ETool : uint8
{
	Unarmed,
	Pickaxe,
	Axe,
	Shovel,
	Sword,
	CreateGrass,//·½¿é
	CreateGlass,
	CreateGravel,
	CreateSoil,
	CreatePinkFlower,//Ö²Îï
	CreateOrangeFlower,
	CreateSmallGrass,
	CreateTallGrass,
	CreateWheat
};

UENUM(BlueprintType)
enum class EMaterial : uint8
{
	None = 1,
	Wooden = 2,
	Stone = 4,
	Iron = 6,
	Diamond = 8,
	Golden = 12
};

UCLASS(BlueprintType)
class UE4MINECRAFT_API AWieldable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWieldable();
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintPure, Category = Inventory)
	UTexture2D* GetThumbnail();

	UFUNCTION(BlueprintPure, Category = Inventory)
	int GetUniqueID();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void SetUniqueID(int id);

	UPROPERTY(EditAnywhere)
	ETool ToolType;

	UPROPERTY(EditAnywhere)
	EMaterial MaterialType;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WieldableMesh;

	UPROPERTY(EditAnywhere)
	UShapeComponent* PickupTrigger;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PickupThumbnail;

	UPROPERTY(EditAnywhere)
	int UniqueID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	TSubclassOf<class ABlock> BlockClass;

	bool bIsActive;

	UFUNCTION()
	void OnRadiusEnter(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Hide(bool bIsHide);

	void OnUsed();
};
