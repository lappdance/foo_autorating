#ifndef FOO_DAR_INCLUDE_DAR_RATING_FIELD_H
#define FOO_DAR_INCLUDE_DAR_RATING_FIELD_H

#pragma once

namespace auto_rating {

template <typename provider_type>
struct FieldInfo {
	typedef bool (provider_type::*eval_func)(metadb_handle*, titleformat_text_out*);
	
	const char* name;
	eval_func func;
};

class RatingFieldProvider : public metadb_display_field_provider {
	private:
		typedef FieldInfo<RatingFieldProvider> field_info_type;
		
	private:
		static const field_info_type _fieldInfo[];
		
		//there are two playback stat components, which use different variable names.
		//I cannot tell which plugin is loaded, so I must prepare for either var name.
		static titleformat_object::ptr _playcount;
		static titleformat_object::ptr _playcount2;
		static titleformat_object::ptr _added;
		static titleformat_object::ptr _firstplayed;
		static titleformat_object::ptr _lastplayed;
		static titleformat_object::ptr _rating;
		
		static time_t _now;
	
	private:
		static bool scriptsAreLoaded();
		static bool loadScripts();

	private:
		bool calculateRating(metadb_handle* handle, titleformat_text_out* out);
		bool calculateHotness(metadb_handle* handle, titleformat_text_out* out);
	
	public: //metadb_display_field_provider methods
		virtual t_uint32 get_field_count();
		
		virtual void get_field_name(t_uint32 index, pfc::string_base& out);
		
		/**
		 Evaluates the specified field.
		 @param index Index of field being processed : 0 <= index < get_field_count().
		 @param handle Handle to item being processed. You can safely call "locked" methods on this handle to retrieve track information and such.
		 @param out Interface receiving your text output.
		 @returns Return true to indicate that the field is present so if it's enclosed in square brackets, contents of those brackets should not be skipped, false otherwise.
		**/
		virtual bool process_field(t_uint32 index, metadb_handle* handle,
		                           titleformat_text_out* out);
	
	public:
		/**
		 Calculates a file's rating.
		 @param [in] file The file to rate.
		 @return The file's rating, [0..infinity]. There is no upper bound to the
		         rating formula, but it practice it will likely not go far above 20 000.
		         If the return value is < 0, there was an error and the rating cannot
		         be calculated: the handle is invalid, or, library data is not loaded,
		         etc.
		**/
		int getRating(metadb_handle* file) const;
		/**
		 Calculate a file's "hotness", how actively played it is.
		 @param [in] file The file to rate.
		 @return The file's hotness, [0..100]. It should be capped to 100, anyway.
		         I'll have to experiment with the algorithm until I understand it.
				 If the return value is < 0, there was an error and the hotness cannot
		         be calculated: the handle is invalid, or, library data is not loaded,
		         etc.
		**/
		int getHotness(metadb_handle* file) const;
};

class RatingPreview {
};

} //~namespace auto_rating

#endif //FOO_DAR_INCLUDE_DAR_RATING_FIELD_H
