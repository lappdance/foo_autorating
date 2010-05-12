#ifndef FOO_DAR_INCLUDE_DAR_UPDATE_CALLBACK_H
#define FOO_DAR_INCLUDE_DAR_UPDATE_CALLBACK_H

#pragma once

namespace auto_rating {

class UpdateCallback : public main_thread_callback {
	public:
		static void registerCallback();
	
	public: //main_thread_callback methods
		virtual void callback_run();
};

} //~namespace auto_rating

#endif //FOO_DAR_INCLUDE_DAR_UPDATE_CALLBACK_H
