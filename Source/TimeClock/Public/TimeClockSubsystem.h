// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/TimerHandle.h"
#include "TimeClockSubsystem.generated.h"

#define LINE_DELIMITER TEXT("_END_OF_LINE_")
#define VALUE_DELIMITER TEXT("_VALUE_")

USTRUCT(BlueprintType)
struct TIMECLOCK_API FTimeClockData
{
	GENERATED_BODY()

	// Name of the project these stats were taken from.
	UPROPERTY(BlueprintReadWrite, Category = "Time Clock")
	FString ProjectName;
	// The date at which the stats started being recorded.
	UPROPERTY(BlueprintReadWrite, Category = "Time Clock")
	FDateTime StartDate;
	// Last time this project's stats were saved.
	UPROPERTY(BlueprintReadWrite, Category = "Time Clock")
	FDateTime LastSaved;
	// The total second spent with project opened.
	UPROPERTY(BlueprintReadWrite, Category = "Time Clock")
	int TotalSecondsSpent;
	// Last time this project's stats were saved before today. Only used to calculate time spent today.
	UPROPERTY(BlueprintReadOnly, Category = "Time Clock")
	FDateTime LastSavedBeforeToday;	
	// The total second spent with project opened before today. Only used to calculate time spent today.
	UPROPERTY(BlueprintReadOnly, Category = "Time Clock")
	int SecondsSpentBeforeToday;

	FTimeClockData()
	{
		ProjectName = "Unknown";
		StartDate = FDateTime();
		LastSaved = FDateTime();
		TotalSecondsSpent = 0;
		LastSavedBeforeToday = FDateTime();
		SecondsSpentBeforeToday = 0;
	}

	FTimeClockData(FString InProjectName, FDateTime InStartDate, FDateTime InLastModified, int InMinutesSpent, FDateTime InLastSavedBeforeToday, int InSecondsSpentBeforeToday)
	{
		ProjectName = InProjectName;
		StartDate = InStartDate;
		LastSaved = InLastModified;
		TotalSecondsSpent = InMinutesSpent;
		LastSavedBeforeToday = InLastSavedBeforeToday;
		SecondsSpentBeforeToday = InSecondsSpentBeforeToday;
	}
	
	FString GetTotalTimeString() const
	{
		FTimespan Time = FTimespan::FromSeconds(TotalSecondsSpent);
		int32 Hours = (int32)Time.GetTotalHours();
		int32 Minutes = Time.GetMinutes();
	
		return FString::Printf(TEXT("%dh %dm"), Hours, Minutes);
	}
		
	FString GetTimeTodayString() const
	{
		FTimespan Time = FTimespan::FromSeconds(TotalSecondsSpent - SecondsSpentBeforeToday);
		int32 Hours = (int32)Time.GetTotalHours();
		int32 Minutes = Time.GetMinutes();
	
		return FString::Printf(TEXT("%dh %dm"), Hours, Minutes);
	}
	
	FString GetStartDateString() const
	{
		return FText::AsDate(StartDate).ToString();
	}
};

USTRUCT(BlueprintType)
struct TIMECLOCK_API FTimeClockAlarmData
{
	GENERATED_BODY()

	// Name of the project these stats were taken from.
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Time Clock Alarm")
	FString AlarmText;
	// The data at which the stats started being recorded.
	UPROPERTY(config, EditAnywhere, BlueprintReadWrite, Category = "Time Clock Alarm")
	FDateTime AlarmTime;

	FTimeClockAlarmData()
	{
		AlarmText = "Alarm not initialised.";
	}

	FTimeClockAlarmData(FString InAlarmText, FDateTime InAlarmTime)
	{
		AlarmText = InAlarmText;
		AlarmTime = InAlarmTime;
	}
	inline bool operator==(const FTimeClockAlarmData& other) const
	{
		return AlarmText == other.AlarmText && AlarmTime == other.AlarmTime;
	}
};


// No file I/O instance required; functions are provided statically by UTimeClockFileIO

UCLASS(BlueprintType, Blueprintable)
class TIMECLOCK_API UTimeClockSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	UTimeClockSubsystem();

	/* ===== UEditorSubsystem Implementation ===== */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UTimeClockSubsystem* GetTimeClockSubsystem();

	// The function called to check for any alarms to trigger.
	void TimeClockRefreshTick();
	
	// Timer responsible for updating alarms and project data
	FTimerHandle TimeClockRefreshTimer;
	
	
	/* ===== TimeClock Core ===== */
	// The runtime TimeClock data for the opened project. Its value might differ to the saved value.
	UPROPERTY()
	FTimeClockData ActiveProjectData;
	
	// Active Project's thumbnail
	UPROPERTY()
	UTexture2D* ActiveProjectThumbnail = nullptr;

	//The date at which the current session started (date at which the project was opened).
	UPROPERTY()
	FDateTime ActiveSessionStartDate;

	// Manually update the data for the current project. This will update the time spent today. Does not save the updated data (use SaveCurrentProjectData() instead).

	void RefreshWorkTime();

	void RefreshProjectThumbnail();
	
	// Returns the currently opened project's name.
	FString GetProjectName();

	// Returns the full date from the computer's time.
	FDateTime GetDateNow();

	// Save the ActiveTimeClockData.
	bool SaveCurrentProjectData();

	// Returns the TimeClock data for the current project.
	bool GetCurrentProjectData(FTimeClockData& ProjectData);
	
	// Saves the input TimeClockData (useful to update closed project).
	bool SaveProjectData(FTimeClockData TimeClockData);

	// Deletes the saved data for a project.
	bool DeleteSavedProjectData(FString ProjectName);

	// Returns the saved data project for the input project name if any.
	bool GetSavedDataForProject(FString ProjectName, FTimeClockData& ProjectData);
	
	// Returns all saved project data.
	TArray<FTimeClockData> GetAllSavedProjectData();


	/* ===== TimeClock Alarm ===== */

	// How long should the notification stay for.
	float AlarmDuration = 5.0f;

	void RefreshAlarms();
	
	// Triggers the alarm notification from FTimeClockAlarmData struct.
	void TriggerAlarm(FTimeClockAlarmData Alarm);

	// Array of all the alarms that haven't been triggered.
	TArray<FTimeClockAlarmData> ActiveAlarms;

	// Adds and saves a new alarm.
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Time Clock Alarm"), Category = "Time Clock Alarm")
	void AddAlarm(FTimeClockAlarmData Alarm);

	// Removes and saves an alarm if it exists.
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Time Clock Alarm"), Category = "Time Clock Alarm")
	void RemoveAlarm(FTimeClockAlarmData Alarm);

	// Returns all saved alarms.
	UFUNCTION(BlueprintCallable, meta = (Keywords = "Time Clock Alarm"), Category = "Time Clock Alarm")
	TArray<FTimeClockAlarmData> GetAllAlarms();

private:

    // Initialise TimeClock core tracking for the current project.
    void InitialiseActiveProject();
};
