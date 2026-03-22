// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "TimeClock.h"
#include "TimeClockSubsystem.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimeClockBPLibrary.generated.h"

class UTimeClockSubsystem;

UCLASS()
class TIMECLOCK_API UTimeClockBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    
    // Returns the Time Clock Editor Subsystem.
    UFUNCTION(BlueprintPure, meta = (Keywords = "Time Clock Subsystem"), Category = "Time Clock")
        static UTimeClockSubsystem* GetTimeClockSubsystem();

	// Creates and spawns an editor notification.
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Time Clock"), Category = "Time Clock")
		static void AddEditorNotification(FString Message, float Duration, bool PlaySound);
};
