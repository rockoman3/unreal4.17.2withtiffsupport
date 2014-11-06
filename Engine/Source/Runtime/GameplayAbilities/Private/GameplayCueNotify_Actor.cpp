// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilitySystemPrivatePCH.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayTagsModule.h"
#include "GameplayCueManager.h"
#include "AbilitySystemComponent.h"

AGameplayCueNotify_Actor::AGameplayCueNotify_Actor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	IsOverride = true;
}

#if WITH_EDITOR
void AGameplayCueNotify_Actor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->bAccelerationMapOutdated = true;
}
#endif

void AGameplayCueNotify_Actor::DeriveGameplayCueTagFromAssetName()
{
	// In the editor, attempt to infer GameplayCueTag from our asset name (if there is no valid GameplayCueTag already).
	#if WITH_EDITOR
	if (GIsEditor)
	{
		if (GameplayCueTag.IsValid() == false)
		{
			FString MyName = GetName();

			MyName.RemoveFromStart(TEXT("Default__"));
			MyName.RemoveFromEnd(TEXT("_c"));

			MyName.ReplaceInline(TEXT("_"), TEXT("."));

			if (!MyName.Contains(TEXT("GameplayCue")))
			{
				MyName = FString(TEXT("GameplayCue.")) + MyName;
			}

			IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
			GameplayCueTag = GameplayTagsModule.GetGameplayTagsManager().RequestGameplayTag(FName(*MyName), false);
		}
		GameplayCueName = GameplayCueTag.GetTagName();
	}
	#endif
}

void AGameplayCueNotify_Actor::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		DeriveGameplayCueTagFromAssetName();
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading())
	{
		DeriveGameplayCueTagFromAssetName();
	}
}

void AGameplayCueNotify_Actor::PostInitProperties()
{
	Super::PostInitProperties();
	DeriveGameplayCueTagFromAssetName();
}

bool AGameplayCueNotify_Actor::HandlesEvent(EGameplayCueEvent::Type EventType) const
{
	return true;
}

void AGameplayCueNotify_Actor::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, FGameplayCueParameters Parameters)
{
	if (MyTarget && !MyTarget->IsPendingKill())
	{
		K2_HandleGameplayCue(MyTarget, EventType, Parameters);

		switch (EventType)
		{
		case EGameplayCueEvent::OnActive:
			OnActive(MyTarget, Parameters);
			break;

		case EGameplayCueEvent::Executed:
			OnExecute(MyTarget, Parameters);
			break;

		case EGameplayCueEvent::Removed:
			OnRemove(MyTarget, Parameters);
			break;
		};
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("Null Target"));
	}
}

void AGameplayCueNotify_Actor::OnOwnerDestroyed()
{
	// May need to do extra cleanup in child classes
	Destroy();
	//MarkPendingKill();
}
