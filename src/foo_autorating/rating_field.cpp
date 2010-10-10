#include"stdafx.h"
#include"rating_field.h"
#include"variables.h"
#include<ctime>
using namespace auto_rating;
using pfc::string_base;
using pfc::string8;
using std::stringstream;

namespace auto_rating {

service_factory_single_t<RatingFieldProvider> g_factory;

struct ScriptInfo {
	const char* script;
	titleformat_object::ptr& obj;
};

time_t makeTime(const char* text) {
	//0         1    
	//0123456789012345678
	//2008-05-24 15:14:13
	
	if(!text || _mbslen(reinterpret_cast<const unsigned char*>(text)) < 19)
		return 0;
	
	std::string copy = text;
	static const unsigned int zeros[] = { 4, 7, 10, 13, 16 };
	for(unsigned int i=0; i<ARRAYSIZE(zeros); ++i)
		copy[zeros[i]] = ' ';
	
	tm time = {0};
	
	std::istringstream ss(copy);
	ss >> time.tm_year >> time.tm_mon >> time.tm_mday
	   >> time.tm_hour >> time.tm_min >> time.tm_sec;
	//the runtime library measures the year relative to 1900
	time.tm_year -= 1900;
	//the runtime library expects months to be 0-based, but the text string given
	//uses 1-base.
	time.tm_mon -= 1;
	//I do not know whether DST is in effect
	time.tm_isdst = -1;
	
	return mktime(&time);
}

} //~namespace auto_rating

const FieldInfo<RatingFieldProvider> RatingFieldProvider::_fieldInfo[] = {
	{ "auto_rating", &RatingFieldProvider::calculateRating },
	{ "hotness", &RatingFieldProvider::calculateHotness }
};

titleformat_object::ptr RatingFieldProvider::_playcount;
titleformat_object::ptr RatingFieldProvider::_playcount2;
titleformat_object::ptr RatingFieldProvider::_added;
titleformat_object::ptr RatingFieldProvider::_firstplayed;
titleformat_object::ptr RatingFieldProvider::_lastplayed;
titleformat_object::ptr RatingFieldProvider::_rating;

bool RatingFieldProvider::scriptsAreLoaded() {
	return _playcount.is_valid() &&
	       _playcount2.is_valid() &&
	       _added.is_valid() &&
	       _firstplayed.is_valid() &&
	       _lastplayed.is_valid() &&
		   _rating.is_valid();
}

bool RatingFieldProvider::loadScripts() {
	static const ScriptInfo scripts[] = {
		{ "[%play_count%]", _playcount },
		{ "[%play_counter%]", _playcount2 },
		{ "[%added%]", _added },
		{ "[%first_played%]", _firstplayed },
		{ "[%last_played%]", _lastplayed },
		{ "[%rating%]", _rating }
	};
	
	bool success = true;
	
	static_api_ptr_t<titleformat_compiler> compiler;
	for(unsigned int i=0; i<ARRAYSIZE(scripts); ++i) {
		success &= compiler->compile(scripts[i].obj, scripts[i].script);
	}
	
	return success;
}

bool RatingFieldProvider::calculateRating(metadb_handle* handle,
                                          titleformat_text_out* out) {	
	const int rating = this->getRating(handle);
	
	const bool haveResult = rating >= 0;
	if(haveResult) {
		out->write_int(titleformat_inputtypes::unknown, rating);
	}
	
	return haveResult;
}

bool RatingFieldProvider::calculateHotness(metadb_handle* handle,
                                          titleformat_text_out* out) {	
	const int hotness = this->getHotness(handle);
	
	const bool haveResult = hotness >= 0;
	if(haveResult) {
		out->write_int(titleformat_inputtypes::unknown, hotness);
	}
	
	return haveResult;
}

t_uint32 RatingFieldProvider::get_field_count() {
	return ARRAYSIZE(RatingFieldProvider::_fieldInfo);
}

void RatingFieldProvider::get_field_name(t_uint32 index, string_base& out) {
	out.reset();
	
	if(index < this->get_field_count())
		out = RatingFieldProvider::_fieldInfo[index].name;
}

bool RatingFieldProvider::process_field(t_uint32 index, metadb_handle* handle,
                                        titleformat_text_out* out) {
	if(!out || !handle)
		return false;
	
	if(index < this->get_field_count()) {
		const field_info_type& info = RatingFieldProvider::_fieldInfo[index];
		return (this->*info.func)(handle, out);
	} else {
		return false;
	}
}

int RatingFieldProvider::getRating(metadb_handle* file) const {
	//according to performance counter, titleformat lookup is between 30 and 40x
	//slower than actual math. In fact, may be more than 40 b/c I didn't time the
	//age lookup.
	//I absolutely must find a way to get at playback info w/o scripting.
	
	const file_info* info;
	const bool haveInfo = file->get_info_locked(info);

//	static_api_ptr_t<library_manager> library;
//	metadb_handle::ptr handlePtr = file;
//	const bool shouldRate = g_rateOutsideLibrary || library->is_item_in_library(handlePtr);
	const bool shouldRate = true;
	
	bool haveScripts = RatingFieldProvider::scriptsAreLoaded();
	if(!haveScripts)
		haveScripts = RatingFieldProvider::loadScripts();
	
	double rating = -1;

	if(haveInfo && haveScripts && shouldRate) {
		time_t now = time(NULL);
		
		string8 result;
		file->format_title_nonlocking(NULL, result, _firstplayed, NULL);
		time_t time = makeTime(result);
		
		//if the song has not been played, use the current time.
		//it is easier to set the number to a legitimate value than it is to
		//constantly check for an illegal one while calculating the rating.
		const time_t firstplayed = time ? time : now;
		//>= is the correct operator to use, not >.
		//in real life, a song will never have been played "right now", but if playback
		//stats aren't available, I use the current time as the first played time,
		//so the math below will return a meaningful answer.
		assert(now >= firstplayed && "song was played in future");
		
		file->format_title_nonlocking(NULL, result, _added, NULL);
		time = makeTime(result);
		//versions of foo_playcount older than 2.3 do not record when a track was
		//added to the library, and other statistic plugins may not record this value
		//at all. if the %added% tag does not exist, use the first played time.
		const time_t added = time ? time : firstplayed;
		assert(now >= added && "song was added in future");
		
		static const int secondsPerDay = 60 * 60 * 24;
		const double age = difftime(now, added) / secondsPerDay;
		assert(age >= 0 && "age cannot be negative");
		
		static const int minimumAge = 5;
		//if the song is less than a week old, there is not enough listening history
		//to examine to give a meaningful rating.
		if(!g_haveMinimumAge || (age > minimumAge)) {
			file->format_title_nonlocking(NULL, result, _lastplayed, NULL);
			time_t time = makeTime(result);

			//like the first play time, set this number to a legitimate value
			const time_t lastplayed = time ? time : now;
			assert(now >= lastplayed && "song was played in future");
			
			const double firstplayDelta = difftime(now, firstplayed) / secondsPerDay;
			const double lastplayDelta = difftime(now, lastplayed) / secondsPerDay;
			assert(firstplayDelta >= 0 && lastplayDelta >= 0 && "play times are not normalized");
			
			//use the official playback stat field, if it exists
			file->format_title_nonlocking(NULL, result, _playcount, NULL);
			//if the field resolves to nothing, try the unofficial playback stat field
			if(result.length() < 1)
				file->format_title_nonlocking(NULL, result, _playcount2, NULL);
			
			//@c result contains either a number > 0 or an empty string.
			//if the string is empty, @c atoi will return 0, so I do not need to check
			//@c result myself.
			const int playcount = atoi(result);

			const double length = info->get_length();
			//intercept, THEN slope
//			length += g_lengthIntercept;
//			length *= g_lengthModifier;

			const double d1 = (length + 540) * 0.25;
			static const double longBonus = 30;
			const double d2 = pow(length / 10 / longBonus, 2);
			const double d3 = d1 + d2;
			
			//do not allow a division by 0.
			//if the user is not using a pseudo-rating for newly-added tracks, it is
			//possible, though unlikely, that a new track's rating will be queried before
			//1 second has passed since it was added. Divisions by zero are bad, so I
			//will treat all tracks as though they are at least 1 second old.
			//@c age is measured in days, not seconds
			const double oneSecond = 1.0 / secondsPerDay;
			const double playsPerPeriod = 10000 * playcount / max(age, oneSecond);
			const double prediction = playsPerPeriod / g_predictionModifier;

			const double agePenalty = age * g_agePenalty + 625 / age;
			
			//prevent another division by 0.
			//it is entirely possible that the song has not been played before its
			//rating is queried, so check for 0 before dividing.
			const double delayPenalty = g_delayPenalty * (age - firstplayDelta) / max(playcount, 1) / (d3 / 6 + 70);
			
			//I don't understand what this value is
			const double dd = (age - lastplayDelta + 50) / 10;
			const double pd2 = dd * playsPerPeriod / 100;
			

			rating = 10000 + (length * playcount / 10);
			rating += prediction + pd2 - agePenalty - delayPenalty;
			rating *= (1 - length * lastplayDelta * playcount * 5 / pow(10.0,8));

			rating = max(rating, 0.0);
		} else {
			rating = 10000.0;
		}
	}
	
	return static_cast<int>(rating);
}

int RatingFieldProvider::getHotness(metadb_handle* file) const {
	//there are 24 hours in a day
	//@c multiplier makes things be measured in hours instead of days
	static const double multiplier = 24;
	static const double baseFrequency = 90 * multiplier;
	static const double baseDecay = 28 * multiplier;

	const time_t now = time(NULL);
	
	string8 result;
	file->format_title_nonlocking(NULL, result, _lastplayed, NULL);
	time_t time = makeTime(result);
	const time_t lastPlayed = time ? time : now;

	file->format_title_nonlocking(NULL, result, _firstplayed, NULL);
	time = makeTime(result);
	const time_t firstPlayed = time ? time : now;

	//use the official playback stat field, if it exists
	file->format_title_nonlocking(NULL, result, _playcount, NULL);
	//if the field resolves to nothing, try the unofficial playback stat field
	if(result.length() < 1)
		file->format_title_nonlocking(NULL, result, _playcount2, NULL);
	
	//@c result contains either a number > 0 or an empty string.
	//if the string is empty, @c atoi will return 0, so I do not need to check
	//@c result myself.
	const int plays = atoi(result);
	if(plays == 0)
		return 0;
	
	static const int avgrating = 3;
	file->format_title_nonlocking(NULL, result, _rating, NULL);
	const int rating = result.length() > 0 ? atoi(result) : avgrating;
	
	const double age = difftime(lastPlayed, firstPlayed) / 3600;
	const double recentness = difftime(now, lastPlayed) / 3600;
	//assume a minimum of 1 plays to avoid a divide by zero
	const double decay = max(plays, 1) * baseFrequency * baseDecay * rating * 100 / (max(age, baseFrequency) * avgrating) / 100;
	const double rawhotness = max(decay - recentness, 0.0) * 100 / decay;
	const double forecast = max(decay - (max(baseDecay - recentness, 0.0) / 2 + recentness), 0.0) * 100 / decay;
	const double hotness = (rawhotness + forecast) / 2;

	return static_cast<int>(hotness);
}
