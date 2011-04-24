/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_PROPERTYBROWSER_H
#define DIVERSIA_QTOGREEDITOR_PROPERTYBROWSER_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class PropertyBrowser : public QtTreePropertyBrowser
{
public:
    PropertyBrowser( QWidget* pParent );
    ~PropertyBrowser();

    void load( const camp::UserObject& rUserObject );
    void clear();
    inline CampPropertyManager* getPropertyManager() const { return mPropertyManager; }

private:
    CampPropertyManager* mPropertyManager;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_PROPERTYBROWSER_H