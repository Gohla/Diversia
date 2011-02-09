/*
-----------------------------------------------------------------------------
Copyright (c) 2008-2010 Diversia

This file is part of Diversia.
-----------------------------------------------------------------------------
*/

#include "Platform/StableHeaders.h"

#include "UI/ConnectDialog.h"
#include "Client/Communication/GridManager.h"
#include "State/LoadingState.h"

namespace Diversia
{
namespace QtOgreEditor
{
//------------------------------------------------------------------------------

ConnectDialog::ConnectDialog( QWidget* pParent, Qt::WFlags flags ):
	QDialog( pParent, flags )
{
	mUI.setupUi( this );

    EditorGlobals::mConnectDialog = this;
}

ConnectDialog::~ConnectDialog()
{
    EditorGlobals::mConnectDialog = 0;
}

void ConnectDialog::init()
{
    mUI.editServerAddress->setText( QString( mLastServer.mAddress.c_str() ) );
    mUI.editServerPort->setText( QString( boost::lexical_cast<String>( mLastServer.mPort ).c_str() ) );
    mUI.editUsername->setText( mLastUsername );
    mUI.editNickname->setText( mLastNickname );

    QObject::connect( mUI.editServerAddress, SIGNAL( textChanged( QString ) ), this, 
        SLOT( textChanged() ) );
    QObject::connect( mUI.editServerPort, SIGNAL( textChanged( QString ) ), this, 
        SLOT( textChanged() ) );
    QObject::connect( mUI.editUsername, SIGNAL( textChanged( QString ) ), this, 
        SLOT( textChanged() ) );
    QObject::connect( mUI.editNickname, SIGNAL( textChanged( QString ) ), this, 
        SLOT( textChanged() ) );
}

void ConnectDialog::accept()
{
    EditorGlobals::mGrid->createServer( GridPosition( 0, 0 ), ServerInfo( 
        mUI.editServerAddress->text().toStdString(), 
        mUI.editServerPort->text().toShort() ), UserInfo( mUI.editNickname->text().toStdString(), 
        mUI.editUsername->text().toStdString(), mUI.editPassword->text().toStdString() ) );
    EditorGlobals::mState->pushState( new LoadingState() );

    mUI.editPassword->clear();

    ConnectDialog::storeLastInfo();
    QDialog::setVisible( false );
}

void ConnectDialog::reject()
{
    mUI.editPassword->clear();

    QDialog::setVisible( false );
}

void ConnectDialog::textChanged()
{
    ConnectDialog::storeLastInfo();
}

void ConnectDialog::storeLastInfo()
{
    mLastServer.mAddress = mUI.editServerAddress->text().toStdString();
    mLastServer.mPort = mUI.editServerPort->text().toUShort();
    mLastUsername = mUI.editUsername->text();
    mLastNickname = mUI.editNickname->text();
}

//------------------------------------------------------------------------------
} // Namespace QtOgreEditor
} // Namespace Diversia