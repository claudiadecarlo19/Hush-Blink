#include "InGameUI.h"

#include "TeleportStealthCharacter.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UInGameUI::NativeConstruct()
{
    Super::NativeConstruct();
    UpdateValues();
}


void UInGameUI::NativeTick(
    const FGeometry& MyGeometry,
    float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdateTeleportCooldown();
}


void UInGameUI::UpdateValues()
{
    if (!Player)
        return;

    // HEALTH

    if (HealthBar)
    {
        const float HealthPercent =
            Player->MaxHealth > 0.f
                ? Player->CurrentHealth / Player->MaxHealth
                : 0.f;

        HealthBar->SetPercent(FMath::Clamp(HealthPercent, 0.f, 1.f));
    }

    if (HealthText)
    {
        HealthText->SetText(
            FText::FromString(
                FString::Printf(TEXT("%.0f / %.0f"),Player->CurrentHealth,Player->MaxHealth)
            )
        );
    }

    // LIVES

    SetIconAvailable(LifeIcon1,Player->RemainingLives >= 1);
    SetIconAvailable(LifeIcon2,Player->RemainingLives >= 2);
    SetIconAvailable(LifeIcon3,Player->RemainingLives >= 3);

    // THROWABLES

    SetIconAvailable(ThrowableIcon1,Player->CurrentThrowableCount >= 1);
    SetIconAvailable(ThrowableIcon2,Player->CurrentThrowableCount >= 2);
    SetIconAvailable(ThrowableIcon3,Player->CurrentThrowableCount >= 3);

    // OBJECTIVES

    if (ObjectiveText)
    {
        ObjectiveText->SetText(
            FText::FromString(
                FString::Printf(TEXT("Loot: %d / %d"),Player->CollectedObjectiveCount,Player->RequiredObjectiveCount)
            )
        );
    }

    if (ObjectiveProgressBar)
    {
        const float ObjectivePercent =
            Player->RequiredObjectiveCount > 0
                ? static_cast<float>(Player->CollectedObjectiveCount) /static_cast<float>(Player->RequiredObjectiveCount)
                : 0.f;

        ObjectiveProgressBar->SetPercent(FMath::Clamp(ObjectivePercent, 0.f, 1.f));
    }

    UpdateTeleportCooldown();
}


void UInGameUI::UpdateTeleportCooldown()
{
    if (!Player)
        return;

    const float RemainingTime =Player->GetTeleportCooldownRemaining();

    const bool bTeleportReady = Player->bCanTeleport;

    if (TeleportIcon)
    {
        TeleportIcon->SetRenderOpacity(bTeleportReady ? 1.f : 0.35f);
    }

    /*if (TeleportCooldownBar)
    {
        float CooldownPercent = 1.f;

        if (!bTeleportReady && Player->TeleportCooldown > 0.f)
        {
            CooldownPercent =1.f -RemainingTime /Player->TeleportCooldown;
        }

        TeleportCooldownBar->SetPercent(FMath::Clamp(CooldownPercent, 0.f, 1.f));
    }*/

    if (TeleportCooldownText)
    {
        if (bTeleportReady)
        {
            TeleportCooldownText->SetText(FText::FromString(TEXT("READY")));
        }
        else
        {
            TeleportCooldownText->SetText(
                FText::FromString(
                    FString::Printf(TEXT("%.0f"),FMath::CeilToFloat(RemainingTime))
                )
            );
        }
    }
}


void UInGameUI::SetIconAvailable(UImage* Icon,bool bAvailable)
{
    if (!Icon)
        return;
    
    Icon->SetRenderOpacity(bAvailable ? 1.f : 0.2f);
}

