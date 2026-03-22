// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#include "TimeClockStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "Misc/Paths.h"
#include "Brushes/SlateImageBrush.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/SlateRenderer.h"

TSharedPtr<FSlateStyleSet> FTimeClockStyle::StyleInstance = nullptr;

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon128x128(128.0f, 128.0f);


void FTimeClockStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FTimeClockStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		ensure(StyleInstance.IsUnique());
		StyleInstance.Reset();
	}
}

FName FTimeClockStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("TimeClockStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedRef<FSlateStyleSet> FTimeClockStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(FTimeClockStyle::GetStyleSetName()));
	Style->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("TimeClock/Resources"));

	/* ==== Icons ==== */
	Style->Set("TimeClock.Icon_16", new IMAGE_BRUSH(TEXT("Icon_TimeClock_16x"), Icon16x16));
	Style->Set("TimeClock.Icon_40", new IMAGE_BRUSH(TEXT("Icon_TimeClock_40x"), Icon40x40));
	
	Style->Set("TimeClock.HomeIcon40", new IMAGE_BRUSH(TEXT("Home_40x"), Icon40x40));
	Style->Set("TimeClock.ProjectsIcon40", new IMAGE_BRUSH(TEXT("Projects_40x"), Icon40x40));
	Style->Set("TimeClock.AlarmsIcon40", new IMAGE_BRUSH(TEXT("Alarm_40x"), Icon40x40));
	Style->Set("TimeClock.EditIcon40", new IMAGE_BRUSH(TEXT("Edit_40x"), Icon40x40));
	Style->Set("TimeClock.RemoveIcon40", new IMAGE_BRUSH(TEXT("Remove_40x"), Icon40x40));
	Style->Set("TimeClock.AddIcon40", new IMAGE_BRUSH(TEXT("Add_40x"), Icon40x40));
	Style->Set("TimeClock.ExportIcon40", new IMAGE_BRUSH(TEXT("Export_40x"), Icon40x40));
	
	Style->Set("TimeClock.DefaultIcon128", new IMAGE_BRUSH(TEXT("DefaultIcon_128"), Icon128x128));

	/* ==== Fonts ==== */
	const FTextBlockStyle NormalText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
	Style->Set("TimeClock.NormalText", FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::Get().GetFontStyle("NormalFont")));

	Style->Set("TimeClock.TitleText", FTextBlockStyle(NormalText)
		.SetFont(FCoreStyle::Get().GetFontStyle("NormalFontBold")));

	/* ==== Buttons Styles ==== */
	FButtonStyle SimpleButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton");
	Style->Set("TimeClock.ButtonSimple", FButtonStyle(SimpleButtonStyle));
	
	const FLinearColor RemoveColor = FLinearColor(1.0f, 0.25f, 0.25f);
	const FLinearColor EditColor =FLinearColor(1.0f, 0.65f, 0.25f);
	
	FButtonStyle RemoveButtonStyle = SimpleButtonStyle;
	RemoveButtonStyle.Normal.TintColor = FSlateColor(RemoveColor);
	RemoveButtonStyle.Hovered.TintColor = FSlateColor(RemoveColor * 0.5f);
	RemoveButtonStyle.Pressed.TintColor = FSlateColor(RemoveColor * 0.35f);
	Style->Set("TimeClock.ButtonRemove", RemoveButtonStyle);
	
	FButtonStyle EditButtonStyle = SimpleButtonStyle;
	EditButtonStyle.Normal.TintColor = FSlateColor(EditColor);
	EditButtonStyle.Hovered.TintColor = FSlateColor(EditColor * 0.5f);
	EditButtonStyle.Pressed.TintColor = FSlateColor(EditColor * 0.35f);
	Style->Set("TimeClock.ButtonEdit", EditButtonStyle);

	return Style;
}

#undef IMAGE_BRUSH

void FTimeClockStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FTimeClockStyle::Get()
{
	return *StyleInstance;
}
