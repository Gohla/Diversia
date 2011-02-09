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

#ifndef DIVERSIA_SHARED_CLIENTSERVERPLUGINTEMPLATE_H
#define DIVERSIA_SHARED_CLIENTSERVERPLUGINTEMPLATE_H

#include "Shared/Platform/Prerequisites.h"

namespace Diversia
{
//------------------------------------------------------------------------------

template <class Plugin, class PluginManager, unsigned char Type, const char* TypeName, class Data> 
class DIVERSIA_SHARED_API ClientServerPluginTemplate : public Plugin
{
public:
    typedef ClientServerPluginTemplate<Plugin, PluginManager, Type, TypeName, Data> TemplateType;

    /**
    Constructor. 
    
    @param  mode                        The mode (Client/Server) the plugin will run in. 
    @param [in,out] rPluginMan
    ager      The plugin manager. 
    @param [in,out] rRakPeer            The peer interface. 
    @param [in,out] rReplicaManager     The replica manager. 
    @param [in,out] rNetworkIDManager   The network ID manager. 
    **/
    ClientServerPluginTemplate( Mode mode, PluginManager& rPluginManager, 
        RakNet::RakPeerInterface& rRakPeer, RakNet::ReplicaManager3& rReplicaManager, 
        RakNet::NetworkIDManager& rNetworkIDManager ):
        Plugin( mode, rPluginManager, rRakPeer, rReplicaManager, rNetworkIDManager, 
            camp::UserObject( this ) ),
        mTypeName( TypeName )
    {

    }

	/**
    Gets the plugin type.
    **/
    inline ClientServerPluginTypeEnum getType() const { return Type; }
    static inline ClientServerPluginTypeEnum getTypeStatic() { return Type; }
    /**
    Gets the plugin type name.
    **/
    inline String getTypeName() const { return TypeName; }
    static inline String getTypeNameStatic() { return TypeName; }

    template <const char* DataName> static void campBinding()
    {
        camp::Class::declare<TemplateType>( String( "ClientServerPluginTemplate" ) +
            camp::classByType<Data>().name() )
        	// Constructors
        	// Properties (read-only)
        	// Properties (read/write)
            .property( DataName, &TemplateType::mData );
        	// Functions
        	// Static functions
        	// Operators
    }
    
private:
    Data        mData;
    const char* mTypeName;

};

//------------------------------------------------------------------------------
} // Namespace Diversia

/**
Macro to define a plugin with one variable. This macro must be used outside any namespace!

@param  class       The plugin class to inherit from.
@param  manager     The plugin manager to accept in the constructor. 
@param  type        The plugin type to use (of type ClientServerPluginTypeEnum)
@param  typeName    The name of the plugin, without string quotes.
@param  dataType    The data type of the variable inside the plugin.
@param  dataName    The name of the data, used in the camp binding of this plugin, without string 
                    quotes.
@param  nameSpace   The namespace the plugin must be created in.
**/
#define DIVERSIA_SHARED_CLIENTSERVERPLUGINTEMPLATE( class, manager, type, typeName, dataType, dataName, nameSpace ) \
namespace Diversia \
{ \
    namespace nameSpace \
    { \
        char g##typeName##Plugin[] = #typeName; \
        typedef ClientServerPluginTemplate<nameSpace::class, nameSpace::manager, type, g##typeName##Plugin, \
            dataType> typeName##Plugin; \
    } \
} \
char g##typeName##PluginDataName[] = #dataName; \
CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::nameSpace::typeName##Plugin, \
    &Diversia::nameSpace::typeName##Plugin::campBinding<g##typeName##PluginDataName> );

#endif // DIVERSIA_SHARED_CLIENTSERVERPLUGINTEMPLATE_H