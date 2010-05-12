#ifndef FOO_DAR_INCLUDE_DAR_STRING_RESOURCE_H
#define FOO_DAR_INCLUDE_DAR_STRING_RESOURCE_H

#pragma once

namespace auto_rating {

std::wstring loadString(HINSTANCE instance, UINT id, LANGID lang=::GetUserDefaultLangID());

size_t getStringLength(HINSTANCE instance, UINT id, LANGID lang=::GetUserDefaultLangID());

} //~namespace auto_rating

#endif //FOO_DAR_INCLUDE_DAR_STRING_RESOURCE_H
