/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

Original file used from the OGRE project, license:
-----------------------------------------------------------------------------
Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_UTIL_EXCEPTION_H
#define DIVERSIA_UTIL_EXCEPTION_H

#include "Util/Platform/Prerequisites.h"

#include <exception>

// RELEASE_EXCEPTIONS mode
#if DIVERSIA_ASSERT_MODE == 1
#   ifdef DIVERSIA_DEBUG
#       define DivAssert( a, b ) assert( (a) && (b) )
#   else
#       if DIVERSIA_COMPILER != DIVERSIA_COMPILER_BORL
#           define DivAssert( a, b ) if( !(a) ) DIVERSIA_EXCEPT( Diversia::Util::Exception::ERR_RT_ASSERTION_FAILED, (b), "no function info")
#       else
#           define DivAssert( a, b ) if( !(a) ) DIVERSIA_EXCEPT( Diversia::Util::Exception::ERR_RT_ASSERTION_FAILED, (b), __FUNC__ )
#       endif
#   endif

// EXCEPTIONS mode
#elif DIVERSIA_ASSERT_MODE == 2
#   if DIVERSIA_COMPILER != DIVERSIA_COMPILER_BORL
#       define DivAssert( a, b ) if( !(a) ) DIVERSIA_EXCEPT( Diversia::Util::Exception::ERR_RT_ASSERTION_FAILED, (b), "no function info")
#   else
#       define DivAssert( a, b ) if( !(a) ) DIVERSIA_EXCEPT( Diversia::Util::Exception::ERR_RT_ASSERTION_FAILED, (b), __FUNC__ )
#   endif

// STANDARD mode
#else
#   define DivAssert( a, b ) assert( (a) && (b) )
#endif

namespace Diversia
{
namespace Util
{
/** When thrown, provides information about an error that has occurred inside the engine.
    @remarks
        Diversia never uses return values to indicate errors. Instead, if an
        error occurs, an exception is thrown, and this is the object that
        encapsulates the detail of the problem. The application using
        Diversia should always ensure that the exceptions are caught, so all
        Diversia engine functions should occur within a
        try{} catch(Diversia::Util::Exception& e) {} block.
    @par
        The user application should never create any instances of this
        object unless it wishes to unify its error handling using the
        same object.
*/
class DIVERSIA_UTIL_API Exception : public std::exception
{
protected:
    long line;
    int number;
    String typeName;
    String description;
    String source;
    String file;
    mutable String fullDesc;
public:
    /** Static definitions of error codes.
        @todo
            Add many more exception codes, since we want the user to be able
            to catch most of them.
    */
    enum ExceptionCodes {
        ERR_CANNOT_WRITE_TO_FILE,
        ERR_INVALID_STATE,
        ERR_INVALIDPARAMS,
        ERR_RENDERINGAPI_ERROR,
        ERR_DUPLICATE_ITEM,
        ERR_ITEM_NOT_FOUND,
        ERR_FILE_NOT_FOUND,
        ERR_INTERNAL_ERROR,
        ERR_RT_ASSERTION_FAILED,
        ERR_PERMISSION_DENIED,
        ERR_PERMISSION_TOOMANYITEMS,
        ERR_PERMISSION_TOOMANYITEMSPERTIMEFRAME,
        ERR_PERMISSION_BOUNDSDENIED,
        ERR_SYNTAX_ERROR,
        ERR_NOT_IMPLEMENTED
    };

    /** Default constructor.
    */
    Exception( int number, const String& description, const String& source );

    /** Advanced constructor.
    */
    Exception( int number, const String& description, const String& source, const char* type, const char* file, long line );

    /** Copy constructor.
    */
    Exception(const Exception& rhs);

    /// Needed for  compatibility with std::exception
    ~Exception() throw() {}

    /** Assignment operator.
    */
    void operator = (const Exception& rhs);

    /** Returns a string with the full description of this error.
        @remarks
            The description contains the error number, the description
            supplied by the thrower, what routine threw the exception,
            and will also supply extra platform-specific information
            where applicable. For example - in the case of a rendering
            library error, the description of the error will include both
            the place in which OGRE found the problem, and a text
            description from the 3D rendering library, if available.
    */
    virtual const String& getFullDescription( bool unhandled = false ) const;

    /** Gets the error code.
    */
    virtual int getNumber() const throw();

    /** Gets the source function.
    */
    virtual const String& getSource() const { return source; }

    /** Gets source file name.
    */
    virtual const String& getFile() const { return file; }

    /** Gets line number.
    */
    virtual long getLine() const { return line; }

    /** Returns a string with only the 'description' field of this exception. Use
        getFullDescriptionto get a full description of the error including line number,
        error number and what function threw the exception.
    */
    virtual const String& getDescription() const { return description; }

    /// Override std::exception::what
    const char* what() const throw() { return getFullDescription().c_str(); }

    /**
    Logs the exception to an error message using the Util logger.
    **/
    void log() const;

    /**
    Logs the exception to an error message to a given logger.
    **/
    void log( boost::log::sources::severity_channel_logger_mt< Diversia::Util::LogLevel >& rLogger,
        bool unhandled = false ) const;
};

/** Template struct which creates a distinct type for each exception code.
@note
This is useful because it allows us to create an overloaded method
for returning different exception types by value without ambiguity.
From 'Modern C++ Design' (Alexandrescu 2001).
*/
template <int num>
struct DIVERSIA_UTIL_API ExceptionCodeType
{
    enum { number = num };
};

// Specialised exceptions allowing each to be caught specifically
// backwards-compatible since exception codes still used

class DIVERSIA_UTIL_API UnimplementedException : public Exception
{
public:
    UnimplementedException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "UnimplementedException", file, line) {}
};
class DIVERSIA_UTIL_API FileNotFoundException : public Exception
{
public:
    FileNotFoundException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "FileNotFoundException", file, line) {}
};
class DIVERSIA_UTIL_API IOException : public Exception
{
public:
    IOException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "IOException", file, line) {}
};
class DIVERSIA_UTIL_API InvalidStateException : public Exception
{
public:
    InvalidStateException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "InvalidStateException", file, line) {}
};
class DIVERSIA_UTIL_API InvalidParametersException : public Exception
{
public:
    InvalidParametersException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "InvalidParametersException", file, line) {}
};
class DIVERSIA_UTIL_API ItemIdentityException : public Exception
{
public:
    ItemIdentityException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "ItemIdentityException", file, line) {}
};
class DIVERSIA_UTIL_API InternalErrorException : public Exception
{
public:
    InternalErrorException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "InternalErrorException", file, line) {}
};
class DIVERSIA_UTIL_API RenderingAPIException : public Exception
{
public:
    RenderingAPIException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "RenderingAPIException", file, line) {}
};
class DIVERSIA_UTIL_API RuntimeAssertionException : public Exception
{
public:
    RuntimeAssertionException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "RuntimeAssertionException", file, line) {}
};
class DIVERSIA_UTIL_API PermissionDeniedException : public Exception
{
public:
    PermissionDeniedException(int number, const String& description, const String& source,
        const char* file, long line, int resultType):
        Exception(number, description, source, "PermissionDeniedException", file, line),
        mResultType( resultType ) {}

    int mResultType;
};
class DIVERSIA_UTIL_API SyntaxErrorException : public Exception
{
public:
    SyntaxErrorException(int number, const String& description, const String& source, const char* file, long line)
        : Exception(number, description, source, "SyntaxErrorException", file, line) {}
};

/** Class implementing dispatch methods in order to construct by-value
    exceptions of a derived type based just on an exception code.
@remarks
    This nicely handles construction of derived Exceptions by value (needed
    for throwing) without suffering from ambiguity - each code is turned into
    a distinct type so that methods can be overloaded. This allows DIVERSIA_EXCEPT
    to stay small in implementation (desirable since it is embedded) whilst
    still performing rich code-to-type mapping.
*/
class ExceptionFactory
{
private:
    /// Private constructor, no construction
    ExceptionFactory() {}
public:
    static UnimplementedException create(
        ExceptionCodeType<Exception::ERR_NOT_IMPLEMENTED> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return UnimplementedException(code.number, desc, src, file, line);
    }
    static FileNotFoundException create(
        ExceptionCodeType<Exception::ERR_FILE_NOT_FOUND> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return FileNotFoundException(code.number, desc, src, file, line);
    }
    static IOException create(
        ExceptionCodeType<Exception::ERR_CANNOT_WRITE_TO_FILE> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return IOException(code.number, desc, src, file, line);
    }
    static InvalidStateException create(
        ExceptionCodeType<Exception::ERR_INVALID_STATE> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return InvalidStateException(code.number, desc, src, file, line);
    }
    static InvalidParametersException create(
        ExceptionCodeType<Exception::ERR_INVALIDPARAMS> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return InvalidParametersException(code.number, desc, src, file, line);
    }
    static ItemIdentityException create(
        ExceptionCodeType<Exception::ERR_ITEM_NOT_FOUND> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return ItemIdentityException(code.number, desc, src, file, line);
    }
    static ItemIdentityException create(
        ExceptionCodeType<Exception::ERR_DUPLICATE_ITEM> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return ItemIdentityException(code.number, desc, src, file, line);
    }
    static InternalErrorException create(
        ExceptionCodeType<Exception::ERR_INTERNAL_ERROR> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return InternalErrorException(code.number, desc, src, file, line);
    }
    static RenderingAPIException create(
        ExceptionCodeType<Exception::ERR_RENDERINGAPI_ERROR> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return RenderingAPIException(code.number, desc, src, file, line);
    }
    static RuntimeAssertionException create(
        ExceptionCodeType<Exception::ERR_RT_ASSERTION_FAILED> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return RuntimeAssertionException(code.number, desc, src, file, line);
    }
    static PermissionDeniedException create(
        ExceptionCodeType<Exception::ERR_PERMISSION_DENIED> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return PermissionDeniedException(code.number, desc, src, file, line, 0);
    }
    static PermissionDeniedException create(
        ExceptionCodeType<Exception::ERR_PERMISSION_TOOMANYITEMS> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return PermissionDeniedException(code.number, desc, src, file, line, 1);
    }
    static PermissionDeniedException create(
        ExceptionCodeType<Exception::ERR_PERMISSION_TOOMANYITEMSPERTIMEFRAME> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return PermissionDeniedException(code.number, desc, src, file, line, 2);
    }
    static PermissionDeniedException create(
        ExceptionCodeType<Exception::ERR_PERMISSION_BOUNDSDENIED> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return PermissionDeniedException(code.number, desc, src, file, line, 3);
    }
    static SyntaxErrorException create(
        ExceptionCodeType<Exception::ERR_SYNTAX_ERROR> code,
        const String& desc,
        const String& src, const char* file, long line)
    {
        return SyntaxErrorException(code.number, desc, src, file, line);
    }
};

#ifndef DIVERSIA_EXCEPT
#define DIVERSIA_EXCEPT(num, desc, src) throw Diversia::Util::ExceptionFactory::create( \
    Diversia::Util::ExceptionCodeType<num>(), desc, src, __FILE__, __LINE__ );
#endif
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_EXCEPTION_H