// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "TimeClockSubsystem.h"
#include "TimeClock.h"
#include "TimeClockBPLibrary.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SSpinBox.h"

class STimeClockEditProjectDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimeClockEditProjectDialog) {}
		SLATE_ARGUMENT(FTimeClockData, ProjectData);
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow);
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs)
	{
		ProjectData = InArgs._ProjectData;
		ParentWindow = InArgs._ParentWindow;

		if (ParentWindow.IsValid())
		{
			FString Title = FString::Printf(TEXT("Edit Project: %s"), *ProjectData.ProjectName);
			ParentWindow->SetTitle(FText::FromString(Title));
		}

		FTimespan TotalTime = FTimespan::FromSeconds(ProjectData.TotalSecondsSpent);
		int32 InitialHours = (int32)TotalTime.GetTotalHours();
		int32 InitialMinutes = TotalTime.GetMinutes();

		ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)

				// Start Date Title
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 15.0f, 0.0f, 5.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Start Date (dd/mm/yyyy):"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
				]

				// Start Date Input
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				[
					SNew(SBox)
					.WidthOverride(150.0f)
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::Get().GetBrush("Menu.Background"))
						.Padding(2.0f)
						[
							SNew(SEditableText)
							.Text(FText::FromString(ProjectData.StartDate.ToString(TEXT("%d/%m/%Y"))))
							.OnTextChanged_Lambda([this](const FText& InText) {
								FString DateString = InText.ToString();
								TArray<FString> DateParts;
								DateString.ParseIntoArray(DateParts, TEXT("/"), true);
								if (DateParts.Num() == 3)
								{
									int32 Day = FCString::Atoi(*DateParts[0]);
									int32 Month = FCString::Atoi(*DateParts[1]);
									int32 Year = FCString::Atoi(*DateParts[2]);

									if (FDateTime::Validate(Year, Month, Day, 0, 0, 0, 0))
									{
										ProjectData.StartDate = FDateTime(Year, Month, Day);
									}
								}
							})
						]
					]
				]

				// Time spent Title
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 15.0f, 0.0f, 5.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Total Time Spent:"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
				]

				// Sliders
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				[
					SNew(SHorizontalBox)

					// Slider hours
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SBox)
						.WidthOverride(60.0f)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::Get().GetBrush("Menu.Background"))
							.Padding(1.0f)
							[
								SNew(SSpinBox<int32>)
								.Value(InitialHours)
								.MinValue(0)
								.MaxSliderValue(999)
								.OnValueChanged_Lambda([this](int32 InValue) {
									FTimespan OldTime = FTimespan::FromSeconds(ProjectData.TotalSecondsSpent);
									ProjectData.TotalSecondsSpent = InValue * 3600 + OldTime.GetMinutes() * 60 + OldTime.GetSeconds();
								})
							]
						]
					]

					// Slider minutes
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(FText::FromString("h"))
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(10.0f, 0.0f, 0.0f, 0.0f)
					[
						SNew(SBox)
						.WidthOverride(60.0f)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::Get().GetBrush("Menu.Background"))
							.Padding(1.0f)
							[
								SNew(SSpinBox<int32>)
								.Value(InitialMinutes)
								.MinValue(0)
								.MaxValue(59)
								.OnValueChanged_Lambda([this](int32 InValue) {
									FTimespan OldTime = FTimespan::FromSeconds(ProjectData.TotalSecondsSpent);
									ProjectData.TotalSecondsSpent = (int32)OldTime.GetTotalHours() * 3600 + InValue * 60 + OldTime.GetSeconds();
								})
							]
						]
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(FText::FromString("m"))
					]
				]

				// Buttons
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 20.0f, 0.0f, 0.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(5.0f, 0.0f)
					[
						SNew(SButton)
						.Text(FText::FromString("Save"))
						.OnClicked(this, &STimeClockEditProjectDialog::OnSaveClicked)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Cancel"))
						.OnClicked(this, &STimeClockEditProjectDialog::OnCancelClicked)
					]
				]
			]
		];
	}

private:
	FReply OnSaveClicked()
	{
  UTimeClockSubsystem* TimeClockCore = UTimeClockBPLibrary::GetTimeClockSubsystem();
		if (!TimeClockCore)
		{
			UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Project data cannot be edited."))
			return FReply::Handled();
		}
		
		TimeClockCore->SaveProjectData(ProjectData);
		
		if (ParentWindow.IsValid())
		{
			ParentWindow->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	FReply OnCancelClicked()
	{
		if (ParentWindow.IsValid())
		{
			ParentWindow->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	FTimeClockData ProjectData;
	TSharedPtr<SWindow> ParentWindow;
};
