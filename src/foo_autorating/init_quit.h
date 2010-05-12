#ifndef FOO_DAR_INCLUDE_DAR_INIT_QUIT_H
#define FOO_DAR_INCLUDE_DAR_INIT_QUIT_H

#pragma once

namespace auto_rating {

/**
 This class sets up a timer so I can periodically alert Foobar that tracks' ratings
 have decayed.
**/
class TimerInit : public initquit {
	private:
		int _timerID;
	
	private:
		/**
		 This function is called whenever the timer fires. It alerts the library that
		 the @c auto_rating field has changed for all songs.
		**/
		static void __stdcall timerProc(HWND, UINT, UINT_PTR timerID, DWORD);
	
	public: //initquit methods
		/** Initializes a timer and begins waiting on it. */
		virtual void on_init();
		/** Clears the timer and the wait. */
		virtual void on_quit();
};

} //~namespace auto_rating

#endif //FOO_DAR_INCLUDE_DAR_INIT_QUIT_H
