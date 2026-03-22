// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "TimeClockSubsystem.h"

class STimeClockHome : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimeClockHome) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
	
	void RefreshProjectData();
private:
	FText GetSessionTimeText() const;
	const FSlateBrush* GetThumbnailFromProject();

	TSharedPtr<class SImage> ProjectThumbnail;
	TSharedPtr<class FDeferredCleanupSlateBrush> DefferedBrush;
	FTimeClockData CurrentProjectData;
};
