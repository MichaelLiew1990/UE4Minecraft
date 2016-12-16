// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "UE4MinecraftGameMode.generated.h"

UENUM(BlueprintType)
enum class EHUDState : uint8
{
	HS_InGame,
	HS_Inventory,
	HS_CraftMenu
};

UCLASS(minimalapi)
class AUE4MinecraftGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUE4MinecraftGameMode();

	virtual void BeginPlay() override;

	void ApplyHUDChanges();

	EHUDState GetHUDState();

	UFUNCTION(BlueprintCallable, Category = "HUD Functions")
	void ChangeHUDState(EHUDState NewState);

	bool ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool ShowMouseCursor, bool EnableClickEvent);

protected:
	EHUDState HUDState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BlueprintWidgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> InGameHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BlueprintWidgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> InventoryHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BlueprintWidgets", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> CraftMenuHUDClass;

	class UUserWidget* CurrentWidget;
};



