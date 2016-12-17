// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Minecraft.h"
#include "UE4MinecraftCharacter.h"
#include "Wieldable.h"


// Sets default values
AWieldable::AWieldable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupTrigger"));
	PickupTrigger->bGenerateOverlapEvents = true;
	PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWieldable::OnRadiusEnter);
	
	WieldableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WieldableMesh"));
	WieldableMesh->AttachToComponent(PickupTrigger, FAttachmentTransformRules::SnapToTargetIncludingScale);
	WieldableMesh->SetRelativeRotation(FRotator(0.f, 0.f, -45.f));

	MaterialType = EMaterial::None;
	ToolType = ETool::Unarmed;
}

// void AWieldable::BeginPlay()
// {
// 	
// }

// Called every frame
void AWieldable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FRotator rotation = WieldableMesh->GetComponentRotation();
	rotation.Yaw += 1.f;
	WieldableMesh->SetRelativeRotation(rotation);
}

void AWieldable::OnRadiusEnter(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsActive)
	{
		AUE4MinecraftCharacter* Character = Cast<AUE4MinecraftCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		Character->FP_Gun->SetStaticMesh(WieldableMesh->StaticMesh);
		Character->AddItemToInventory(this);

		OnPickedUp();
	}
}

void AWieldable::OnPickedUp()
{
	WieldableMesh->SetVisibility(false);
	bIsActive = false;
}

void AWieldable::OnUsed()
{
	Destroy();
}