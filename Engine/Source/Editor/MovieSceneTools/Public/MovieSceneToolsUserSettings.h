// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "MovieSceneToolsUserSettings.generated.h"

UENUM()
enum class EThumbnailQuality : uint8
{
	Draft,
	Normal,
	Best,
};

UCLASS(config=EditorSettings)
class MOVIESCENETOOLS_API UMovieSceneUserThumbnailSettings : public UObject
{
public:
	UMovieSceneUserThumbnailSettings(const FObjectInitializer& Initializer);
	
	GENERATED_BODY()

	/** Whether to draw thumbnails or not */
	UPROPERTY(EditAnywhere, config, Category=General)
	bool bDrawThumbnails;

	/** Whether to draw a single thumbnail for this section or as many as can fit */
	UPROPERTY(EditAnywhere, config, Category=General, meta=(EditCondition=bDrawThumbnails))
	bool bDrawSingleThumbnails;

	/** Size at which to draw thumbnails on thumbnail sections */
	UPROPERTY(EditAnywhere, config, Category=General, meta=(ClampMin=1, ClampMax=1024, EditCondition=bDrawThumbnails))
	FIntPoint ThumbnailSize;

	/** Quality to render the thumbnails with */
	UPROPERTY(EditAnywhere, config, Category=General, meta=(EditCondition=bDrawThumbnails))
	EThumbnailQuality Quality;

	DECLARE_EVENT(UMovieSceneUserThumbnailSettings, FOnForceRedraw)
	FOnForceRedraw& OnForceRedraw() { return OnForceRedrawEvent; }
	void BroadcastRedrawThumbnails() const { OnForceRedrawEvent.Broadcast(); }

	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	FOnForceRedraw OnForceRedrawEvent;
};
