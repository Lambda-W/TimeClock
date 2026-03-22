// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TimeClockSubsystem.h" // For FTimeClockData and delimiters
#include "TimeClockFileIO.generated.h"

/**
 * Encapsulates all file I/O and (de)serialization logic for TimeClock data.
 * This keeps UTimeClockSubsystem focused on runtime state and business logic.
 */
UCLASS()
class TIMECLOCK_API UTimeClockFileIO : public UObject
{
    GENERATED_BODY()

public:
    // Returns the filename of the TimeClock save file (Usually in My Documents).
    static FString GetSavePath();

    // Loads the Save File.
    static bool LoadSaveFile(FString& FileContent);

    // Writes the Save File (replaces its content).
    static bool WriteSaveFile(const FString& FileContent);

    // Used to write the Save File's content (converts FTimeClockData array to string, applying the delimiters).
    static FString SerialiseFileContent(const TArray<FTimeClockData>& TimeClockData);

    // Used to read the loaded Save File's content (converts its content to FTimeClockData array, taking into account the delimiters).
    static TArray<FTimeClockData> DeserialiseFileContent(const FString& FileContent);

    // Converts TimeClockData to string, applying the ValueDelimiter.
    static bool SerialiseTimeClockData(const FTimeClockData& StructData, FString& OutString);

    // Converts string to TimeClockData, taking into account the ValueDelimiter.
    static bool DeserialiseTimeClockData(const FString& StringData, FTimeClockData& OutStruct);

    // Formats TimeClock Data struct for export as CSV
    static bool FormatProjectDataForExport(const FTimeClockData& StructData, FString& OutCsvLine);

    // Export the input data in a CSV or TXT format.
    static bool ExportDataToCSV(const TArray<FTimeClockData>& Data, const FString& Directory, bool bAsTextFile);
};
