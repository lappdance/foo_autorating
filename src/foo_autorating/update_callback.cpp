#include"stdafx.h"
#include"update_callback.h"
using namespace auto_rating;

namespace auto_rating {

//service_factory_t<UpdateCallback> g_factory;

} //~namespace auto_rating

void UpdateCallback::registerCallback() {
	service_ptr_t<UpdateCallback> callback = new service_impl_t<UpdateCallback>();
	static_api_ptr_t<main_thread_callback_manager> callbackMgr;
	
	callbackMgr->add_callback(callback);
}

void UpdateCallback::callback_run() {
	static_api_ptr_t<library_manager> library;
		
	metadb_handle_list everything;
	library->get_all_items(everything);

	static_api_ptr_t<metadb_io> db;
	db->dispatch_refresh(everything);
}
