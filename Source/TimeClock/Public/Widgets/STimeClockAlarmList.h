// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "TimeClockSubsystem.h"

class STimeClockAlarmList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimeClockAlarmList) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

	void RefreshList();

private:
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FTimeClockAlarmData> Item, const TSharedRef<STableViewBase>& OwnerTable);
	
	TArray<TSharedPtr<FTimeClockAlarmData>> AlarmListSource;
	TSharedPtr<SListView<TSharedPtr<FTimeClockAlarmData>>> ListView;

	FReply OnAddAlarmClicked();
};
