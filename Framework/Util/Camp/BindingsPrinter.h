/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.

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

#ifndef DIVERSIA_UTIL_BINDINGSPRINTER_H
#define DIVERSIA_UTIL_BINDINGSPRINTER_H

#include "Util/Platform/Prerequisites.h"

#include <camp/classvisitor.hpp>

namespace Diversia
{
namespace Util
{
//------------------------------------------------------------------------------

class DIVERSIA_UTIL_API BindingsPrinter : public camp::ClassVisitor
{
public:
    void visit( const camp::SimpleProperty& property )
    {
        std::cout << "Simple property: " << property.name() << std::endl;

        for( unsigned int i = 0; i < property.tagCount(); ++i )
        {
            const camp::Value& tagKey = property.tagId( i );
            const camp::Value& tagValue = property.tag( tagKey );

            if( tagValue != camp::Value::nothing )
            {
                std::cout << "\tTag " << tagKey.to<std::string>() <<
                    ":" << tagValue.to<std::string>() << std::endl;
            }
            else
                std::cout << "\tTag " << tagKey.to<std::string>() << std::endl;
        }
    }

    void visit( const camp::ArrayProperty& property )
    {
        std::cout << "Array property: " << property.name() << " - "
            << "dynamic: " << property.dynamic() << std::endl;

        for( unsigned int i = 0; i < property.tagCount(); ++i )
        {
            const camp::Value& tagKey = property.tagId( i );
            const camp::Value& tagValue = property.tag( tagKey );

            if( tagValue != camp::Value::nothing )
            {
                std::cout << "\tTag " << tagKey.to<std::string>() <<
                    ":" << tagValue.to<std::string>() << std::endl;
            }
            else
                std::cout << "\tTag " << tagKey.to<std::string>() << std::endl;
        }
    }

    void visit( const camp::EnumProperty& property )
    {
        std::cout << "Enum property: " << property.name() << " - "
            << "owner enum: " << property.getEnum().name() << std::endl;

        for( unsigned int i = 0; i < property.tagCount(); ++i )
        {
            const camp::Value& tagKey = property.tagId( i );
            const camp::Value& tagValue = property.tag( tagKey );

            if( tagValue != camp::Value::nothing )
            {
                std::cout << "\tTag " << tagKey.to<std::string>() <<
                    ":" << tagValue.to<std::string>() << std::endl;
            }
            else
                std::cout << "\tTag " << tagKey.to<std::string>() << std::endl;
        }
    }

    void visit( const camp::UserProperty& property )
    {
        std::cout << "User property: " << property.name() << " - "
            << "owner class: " << property.getClass().name() << std::endl;

        for( unsigned int i = 0; i < property.tagCount(); ++i )
        {
            const camp::Value& tagKey = property.tagId( i );
            const camp::Value& tagValue = property.tag( tagKey );

            if( tagValue != camp::Value::nothing )
            {
                std::cout << "\tTag " << tagKey.to<std::string>() <<
                    ":" << tagValue.to<std::string>() << std::endl;
            }
            else
                std::cout << "\tTag " << tagKey.to<std::string>() << std::endl;
        }
    }

    void visit( const camp::Function& function )
    {
        std::cout << "Function: " << function.name() << " - "
            << "number of arguments:" << function.argCount() << std::endl;

        for( unsigned int i = 0; i < function.tagCount(); ++i )
        {
            const camp::Value& tagKey = function.tagId( i );
            const camp::Value& tagValue = function.tag( tagKey );

            if( tagValue != camp::Value::nothing )
            {
                std::cout << "\tTag " << tagKey.to<std::string>() <<
                ":" << tagValue.to<std::string>() << std::endl;
            }
            else
                std::cout << "\tTag " << tagKey.to<std::string>() << std::endl;
        }
    }
};

//------------------------------------------------------------------------------
} // Namespace Util
} // Namespace Diversia

#endif // DIVERSIA_UTIL_BINDINGSPRINTER_H