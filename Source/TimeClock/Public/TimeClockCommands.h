// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "EditorStyleSet.h"


class FTimeClockCommands : public TCommands<FTimeClockCommands>
{
public:

	FTimeClockCommands()
		: TCommands<FTimeClockCommands>
		(
			TEXT("TimeClock"),
			NSLOCTEXT("Contexts", "TimeClock", "TimeClock Plugin"),
			NAME_None,
#if ENGINE_MAJOR_VERSION <= 5 && ENGINE_MINOR_VERSION < 1
			FEditorStyle::GetStyleSetName()
#elif ENGINE_MAJOR_VERSION <= 5 && ENGINE_MINOR_VERSION >= 1
			FAppStyle::GetAppStyleSetName()
#endif
			) {}

	virtual void RegisterCommands() override;

public:

	TSharedPtr<FUICommandInfo> OpenTimeClockCommand;
};