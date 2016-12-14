// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "UE4MinecraftHUD.generated.h"

UCLASS()
class AUE4MinecraftHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUE4MinecraftHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

