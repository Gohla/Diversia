/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_OBJECTTEMPLATETREEVIEW_H
#define DIVERSIA_QTOGREEDITOR_OBJECTTEMPLATETREEVIEW_H

#include "Platform/Prerequisites.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class ObjectTemplateTreeView : public QTreeView, public sigc::trackable
{
    Q_OBJECT

public:
    /**
    Constructor. 
    
    @param [in,out] pParent The parent object. 
    **/
    ObjectTemplateTreeView( QWidget* pParent = 0 );
    /**
    Destructor. 
    **/
    ~ObjectTemplateTreeView();

    void init();

private:
    void serverChange( ServerAbstract& rServer, bool created );
    void pluginChange( ClientServerPlugin& rPlugin, bool created );

    ClientObjectTemplateManager*  mObjectTemplateManager;
    ObjectComponentTemplateModel* mModel;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_OBJECTTEMPLATETREEVIEW_H