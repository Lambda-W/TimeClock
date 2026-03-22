// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TimeClock.h"
#include "TimeClockCommands.h"
#include "TimeClockSettings.h"
#include "TimeClockStyle.h"

#include "ISettingsModule.h"

#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/STimeClockMainWidget.h"
#include "Framework/Docking/TabManager.h"



#define LOCTEXT_NAMESPACE "FTimeClockModule"
DEFINE_LOG_CATEGORY(TimeClock);

static const FName TimeClockTabName("TimeClock");


void FTimeClockModule::StartupModule()
{
	UE_LOG(TimeClock, Display, TEXT("TimeClock module startup."));

	FTimeClockStyle::Initialize();

	RegisterSettings();

	RegisterCommands();
	RegisterTab();
}

void FTimeClockModule::ShutdownModule()
{
	FTimeClockCommands::Unregister();
	FTimeClockStyle::Shutdown();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TimeClockTabName);
}

void FTimeClockModule::RegisterCommands()
{
	FTimeClockCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FTimeClockCommands::Get().OpenTimeClockCommand,
		FExecuteAction::CreateRaw(this, &FTimeClockModule::OpenTimeClock)
	);
}

void FTimeClockModule::RegisterTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TimeClockTabName, 
	FOnSpawnTab::CreateLambda([](const FSpawnTabArgs& Args)
	{
		return SNew(SDockTab)
		   .TabRole(ETabRole::NomadTab)
		   [
			   SNew(STimeClockMainWidget)
		   ];
	}))
	.SetCanSidebarTab(true)
	.SetAutoGenerateMenuEntry(false) // Need to figure out how to create custom .SetGroup()  
	.SetIcon(FSlateIcon(FTimeClockStyle::GetStyleSetName(), TEXT("TimeClock.Icon_40")));
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedPtr<FExtender> NewMenuExtender = MakeShareable(new FExtender);


	NewMenuExtender->AddMenuExtension("LevelEditor",
		EExtensionHook::After,
		PluginCommands,
		FMenuExtensionDelegate::CreateRaw(this, &FTimeClockModule::CreateWindowButton));

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(NewMenuExtender);

	
}

void FTimeClockModule::CreateWindowButton(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("CustomMenu", TAttribute<FText>(FText::FromString("Time Clock")));

	MenuBuilder.AddMenuEntry(
			FTimeClockCommands::Get().OpenTimeClockCommand,
			NAME_None,
			FTimeClockCommands::Get().OpenTimeClockCommand->GetLabel(),
			FTimeClockCommands::Get().OpenTimeClockCommand->GetDescription(),
			FSlateIcon(FTimeClockStyle::GetStyleSetName(), TEXT("TimeClock.Icon_16")),
			NAME_None
			);	

	MenuBuilder.EndSection();
}

void FTimeClockModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Time Clock",
			LOCTEXT("RuntimeSettingsName", "Time Clock"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the Time Clock settings"),
			GetMutableDefault<UTimeClockSettings>());
	}
}

void FTimeClockModule::OpenTimeClock()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TimeClockTabName);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTimeClockModule, TimeClock);
