// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

class ATeleportStealthCharacter;
class UProgressBar;
class UTextBlock;
class UImage;

UCLASS()
class TELEPORTSTEALTH_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ATeleportStealthCharacter* Player;

	UFUNCTION()
	void UpdateValues();

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

	// HEALTH

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	// LIVES

	UPROPERTY(meta = (BindWidget))
	UImage* LifeIcon1;

	UPROPERTY(meta = (BindWidget))
	UImage* LifeIcon2;

	UPROPERTY(meta = (BindWidget))
	UImage* LifeIcon3;

	// TELEPORT

	UPROPERTY(meta = (BindWidget))
	UImage* TeleportIcon;

	//UPROPERTY(meta = (BindWidget))
	//UProgressBar* TeleportCooldownBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeleportCooldownText;

	// THROWABLES

	UPROPERTY(meta = (BindWidget))
	UImage* ThrowableIcon1;

	UPROPERTY(meta = (BindWidget))
	UImage* ThrowableIcon2;

	UPROPERTY(meta = (BindWidget))
	UImage* ThrowableIcon3;

	// OBJECTIVES

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ObjectiveText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ObjectiveProgressBar;

private:
	void UpdateTeleportCooldown();

	void SetIconAvailable(
		UImage* Icon,
		bool bAvailable
	);
};

