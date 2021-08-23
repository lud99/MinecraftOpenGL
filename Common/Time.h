#pragma once

struct Time
{
	static float DeltaTime;
	static float ElapsedTime;

	static constexpr float FixedTimestep = 1.0f / 60.0f; // 60fps
	static constexpr float TickRate = 1.0f / 20.0f; // 20fps
};