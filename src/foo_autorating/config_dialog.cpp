#include"stdafx.h"
#include"config_dialog.h"
#include"resource.h"
#include"string_resource.h"
#include"variables.h"
#include"update_callback.h"
#define _USE_MATH_DEFINES 1
#include<math.h>
using namespace auto_rating;
using std::wstring;

namespace auto_rating {

preferences_page_factory_t<ConfigDialog> g_factory;

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
	track.SetRange(0, 8);
	track.SetPos(static_cast<int>(log(g_lengthIntercept / 2836.0) / -0.7));
		
	track = this->GetDlgItem(IDC_AGE);
	track.SetRange(0, 8);
	track.SetPos(static_cast<int>((g_agePenalty - 0.50f) / 0.125f));
	
	track = this->GetDlgItem(IDC_DELAY);
	track.SetRange(0, 8);
	track.SetPos(g_delayPenalty);
	
	track = this->GetDlgItem(IDC_PREDICT);
	track.SetRange(0, 8);
	track.SetPos((g_predictionModifier - 20) / 10);
	
	this->CheckDlgButton(IDC_USEMINIMUMAGE, g_haveMinimumAge);
	this->CheckDlgButton(IDC_RATEOUTSIDELIBRARY, g_rateOutsideLibrary);
	
	return true;
}

LRESULT ConfigDialog::onDestroy() {
	g_haveMinimumAge = this->IsDlgButtonChecked(IDC_USEMINIMUMAGE) != 0;
	g_rateOutsideLibrary = this->IsDlgButtonChecked(IDC_RATEOUTSIDELIBRARY) != 0;
	
	WTL::CTrackBarCtrl track = this->GetDlgItem(IDC_AGE);
	g_agePenalty = 0.50f + 0.125f * track.GetPos();
	
	track = this->GetDlgItem(IDC_DELAY);
	g_delayPenalty = track.GetPos();
	
	track = this->GetDlgItem(IDC_PREDICT);
	g_predictionModifier = 20 + 10 * track.GetPos();
	
	track = this->GetDlgItem(IDC_LENGTH);
	const int x = track.GetPos();
	//these equations are very strange.
	//they are based off of carpman's values, but fitted to a curve using powers of
	//@c e and polynomial equations.
	//I do not know how carpman arrived at the original values.
	g_lengthModifier = static_cast<float>(-0.0033 * x * x * x +
	                                       0.0395 * x * x +
	                                       0.0072 * x +
	                                       0.0622
	                                      );
	g_lengthIntercept = static_cast<int>(2836 * pow(M_E, x * -0.7));
	
	this->emitRatingChanged();
	
	return 0;
}

void ConfigDialog::emitRatingChanged() const {
	UpdateCallback::registerCallback();
}

HWND ConfigDialog::create(HWND parent) {
	return parent_type::Create(parent, 0L);
}

const char* ConfigDialog::get_name() { return "Auto Rating"; }

void ConfigDialog::reset() {}
