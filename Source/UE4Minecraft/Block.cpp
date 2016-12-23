//// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Minecraft.h"
#include "Block.h"


ABlock::ABlock()
{
	SM_Block = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));

	Resistance = 20.f;
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();
	CurrentLife = Resistance;
}

bool ABlock::Break(float Demage, class UParticleSystem* Particle, FVector ParticleLocation, class UParticleSystem* ParticleBoom)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Break-" + GetName()));

	CurrentLife -= Demage;

	if (BreakSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreakSound, GetActorLocation());
	}

	if (Particle != NULL)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, Particle, ParticleLocation);
	}

	UMaterialInstanceDynamic* MatInstance = SM_Block->CreateDynamicMaterialInstance(0);
	if (MatInstance != nullptr)
	{
		MatInstance->SetScalarParameterValue(FName("CrackingValue"), (CurrentLife>0.f ? CurrentLife : 0.f) / Resistance);
	}

	if (CurrentLife <= 0.f)
	{
		OnBroken(ParticleBoom);
		return true;
	}

	return false;
}

void ABlock::OnBroken(class UParticleSystem* ParticleBoom)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleBoom, GetActorLocation());
	Destroy();
}
