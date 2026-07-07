// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.


#include "TimeClockSubsystem.h"
#include "TimeClock.h"
#include "TimeClockSettings.h"
#include "TimeClockNotificationManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Editor.h"
#include "ImageUtils.h"
#include "LevelEditor.h"
#include "Misc/CoreDelegates.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "UObject/ObjectSaveContext.h"
#include "TimeClockFileIO.h"

UTimeClockSubsystem::UTimeClockSubsystem() : UEditorSubsystem()
{
}

/* ===== UEditorSubsystem Implementation ===== */
void UTimeClockSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ActiveAlarms = GetAllAlarms();
    InitialiseActiveProject();

	// Tick every minute 
	if (!TimeClockRefreshTimer.IsValid())
	{
		GEditor->GetTimerManager().Get().SetTimer(TimeClockRefreshTimer, this, &UTimeClockSubsystem::TimeClockRefreshTick, 60.f, true, 4.0f);
	}
	
	UE_LOG(TimeClock, Log, TEXT("TimeClock Alarm started"));
}

void UTimeClockSubsystem::TimeClockRefreshTick()
{
	RefreshAlarms();
	RefreshWorkTime();
	SaveCurrentProjectData();
}

void UTimeClockSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UTimeClockSubsystem* UTimeClockSubsystem::GetTimeClockSubsystem()
{
	if (!GEditor)
	{
		return nullptr;
	}

	UTimeClockSubsystem* Subsystem = GEditor->GetEditorSubsystem<UTimeClockSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(TimeClock, Error, TEXT("TimeClockSubsystem not found."));
		return nullptr;
	}
	
	return Subsystem;
}


/* ===== TimeClock Core ===== */

void UTimeClockSubsystem::InitialiseActiveProject()
{
	FString CurrentProjectName = GetProjectName();

	ActiveSessionStartDate = GetDateNow();
	
	RefreshProjectThumbnail();
	
	// Check if there is any saved data for this project, load it if true.
	if (GetSavedDataForProject(CurrentProjectName, ActiveProjectData))
	{
		FDateTime CurrentTime = GetDateNow();

		// Checks if it is the first time the project is opened today.
		if (ActiveProjectData.LastSaved.GetDate() < CurrentTime.GetDate())
		{
			// If it is, update the BeforeToday values
			ActiveProjectData.LastSavedBeforeToday = ActiveProjectData.LastSaved;
			ActiveProjectData.SecondsSpentBeforeToday = ActiveProjectData.TotalSecondsSpent;
		}

		ActiveProjectData.LastSaved = CurrentTime;
		
		RefreshWorkTime();
		SaveProjectData(ActiveProjectData);
		
		UE_LOG(TimeClock, Display, TEXT("Starting TimeClock for known project named: %s"), *CurrentProjectName);
		return;
	}

	// If not, start a new entry for this project.
	{
		UE_LOG(TimeClock, Display, TEXT("Starting TimeClock for new project named: %s"), *CurrentProjectName);

		ActiveProjectData.ProjectName = GetProjectName();
		ActiveProjectData.StartDate = GetDateNow();
		ActiveProjectData.LastSaved = GetDateNow();
		ActiveProjectData.TotalSecondsSpent = 0;
		ActiveProjectData.LastSavedBeforeToday = GetDateNow();
		ActiveProjectData.SecondsSpentBeforeToday = 0;
		
		SaveProjectData(ActiveProjectData);
		return;
	}
}

void UTimeClockSubsystem::RefreshWorkTime()
{
	int NewTimeSpent;
	FDateTime CurrentTime = GetDateNow();

	// Compare the time difference (in seconds) between now and the last time the data was saved
	NewTimeSpent = CurrentTime.ToUnixTimestamp() - ActiveProjectData.LastSaved.ToUnixTimestamp();

	// This only happens if the data was last saved with a date in the future (should not happen).
	if (NewTimeSpent < 0)
	{
		UE_LOG(TimeClock, Warning, TEXT("It seems the stats for this project were saved in the future. I don't see any flying cars though? Dates were adjusted, time spent is unchanged. Please restart the editor or save TimeClockData for current project."));
		ActiveProjectData.LastSaved = CurrentTime;
		ActiveProjectData.LastSavedBeforeToday = ActiveProjectData.StartDate;
		ActiveProjectData.SecondsSpentBeforeToday = ActiveProjectData.TotalSecondsSpent;
		return;
	}
	
	// Update the struct values. This still needs to be saved!
	ActiveProjectData.TotalSecondsSpent = ActiveProjectData.TotalSecondsSpent + NewTimeSpent;
	ActiveProjectData.LastSaved = CurrentTime;
}

void UTimeClockSubsystem::RefreshProjectThumbnail()
{
	// Loads Project's Texture
	const FString ProjectThumbnailPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("AutoScreenshot.png"));
	if (FPaths::FileExists(ProjectThumbnailPath))
	{
		UTexture2D* TempThumbnail = FImageUtils::ImportFileAsTexture2D(ProjectThumbnailPath);
		if (TempThumbnail)
		{
			ActiveProjectThumbnail = TempThumbnail;
		}
	}
}

bool UTimeClockSubsystem::SaveCurrentProjectData()
{
	RefreshWorkTime();
	return SaveProjectData(ActiveProjectData);

}

bool UTimeClockSubsystem::SaveProjectData(FTimeClockData TimeClockData)
{
	// Load all saved projects
	TArray<FTimeClockData> tempAllProjectData;
	tempAllProjectData = GetAllSavedProjectData();
	bool Success;

	// Check if other projects were already saved.
	if (tempAllProjectData.Num() > 0)
	{
		// If so look for an entry that matches the input name.
		for (int i = 0; i < tempAllProjectData.Num(); i++)
		{
			if (tempAllProjectData[i].ProjectName == TimeClockData.ProjectName)
			{
				// Update the loaded data with the input data, and sets a new LastUpdated value.
				tempAllProjectData[i] = TimeClockData;
				tempAllProjectData[i].LastSaved = GetDateNow();

				// Checks if the input data corresponds to the currently opened project, if so refreshes the active data.
				if (TimeClockData.ProjectName == ActiveProjectData.ProjectName)
				{
					ActiveProjectData = tempAllProjectData[i];
				}

    // Saves the updated data (call FileIO directly)
    Success = UTimeClockFileIO::WriteSaveFile(UTimeClockFileIO::SerialiseFileContent(tempAllProjectData));
				UE_LOG(TimeClock, Display, TEXT("TimeClock stats updated for project named: %s"), *TimeClockData.ProjectName);
				return Success;
			}
		}
	}

 // If this is the first project to be saved, write the entry to the file.
 tempAllProjectData.Add(ActiveProjectData);
 Success = UTimeClockFileIO::WriteSaveFile(UTimeClockFileIO::SerialiseFileContent(tempAllProjectData));
	UE_LOG(TimeClock, Display, TEXT("TimeClock stats created for project named: %s"), *TimeClockData.ProjectName);
	return Success;
}

bool UTimeClockSubsystem::GetCurrentProjectData(FTimeClockData& ProjectData)
{
	// This returns the active values, not the saved ones!
	RefreshWorkTime();

	ProjectData = ActiveProjectData;
	return true;
}

bool UTimeClockSubsystem::GetSavedDataForProject(FString ProjectName, FTimeClockData &ProjectData)
{
	// Loads project data from file
	TArray<FTimeClockData> tempAllProjectData;
	tempAllProjectData = GetAllSavedProjectData();

	// Checks if there are any entries saved.
	if (tempAllProjectData.Num() > 0)
	{
		// Look for the saved project that matches the input name.
		for (int i = 0; i < tempAllProjectData.Num(); i++)
		{
			if (tempAllProjectData[i].ProjectName == ProjectName)
			{
				ProjectData = tempAllProjectData[i];
				return true;
			}
		}
	}

	UE_LOG(TimeClock, Warning, TEXT("No saved TimeClock data for project named: %s"), *ProjectName);
	ProjectData = FTimeClockData();
	return false;
}

TArray<FTimeClockData> UTimeClockSubsystem::GetAllSavedProjectData()
{
	FString SaveFileContent;
	UTimeClockFileIO::LoadSaveFile(SaveFileContent);

	return UTimeClockFileIO::DeserialiseFileContent(SaveFileContent);
}

bool UTimeClockSubsystem::DeleteSavedProjectData(FString ProjectName)
{
	TArray<FTimeClockData> tempAllProjectData;
	tempAllProjectData = GetAllSavedProjectData();

	if (tempAllProjectData.Num() > 0)
	{
		// Look for the saved project that matches the input name.
		for (int i = tempAllProjectData.Num() - 1; i >= 0; i--)
		{
			if (tempAllProjectData[i].ProjectName == ProjectName)
			{
    tempAllProjectData.RemoveAt(i);

    UTimeClockFileIO::WriteSaveFile(UTimeClockFileIO::SerialiseFileContent(tempAllProjectData));
				UE_LOG(TimeClock, Display, TEXT("TimeClock stats deleted for project named: %s"), *ProjectName);
				return true;
			}
		}
	}

	UE_LOG(TimeClock, Warning, TEXT("Couldn't delete TimeClock data for project named: %s"), *ProjectName);
	return false;

}

FString UTimeClockSubsystem::GetProjectName()
{
	// This isn't the most elegant way to get the Project's name, but it works for now.
	FString ProjectName = FString(FPaths::GetBaseFilename(FPaths::GetProjectFilePath()));

	return ProjectName;
}

FDateTime UTimeClockSubsystem::GetDateNow()
{
	return FDateTime::Now();
}





/* ===== TimeClock Alarm ===== */
void UTimeClockSubsystem::RefreshAlarms()
{
	TArray<FTimeClockAlarmData> AlarmsToTrigger;
	FDateTime CurrentTime = FDateTime::Now();

	// For each registered alarms
	for (int i = 0; i < ActiveAlarms.Num(); i++)
	{
		// Is the hour value the same
		if (ActiveAlarms[i].AlarmTime.GetHour() == CurrentTime.GetHour())
		{
			// Is the minute value +/- 5 minutes the same.
			if (ActiveAlarms[i].AlarmTime.GetMinute() == CurrentTime.GetMinute())
			{
				// Add it to temp array
				AlarmsToTrigger.Add(ActiveAlarms[i]);
			}
		}
	}

	// For each alarm that have to be triggered
	if (AlarmsToTrigger.Num() > 0)
	{
		// Reverse loop as we'll be removing them to ensure they're not triggered again.
		for (int j = AlarmsToTrigger.Num() - 1; j >= 0; j--)
		{
			TriggerAlarm(AlarmsToTrigger[j]);
			ActiveAlarms.Remove(AlarmsToTrigger[j]);
		}
	}
}

void UTimeClockSubsystem::TriggerAlarm(FTimeClockAlarmData Alarm)
{
	// Send editor notification
	FTimeClockNotificationManager::Get()->DispatchNotification(Alarm.AlarmText, AlarmDuration, true);

	UE_LOG(TimeClock, Warning, TEXT("TimeClock Alarm: %s"), *Alarm.AlarmText);
}

void UTimeClockSubsystem::AddAlarm(FTimeClockAlarmData Alarm)
{
	// Add the alarm to the active pool of alarms.
	ActiveAlarms.AddUnique(Alarm);	
	
	// Add the alarm to the config file.
	UTimeClockSettings* TimeClockSettings = GetMutableDefault<UTimeClockSettings>();
	TimeClockSettings->Alarms.AddUnique(Alarm);
	TimeClockSettings->TryUpdateDefaultConfigFile();
}

void UTimeClockSubsystem::RemoveAlarm(FTimeClockAlarmData Alarm)
{
	// Remove the alarm from the active pool if present
	if (ActiveAlarms.Contains(Alarm))
	{
		ActiveAlarms.Remove(Alarm);
	}

	// Remove the alarm from the config file.
	UTimeClockSettings* TimeClockSettings = GetMutableDefault<UTimeClockSettings>();

	if (TimeClockSettings->Alarms.Contains(Alarm))
	{
		TimeClockSettings->Alarms.RemoveSingle(Alarm);
		TimeClockSettings->TryUpdateDefaultConfigFile();
	}
}

TArray<FTimeClockAlarmData> UTimeClockSubsystem::GetAllAlarms()
{
	UTimeClockSettings* TimeClockSettings = GetMutableDefault<UTimeClockSettings>();
	return TimeClockSettings->Alarms;
}
