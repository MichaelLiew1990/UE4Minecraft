// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4Minecraft.h"
#include "UE4MinecraftCharacter.h"
#include "UE4MinecraftProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"

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

	Reach = 200.f;

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

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AUE4MinecraftCharacter::OnHit);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AUE4MinecraftCharacter::EndHit);

	PlayerInputComponent->BindAxis("MoveForward", this, &AUE4MinecraftCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AUE4MinecraftCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AUE4MinecraftCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AUE4MinecraftCharacter::LookUpAtRate);
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
			return true;
		}
	}
	return false;
}

UTexture2D * AUE4MinecraftCharacter::GetThumbnailAtInventorySlot(uint8 Slot)
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

void AUE4MinecraftCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUE4MinecraftCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUE4MinecraftCharacter::MoveUpInventorySlot()
{
	CurrentInventorySlot = FMath::Abs((CurrentInventorySlot + 1) % NUM_OF_INVENTORY_SLOTS);
}

void AUE4MinecraftCharacter::MoveDownInventorySlot()
{
	if (CurrentInventorySlot == 0)
	{
		CurrentInventorySlot = NUM_OF_INVENTORY_SLOTS - 1;
		return;
	}
	CurrentInventorySlot = FMath::Abs((CurrentInventorySlot - 1) % NUM_OF_INVENTORY_SLOTS);
}

void AUE4MinecraftCharacter::OnHit()
{
	PlayHitAnim();

	if (CurrentBlock != nullptr)
	{
		bIsBreaking = true;

		float TimeBetweenBreaks = ((CurrentBlock->Resistance) / 100.f) / 2.f;
		GetWorld()->GetTimerManager().SetTimer(BlockBreakingHandle, this, &AUE4MinecraftCharacter::BreakBlock, TimeBetweenBreaks, true);
		GetWorld()->GetTimerManager().SetTimer(HitAnimHandle, this, &AUE4MinecraftCharacter::PlayHitAnim, 0.4f, true);
	}
}

void AUE4MinecraftCharacter::EndHit()
{
	GetWorld()->GetTimerManager().ClearTimer(BlockBreakingHandle);
	GetWorld()->GetTimerManager().ClearTimer(HitAnimHandle);

	bIsBreaking = false;

	if (CurrentBlock != nullptr)
	{
		CurrentBlock->ResetBlock();
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

	GetWorld()->LineTraceSingleByChannel(LinetraceHit, StartTrace, EndTrace, ECollisionChannel::ECC_WorldDynamic, CQP);

	ABlock* PotentialBlock = Cast<ABlock>(LinetraceHit.GetActor());

	if (PotentialBlock == NULL)
	{
		CurrentBlock = nullptr;
		return;
	}
	else
	{
		CurrentBlock = PotentialBlock;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, *CurrentBlock->GetName());
	}
}

void AUE4MinecraftCharacter::BreakBlock()
{
	if (bIsBreaking && CurrentBlock != nullptr && !CurrentBlock->IsPendingKill())
	{
		CurrentBlock->Break();
	}
}
