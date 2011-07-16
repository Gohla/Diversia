/*
--------------------------------------------------------------------------------
This file is part of Diversia engine
http://www.diversia.org/
Copyright (c) 2008-2011 Gabriël Konat
--------------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_RESOURCETREEVIEW_H
#define DIVERSIA_QTOGREEDITOR_RESOURCETREEVIEW_H

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class ResourceTreeView : public QTreeView, public sigc::trackable
{
    Q_OBJECT

public:
    ResourceTreeView( QWidget* pParent = 0 );
    ~ResourceTreeView();

    void init();
    
private:
    void serverChange( ServerAbstract& rServer, bool created );
    void pluginChange( Plugin& rPlugin, bool created );

    ResourceModel* mModel;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_RESOURCETREEVIEW_H
