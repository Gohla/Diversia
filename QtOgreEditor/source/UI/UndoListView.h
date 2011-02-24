/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#ifndef DIVERSIA_QTOGREEDITOR_UNDOLISTVIEW_H
#define DIVERSIA_QTOGREEDITOR_UNDOLISTVIEW_H

#include "Platform/Prerequisites.h"
#include <QListView>

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

class UndoListView : public QListView
{
    Q_OBJECT

public:
    UndoListView( QWidget* pParent = 0 );
    ~UndoListView();

    /**
    Gets an undo item. 
    
    @param  rObjectName Name of the object.
    **/
    ObjectItem& getUndoItem( const String& rObjectName ) const;
    
private:
    void mousePressEvent( QMouseEvent* pEvent );

    UndoModel* mModel;

};

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia

#endif // DIVERSIA_QTOGREEDITOR_UNDOLISTVIEW_H