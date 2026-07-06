// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "TimeClockSubsystem.h"
#include "TimeClock.h"
#include "TimeClockBPLibrary.h"

#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Input/SComboBox.h"

class STimeClockAddAlarmDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimeClockAddAlarmDialog) {}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow);
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs)
	{
		ParentWindow = InArgs._ParentWindow;
		AlarmData.AlarmTime = FDateTime::Now() + FTimespan::FromHours(1);
		AlarmData.AlarmText = "New Alarm";

		for (int32 i = 0; i < 24; ++i) HoursOptions.Add(MakeShareable(new int32(i)));
		for (int32 i = 0; i < 60; i += 5) MinutesOptions.Add(MakeShareable(new int32(i)));

		SelectedHour = HoursOptions[1];
		SelectedMinute = MinutesOptions[0];

		ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)

				// Alarm text Title
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(FText::FromString("Alarm Label:"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
				]

				// Alarm text Content
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				[
					SNew(SBox)
					.WidthOverride(200.0f)
					.HeightOverride(30.0f)
					[
						SNew(SBorder)
						.BorderImage(FAppStyle::Get().GetBrush("Menu.Background"))
						.Padding(2.0f)
						[
							SNew(SEditableText)
							.Text(FText::FromString(AlarmData.AlarmText))
							.Justification(ETextJustify::Center)
							.OnTextCommitted_Lambda([this](const FText& InText, ETextCommit::Type InCommitType) {
								AlarmData.AlarmText = InText.ToString();
							})
						]
					]
				]

				// Alarm time Title
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 15.0f, 0.0f, 5.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Time:"))
					.Font(FAppStyle::Get().GetFontStyle("NormalFontBold"))
				]

				// Alarm time Content
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(0.0f, 5.0f)
				[
					SNew(SHorizontalBox)
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
								SAssignNew(HoursComboBox, SComboBox<TSharedPtr<int32>>)
								.OptionsSource(&HoursOptions)
								.OnGenerateWidget_Lambda([](TSharedPtr<int32> InItem) {
									return SNew(STextBlock).Text(FText::AsNumber(*InItem));
								})
								.OnSelectionChanged_Lambda([this](TSharedPtr<int32> InItem, ESelectInfo::Type InSelectInfo) {
									SelectedHour = InItem;
									UpdateSelectedTime();
								})
								.InitiallySelectedItem(SelectedHour)
								[
									SNew(STextBlock).Text_Lambda([this]() { return FText::AsNumber(*SelectedHour); })
								]
							]
						]
					]
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
								SAssignNew(MinutesComboBox, SComboBox<TSharedPtr<int32>>)
								.OptionsSource(&MinutesOptions)
								.OnGenerateWidget_Lambda([](TSharedPtr<int32> InItem) {
									return SNew(STextBlock).Text(FText::AsNumber(*InItem));
								})
								.OnSelectionChanged_Lambda([this](TSharedPtr<int32> InItem, ESelectInfo::Type InSelectInfo) {
									SelectedMinute = InItem;
									UpdateSelectedTime();
								})
								.InitiallySelectedItem(SelectedMinute)
								[
									SNew(STextBlock).Text_Lambda([this]() { return FText::AsNumber(*SelectedMinute); })
								]
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

				// Confirm button
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
						.Text(FText::FromString("Add"))
						.OnClicked(this, &STimeClockAddAlarmDialog::OnAddClicked)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.Text(FText::FromString("Cancel"))
						.OnClicked(this, &STimeClockAddAlarmDialog::OnCancelClicked)
					]
				]
			]
		];
	}

private:
	FReply OnAddClicked()
	{
		UpdateSelectedTime();
		
  UTimeClockSubsystem* TimeClockAlarm = UTimeClockBPLibrary::GetTimeClockSubsystem();
		if (!TimeClockAlarm)
		{
			UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Could not create new alarm."))
			return FReply::Handled();
		}
		
		TimeClockAlarm->AddAlarm(AlarmData);
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

	void UpdateSelectedTime()
	{
		if (SelectedHour.IsValid() && SelectedMinute.IsValid())
		{
			FDateTime Now = FDateTime::Now();
			AlarmData.AlarmTime = FDateTime(Now.GetYear(), Now.GetMonth(), Now.GetDay(), *SelectedHour, *SelectedMinute, 0);
		}
	}

	FTimeClockAlarmData AlarmData;
	TSharedPtr<SWindow> ParentWindow;

	TSharedPtr<SComboBox<TSharedPtr<int32>>> HoursComboBox;
	TSharedPtr<SComboBox<TSharedPtr<int32>>> MinutesComboBox;

	TArray<TSharedPtr<int32>> HoursOptions;
	TArray<TSharedPtr<int32>> MinutesOptions;
	TSharedPtr<int32> SelectedHour;
	TSharedPtr<int32> SelectedMinute;
};
