// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Block.h"
#include "Wieldable.h"
#include "UE4MinecraftCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUE4MinecraftCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
public:
	AUE4MinecraftCharacter();

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UStaticMeshComponent* FP_Gun;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UFUNCTION(BlueprintPure, Category = HUD)
	int GetCurrentInventorySlot();

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItemToInventory(AWieldable* Item);

	UFUNCTION(BlueprintPure, Category = Inventory)
	UTexture2D* GetThumbnailAtInventorySlot(int Slot);

	ETool ToolType;
	EMaterial MaterialType;

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface


private:
	const int NUM_OF_INVENTORY_SLOTS = 8;

	int CurrentInventorySlot;

	void MoveUpInventorySlot();
	void MoveDownInventorySlot();

	bool bIsBreaking;

	void OnHit();
	void EndHit();

	void PlayHitAnim();

	void CheckForBlock();

	void BreakBlock();

	ABlock* CurrentBlock;

	float Reach;

	FTimerHandle BlockBreakingHandle;
	FTimerHandle HitAnimHandle;

	UPROPERTY(EditAnywhere)
	TArray<AWieldable*> Inventory;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

