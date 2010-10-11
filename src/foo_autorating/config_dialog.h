#ifndef FOO_DAR_INCLUDE_DAR_CONFIG_DIALOG_H
#define FOO_DAR_INCLUDE_DAR_CONFIG_DIALOG_H

#pragma once

#include"resource.h"
#include"rating_field.h"
#include<map>

namespace auto_rating {

#define DDX_TRACKBAR(nID, var) \
	if(nCtlID == (UINT)-1 || nCtlID == nID) { \
		if(!DDX_TrackBar(this, nID, var, bSaveAndValidate)) \
			return FALSE; \
	}

#define DDX_COMBOBOX(nID, var) \
	if(nCtlID == (UINT)-1 || nCtlID == nID) { \
		if(!DDX_ComboBox(this, nID, var, bSaveAndValidate)) \
			return FALSE; \
	}

template<typename this_type, typename value_type>
int DDX_TrackBar(this_type* self, UINT nID, value_type& value, int save) {
	HWND ctrl = self->GetDlgItem(nID);
	
	if(save) {
		value = static_cast<value_type>(::SendMessage(ctrl, TBM_GETPOS, 0, 0));
	} else {
		value_type min = static_cast<value_type>(::SendMessage(ctrl, TBM_GETRANGEMIN, 0, 0));
		value_type max = static_cast<value_type>(::SendMessage(ctrl, TBM_GETRANGEMAX, 0, 0));
		
		if(value < min || value > max) {
			ATLTRACE2(atlTraceUI, 0, _T("ATL: Warning - dialog data trackbar value (%d) out of range.\n"), value);		
			value = min;
		}
		
		::SendMessage(ctrl, TBM_SETPOS, true, static_cast<LPARAM>(value));
	}
	
	return 1;
}

template<typename this_type, typename value_type>
int DDX_ComboBox(this_type* self, UINT nID, value_type& value, int save) {
	return 0;
}

class ConfigDialog : public preferences_page_instance,
                     public ATL::CDialogImpl<ConfigDialog>,
                     public WTL::CWinDataExchange<ConfigDialog> {
	private:
		typedef ATL::CDialogImpl<ConfigDialog> parent_type;
		typedef WTL::CWinDataExchange<ConfigDialog> dde_type;
	
	public:
		static const int IDD = IDD_CONFIGURERATING;
		static const GUID GUID;
	
	private:
		//cannot do this? how to get at rating service?
//		static_api_ptr_t<RatingFieldProvider> _ratingApi;
//		RatingPreview _ratingPreview;
	
	public:
		ConfigDialog(preferences_page_callback::ptr p) {}
	
	private:
		LRESULT onInitDialog(HWND focus, LPARAM lparam);
		LRESULT onDestroy();
		LRESULT onControlChange(UINT code, int id, HWND ctrl);
	
	public:
		BEGIN_MSG_MAP_EX(ConfigDialog)
			MSG_WM_INITDIALOG(this->onInitDialog)
			MSG_WM_DESTROY(this->onDestroy)
		END_MSG_MAP()
	
	public:
		void emitRatingChanged() const;
		
	
	public: //foobar::preferences_page methods
		virtual HWND create(HWND parent);
		//! Queries whether this page supports "reset page" feature.
		virtual bool reset_query() { return false; }
		//! Activates "reset page" feature. It is safe to assume that the preferences page dialog does not exist at the point this is called (caller destroys it before calling reset and creates it again afterwards).
		virtual void reset();

		virtual void apply() {}
		virtual t_uint32 get_state() { return 0; }

};

class ConfigDialogImpl : public preferences_page_impl<ConfigDialog> {
	public:
		//! Retrieves name of the prefernces page to be displayed in preferences tree (static string).
		virtual const char* get_name();
		//! Retrieves GUID of the page.
		virtual ::GUID get_guid() { return ConfigDialog::GUID; }
		//! Retrieves GUID of parent page/branch of this page. See preferences_page::guid_* constants for list of standard parent GUIDs. Can also be a GUID of another page or a branch (see: preferences_branch).
		virtual ::GUID get_parent_guid() { return guid_tools; }

};

} //~namespace auto_rating

#endif //FOO_DAR_INCLUDE_DAR_CONFIG_DIALOG_H
