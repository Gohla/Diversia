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

#ifndef DIVERSIA_UTIL_STLHELPERS_H
#define DIVERSIA_UTIL_STLHELPERS_H

#include "Util/Platform/Prerequisites.h"

//------------------------------------------------------------------------------
namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------
/**
 * Wrapper for std::find, returns true if
 *  value is in container, false otherwise.
 */
template <class Container, class Value>
_INLINE bool contains(Container &container, Value &value)
{
	return (std::find(
		container.begin(), container.end(), value) != container.end());
}
//------------------------------------------------------------------------------
template <class Iterator, class Value>
_INLINE bool contains(Iterator &begin, Iterator &end, Value &value)
{
	return (std::find(begin, end, value) != end);
}
//------------------------------------------------------------------------------
/**
 * Wrapper for std::find, returns the first
 *  iterator for which *iterator == value or
 *  container.end() if value is not in container.
 */
template <class Container, class Value>
_INLINE typename Container::iterator find(Container &container, Value &value)
{
	return std::find(container.begin(), container.end(), value);
}
//------------------------------------------------------------------------------
template <class Iterator, class Value>
_INLINE Iterator find(Iterator &begin, Iterator &end, Value &value)
{
	return std::find(begin, end, value);
}
//------------------------------------------------------------------------------
/**
 * Wrapper for std::count, returns the number
 *  of items for which *iterator == value.
 */
template <class Container, class Value>
_INLINE int count(Container &container, Value &value)
{
	return std::count(container.begin(), container.end(), value);
}
//------------------------------------------------------------------------------
template <class Iterator, class Value>
_INLINE int count(Iterator &begin, Iterator &end, Value &value)
{
	return std::count(begin, end, value);
}
//------------------------------------------------------------------------------
/**
 * Wrapper for std::for_each, returns the function
 *  object after it has been applied to all elements
 *  in container.
 */
template <class Container, class UnaryFunction>
_INLINE UnaryFunction for_each(Container container, UnaryFunction function)
{
	return std::for_each(container.begin(), container.end(), function);
}
//------------------------------------------------------------------------------
template <class Iterator, class UnaryFunction>
_INLINE UnaryFunction for_each(
	Iterator &begin, Iterator &end, UnaryFunction function)
{
	return std::for_each(begin, end, function);
}
//------------------------------------------------------------------------------
/**
 * Stores both the current value, and the last value before that
 *  in one convenient class.
 */
template <typename Type>
class ValueBuffer
{
public:
	typedef Type value_type;

	value_type			current;
	value_type			previous;

	ValueBuffer(value_type current = 
		value_type(), value_type previous = value_type())
		: current(current), previous(previous) {};

	void operator =(value_type value)
	{
		previous = current;
		current = value;
	}

	operator value_type()
	{
		return current;
	}
}; // class ValueBuffer
//------------------------------------------------------------------------------
} // namespace Helper
//------------------------------------------------------------------------------
// Not in namespace helper, because it's used too often.
template <typename Type>
_INLINE void safe_delete(Type *& pointer)
{
	delete pointer;
	pointer = 0;
}
//------------------------------------------------------------------------------
template<class Type, template <typename> class PTR>
_INLINE void safe_delete(PTR<Type> &pointer)
{
	pointer.reset();
}
//------------------------------------------------------------------------------
} // namespace Tide
//------------------------------------------------------------------------------

#endif // DIVERSIA_UTIL_STLHELPERS_H