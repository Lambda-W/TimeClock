// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "TimeClockSubsystem.h"
#include "TimerManager.h"

class SImage;
class STimeClockHome;
class STimeClockProjectList;
class STimeClockAlarmList;

class STimeClockMainWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(STimeClockMainWidget) {}
    SLATE_END_ARGS();

    void Construct(const FArguments& InArgs);
    virtual ~STimeClockMainWidget();

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    void StartRefreshTimer();
    FReply SetActivePage(int32 PageIndex);
    void OnRefreshTimerTick();

private:
    TSharedPtr<class SBox> ContentBox;

    TSharedPtr<STimeClockHome> HomePage;
    TSharedPtr<STimeClockProjectList> ProjectListPage;
    TSharedPtr<STimeClockAlarmList> AlarmListPage;

    TSharedPtr<SImage> ButtonImage_Home;
    TSharedPtr<SImage> ButtonImage_Projects;
    TSharedPtr<SImage> ButtonImage_Alarms;
    
    FTimerHandle RefreshTimerHandle;
    
    int ActivePage = 0;
};
