#ifndef FOO_DAR_INCLUDE_DAR_VARIABLES_H
#define FOO_DAR_INCLUDE_DAR_VARIABLES_H

#pragma once

namespace auto_rating {

typedef cfg_int_t<float> cfg_float;

extern cfg_bool g_haveMinimumAge;

extern cfg_int g_lengthIntercept;
extern cfg_float g_lengthModifier;
extern cfg_int g_longTrackModifier;
extern cfg_int g_predictionModifier;
extern cfg_float g_agePenalty;
extern cfg_int g_delayPenalty;

} //~namespace auto_rating

#endif //FOO_DAR_INCLUDE_DAR_VARIABLES_H
