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

UndoItem::UndoItem( UndoCommand* pUndoCommand ):
    QStandardItem(),
    mUndoCommand( pUndoCommand )
{
    if( mUndoCommand ) QStandardItem::setText( QString::fromStdString( mUndoCommand->getName() ) );
    else QStandardItem::setText( "<None>" );
    QStandardItem::setEditable( false );
    QStandardItem::setData( qVariantFromValue( (void*) pUndoCommand ), Qt::UserRole + 1 );
}

UndoItem::~UndoItem()
{

}

void UndoItem::setCurrent( bool current )
{
    if( current )
    {
        QStandardItem::setIcon( QIcon( ":/Icons/Icons/actions/reload.png" ) );
    }
    else
    {
        QStandardItem::setIcon( QIcon() );
    }
}

//------------------------------------------------------------------------------

UndoModel::UndoModel( UndoStack& rUndoStack, QObject* pParent /*= 0*/ ):
    QStandardItemModel( pParent ),
    mUndoStack( rUndoStack ),
    mCurrentItem( 0 )
{
    mUndoStack.connectChange( sigc::mem_fun( this, &UndoModel::undoCommandChange ) );
    mUndoStack.connectCurrentChanged( sigc::mem_fun( this, &UndoModel::currentCommandChanged ) );
    UndoModel::undoCommandChange( 0, true );
}

UndoModel::~UndoModel()
{

}

void UndoModel::undoCommandChange( UndoCommand* pUndoCommand, bool created )
{
    if( created )
    {
        UndoItem* item = new UndoItem( pUndoCommand );
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

void UndoModel::currentCommandChanged( UndoCommand* pUndoCommand )
{
    if( mCurrentItem ) mCurrentItem->setCurrent( false );

    UndoItems::iterator i = mUndoItems.find( pUndoCommand );
    if( i != mUndoItems.end() ) mCurrentItem = i->second;

    if( mCurrentItem ) mCurrentItem->setCurrent( true );
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia