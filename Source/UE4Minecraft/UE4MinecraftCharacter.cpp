// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4Minecraft.h"
#include "UE4MinecraftCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AUE4MinecraftCharacter

AUE4MinecraftCharacter::AUE4MinecraftCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//自定义一些数值
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 100.f;
	GetCharacterMovement()->MaxStepHeight = 0.f;
	GetCharacterMovement()->SetWalkableFloorAngle(0.f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetVisibility(false);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	FP_Gun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	Reach = 300.f;

	Inventory.SetNum(NUM_OF_INVENTORY_SLOTS);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
}

void AUE4MinecraftCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	FP_Gun->RelativeLocation = FVector(-5.f, -5.f, 15.f);
	FP_Gun->RelativeRotation = FRotator(150.f, 90.f, 90.f);
	FP_Gun->RelativeScale3D = FVector(0.4f, 0.4f, 0.4f);
}

void AUE4MinecraftCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckForBlock();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUE4MinecraftCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("InventoryUp", IE_Pressed, this, &AUE4MinecraftCharacter::MoveUpInventorySlot);
	PlayerInputComponent->BindAction("InventoryDown", IE_Pressed, this, &AUE4MinecraftCharacter::MoveDownInventorySlot);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AUE4MinecraftCharacter::Throw);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AUE4MinecraftCharacter::OnHit);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AUE4MinecraftCharacter::EndHit);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUE4MinecraftCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUE4MinecraftCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AUE4MinecraftCharacter::MouseTurn);
	PlayerInputComponent->BindAxis("LookUp", this, &AUE4MinecraftCharacter::MouseLookUp);
}

int AUE4MinecraftCharacter::GetCurrentInventorySlot()
{
	return CurrentInventorySlot;
}

bool AUE4MinecraftCharacter::AddItemToInventory(AWieldable * Item)
{
	if (Item != NULL)
	{
		const int32 AvailableSlot = Inventory.Find(nullptr);
		if (AvailableSlot != INDEX_NONE)
		{
			Inventory[AvailableSlot] = Item;
			CurrentInventorySlot = AvailableSlot;
			UpdateWieldableItem();
			return true;
		}
	}
	return false;
}

UTexture2D * AUE4MinecraftCharacter::GetThumbnailAtInventorySlot(int Slot)
{
	if (Inventory[Slot] != NULL)
	{
		return Inventory[Slot]->PickupThumbnail;
	}
	return nullptr;
}

void AUE4MinecraftCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AUE4MinecraftCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AUE4MinecraftCharacter::MouseTurn(float v)
{
	//if (Cast<AUE4MinecraftGameMode>(GetWorld()->GetAuthGameMode())->GetHUDState() != EHUDState::HS_ToolBar) return;
	AddControllerYawInput(v);
}

void AUE4MinecraftCharacter::MouseLookUp(float v)
{
	//if (Cast<AUE4MinecraftGameMode>(GetWorld()->GetAuthGameMode())->GetHUDState() != EHUDState::HS_ToolBar) return;
	AddControllerPitchInput(v);
}

void AUE4MinecraftCharacter::UpdateWieldableItem()
{
	Inventory[CurrentInventorySlot] != NULL ?
		FP_Gun->SetStaticMesh(Inventory[CurrentInventorySlot]->WieldableMesh->StaticMesh) : FP_Gun->SetStaticMesh(ArmMesh);
	if (Inventory[CurrentInventorySlot] && (int8)(Inventory[CurrentInventorySlot]->ToolType) >= (int8)(ETool::CreateGrass))
	{
		FP_Gun->RelativeScale3D = FVector(0.1f, 0.1f, 0.1f);
	}
	else
	{
		FP_Gun->RelativeScale3D = FVector(0.4f, 0.4f, 0.4f);
	}
}

AWieldable* AUE4MinecraftCharacter::GetCurrentWieldedItem()
{
	return Inventory[CurrentInventorySlot] != NULL ? Inventory[CurrentInventorySlot] : nullptr;
}

void AUE4MinecraftCharacter::Throw()
{
	//if (Cast<AUE4MinecraftGameMode>(GetWorld()->GetAuthGameMode())->GetHUDState() != EHUDState::HS_ToolBar) return;
	AWieldable* ItemToThrow = GetCurrentWieldedItem();
	if (ItemToThrow == NULL) return;

	FHitResult LinetraceHit;
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = StartTrace + (FirstPersonCameraComponent->GetForwardVector() * Reach);
	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);
	CQP.AddIgnoredActor(ItemToThrow);

	GetWorld()->LineTraceSingleByChannel(LinetraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CQP);
	AActor* OtherActor = LinetraceHit.GetActor();
	if (OtherActor != NULL)
	{
		FVector DropLocation = EndTrace;
		AWieldable* ItemToPickup = Cast<AWieldable>(OtherActor);
		//扔在其它物品上
		if (ItemToPickup != NULL && ItemToPickup->bIsActive)
		{
			DropLocation = ItemToPickup->GetActorLocation();
			Inventory[CurrentInventorySlot] = ItemToPickup;
			ItemToPickup->Hide(true);
		}
		else//扔在空地上
		{
			DropLocation = LinetraceHit.ImpactPoint + (LinetraceHit.ImpactNormal*20.f);
			Inventory[CurrentInventorySlot] = NULL;
		}
		ItemToThrow->SetActorLocationAndRotation(DropLocation, FRotator::ZeroRotator);
		ItemToThrow->Hide(false);
	}

	UpdateWieldableItem();
}

void AUE4MinecraftCharacter::MoveUpInventorySlot()
{
	CurrentInventorySlot = FMath::Abs((CurrentInventorySlot + 1) % NUM_OF_INVENTORY_SLOTS);
	UpdateWieldableItem();
}

void AUE4MinecraftCharacter::MoveDownInventorySlot()
{
	if (CurrentInventorySlot == 0)
	{
		CurrentInventorySlot = NUM_OF_INVENTORY_SLOTS - 1;
		UpdateWieldableItem();
		return;
	}
	CurrentInventorySlot = FMath::Abs((CurrentInventorySlot - 1) % NUM_OF_INVENTORY_SLOTS);
	UpdateWieldableItem();
}

void AUE4MinecraftCharacter::OnHit()
{
	//if (Cast<AUE4MinecraftGameMode>(GetWorld()->GetAuthGameMode())->GetHUDState() != EHUDState::HS_ToolBar) return;
	PlayHitAnim();

	if (GetCurrentWieldedItem() && (int8)(GetCurrentWieldedItem()->ToolType) >= (int8)(ETool::CreateGrass))
	{
		FHitResult LinetraceHit;
		FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
		FVector EndTrace = StartTrace + FirstPersonCameraComponent->GetForwardVector() * Reach;
		FCollisionQueryParams CQP;
		CQP.AddIgnoredActor(this);
		CQP.AddIgnoredActor(GetCurrentWieldedItem());
		GetWorld()->LineTraceSingleByChannel(LinetraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CQP);

		AActor* OtherActor = LinetraceHit.GetActor();
		ABlock* OtherBlock = Cast<ABlock>(OtherActor);
		if (OtherBlock != NULL)
		{
			GetWorld()->SpawnActor<ABlock>(GetCurrentWieldedItem()->BlockClass,
				OtherActor->GetActorLocation() + (LinetraceHit.ImpactNormal * 100.f), FRotator(0.f, 0.f, 0.f), FActorSpawnParameters());
		}
	}
	else
	{
		ABlock* CurrentBlock = Cast<ABlock>(CurrentHitItem);
		if (CurrentBlock != nullptr)
		{
			bIsBreaking = true;

			float TimeBetweenBreaks = ((CurrentBlock->Resistance) / 100.f) / 2.f;
			GetWorld()->GetTimerManager().SetTimer(BlockBreakingHandle, this, &AUE4MinecraftCharacter::BreakBlock, TimeBetweenBreaks, true);
			GetWorld()->GetTimerManager().SetTimer(HitAnimHandle, this, &AUE4MinecraftCharacter::PlayHitAnim, 0.4f, true);
		}
	}
}

void AUE4MinecraftCharacter::EndHit()
{
	//if (Cast<AUE4MinecraftGameMode>(GetWorld()->GetAuthGameMode())->GetHUDState() != EHUDState::HS_ToolBar) return;
	if (GetCurrentWieldedItem() && (int8)(GetCurrentWieldedItem()->ToolType) >= (int8)(ETool::CreateGrass))
	{
		//
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(BlockBreakingHandle);
		GetWorld()->GetTimerManager().ClearTimer(HitAnimHandle);

		bIsBreaking = false;

		ABlock* CurrentBlock = Cast<ABlock>(CurrentHitItem);
		if (CurrentBlock != nullptr)
		{
			CurrentBlock->ResetBlock();
		}
	}
}

void AUE4MinecraftCharacter::PlayHitAnim()
{
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AUE4MinecraftCharacter::CheckForBlock()
{
	FHitResult LinetraceHit;

	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector EndTrace = StartTrace + FirstPersonCameraComponent->GetForwardVector() * Reach;

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(LinetraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CQP);

	ABlock* PotentialBlock = Cast<ABlock>(LinetraceHit.GetActor());
	AWieldable* PotentialWield = Cast<AWieldable>(LinetraceHit.GetActor());

	ABlock* LastBlock = Cast<ABlock>(CurrentHitItem);
	AWieldable* LastWield = Cast<AWieldable>(CurrentHitItem);

	//还原之前高亮
	if (LastBlock != nullptr)
	{
		LastBlock->SM_Block->SetRenderCustomDepth(false);
	}
	if (LastWield != nullptr)
	{
		LastWield->WieldableMesh->SetRenderCustomDepth(false);
	}

	//设置当前
	if (LinetraceHit.GetActor() == NULL)
	{
		CurrentHitItem = nullptr;
	}
	else
	{
		CurrentHitItem = LinetraceHit.GetActor();
	}

	//启动当前高亮
	if (PotentialBlock != nullptr)
	{
		PotentialBlock->SM_Block->SetRenderCustomDepth(true);
	}
	if (PotentialWield != nullptr)
	{
		PotentialWield->WieldableMesh->SetRenderCustomDepth(true);
	}
}

void AUE4MinecraftCharacter::BreakBlock()
{
	ABlock* CurrentBlock = Cast<ABlock>(CurrentHitItem);
	if (bIsBreaking && CurrentBlock != nullptr && !CurrentBlock->IsPendingKill())
	{
		CurrentBlock->Break();
	}
}
