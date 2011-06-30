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

#include "Util/Platform/StableHeaders.h"

#include "Util/Helper/StringUtil.h"
#include "Util/Math/Vector2.h"
#include "Util/Math/Vector3.h"
#include "Util/Math/Vector4.h"
#include "Util/Math/Matrix3.h"
#include "Util/Math/Matrix4.h"
#include "Util/Math/Quaternion.h"
#include "Util/Math/Colour.h"

using namespace std;

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
String StringUtil::toString(Real val, unsigned short precision, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.precision(precision);
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(int val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------
#if DIVERSIA_ARCH_TYPE == DIVERSIA_ARCHITECTURE_64 || DIVERSIA_PLATFORM == DIVERSIA_PLATFORM_APPLE	
String StringUtil::toString(unsigned int val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------
/*String StringUtil::toString(size_t val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}*/
//------------------------------------------------------------------------------
#if DIVERSIA_COMPILER == DIVERSIA_COMPILER_MSVC
String StringUtil::toString(unsigned long val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
#endif
//------------------------------------------------------------------------------
#else
String StringUtil::toString(size_t val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(unsigned long val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
#endif
//------------------------------------------------------------------------------
String StringUtil::toString(long val, 
	unsigned short width, char fill, std::ios::fmtflags flags)
{
	stringstream stream;
	stream.width(width);
	stream.fill(fill);
	if (flags)
		stream.setf(flags);
	stream << val;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Vector2& val)
{
	stringstream stream;
	stream << val.x << " " << val.y;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Vector3& val)
{
	stringstream stream;
	stream << val.x << " " << val.y << " " << val.z;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Vector4& val)
{
	stringstream stream;
	stream << val.x << " " << val.y << " " << val.z << " " << val.w;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Matrix3& val)
{
	stringstream stream;
	stream << val[0][0] << " " 
		<< val[0][1] << " "			 
		<< val[0][2] << " "			 
		<< val[1][0] << " "			 
		<< val[1][1] << " "			 
		<< val[1][2] << " "			 
		<< val[2][0] << " "			 
		<< val[2][1] << " "			 
		<< val[2][2];
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(bool val, bool yesNo)
{
	if (val)
	{
		if (yesNo)
		{
			return "yes";
		}
		else
		{
			return "true";
		}
	}
	else
		if (yesNo)
		{
			return "no";
		}
		else
		{
			return "false";
		}
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Matrix4& val)
{
	stringstream stream;
	stream << val[0][0] << " " 
		<< val[0][1] << " "			 
		<< val[0][2] << " "			 
		<< val[0][3] << " "			 
		<< val[1][0] << " "			 
		<< val[1][1] << " "			 
		<< val[1][2] << " "			 
		<< val[1][3] << " "			 
		<< val[2][0] << " "			 
		<< val[2][1] << " "			 
		<< val[2][2] << " "			 
		<< val[2][3] << " "			 
		<< val[3][0] << " "			 
		<< val[3][1] << " "			 
		<< val[3][2] << " "			 
		<< val[3][3];
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Quaternion& val)
{
	stringstream stream;
	stream  << val.w << " " << val.x << " " << val.y << " " << val.z;
	return stream.str();
}
//------------------------------------------------------------------------------
String StringUtil::toString(const Colour& val)
{
	stringstream stream;
	stream << val.r << " " << val.g << " " << val.b << " " << val.a;
	return stream.str();
}
//------------------------------------------------------------------------------
Real StringUtil::parseReal(const String& val)
{
	// Use istringstream for direct correspondence with toString
	StringStream str(val);
	Real ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------
int StringUtil::parseInt(const String& val)
{
	// Use istringstream for direct correspondence with toString
	StringStream str(val);
	int ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------
unsigned int StringUtil::parseUnsignedInt(const String& val)
{
	// Use istringstream for direct correspondence with toString
	StringStream str(val);
	unsigned int ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------
long StringUtil::parseLong(const String& val)
{
	// Use istringstream for direct correspondence with toString
	StringStream str(val);
	long ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------
unsigned long StringUtil::parseUnsignedLong(const String& val)
{
	// Use istringstream for direct correspondence with toString
	StringStream str(val);
	unsigned long ret = 0;
	str >> ret;

	return ret;
}
//------------------------------------------------------------------------------
bool StringUtil::parseBool(const String& val)
{
	return (StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
		|| StringUtil::startsWith(val, "1"));
}
//------------------------------------------------------------------------------
Vector2 StringUtil::parseVector2(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() != 2)
	{
		return Vector2::ZERO;
	}
	else
	{
		return Vector2(parseReal(vec[0]),parseReal(vec[1]));
	}
}
//------------------------------------------------------------------------------
Vector3 StringUtil::parseVector3(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() != 3)
	{
		return Vector3::ZERO;
	}
	else
	{
		return Vector3(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]));
	}
}
//------------------------------------------------------------------------------
Vector4 StringUtil::parseVector4(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() != 4)
	{
		return Vector4::ZERO;
	}
	else
	{
		return Vector4(
			parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]),
			parseReal(vec[3]));
	}
}
//------------------------------------------------------------------------------
Matrix3 StringUtil::parseMatrix3(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() != 9)
	{
		return Matrix3::IDENTITY;
	}
	else
	{
		return Matrix3(parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]),
			parseReal(vec[3]),parseReal(vec[4]),parseReal(vec[5]),
			parseReal(vec[6]),parseReal(vec[7]),parseReal(vec[8]));
	}
}
//------------------------------------------------------------------------------
Matrix4 StringUtil::parseMatrix4(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() != 16)
	{
		return Matrix4::IDENTITY;
	}
	else
	{
		return Matrix4(
			parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 
			parseReal(vec[3]), parseReal(vec[4]),parseReal(vec[5]), 
			parseReal(vec[6]), parseReal(vec[7]), parseReal(vec[8]),
			parseReal(vec[9]), parseReal(vec[10]), parseReal(vec[11]), 
			parseReal(vec[12]),parseReal(vec[13]), parseReal(vec[14]), 
			parseReal(vec[15]));
	}
}
//------------------------------------------------------------------------------
Quaternion StringUtil::parseQuaternion(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() != 4)
	{
		return Quaternion::IDENTITY;
	}
	else
	{
		return Quaternion(
			parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 
			parseReal(vec[3]));
	}
}
//------------------------------------------------------------------------------
Colour StringUtil::parseColour(const String& val)
{
	// Split on space
	std::vector<String> vec = StringUtil::split(val);

	if (vec.size() == 4)
	{
		return Colour(
			parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 
			parseReal(vec[3]));
	}
	else if (vec.size() == 3)
	{
		return Colour(
			parseReal(vec[0]),parseReal(vec[1]),parseReal(vec[2]), 1.0f);
	}
	else
	{
		return Colour::Black;
	}
}
//------------------------------------------------------------------------------
bool StringUtil::isNumber(const String& val)
{
	StringStream str(val);
	float tst;
	str >> tst;
	return !str.fail() && str.eof();
}
//------------------------------------------------------------------------------
std::string StringUtil::trim(const std::string& str, bool left, bool right)
{
	size_t lspaces, rspaces, len = str.length(), i;

	lspaces = rspaces = 0;

	if(left)
	{
		for(i = 0;
			i < len && (str[i] == ' ' || str[i] == '\t' || str[i] == '\r');
			++lspaces, ++i);
	}

	if(right && lspaces < len)
	{
		for(i = len - 1;
			i >= 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\r');
			rspaces++, i--);
	}

	return str.substr(lspaces, len-lspaces-rspaces);
}
//------------------------------------------------------------------------------
bool StringUtil::startsWith(
	const std::string& checkstr, const std::string& pattern, bool csensitive)
{
	if (checkstr.empty() == true || pattern.empty() == true)
	{
		return false;
	}
	std::string temp = checkstr.substr(0, pattern.length());

	if (csensitive == true)
	{
		if (temp == pattern)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		toLowerCase(temp);
		toLowerCase(pattern);
		if (temp == pattern)
		{
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
bool StringUtil::endsWith(
	const std::string& checkstr, const std::string& pattern, bool csensitive)
{
	if (checkstr.empty() == true || pattern.empty() == true)
	{
		return false;
	}
	std::string temp = 
		checkstr.substr(checkstr.length() - pattern.length(), pattern.length());

	if (csensitive == true)
	{
		if (temp == pattern)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		toLowerCase(temp);
		toLowerCase(pattern);
		if (temp == pattern)
		{
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
bool StringUtil::contains(const std::string &str1, const std::string &str2)
{
	if (str2.length() > 1)
	{
		return false;
	}
	size_t i = 0;

	for(i = str1.find(str2, 0); i != std::string::npos; i = str1.find(str2, i))
	{
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
std::vector<String> StringUtil::split(const String& str, const String& delims, unsigned int maxSplits)
{
	std::vector<String> ret;
	// Pre-allocate some space for performance
	ret.reserve(maxSplits ? maxSplits+1 : 10);	// 10 is guessed capacity for most case

	unsigned int numSplits = 0;

	// Use STL methods 
	size_t start, pos;
	start = 0;
	do 
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
		{
			// Copy the rest of the string
			ret.push_back( str.substr(start) );
			break;
		}
		else
		{
			// Copy up to delimiter
			ret.push_back( str.substr(start, pos - start) );
			start = pos + 1;
		}
		// parse up to next real data
		start = str.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != String::npos);

	return ret;
}
//------------------------------------------------------------------------------
std::string StringUtil::getFirst(const std::string& firstchar)
{
	return firstchar.substr(0, 1);
}
//------------------------------------------------------------------------------
std::string StringUtil::getLast(const std::string& lastchar)
{
	return lastchar.substr(lastchar.length()-1, lastchar.length());
}
//------------------------------------------------------------------------------
std::string StringUtil::appendInt(const std::string& appendTo, int appendWith)
{
	std::stringstream str;
	str << appendTo << appendWith;
	return str.str();
}
//------------------------------------------------------------------------------
std::string StringUtil::appendFloat(const std::string& appendTo, float appendWith)
{
	std::stringstream str;
	str << appendTo << appendWith;
	return str.str();
}
//------------------------------------------------------------------------------
std::string StringUtil::appendChar(const std::string& appendTo, char appendWith)
{
	std::stringstream str;
	str << appendTo << appendWith;
	return str.str();
}
//------------------------------------------------------------------------------
std::string StringUtil::deleteLeft(const std::string& delstring, size_t delamount)
{
	if (delamount >= delstring.length())
	{
		return delstring;
	}
	return delstring.substr(delamount, delstring.length());
}
//------------------------------------------------------------------------------
std::string StringUtil::deleteRight(const std::string& delstring, size_t delamount)
{
	if (delamount >= delstring.length())
	{
		return delstring;
	}
	return delstring.substr(0, delstring.length() - delamount);
}
//------------------------------------------------------------------------------
std::string StringUtil::replace(
	const std::string& str1, const std::string& replace, 
	const std::string& replaceWith)
{
	size_t i = 0;

	std::string copy = str1;

	for(i = str1.find(replace, 0); i != std::string::npos; i = str1.find(replace, i))
	{
		copy.erase(i, replace.length());
		copy.insert(i, replaceWith);
		i++;
	}
	return copy;
}
//------------------------------------------------------------------------------
std::string StringUtil::upperCaseFirstChar(const std::string& str)
{
	std::string rest = str.substr(1, str.length());
	std::string UpperLetter = toUpperCase(getFirst(str));

	return UpperLetter += rest;
}
//------------------------------------------------------------------------------
std::string StringUtil::lowerCaseFirstChar(const std::string& str)
{
	std::string rest = str.substr(1, str.length());
	std::string LowerLetter = toLowerCase(getFirst(str));

	return LowerLetter += rest;
}
//------------------------------------------------------------------------------
std::string StringUtil::toUpperCase(const std::string& TString) 
{
	std::string retval(TString);
	std::transform(retval.begin(), retval.end(), retval.begin(), 
		static_cast<int(*)(int)>(::toupper));
	return retval;
}
//------------------------------------------------------------------------------
std::string StringUtil::toLowerCase(const std::string& TString) 
{
	std::string retval(TString);
	std::transform(retval.begin(), retval.end(), retval.begin(), 
		static_cast<int(*)(int)>(::tolower));
	return retval;
}
//------------------------------------------------------------------------------
void StringUtil::toLowerCase(std::string& TString) 
{
	std::transform(TString.begin(), TString.end(), TString.begin(), 
		static_cast<int(*)(int)>(::tolower));
}
//------------------------------------------------------------------------------
bool StringUtil::isUpper(const std::string& str)
{
	return str == toUpperCase(str);
}
//------------------------------------------------------------------------------
bool StringUtil::isLower(const std::string& str)
{
	return str == toLowerCase(str);
}
//------------------------------------------------------------------------------
} // namespace Helper
} // namespace Tide
//------------------------------------------------------------------------------