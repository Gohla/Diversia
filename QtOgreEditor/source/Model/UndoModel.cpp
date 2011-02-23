/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "Client/Undo/UndoCommand.h"
#include "Client/Undo/UndoStack.h"
#include "Model/UndoModel.h"

namespace Diversia
{
namespace QtOgreEditor 
{
//------------------------------------------------------------------------------

UndoItem::UndoItem( UndoCommand& rUndoCommand ):
    QStandardItem(),
    mUndoCommand( rUndoCommand )
{
    QStandardItem::setText( QString::fromStdString( mUndoCommand.getName() ) );
}

UndoItem::~UndoItem()
{

}

//------------------------------------------------------------------------------

UndoModel::UndoModel( UndoStack& rUndoStack, QObject* pParent /*= 0*/ ):
    QStandardItemModel( pParent ),
    mUndoStack( rUndoStack )
{
    mUndoStack.connectChange( sigc::mem_fun( this, &UndoModel::undoCommandChange ) );
}

UndoModel::~UndoModel()
{

}

void UndoModel::undoCommandChange( UndoCommand* pUndoCommand, bool created )
{
    if( created )
    {
        UndoItem* item = new UndoItem( *pUndoCommand );
        mUndoItems.insert( std::make_pair( pUndoCommand, item ) );
        QStandardItemModel::appendRow( item );
    }
    else
    {
        UndoItems::iterator i = mUndoItems.find( pUndoCommand );
        if( i != mUndoItems.end() ) 
        {
            QStandardItemModel::removeRow( i->second->row() );
            mUndoItems.erase( i );
        }
    }
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia