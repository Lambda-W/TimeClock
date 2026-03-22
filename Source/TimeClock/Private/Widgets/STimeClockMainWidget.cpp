// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "Widgets/STimeClockMainWidget.h"

#include "TimeClockStyle.h"
#include "TimeClockSettings.h"

#include "Widgets/STimeClockHome.h"
#include "Widgets/STimeClockProjectList.h"
#include "Widgets/STimeClockAlarmList.h"

#include "Framework/Application/SlateApplication.h"
#include "TimerManager.h"
#include "Editor.h"
#include "TimeClockBPLibrary.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

void STimeClockMainWidget::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5.0f)
        .HAlign(HAlign_Center)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(2.0f)
            [
                SNew(SButton)
                .ToolTipText(FText::FromString("See current project"))
                .OnClicked(this, &STimeClockMainWidget::SetActivePage, 0)
                .ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonSimple")
                [
                    SNew(SBox)
                    .WidthOverride(40.0f)
                    .HeightOverride(40.0f)
                    [
                        SAssignNew(ButtonImage_Home, SImage)
                        .Image(FTimeClockStyle::Get().GetBrush("TimeClock.HomeIcon40"))
                    ]
                ]
            ]
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(2.0f)
            [
                SNew(SButton)
                .ToolTipText(FText::FromString("See all projects"))
                .OnClicked(this, &STimeClockMainWidget::SetActivePage, 1)
                .ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonSimple")
                [
                    SNew(SBox)
                    .WidthOverride(40.0f)
                    .HeightOverride(40.0f)
                    [
                        SAssignNew(ButtonImage_Projects, SImage)
                        .Image(FTimeClockStyle::Get().GetBrush("TimeClock.ProjectsIcon40"))
                    ]
                ]
            ]
            
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(2.0f)
            [
                SNew(SButton)
                .ToolTipText(FText::FromString("See alarms for this project"))
                .OnClicked(this, &STimeClockMainWidget::SetActivePage, 2)
                .ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonSimple")
                [
                    SNew(SBox)
                    .WidthOverride(40.0f)
                    .HeightOverride(40.0f)
                    [
                        SAssignNew(ButtonImage_Alarms, SImage)
                        .Image(FTimeClockStyle::Get().GetBrush("TimeClock.AlarmsIcon40"))
                    ]
                ]
            ]
        ]
        
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SAssignNew(ContentBox, SBox)
        ]
    ];

    StartRefreshTimer();
	
    SetActivePage(0);
}

FReply STimeClockMainWidget::SetActivePage(int32 PageIndex)
{	
	ActivePage = PageIndex;
	
    if (ContentBox.IsValid())
    {
    	// Clear content first to release widget references
    	ContentBox->SetContent(SNullWidget::NullWidget);
    	
    	HomePage.Reset();
    	ProjectListPage.Reset();
    	AlarmListPage.Reset();
    	
        // Rebuild the content according to the selected page
        switch (PageIndex)
        {
        case 0:
        {
            SAssignNew(HomePage, STimeClockHome);
            ContentBox->SetContent(HomePage.ToSharedRef());
            break;
        }
        case 1:
        {
            SAssignNew(ProjectListPage, STimeClockProjectList);
            ContentBox->SetContent(ProjectListPage.ToSharedRef());
            break;
        }
        case 2:
        {
            SAssignNew(AlarmListPage, STimeClockAlarmList);
            ContentBox->SetContent(AlarmListPage.ToSharedRef());
            break;
        }
        default:
            break;
        }

        
        // Update button Images
        const FLinearColor ActiveTint = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
        const FLinearColor DefaultTint = FLinearColor(1.0f, 1.0f, 1.0f, 0.3f);
		
		if (ButtonImage_Home.IsValid())
		{
			ButtonImage_Home->SetColorAndOpacity(DefaultTint);
		}
		if (ButtonImage_Projects.IsValid())
		{
			ButtonImage_Projects->SetColorAndOpacity(DefaultTint);
		}
		if (ButtonImage_Alarms.IsValid())
		{
			ButtonImage_Alarms->SetColorAndOpacity(DefaultTint);
		}

		switch (PageIndex)
		{
		case 0:
			if (ButtonImage_Home.IsValid())
			{
				ButtonImage_Home->SetColorAndOpacity(ActiveTint);
			}
			break;
		case 1:
			if (ButtonImage_Projects.IsValid())
			{
				ButtonImage_Projects->SetColorAndOpacity(ActiveTint);
			}
			break;
		case 2:
			if (ButtonImage_Alarms.IsValid())
			{
				ButtonImage_Alarms->SetColorAndOpacity(ActiveTint);
			}
			break;
		default:
			break;
		}
	}

	return FReply::Handled();
}

void STimeClockMainWidget::StartRefreshTimer()
{
	if (GEditor)
	{
		GEditor->GetTimerManager()->SetTimer(RefreshTimerHandle, FTimerDelegate::CreateRaw(this, &STimeClockMainWidget::OnRefreshTimerTick), 60.f, true);
	}
}

void STimeClockMainWidget::OnRefreshTimerTick()
{
	switch (ActivePage)
	{
	case 0:
		{
			SetActivePage(ActivePage);
			break;
		}
	case 1:
		{
			if (ProjectListPage.IsValid())
			{
				ProjectListPage->RefreshList();
			}
			break;
		}
	case 2:
		{
			if (AlarmListPage.IsValid())
			{
				AlarmListPage->RefreshList();
			}
			break;
		}
	default:
		break;
	}
}

STimeClockMainWidget::~STimeClockMainWidget()
{
	if (GEditor)
	{
		GEditor->GetTimerManager()->ClearTimer(RefreshTimerHandle);
	}
}

void STimeClockMainWidget::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	
	OnRefreshTimerTick();
}
