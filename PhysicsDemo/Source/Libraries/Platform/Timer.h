#pragma once

//====================================================================================================
// Filename:	Timer.h
// Created by:	Peter Chan
// Description:	Class for high resolution timer
//====================================================================================================

#include "PlatformCore.h"
#include "OS.h"


namespace jm::Platform
{
	u64 TimeStamp();

	class Timer
	{
	public:
		Timer();

		void Initialize();
		void Update();

		//seconds
		f64 GetElapsedTime() const;
		f64 GetTotalTime() const;
		f64 GetFramesPerSecond() const;
		f64 GetTime() const;

	private:

		LARGE_INTEGER mTicksPerSecond;
		LARGE_INTEGER mLastTick;
		LARGE_INTEGER mCurrentTick;

		f64 m_SecondsPerTick;

		f64 mElapsedTime;
		f64 mTotalTime;

		f64 mNextUpdateTime;
		f64 mFrameSinceLastSecond;
		f64 mFramesPerSecond;
	};
}