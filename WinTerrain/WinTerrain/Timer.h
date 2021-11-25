/**
* \class Timer
*
* \brief Calculates frame/delta time and FPS
*
* \author Paul Robertson
*/

// timer class
// Calculates frame/delta time and FPS.

#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>
namespace core
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		void frame();	///< Update delta time, called every frame

		double getTime();	///< Get delta time
		double getFPS();		///< Get FPS (for display)

	private:
		INT64 frequency = 0;
		double ticksPerS = 0.f;
		INT64 startTime = 0;
		double frameTime = 0.f;
		double fps = 0.f;
		double frames = 0.f;
		double elapsedTime = 0.f;
	};
}

#endif