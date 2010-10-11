#include"stdafx.h"
#include"config_dialog.h"
#include"resource.h"
#include"string_resource.h"
#include"variables.h"
#include"update_callback.h"
using namespace auto_rating;
using std::wstring;

namespace auto_rating {

static preferences_page_factory_t<ConfigDialogImpl> g_impl_factory;

} //~namespace auto_rating

// {20FF92BD-146A-413f-A3F6-034B66B21079}
const GUID ConfigDialog::GUID =
{ 0x20ff92bd, 0x146a, 0x413f, { 0xa3, 0xf6, 0x3, 0x4b, 0x66, 0xb2, 0x10, 0x79 } };

LRESULT ConfigDialog::onInitDialog(HWND focus, LPARAM lparam) {
	static const struct {
		UINT ctrlID;
		UINT stringID;
	} controls[] = {
		{ IDC_CRITERIA, IDS_CRITERIA },
		{ IDC_IMPORTANTTEXT, IDS_IMPORTANT },
		{ IDC_NOTIMPORTANTTEXT, IDS_NOTIMPORTANT },
		{ IDC_LENGTHTEXT, IDS_LENGTH },
		{ IDC_AGETEXT, IDS_AGE },
		{ IDC_DELAYTEXT, IDS_DELAY },
		{ IDC_PREDICTTEXT, IDS_PREDICT },
		{ IDC_USEMINIMUMAGE, IDS_MINIMUMAGE },
		{ IDC_RATEOUTSIDELIBRARY, IDS_RATEOUTSIDELIBRARY }
	};
	
	HINSTANCE instance = ATL::_AtlBaseModule.GetResourceInstance();
	for(int i=0; i<ARRAYSIZE(controls); ++i) {
		const wstring str = loadString(instance, controls[i].stringID);
		this->GetDlgItem(controls[i].ctrlID).SetWindowText(str.c_str());
	}
	
	WTL::CTrackBarCtrl track = this->GetDlgItem(IDC_LENGTH);
	track.SetRange(0, 6);
	track.SetPos((g_lengthIntercept - 1) / 180);
	
	track = this->GetDlgItem(IDC_AGE);
	track.SetRange(0, 10);
	track.SetPos(static_cast<int>((g_agePenalty - 1.0f) * 10));

	track = this->GetDlgItem(IDC_DELAY);
	track.SetRange(0, 8);
	track.SetPos(g_delayPenalty / 50 - 6);
	
	track = this->GetDlgItem(IDC_PREDICT);
	track.SetRange(0, 8);
	track.SetPos(g_predictionModifier / 10 - 2);
	
	this->CheckDlgButton(IDC_USEMINIMUMAGE, g_haveMinimumAge);
	
	return true;
}

LRESULT ConfigDialog::onDestroy() {
	g_haveMinimumAge = this->IsDlgButtonChecked(IDC_USEMINIMUMAGE) != 0;
	
	WTL::CTrackBarCtrl track = this->GetDlgItem(IDC_AGE);
	g_agePenalty = 1.0f + (track.GetPos() / 10.0f);
	
	track = this->GetDlgItem(IDC_DELAY);
	g_delayPenalty = (track.GetPos() + 6) * 50;
	
	track = this->GetDlgItem(IDC_PREDICT);
	g_predictionModifier = (track.GetPos() + 2) * 10;
	
	track = this->GetDlgItem(IDC_LENGTH);
	g_lengthIntercept = (track.GetPos() + 1) * 180;
	g_lengthModifier = (180.0f + g_lengthIntercept) / 180.0f;
	g_longTrackModifier = 12 * g_lengthIntercept + 4000;
	
	this->emitRatingChanged();
	
	return 0;
}

void ConfigDialog::emitRatingChanged() const {
	UpdateCallback::registerCallback();
}

const char* ConfigDialogImpl::get_name() { return "Auto Rating"; }

void ConfigDialog::reset() {}

HWND ConfigDialog::create(HWND) { return NULL; }