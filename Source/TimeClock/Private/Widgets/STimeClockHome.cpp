// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "Widgets/STimeClockHome.h"

#include "ImageUtils.h"
#include "TimeClock.h"
#include "TimeClockBPLibrary.h"
#include "TimeClockStyle.h"

#include "Brushes/SlateImageBrush.h"
#include "Misc/App.h"
#include "Misc/Paths.h"
#include "Slate/DeferredCleanupSlateBrush.h"
#include "Styling/SlateStyle.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"

void STimeClockHome::Construct(const FArguments& InArgs)
{
	RefreshProjectData();

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			
			// Project Thumbnail
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(10.0f)
			[
				SNew(SBox)
				.WidthOverride(128.0f)
				.HeightOverride(128.0f)
				[
					SAssignNew(ProjectThumbnail, SImage) 
					.Image(GetThumbnailFromProject())
				]
			]

			// Project info
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor::White * 0.2f)
				.Padding(10.0f)
				[
					SNew(SVerticalBox)

					// Project Title
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.0f, 10.0f, 0.0f, 10.0f))
					[
						SNew(STextBlock)
						.Text_Lambda([this]() { return FText::FromString(CurrentProjectData.ProjectName); })
						.Justification(ETextJustify::Center)
						.TextStyle(&FTimeClockStyle::Get(), "TimeClock.TitleText")
					]
					
					// Get Total time
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0.0f, 0.0f, 0.0f, 10.0f))
					[
						SNew(STextBlock)
						.Text_Lambda([this]() { return FText::FromString(CurrentProjectData.GetTotalTimeString()); })
						.Justification(ETextJustify::Center)
						.TextStyle(&FTimeClockStyle::Get(), "TimeClock.TitleText")
					]
					
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SSeparator)
						.Orientation(Orient_Horizontal)
					]
					
					// Get current session time
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 5.0f)
					[
						SNew(STextBlock)
						.Text_Lambda([this]() { return GetSessionTimeText(); })
						.Justification(ETextJustify::Center)
						.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
					]
					
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SSeparator)
						.Orientation(Orient_Horizontal)
					]

					// Get TimeSpentToday
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 5.0f)
					[
						SNew(STextBlock)
						.Text_Lambda([this]() { return FText::FromString("Today: " + CurrentProjectData.GetTimeTodayString()); })
						.Justification(ETextJustify::Center)
						.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
					]
					
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SSeparator)
						.Orientation(Orient_Horizontal)
					]

					// Get Start date
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 5.0f)
					[
						SNew(STextBlock)
						.Text_Lambda([this]() { return FText::FromString("Started on: " + CurrentProjectData.GetStartDateString()); })
						.Justification(ETextJustify::Center)
						.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
					]
				]
			]
		]
	];

}

void STimeClockHome::RefreshProjectData()
{
	if (UTimeClockBPLibrary::GetTimeClockSubsystem())
	{
		CurrentProjectData = UTimeClockBPLibrary::GetTimeClockSubsystem()->ActiveProjectData;
		
		if (ProjectThumbnail.IsValid())
		{
			ProjectThumbnail->SetImage(GetThumbnailFromProject());
		}
	}
	else
	{
		UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Current project cannot be displayed."))
		CurrentProjectData.ProjectName = "Ooops! If you see this, please let me know.";
	}
}

FText STimeClockHome::GetSessionTimeText() const
{
	 if (!UTimeClockBPLibrary::GetTimeClockSubsystem())
	 {
	     return FText::GetEmpty();
	 }
    
	FTimespan SessionTime = FDateTime::Now() - UTimeClockBPLibrary::GetTimeClockSubsystem()->ActiveSessionStartDate;
	int32 Hours = (int32)SessionTime.GetTotalHours();
	int32 Minutes = SessionTime.GetMinutes();
	
	return FText::Format(NSLOCTEXT("TimeClock", "SessionTime", "Current Session: {0}h {1}m"), Hours, Minutes);
}

const FSlateBrush* STimeClockHome::GetThumbnailFromProject()
{	
 if (UTexture2D* ThumbnailTexture = UTimeClockBPLibrary::GetTimeClockSubsystem()->ActiveProjectThumbnail)
 {
		if (!DefferedBrush.IsValid())
		{
			DefferedBrush = FDeferredCleanupSlateBrush::CreateBrush(ThumbnailTexture, FVector2D(ThumbnailTexture->GetSizeX(), ThumbnailTexture->GetSizeY()));
		}
		
		const FSlateBrush* Brush = DefferedBrush->TrySlateBrush(DefferedBrush);
		if (Brush)
		{
			return Brush;
		}
	}
	
	// Default thumbnail if the project doesn't have one
	return FTimeClockStyle::Get().GetBrush("TimeClock.DefaultIcon128");
}
