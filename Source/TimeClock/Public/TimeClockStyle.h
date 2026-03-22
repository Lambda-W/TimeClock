// Copyright Lambda Works, Samuel Metters 2019. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * Implements the visual style of the Time Clock plugin.
 */
class FTimeClockStyle
{
public:
	/**
	 * Initializes the style set.
	 */
	static void Initialize();

	/**
	 * Shuts down the style set.
	 */
	static void Shutdown();

	/**
	 * Re-initializes the style set.
	 */
	static void ReloadTextures();

	/**
	 * Returns the style set.
	 *
	 * @return The style set.
	 */
	static const ISlateStyle& Get();

	/**
	 * Returns the name of the style set.
	 *
	 * @return The style set name.
	 */
	static FName GetStyleSetName();

private:
	/**
	 * Creates the style set.
	 *
	 * @return The style set.
	 */
	static TSharedRef<class FSlateStyleSet> Create();

	/**
	 * Holds the style set singleton.
	 */
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};
