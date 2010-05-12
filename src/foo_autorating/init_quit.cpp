#include"stdafx.h"
#include"init_quit.h"
#include"update_callback.h"
using namespace auto_rating;

namespace auto_rating {

initquit_factory_t<TimerInit> timer_factory;

} //~namespace auto_rating

void __stdcall TimerInit::timerProc(HWND, UINT, UINT_PTR timerID, DWORD) {
	UpdateCallback::registerCallback();
}

void TimerInit::on_init() {
	static const int secondsPerHour = 60 * 60;
	_timerID = ::SetTimer(0L, 0, secondsPerHour * 1000, TimerInit::timerProc);

}

void TimerInit::on_quit() {
	::KillTimer(0L, _timerID);
	_timerID = 0;
}
