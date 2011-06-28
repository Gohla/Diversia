/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Camp/CampPropertyManager.h"
#include "UI/PropertyBrowser.h"
#include <QtProperty>

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

PropertyBrowser::PropertyBrowser( QWidget* pParent ):
    QtTreePropertyBrowser( pParent ),
    mPropertyManager( new CampPropertyManager( this ) )
{
    mPropertyManager->setPropertyBrowser( this );
    QtTreePropertyBrowser::setPropertiesWithoutValueMarked( true );
    QtTreePropertyBrowser::setRootIsDecorated( false );
    QtTreePropertyBrowser::setHeaderVisible( true );
    QtTreePropertyBrowser::setAlternatingRowColors( true );
}

PropertyBrowser::~PropertyBrowser()
{

}

void PropertyBrowser::load( const camp::UserObject& rUserObject )
{
    PropertyBrowser::clear();

    std::list<QtProperty*> properties = mPropertyManager->addClassTree( rUserObject.getClass(), 
        rUserObject );
    for( std::list<QtProperty*>::iterator i = properties.begin(); i != properties.end(); ++i )
    {
        QtTreePropertyBrowser::addProperty( *i );
    }
}

void PropertyBrowser::clear()
{
    mPropertyManager->clear();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia