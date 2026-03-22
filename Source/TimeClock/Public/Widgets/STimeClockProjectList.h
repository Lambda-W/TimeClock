// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "TimeClockSubsystem.h"

class STimeClockProjectList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STimeClockProjectList) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
	
	void RefreshList();

private:
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FTimeClockData> Item, const TSharedRef<STableViewBase>& OwnerTable);
	
	TArray<TSharedPtr<FTimeClockData>> ProjectListSource;
	TSharedPtr<SListView<TSharedPtr<FTimeClockData>>> ListView;

	FReply OnExportClicked();
};
