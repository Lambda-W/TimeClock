// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "Widgets/STimeClockProjectList.h"

#include "TimeClock.h"
#include "TimeClockStyle.h"

#include "Widgets/STimeClockEditProjectDialog.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "TimeClockBPLibrary.h"
#include "TimeClockFileIO.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Misc/App.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "Brushes/SlateImageBrush.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Images/SImage.h"

class STimeClockProjectRow : public STableRow<TSharedPtr<FTimeClockData>>
{
public:
	SLATE_BEGIN_ARGS(STimeClockProjectRow) {}
		SLATE_EVENT(FSimpleDelegate, OnDataChanged);
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedPtr<FTimeClockData> InItem)
	{
		Item = InItem;
		OnDataChanged = InArgs._OnDataChanged;

		// Load thumbnail
		// const FSlateBrush* ProjectThumbnailBrush = nullptr;
		// TSharedPtr<FSlateBrush> CustomThumbnailBrush;
		//
		// FString ProjectThumbnailPath = Item.Get()->GetThumbnailPath();
		// if (FPaths::FileExists(ProjectThumbnailPath))
		// {
		// 	CustomThumbnailBrush = MakeShareable(new FSlateImageBrush(ProjectThumbnailPath, FVector2D(192, 192)));
		// 	ProjectThumbnailBrush = CustomThumbnailBrush.Get();
		// }
		// else
		// {
		// 	// Default thumbnail if the project doesn't have one
		// 	ProjectThumbnailBrush = FTimeClockStyle::Get().GetBrush("TimeClock.DefaultIcon128");
		// }

		FTimespan Timespan = FTimespan::FromSeconds(Item->TotalSecondsSpent);
		FString TimeString = FString::Printf(TEXT("%02dh %02dm"), (int32)Timespan.GetTotalHours(), Timespan.GetMinutes());

		STableRow<TSharedPtr<FTimeClockData>>::Construct(
			STableRow<TSharedPtr<FTimeClockData>>::FArguments()
			.Padding(2.0f)
			.Content()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
				.Padding(FMargin(0.0f, 2.0f, 0.0f, 2.0f))
				[
					
					SNew(SBox)
					.WidthOverride(40.0f)
					.HeightOverride(40.0f)
					[
						SNew(SHorizontalBox)

						// Project Thumbnail
						// + SHorizontalBox::Slot()
						// .AutoWidth()
						// .Padding(4, 2)
						// [
						// 	SNew(SBox)
						// 	.WidthOverride(40.0f)
						// 	.HeightOverride(40.0f)
						// 	[
						// 		SNew(SScaleBox)
						// 		.Stretch(EStretch::ScaleToFit)
						// 		[
						// 			SNew(SImage)
						// 			.Image(ProjectThumbnailBrush ? ProjectThumbnailBrush : FAppStyle::Get().GetBrush("DefaultBrush"))
						// 		]
						// 	]
						// ]

						// Project Title
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.Padding(8, 0.f)
						.VAlign(VAlign_Center)
						[
							SNew(SBox)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(Item->ProjectName))
								.TextStyle(&FTimeClockStyle::Get(), "TimeClock.TitleText")
							]
						]

						// Project info
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4, 2)
						.VAlign(VAlign_Center)
						[
							SNew(SVerticalBox)

							// Time spent
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TimeString))
								.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
							]

							// Started on date
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::AsDate(Item->StartDate, EDateTimeStyle::Medium))
								.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
								.ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.6f))
							]
						]

						// Buttons
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(4, 2)
						[
							SNew(SHorizontalBox)

							// Edit buttons
							+ SHorizontalBox::Slot()
							.AutoWidth()
							[
								SNew(SButton)
								.ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonEdit")
								.ContentPadding(FMargin(2.0f))
								.ToolTipText(FText::FromString("Edit Project Data"))
								.OnClicked(this, &STimeClockProjectRow::OnEditClicked)
								[
									SNew(SBox)
									.WidthOverride(30.0f)
									.HeightOverride(30.0f)
									[
										SNew(SImage)
										.Image(FTimeClockStyle::Get().GetBrush("TimeClock.EditIcon40"))
									]
								]
							]

							// Remove buttons
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(5.0f, 0.0f)
							[
								SNew(SButton)
								.ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonRemove")
								.ContentPadding(FMargin(2.0f))
								.ToolTipText(FText::FromString("Delete Project Data"))
								.OnClicked(this, &STimeClockProjectRow::OnDeleteClicked)
								[
									SNew(SBox)
									.WidthOverride(30.0f)
									.HeightOverride(30.0f)
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
	FReply OnEditClicked()
	{
		TSharedRef<SWindow> EditWindow = SNew(SWindow)
			.Title(FText::FromString("Edit Project Data"))
			.ClientSize(FVector2D(400, 200))
			.SupportsMaximize(false)
			.SupportsMinimize(false);

 		EditWindow->SetContent(
			SNew(SBox)
			[
				SNew(STimeClockEditProjectDialog)
				.ProjectData(*Item)
				.ParentWindow(EditWindow)
			]
		);

		EditWindow->GetOnWindowClosedEvent().AddLambda([this](const TSharedRef<SWindow>& Window) {
			OnDataChanged.ExecuteIfBound();
		});

		TSharedPtr<SWindow> RootWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (RootWindow.IsValid())
		{
			FSlateApplication::Get().AddModalWindow(EditWindow, RootWindow.ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(EditWindow);
		}

		return FReply::Handled();
	}

	FReply OnDeleteClicked()
	{
		EAppReturnType::Type Response = FMessageDialog::Open(EAppMsgType::YesNo, 
			FText::Format(NSLOCTEXT("TimeClock", "ConfirmDeleteProject", "Are you sure you want to delete the TimeClock data for project '{0}'?"), FText::FromString(Item->ProjectName)));

		if (Response == EAppReturnType::Yes)
		{
   UTimeClockSubsystem* TimeClockCore = UTimeClockBPLibrary::GetTimeClockSubsystem();
   if (!TimeClockCore)
   {
       UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Project data cannot be deleted."));
       return FReply::Handled();
   }
			
			TimeClockCore->DeleteSavedProjectData(Item->ProjectName);
			OnDataChanged.ExecuteIfBound();
		}
		
		return FReply::Handled();
	}

private:
	TSharedPtr<FTimeClockData> Item;
	FSimpleDelegate OnDataChanged;
};

void STimeClockProjectList::Construct(const FArguments& InArgs)
{	
	RefreshList();

	ChildSlot
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FTimeClockData>>)
			.ListItemsSource(&ProjectListSource)
			.SelectionMode(ESelectionMode::None)
			.OnGenerateRow(this, &STimeClockProjectList::OnGenerateRowForList)
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
					return FText::Format(NSLOCTEXT("TimeClock", "ProjectCount", "{0} Items"), FText::AsNumber(ProjectListSource.Num()));
				})
				.TextStyle(&FTimeClockStyle::Get(), "TimeClock.NormalText")
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.OnClicked(this, &STimeClockProjectList::OnExportClicked)
				.ButtonStyle(&FTimeClockStyle::Get(), "TimeClock.ButtonSimple")
				.ToolTipText(FText::FromString("Export all as CSV"))
				.ContentPadding(0)
				[
					SNew(SBox)
					.WidthOverride(40.0f)
					.HeightOverride(40.0f)
					[
						SNew(SImage)
						.Image(FTimeClockStyle::Get().GetBrush("TimeClock.ExportIcon40"))
					]
				]
			]
		]
	];
}

TSharedRef<ITableRow> STimeClockProjectList::OnGenerateRowForList(TSharedPtr<FTimeClockData> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STimeClockProjectRow, OwnerTable, Item)
		.OnDataChanged(this, &STimeClockProjectList::RefreshList);
}

void STimeClockProjectList::RefreshList()
{
	float ScrollOffset= 0.f;
	if (ListView.IsValid())
	{
		ScrollOffset= ListView.Get()->GetScrollOffset();
	}
	ProjectListSource.Empty();
	
	UTimeClockSubsystem* TimeClockCore = UTimeClockBPLibrary::GetTimeClockSubsystem();
	if (!TimeClockCore)
	{
		UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Project list cannot be displayed."));
		return;
	}

	TArray<FTimeClockData> AllData = TimeClockCore->GetAllSavedProjectData();
	for (const FTimeClockData& Data : AllData)
	{
		ProjectListSource.Add(MakeShared<FTimeClockData>(Data));
	}

	if (ListView.IsValid())
	{
		ListView->RequestListRefresh();
		ListView->SetScrollOffset(ScrollOffset);
	}
}

FReply STimeClockProjectList::OnExportClicked()
{
 if (!UTimeClockBPLibrary::GetTimeClockSubsystem())
    {
        UE_LOG(TimeClock, Warning, TEXT("TimeClockSubsystem is invalid. Project list cannot be exported."));
        return FReply::Handled();
    }
	
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		FString FolderPath;
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		const void* ParentWindowWindowHandle = (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;

		if (DesktopPlatform->OpenDirectoryDialog(ParentWindowWindowHandle, TEXT("Export CSV"), TEXT(""), FolderPath))
		{
			if (!FolderPath.EndsWith(TEXT("\\")) && !FolderPath.EndsWith(TEXT("/")))
			{
				FolderPath += TEXT("/");
			}

   TArray<FTimeClockData> AllData = UTimeClockBPLibrary::GetTimeClockSubsystem()->GetAllSavedProjectData();
   UTimeClockFileIO::ExportDataToCSV(AllData, FolderPath, false);
		}
	}

	return FReply::Handled();
}
