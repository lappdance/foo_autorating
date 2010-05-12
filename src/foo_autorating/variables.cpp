#include"stdafx.h"
#include"variables.h"
using namespace auto_rating;

namespace auto_rating {

// {50A55C18-BE3B-4a2f-9372-93E6E7336E4F}
static const GUID GUID_lengthModifierAdd = 
{ 0x50a55c18, 0xbe3b, 0x4a2f, { 0x93, 0x72, 0x93, 0xe6, 0xe7, 0x33, 0x6e, 0x4f } };
cfg_int g_lengthIntercept(GUID_lengthModifierAdd, 60);

// {7C6895BC-5D8B-48c9-97F9-787A274C0F16}
static const GUID GUID_lengthModifierMul = 
{ 0x7c6895bc, 0x5d8b, 0x48c9, { 0x97, 0xf9, 0x78, 0x7a, 0x27, 0x4c, 0xf, 0x16 } };
cfg_float g_lengthModifier(GUID_lengthModifierMul, 0.75f);

// {3D4C8B33-9384-492d-8D0B-A8EEEB453AAE}
static const GUID GUID_rateOutsideLibrary = 
{ 0x3d4c8b33, 0x9384, 0x492d, { 0x8d, 0xb, 0xa8, 0xee, 0xeb, 0x45, 0x3a, 0xae } };
cfg_bool g_rateOutsideLibrary(GUID_rateOutsideLibrary, false);

// {1EE52136-602D-460b-B5E8-89860397073F}
static const GUID GUID_haveMinimumAge = 
{ 0x1ee52136, 0x602d, 0x460b, { 0xb5, 0xe8, 0x89, 0x86, 0x3, 0x97, 0x7, 0x3f } };
cfg_bool g_haveMinimumAge(GUID_haveMinimumAge, true);

// {F15F33CD-99F9-458b-98BB-959D82C827DA}
static const GUID GUID_agePenalty = 
{ 0xf15f33cd, 0x99f9, 0x458b, { 0x98, 0xbb, 0x95, 0x9d, 0x82, 0xc8, 0x27, 0xda } };
cfg_float g_agePenalty(GUID_agePenalty, 1.25f);

// {B159D911-81AE-47e5-8A30-6EC9923312D9}
static const GUID GUID_delayPenalty = 
{ 0xb159d911, 0x81ae, 0x47e5, { 0x8a, 0x30, 0x6e, 0xc9, 0x92, 0x33, 0x12, 0xd9 } };
cfg_int g_delayPenalty(GUID_delayPenalty, 5);

// {0C79E1C4-C761-4b7e-B3B6-E12FC55FC471}
static const GUID GUID_predict = 
{ 0xc79e1c4, 0xc761, 0x4b7e, { 0xb3, 0xb6, 0xe1, 0x2f, 0xc5, 0x5f, 0xc4, 0x71 } };

cfg_int g_predictionModifier(GUID_predict, 40);

} //~namespace auto_rating
