// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class UE4MINECRAFT_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* SM_Block;
	
	uint8 MinimumMaterial;

	UPROPERTY(EditDefaultsOnly)
	float Resistance;

	UPROPERTY(BlueprintReadWrite)
	float BreakingStage;

	void Break();

	void ResetBlock();

	void OnBroken(bool HasRequiredPickaxe);
};
