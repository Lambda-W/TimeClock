// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.
// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "TimeClockBPLibrary.h"
#include "TimeClockSubsystem.h"
#include "TimeClock.h"
#include "TimeClockNotificationManager.h"

UTimeClockSubsystem* UTimeClockBPLibrary::GetTimeClockSubsystem()
{
    return UTimeClockSubsystem::GetTimeClockSubsystem();
}

void UTimeClockBPLibrary::AddEditorNotification(FString Message, float Duration, bool PlaySound)
{
	FTimeClockNotificationManager::Get()->DispatchNotification(Message, Duration, PlaySound);
	
}
