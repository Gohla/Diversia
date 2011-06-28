/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

Original file used from the OGRE project, license:
-----------------------------------------------------------------------------
TidalEngine Source Code
http://www.tidalware.com/
Copyright (C) 2008-2009 TidalWare
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_STRINGUTIL_H
#define DIVERSIA_UTIL_STRINGUTIL_H

#include "Util/Math/Math.h"

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
typedef std::vector<String> StringVector;
// typedef Helper::SmartPtr<StringVector> StringVectorPtr;
//------------------------------------------------------------------------------
class DIVERSIA_UTIL_API StringUtil
{
public:
	typedef StringStream StrStreamType;

	/** Converts a Real to a String. */
	static String toString(Real val, unsigned short precision = 6,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
	/** Converts a Radian to a String. */
	static String toString(Radian val, unsigned short precision = 6,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) )
	{
		return toString(val.valueAngleUnits(), precision, width, fill, flags);
	}
	/** Converts a Degree to a String. */
	static String toString(Degree val, unsigned short precision = 6,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) )
	{
		return toString(val.valueAngleUnits(), precision, width, fill, flags);
	}
	/** Converts an int to a String. */
	static String toString(int val, unsigned short width = 0,
		char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
#if DIVERSIA_ARCH_TYPE == DIVERSIA_ARCHITECTURE_64 || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE
	/** Converts an unsigned int to a String. */
	static String toString(unsigned int val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
	/** Converts a size_t to a String. */
	static String toString(size_t val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
	#if DIVERSIA_COMPILER == DIVERSIA_COMPILER_MSVC
			/** Converts an unsigned long to a String. */
			static String toString(unsigned long val,
				unsigned short width = 0, char fill = ' ',
				std::ios::fmtflags flags = std::ios::fmtflags(0) );
	#endif
#else
	/** Converts a size_t to a String. */
	static String toString(size_t val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
	/** Converts an unsigned long to a String. */
	static String toString(unsigned long val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
#endif
	/** Converts a long to a String. */
	static String toString(long val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
	/**
	 * Converts a boolean to a String.
	 *
	 * @param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
	 */
	static String toString(bool val, bool yesNo = false);
	/**
	 * Converts a Vector2 to a String.
	 * Format is "x y" (i.e. 2x Real values, space delimited)
	 */
	static String toString(const Vector2& val);
	/**
	 * Converts a Vector3 to a String.
	 * Format is "x y z" (i.e. 3x Real values, space delimited)
	 */
	static String toString(const Vector3& val);
	/**
	 * Converts a Vector4 to a String.
	 * Format is "x y z w" (i.e. 4x Real values, space delimited)
	 */
	static String toString(const Vector4& val);
	/**
	 * Converts a Matrix3 to a String.
	 * Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
	 */
	static String toString(const Matrix3& val);
	/**
	 * Converts a Matrix4 to a String.
	 * Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where
	 *  '01' means row 0 column 1 etc.
	 */
	static String toString(const Matrix4& val);
	/**
	 * Converts a Quaternion to a String.
	 * Format is "w x y z" (i.e. 4x Real values, space delimited)
	 */
	static String toString(const Quaternion& val);
	/**
	 * Converts a Colour to a String.
	 * Format is "r g b a" (i.e. 4x Real values, space delimited).
	 */
	static String toString(const Colour& val);
    /**
     * Convenience function for when you are converting something you do not know the type for
     * and the type could be a string.
    **/
    inline static String toString(const String& val) { return val; }

	/** Converts a String to a Real.
	@returns
		0.0 if the value could not be parsed, otherwise the Real version of the String.
	*/
	static Real parseReal(const String& val);
	/** Converts a String to a Angle.
	@returns
		0.0 if the value could not be parsed, otherwise the Angle version of the String.
	*/
	static inline Radian parseAngle(const String& val) {
		return Angle(parseReal(val));
	}
	/** Converts a String to a whole number.
	@returns
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
	*/
	static int parseInt(const String& val);
	/** Converts a String to a whole number.
	@returns
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
	*/
	static unsigned int parseUnsignedInt(const String& val);
	/** Converts a String to a whole number.
	@returns
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
	*/
	static long parseLong(const String& val);
	/** Converts a String to a whole number.
	@returns
		0.0 if the value could not be parsed, otherwise the numeric version of the String.
	*/
	static unsigned long parseUnsignedLong(const String& val);
	/** Converts a String to a boolean.
	@remarks
		Returns true if case-insensitive match of the start of the string
		matches "true", "yes" or "1", false otherwise.
	*/
	static bool parseBool(const String& val);
	/** Parses a Vector2 out of a String.
	@remarks
		Format is "x y" ie. 2 Real components, space delimited. Failure to parse returns
		Vector2::ZERO.
	*/
	static Vector2 parseVector2(const String& val);
	/** Parses a Vector3 out of a String.
	@remarks
		Format is "x y z" ie. 3 Real components, space delimited. Failure to parse returns
		Vector3::ZERO.
	*/
	static Vector3 parseVector3(const String& val);
	/** Parses a Vector4 out of a String.
	@remarks
		Format is "x y z w" ie. 4 Real components, space delimited. Failure to parse returns
		Vector4::ZERO.
	*/
	static Vector4 parseVector4(const String& val);
	/** Parses a Matrix3 out of a String.
	@remarks
		Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
		Failure to parse returns Matrix3::IDENTITY.
	*/
	static Matrix3 parseMatrix3(const String& val);
	/** Parses a Matrix4 out of a String.
	@remarks
		Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where
		'01' means row 0 column 1 etc. Failure to parse returns Matrix4::IDENTITY.
	*/
	static Matrix4 parseMatrix4(const String& val);
	/** Parses a Quaternion out of a String.
	@remarks
		Format is "w x y z" (i.e. 4x Real values, space delimited).
		Failure to parse returns Quaternion::IDENTITY.
	*/
	static Quaternion parseQuaternion(const String& val);
	/** Parses a Colour out of a String.
	@remarks
		Format is "r g b a" (i.e. 4x Real values, space delimited), or "r g b" which implies
		an alpha value of 1.0 (opaque). Failure to parse returns Colour::Black.
	*/
	static Colour parseColour(const String& val);

	/**
	 * Checks the String is a valid number value.
	 *
	 * @param val - The string to check.
	 */
	static bool isNumber(const String& val);

	/**
	 * Remove leading and trailing spaces from a string.
	 *
	 * @param str - The string to trim.
	 * @param left - Remove it on the left side.
	 * @param right - Remove it on the right side.
	 * \return void (changed string)
	*/
	static std::string trim(const std::string& str, bool left, bool right);

	/**
	 * Returns true or false whether it starts with a argument you gave.
	 *
	 * @param checkstr - The string to check.
	 * @param pattern - The pattern to check if it starts with.
	 * @param csensitive - Case sensitivity.
	 * \return true if it does start with the pattern given, false if not.
	*/
	static bool startsWith(const std::string& checkstr, const std::string& pattern, bool csensitive = true);

	/**
	 * Returns true or false whether it ends with a argument you gave.
	 *
	 * @param checkstr - The string to check.
	 * @param pattern - The pattern to check if it starts with.
	 * @param csensitive - Case sensitivity.
	 * \return true if it does end with the pattern given, false if not.
	*/
	static bool endsWith(const std::string& checkstr, const std::string& pattern, bool csensitive = true);

	/**
	 * Checks if a string contains character.
	 *
	 * @param str1 - The string to check on.
	 * @param str2 - The string to check if its found in str1.
	 * \return true if it does contain the character, false if not.
	*/
	static bool contains(const std::string &str1, const std::string &str2);

	/**
	 * Returns a StringVector that contains all the substrings delimited
	 *  by the characters in the passed <code>delims</code> argument.
	 *
	 * @param delims - A list of delimiter characters to split by.
	 * @param maxSplits - The maximum number of splits to perform (0 for unlimited splits). If this
	 *  parameters is > 0, the splitting process will stop after this many splits, left to right.
	 * \return a vector with the parts.
	 */
	static std::vector<String> split(const String& str, const String& delims = "\t\n ", unsigned int maxSplits = 0);

	/**
	 * Returns the first character in the string.
	 *
	 * @param firstchar - The string where the first char must be recovered.
	 * \return a string with the character in there.
	*/
	static std::string getFirst(const std::string& firstchar);

	/**
	 * Returns the last character in the string.
	 *
	 * @param lastchar - The string where the last char must be recovered.
	 * \return a string with the character in there.
	*/
	static std::string getLast(const std::string& lastchar);

	/**
	 * Appends a int value to the string value given.
	 *
	 * @param appendTo - The string to append to.
	 * @param appendWith - The int value to append to the string.
	 * \return string containing the string plus the int value.
	*/
	static std::string appendInt(const std::string& appendTo, int appendWith);

	/**
	 * Appends a float value to the string value given.
	 *
	 * @param appendTo - The string to append to.
	 * @param appendWith - The float value to append to the string.
	 * \return string containing the string plus the float value.
	*/
	static std::string appendFloat(const std::string& appendTo, float appendWith);

	/**
	 * Appends a char value to the string given.
	 *
	 * @param appendTo - The string to append to.
	 * @param appendWith - The char to append to the string.
	 * \return string containing the string plus the int char value.
	*/
	static std::string appendChar(const std::string& appendTo, char appendWith);

	/**
	 * Deletes a amount of characters starting on the left side.
	 *
	 * @param delstring - The string to delete from.
	 * @param delamount - The amount of characters to delete.
	 * \return the string with the deleted characters.
	*/
	static std::string deleteLeft(const std::string& delstring, size_t delamount);

	/**
	 * Deletes a amount of characters starting on the right side.
	 *
	 * @param delstring - The string to delete from.
	 * @param delamount - The amount of characters to delete.
	 * \return the string with the deleted characters.
	*/
	static std::string deleteRight(const std::string& delstring, size_t delamount);

	/**
	 * Replace characters in a string with other characters.
	 *
	 * @param str1 - The string where characters are going to be replaced.
	 * @param replace - Replace this character...
	 * @param replaceWith - With this one.
	 * \return the string with the replaced characters.
	*/
	static std::string replace(const std::string& str1, const std::string& replace, const std::string& replaceWith);

	/**
	 * Makes the first character of a string upper case.
	 *
	 * @param str - The string where the first character becomes upper case.
	 * \return the string with the upper case character.
	*/
	static std::string upperCaseFirstChar(const std::string& str);

	/**
	* Makes the first character of a string lower case.
	*
	* @param str - The string where the first character becomes lower case.
	* \return the string with the lower case character.
	*/
	static std::string lowerCaseFirstChar(const std::string& str);

	/**
	* Transforms a string into upper case characters.
	*
	* @param TString - The string to convert.
	* \return the string containing upper case characters.
	*/
	static std::string toUpperCase(const std::string& TString);

	/**
	 * Transforms a string into lower case characters.
	 *
	 * @param TString - The string to convert.
	 * \return the string containing lower case characters.
	*/
	static std::string toLowerCase(const std::string& TString);

	/**
	* Transforms a string into lower case characters.
	*
	* @param TString - The string to convert.
	*/
	static void toLowerCase(std::string& TString);

	/**
	 * Returns if the string is in upper case.
	 *
	 * @param str - The string to check.
	 * \return true if it is in upper case, false if not.
	*/
	static bool isUpper(const std::string& str);

	/**
	 * Returns if the string is in lower case.
	 *
	 * @param str - The string to check.
	 * \return true if it is in lower case, false if not.
	*/
	static bool isLower(const std::string& str);
}; // class StringUtil
//------------------------------------------------------------------------------
} // namespace Helper
} // namespace Tide
//------------------------------------------------------------------------------

#endif // DIVERSIA_STRINGUTIL_H