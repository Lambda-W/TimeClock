// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#define TIMECLOCK_MODULE_NAME TEXT("TimeClock")
class FMenuBuilder;
class FUICommandList;
DECLARE_LOG_CATEGORY_EXTERN(TimeClock, Log, All)

class FTimeClockModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	// Registers the plugin's settings as project settings.
	void RegisterSettings();

	

	/* ==== Window dropdown button ==== */
	
	// Maps of commands and thier bindings
	TSharedPtr<FUICommandList> PluginCommands;

	// Binds commands and map them.
	void RegisterCommands();

	// Registers TimeClock commands to the "Window" dropdown.
	void RegisterTab();

	// Spawns the command buttons. Happens when the "Window" dropdown is clicked.
	void CreateWindowButton(FMenuBuilder& MenuBuilder);

	/** This function will be bound to Command (by default it will open the Tab) */
	void OpenTimeClock();

};
