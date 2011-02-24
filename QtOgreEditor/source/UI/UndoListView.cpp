/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Undo/UndoStack.h"
#include "Model/UndoModel.h"
#include "UI/UndoListView.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

UndoListView::UndoListView( QWidget* pParent /*= 0*/ ):
    QListView( pParent )
{
    mModel = new UndoModel( *GlobalsBase::mUndoStack, this );

    QListView::setModel( mModel );
    QListView::setSelectionMode( QAbstractItemView::SingleSelection );
}

UndoListView::~UndoListView()
{

}

void UndoListView::mousePressEvent( QMouseEvent* pEvent )
{
    QModelIndex index = QListView::indexAt( pEvent->pos() );

    if( index.isValid() )
    {
        UndoCommand* undoCommand = static_cast<UndoCommand*>( 
            index.data( Qt::UserRole + 1 ).value<void *>() );
        if( undoCommand ) GlobalsBase::mUndoStack->undoOrRedoTo( undoCommand );
        else GlobalsBase::mUndoStack->undoAll();
    }

    QListView::mousePressEvent( pEvent );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia