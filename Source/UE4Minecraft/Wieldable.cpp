// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4Minecraft.h"
#include "UE4MinecraftCharacter.h"

static int WieldUniqueID = 0;
// Sets default values
AWieldable::AWieldable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PickupTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupTrigger"));
	PickupTrigger->bGenerateOverlapEvents = true;
	PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &AWieldable::OnRadiusEnter);
	//PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//PickupTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	WieldableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WieldableMesh"));
	WieldableMesh->SetupAttachment(PickupTrigger);
	WieldableMesh->SetRelativeRotation(FRotator(0.f, 0.f, -45.f));

	MaterialType = EMaterial::None;
	ToolType = ETool::Unarmed;

	bIsActive = true;

	WieldUniqueID += 1;
	UniqueID = WieldUniqueID;
}

// Called every frame
void AWieldable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator rotation = WieldableMesh->GetComponentRotation();
	rotation.Yaw += 1.f;
	WieldableMesh->SetRelativeRotation(rotation);
}

UTexture2D* AWieldable::GetThumbnail()
{
	return PickupThumbnail;
}

int AWieldable::GetUniqueID()
{
	return UniqueID;
}

void AWieldable::SetUniqueID(int id)
{
	UniqueID = id;
}

void AWieldable::OnRadiusEnter(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AUE4MinecraftCharacter>(OtherActor) == NULL || !bIsActive)
	{
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("´¥Åö"));

	AUE4MinecraftCharacter* Character = Cast<AUE4MinecraftCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Character->FP_Gun->SetStaticMesh(WieldableMesh->StaticMesh);
	Character->AddItemToInventory(this);

	Hide(true);
}

void AWieldable::Hide(bool bIsHide)
{
	WieldableMesh->SetVisibility(!bIsHide);
	bIsActive = !bIsHide;

	if (bIsActive)
	{
		//PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//PickupTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
		PickupTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	}
	else
	{
		//PickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		PickupTrigger->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	}
}

void AWieldable::OnUsed()
{
	Destroy();
}
