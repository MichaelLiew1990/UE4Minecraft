// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Block.generated.h"

UENUM(BlueprintType)
enum class BlockType : uint8
{
	Grass,//方块
	Glass,
	Gravel,
	Soil,
	PinkFlower,//植物
	OrangeFlower,
	SmallGrass,
	TallGrass,
	Wheat
};

UCLASS()
class UE4MINECRAFT_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlock();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	UStaticMeshComponent* SM_Block;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	BlockType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	class USoundBase* BreakSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	class USoundBase* DestroySound;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float Resistance;

	//返回是否摧毁
	bool Break(float Demage, class UParticleSystem* Particle, FVector ParticleLocation, class UParticleSystem* ParticleBoom);

	void OnBroken(class UParticleSystem* ParticleBoom);

	float CurrentLife;
};
