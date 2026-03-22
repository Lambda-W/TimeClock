// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "Widgets/STimeClockAlarmList.h"

#include "TimeClockBPLibrary.h"
#include "TimeClockStyle.h"
#include "Widgets/STimeClockAddAlarmDialog.h"

#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Misc/MessageDialog.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Images/SImage.h"

class STimeClockAlarmRow : public STableRow<TSharedPtr<FTimeClockAlarmData>>
{
public:
	SLATE_BEGIN_ARGS(STimeClockAlarmRow) {}
		SLATE_EVENT(FSimpleDelegate, OnDataChanged);
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedPtr<FTimeClockAlarmData> InItem)
	{
		Item = InItem;
		OnDataChanged = InArgs._OnDataChanged;

		STableRow<TSharedPtr<FTimeClockAlarmData>>::Construct(
			STableRow<TSharedPtr<FTimeClockAlarmData>>::FArguments()
			.Padding(2.0f)
			.Content()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
				.Padding(FMargin(0.0f, 2.0f, 0.0f, 2.0f))
				[
					SNew(SBox)
					.HeightOverride(40.0f)
					[
						SNew(SHorizontalBox)
						
						// Alarm icon
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4, 2)
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.WidthOverride(40.0f)
							.HeightOverride(40.0f)
							[
								SNew(SScaleBox)
								.Stretch(EStretch::ScaleToFit)
								[
									SNew(SImage)
									.Image(FTimeClockStyle::Get().GetBrush("TimeClock.AlarmsIcon40"))
								]
							]
						]
						
						// Alarm text
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.Padding(4, 2)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->AlarmText))
							.TextStyle(&FTimeClockStyle::Get(), "TimeClock.TitleText")
						]
						
						// Alarm time
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4, 2)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
							.Text(FText::FromString(Item->AlarmTime.ToString(TEXT("%H:%M"))))
							.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
						]
						
						// Remove button
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4 + 5, 2) // Line up with project list buttons
						[
							SNew(SButton)
							.ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonRemove")
							.ContentPadding(FMargin(2.0f))
							.ToolTipText(FText::FromString("Remove Alarm"))
							.OnClicked(this, &STimeClockAlarmRow::OnDeleteClicked)
							[
								SNew(SBox)
								.WidthOverride(30.0f)
								.HeightOverride(30.0f)
								[
									SNew(SScaleBox)
									.Stretch(EStretch::ScaleToFit)
									[
										SNew(SImage)
										.Image(FTimeClockStyle::Get().GetBrush("TimeClock.RemoveIcon40"))
									]
								]
							]
						]
					]
				]
			],
			InOwnerTableView
		);
	}

private:
	FReply OnDeleteClicked()
	{
		EAppReturnType::Type Response = FMessageDialog::Open(EAppMsgType::YesNo, 
			FText::Format(NSLOCTEXT("TimeClock", "ConfirmDeleteAlarm", "Are you sure you want to delete the alarm '{0}' at {1}:{2}?"), 
				FText::FromString(Item->AlarmText), 
				FText::AsNumber(Item->AlarmTime.GetHour()), 
				FText::AsNumber(Item->AlarmTime.GetMinute())));

		if (Response == EAppReturnType::Yes)
		{
   UTimeClockSubsystem* TimeClockAlarm = UTimeClockBPLibrary::GetTimeClockSubsystem();
			if (!TimeClockAlarm)
			{
				UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Alarm list cannot be removed."))
				return FReply::Handled();
			}
			
			TimeClockAlarm->RemoveAlarm(*Item);
			OnDataChanged.ExecuteIfBound();
		}
		
		return FReply::Handled();
	}

private:
	TSharedPtr<FTimeClockAlarmData> Item;
	FSimpleDelegate OnDataChanged;
};

void STimeClockAlarmList::Construct(const FArguments& InArgs)
{
	RefreshList();

	ChildSlot
	[
		SNew(SVerticalBox)

		// Main content
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FTimeClockAlarmData>>)
			.ListItemsSource(&AlarmListSource)
			.SelectionMode(ESelectionMode::None)
			.OnGenerateRow(this, &STimeClockAlarmList::OnGenerateRowForList)
			
		]

		// Bottom bar
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text_Lambda([this]() {
					return FText::Format(NSLOCTEXT("TimeClock", "AlarmCount", "{0} Items"), FText::AsNumber(AlarmListSource.Num()));
				})
				.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.ToolTipText(FText::FromString("Add Alarm"))
				.OnClicked(this, &STimeClockAlarmList::OnAddAlarmClicked)
				.ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonSimple")
				.ContentPadding(0)
				[
					SNew(SBox)
					.WidthOverride(40.0f)
					.HeightOverride(40.0f)
					[
						SNew(SImage)
						.Image(FTimeClockStyle::Get().GetBrush("TimeClock.AddIcon40"))
					]
				]
			]
		]
	];
}

TSharedRef<ITableRow> STimeClockAlarmList::OnGenerateRowForList(TSharedPtr<FTimeClockAlarmData> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STimeClockAlarmRow, OwnerTable, Item)
		.OnDataChanged(this, &STimeClockAlarmList::RefreshList);
}

void STimeClockAlarmList::RefreshList()
{
	float ScrollOffset= 0.f;
	if (ListView.IsValid())
	{
		ScrollOffset= ListView.Get()->GetScrollOffset();
	}
	AlarmListSource.Empty();

	UTimeClockSubsystem* TimeClockAlarm = UTimeClockBPLibrary::GetTimeClockSubsystem();
	if (!TimeClockAlarm)
	{
		UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Alarm list cannot be displayed."))
		return;
	}

	TArray<FTimeClockAlarmData> AllAlarms = TimeClockAlarm->GetAllAlarms();
	for (const FTimeClockAlarmData& Alarm : AllAlarms)
	{
		AlarmListSource.Add(MakeShared<FTimeClockAlarmData>(Alarm));
	}

	if (ListView.IsValid())
	{
		ListView->RequestListRefresh();
		ListView->SetScrollOffset(ScrollOffset);
	}
}

FReply STimeClockAlarmList::OnAddAlarmClicked()
{
	TSharedRef<SWindow> AddWindow = SNew(SWindow)
		.Title(FText::FromString("Add Alarm"))
		.ClientSize(FVector2D(400, 200))
		.SupportsMaximize(false)
		.SupportsMinimize(false);

	AddWindow->SetContent(
		SNew(SBox)
		[
			SNew(STimeClockAddAlarmDialog)
			.ParentWindow(AddWindow)
		]
	);

	AddWindow->GetOnWindowClosedEvent().AddLambda([this](const TSharedRef<SWindow>& Window) {
		RefreshList();
	});

	TSharedPtr<SWindow> RootWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (RootWindow.IsValid())
	{
		FSlateApplication::Get().AddModalWindow(AddWindow, RootWindow.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(AddWindow);
	}

	return FReply::Handled();
}
