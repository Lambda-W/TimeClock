// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "TimeClockFileIO.h"
#include "TimeClock.h"

#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

FString UTimeClockFileIO::GetSavePath()
{
    FString TempPath = FPlatformProcess::UserDir();
    TempPath.Append(TEXT("UE4_TimeClockData.txt"));
    return TempPath;
}

bool UTimeClockFileIO::LoadSaveFile(FString& FileContent)
{
    return FFileHelper::LoadFileToString(FileContent, *GetSavePath(), FFileHelper::EHashOptions::None, 0);
}

bool UTimeClockFileIO::WriteSaveFile(const FString& FileContent)
{
    return FFileHelper::SaveStringToFile(*FileContent, *GetSavePath(), FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), 0);
}

FString UTimeClockFileIO::SerialiseFileContent(const TArray<FTimeClockData>& TimeClockData)
{
    FString SerialisedFileContent;

    for (int32 i = 0; i < TimeClockData.Num(); i++)
    {
        FString SerialisedData;
        if (SerialiseTimeClockData(TimeClockData[i], SerialisedData))
        {
            SerialisedFileContent.Append(SerialisedData + LINE_DELIMITER);
        }
    }

    return SerialisedFileContent;
}

TArray<FTimeClockData> UTimeClockFileIO::DeserialiseFileContent(const FString& FileContent)
{
    TArray<FTimeClockData> DeserialisedFileContent;
    TArray<FString> ParsedFileContent;

    FString TempContent = FileContent;
    TempContent.ParseIntoArray(ParsedFileContent, LINE_DELIMITER, true);

    for (int32 i = 0; i < ParsedFileContent.Num(); i++)
    {
        FTimeClockData LineData;
        if (DeserialiseTimeClockData(ParsedFileContent[i], LineData))
        {
            DeserialisedFileContent.Add(LineData);
        }
    }

    return DeserialisedFileContent;
}

bool UTimeClockFileIO::SerialiseTimeClockData(const FTimeClockData& StructData, FString& OutString)
{
    FString ReturnData =
        StructData.ProjectName
        + VALUE_DELIMITER + StructData.StartDate.ToString()
        + VALUE_DELIMITER + StructData.LastSaved.ToString()
        + VALUE_DELIMITER + FString::FromInt(StructData.TotalSecondsSpent)
        + VALUE_DELIMITER + StructData.LastSavedBeforeToday.ToString()
        + VALUE_DELIMITER + FString::FromInt(StructData.SecondsSpentBeforeToday);

    OutString = ReturnData;
    return true;
}

bool UTimeClockFileIO::DeserialiseTimeClockData(const FString& StringData, FTimeClockData& OutStruct)
{
    FString TempStringData = StringData;
    TempStringData.RemoveFromEnd(LINE_DELIMITER);

    TArray<FString> ParsedLineContent;
    TempStringData.ParseIntoArray(ParsedLineContent, VALUE_DELIMITER, false);

    FTimeClockData LineData;

    if (ParsedLineContent.Num() == 6)
    {
        LineData.ProjectName = ParsedLineContent[0];
        FDateTime::Parse(ParsedLineContent[1], LineData.StartDate);
        FDateTime::Parse(ParsedLineContent[2], LineData.LastSaved);
        LineData.TotalSecondsSpent = FCString::Atoi(*ParsedLineContent[3]);
        FDateTime::Parse(ParsedLineContent[4], LineData.LastSavedBeforeToday);
        LineData.SecondsSpentBeforeToday = FCString::Atoi(*ParsedLineContent[5]);

        if (LineData.ProjectName != TEXT("") || LineData.ProjectName != TEXT("Unknown"))
        {
            OutStruct = LineData;
            return true;
        }
    }

    UE_LOG(TimeClock, Warning, TEXT("Cannot deserialise input string into TimeClockData: %s"), *StringData);
    OutStruct = LineData;
    return false;
}

bool UTimeClockFileIO::FormatProjectDataForExport(const FTimeClockData& StructData, FString& OutCsvLine)
{
    FString ReturnData =
        StructData.ProjectName
        + TEXT(",") + StructData.StartDate.ToString()
        + TEXT(",") + StructData.LastSaved.ToString()
        + TEXT(",") + FString::FromInt(StructData.TotalSecondsSpent);

    OutCsvLine = ReturnData;
    return true;
}

bool UTimeClockFileIO::ExportDataToCSV(const TArray<FTimeClockData>& Data, const FString& Directory, bool bAsTextFile)
{
    TArray<FString> ExportString;
    ExportString.Add(FString("ProjectName,StartDate,LastSaved,TotalTimeSpent"));

    for (int32 i = 0; i < Data.Num(); i++)
    {
        FString ItemData;
        if (FormatProjectDataForExport(Data[i], ItemData))
        {
            ExportString.Add(ItemData);
        }
        else
        {
            UE_LOG(TimeClock, Error, TEXT("Failed to export, invalid TimeClockData."));
            return false;
        }
    }

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.DirectoryExists(*Directory))
    {
        UE_LOG(TimeClock, Error, TEXT("Failed to export, directory does not exist."));
        return false;
    }

    FString FileExtension = TEXT(".csv");
    if (bAsTextFile)
    {
        FileExtension = TEXT(".txt");
    }

    FString FilePath = Directory + TEXT("TimeClockExport") + FileExtension;

    int32 DuplicateFileIndex = 0;
    while (PlatformFile.FileExists(*FilePath))
    {
        DuplicateFileIndex++;
        FilePath = Directory + TEXT("TimeClockExport") + FString::FromInt(DuplicateFileIndex) + FileExtension;
    }

    FFileHelper::SaveStringArrayToFile(ExportString, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), 0);

    UE_LOG(TimeClock, Display, TEXT("TimeClockData exported at: %s"), *FilePath);
    return true;
}
