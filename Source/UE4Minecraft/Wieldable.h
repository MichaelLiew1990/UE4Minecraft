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
	Sword
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

UCLASS()
class UE4MINECRAFT_API AWieldable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWieldable();
	
/*	virtual void BeginPlay() override;*/

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere)
	ETool ToolType;

	UPROPERTY(EditAnywhere)
	EMaterial MaterialType;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* WieldableMesh;

	UShapeComponent* PickupTrigger;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PickupThumbnail;

	bool bIsActive;

	UFUNCTION()
	void OnRadiusEnter(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnPickedUp();

	void OnUsed();
};
