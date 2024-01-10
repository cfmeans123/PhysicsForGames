#include "Timer.h"

//====================================================================================================
// Filename:	Timer.cpp
// Created by:	Peter Chan
//====================================================================================================

#include <array>

namespace jm::Platform
{
	u64 TimeStamp()
	{
		FILETIME currentTime;
		GetSystemTimeAsFileTime(&currentTime);

		ULARGE_INTEGER utc;
		utc.HighPart = currentTime.dwHighDateTime;
		utc.LowPart = currentTime.dwLowDateTime;

		static_assert(bitsizeof(ULARGE_INTEGER) == bitsizeof(u64));
		return static_cast<u64>(utc.QuadPart);
	}

	Timer::Timer()
		: mTicksPerSecond()
		, mLastTick()
		, mCurrentTick()
		, mElapsedTime(0.0)
		, mTotalTime(0.0)
		, mNextUpdateTime(0.0)
		, mFrameSinceLastSecond(0.0)
		, mFramesPerSecond(0.0)
	{
		mTicksPerSecond.QuadPart = 0;
		mLastTick.QuadPart = 0;
		mCurrentTick.QuadPart = 0;
	}

	void Timer::Initialize()
	{
		// Get the system clock frequency and current tick
		QueryPerformanceFrequency(&mTicksPerSecond);
		QueryPerformanceCounter(&mCurrentTick);
		mLastTick = mCurrentTick;

		m_SecondsPerTick = 1.0 / static_cast<f64>(mTicksPerSecond.QuadPart);

		// Reset
		mElapsedTime = 0.0;
		mTotalTime = 0.0;
		mNextUpdateTime = 0.0;
		mFrameSinceLastSecond = 0.0;
		mFramesPerSecond = 0.0;
	}

	void Timer::Update()
	{
		// Get the current tick count
		QueryPerformanceCounter(&mCurrentTick);

		// Calculate the total time and elapsed time
		mElapsedTime = static_cast<f64>(mCurrentTick.QuadPart - mLastTick.QuadPart) * m_SecondsPerTick;
		mTotalTime += mElapsedTime;

		// Update the last tick count
		mLastTick = mCurrentTick;

		// Calculate the FPS
		mFrameSinceLastSecond += 1.0;
		if (mTotalTime >= mNextUpdateTime)
		{
			mFramesPerSecond = mFrameSinceLastSecond;
			mFrameSinceLastSecond = 0.0;
			mNextUpdateTime += 1.0;
		}
	}

	f64 Timer::GetElapsedTime() const
	{
		return mElapsedTime;
	}

	f64 Timer::GetTotalTime() const
	{
		return mTotalTime;
	}

	f64 Timer::GetFramesPerSecond() const
	{
		return mFramesPerSecond;
	}

	f64 Timer::GetTime() const
	{
		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);
		return static_cast<f64>(tick.QuadPart) * m_SecondsPerTick;
	}
}