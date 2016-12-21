// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UE4Minecraft.h"
#include "UE4MinecraftGameMode.h"
#include "UE4MinecraftHUD.h"
#include "UE4MinecraftCharacter.h"

AUE4MinecraftGameMode::AUE4MinecraftGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/_Game/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AUE4MinecraftHUD::StaticClass();

	HUDState = EHUDState::HS_ToolBar;
}

void AUE4MinecraftGameMode::BeginPlay()
{
	Super::BeginPlay();

	ApplyHUDChanges();
}


void AUE4MinecraftGameMode::ApplyHUDChanges()
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromParent();
	}

	switch (HUDState)
	{
	case EHUDState::HS_ToolBar:
	{
		ApplyHUD(ToolBarHUDClass, false, false);
		break;
	}
	case EHUDState::HS_StoreHouse:
	{
		ApplyHUD(StoreHouseHUDClass, true, true);
		break;
	}
	default:
	{
		ApplyHUD(ToolBarHUDClass, false, false);
		break;
	}
	}
}

EHUDState AUE4MinecraftGameMode::GetHUDState()
{
	return HUDState;
}

void AUE4MinecraftGameMode::ChangeHUDState(EHUDState NewState)
{
	HUDState = NewState;
	ApplyHUDChanges();
}

class UUserWidget* AUE4MinecraftGameMode::GetCurrentWidget()
{
	return CurrentWidget;
}

bool AUE4MinecraftGameMode::ApplyHUD(TSubclassOf<class UUserWidget> WidgetToApply, bool ShowMouseCursor, bool EnableClickEvent)
{
	AUE4MinecraftCharacter* MyCharacter = Cast<AUE4MinecraftCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	APlayerController* MyController = GetWorld()->GetFirstPlayerController();

	if (WidgetToApply != nullptr)
	{
		MyController->bShowMouseCursor = ShowMouseCursor;
		MyController->bEnableClickEvents = EnableClickEvent;
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetToApply);

		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
			return true;
		}
	}

	return false;
}
