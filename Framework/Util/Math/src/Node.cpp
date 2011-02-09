/*
-----------------------------------------------------------------------------
This source file is part of DIVERSIA
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

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

#include "Util/Platform/StableHeaders.h"

#include "Util/Math/Node.h"

namespace Diversia 
{
namespace Util
{

Node::QueuedUpdates Node::msQueuedUpdates;

//-----------------------------------------------------------------------

Node::Node(const String& name)
    :
    mParent(0),
    mNeedParentUpdate(false),
    mNeedChildUpdate(false),
    mParentNotified(false),
    mQueuedForUpdate(false),
    mName(name),
    mOrientation(Quaternion::IDENTITY),
    mPosition(Vector3::ZERO),
    mScale(Vector3::UNIT_SCALE),
    mInheritOrientation(true),
    mInheritScale(true),
    mDerivedOrientation(Quaternion::IDENTITY),
    mDerivedPosition(Vector3::ZERO),
    mDerivedScale(Vector3::UNIT_SCALE),
    mInitialPosition(Vector3::ZERO),
    mInitialOrientation(Quaternion::IDENTITY),
    mInitialScale(Vector3::UNIT_SCALE),
    mCachedTransformOutOfDate(true),
    mListener(0)
{
    needUpdate();
}

//-----------------------------------------------------------------------
Node::~Node()
{
    // Call listener (note, only called if there's something to do)
    if (mListener)
    {
        mListener->nodeDestroyed(this);
    }

    removeAllChildren();
    if(mParent)
        mParent->removeChild(this);

    if (mQueuedForUpdate)
    {
        // Erase from queued updates
        QueuedUpdates::iterator it =
            std::find(msQueuedUpdates.begin(), msQueuedUpdates.end(), this);
        assert(it != msQueuedUpdates.end());
        if (it != msQueuedUpdates.end())
        {
            // Optimised algorithm to erase an element from unordered vector.
            *it = msQueuedUpdates.back();
            msQueuedUpdates.pop_back();
        }
    }

}
//-----------------------------------------------------------------------
Node* Node::getRootParent()
{
    // TODO: Efficient implementation.
    Node* node = this;
    while( node->hasParent() )
    {
        node = node->getParent();
    }
    return node;
}
//-----------------------------------------------------------------------
void Node::setParent(Node* parent)
{
    bool different = (parent != mParent);

    mParent = parent;
    // Request update from parent
    mParentNotified = false ;
    needUpdate();

    // Call listener (note, only called if there's something to do)
    if (mListener && different)
    {
        if (mParent)
            mListener->nodeAttached(this);
        else
            mListener->nodeDetached(this);
    }

}

//-----------------------------------------------------------------------
const Matrix4& Node::_getFullTransform() const
{
    if (mCachedTransformOutOfDate)
    {
        // Use derived values
        mCachedTransform.makeTransform(
            _getDerivedPosition(),
            _getDerivedScale(),
            _getDerivedOrientation());
        mCachedTransformOutOfDate = false;
    }
    return mCachedTransform;
}
//-----------------------------------------------------------------------
void Node::_update(bool updateChildren, bool parentHasChanged)
{
    // always clear information about parent notification
    mParentNotified = false ;

    // Short circuit the off case
    if (!updateChildren && !mNeedParentUpdate && !mNeedChildUpdate && !parentHasChanged )
    {
        return;
    }


    // See if we should process everyone
    if (mNeedParentUpdate || parentHasChanged)
    {
        // Update transforms from parent
        _updateFromParent();
    }

    if (mNeedChildUpdate || parentHasChanged)
    {

        ChildNodeMap::iterator it, itend;
        itend = mChildren.end();
        for (it = mChildren.begin(); it != itend; ++it)
        {
            Node* child = it->second;
            child->_update(true, true);
        }
        mChildrenToUpdate.clear();
    }
    else
    {
        // Just update selected children

        ChildUpdateSet::iterator it, itend;
        itend = mChildrenToUpdate.end();
        for(it = mChildrenToUpdate.begin(); it != itend; ++it)
        {
            Node* child = *it;
            child->_update(true, false);
        }

        mChildrenToUpdate.clear();
    }

    mNeedChildUpdate = false;

}
//-----------------------------------------------------------------------
void Node::_updateFromParent() const
{
    updateFromParentImpl();

    // Call listener (note, this method only called if there's something to do)
    if (mListener)
    {
        mListener->nodeUpdated(this);
    }

    mTransformChangeSignal( *this );
}
//-----------------------------------------------------------------------
void Node::updateFromParentImpl() const
{
    if (mParent)
    {
        // Update orientation
        const Quaternion& parentOrientation = mParent->_getDerivedOrientation();
        if (mInheritOrientation)
        {
            // Combine orientation with that of parent
            mDerivedOrientation = parentOrientation * mOrientation;
        }
        else
        {
            // No inheritence
            mDerivedOrientation = mOrientation;
        }

        // Update scale
        const Vector3& parentScale = mParent->_getDerivedScale();
        if (mInheritScale)
        {
            // Scale own position by parent scale, NB just combine
            // as equivalent axes, no shearing
            mDerivedScale = parentScale * mScale;
        }
        else
        {
            // No inheritence
            mDerivedScale = mScale;
        }

        // Change position vector based on parent's orientation & scale
        mDerivedPosition = parentOrientation * (parentScale * mPosition);

        // Add altered position vector to parents
        mDerivedPosition += mParent->_getDerivedPosition();
    }
    else
    {
        // Root node, no parent
        mDerivedOrientation = mOrientation;
        mDerivedPosition = mPosition;
        mDerivedScale = mScale;
    }

    mCachedTransformOutOfDate = true;
    mNeedParentUpdate = false;

}
//-----------------------------------------------------------------------
void Node::addChild(Node* child)
{
    if (child->mParent)
    {
        DIVERSIA_EXCEPT(Exception::ERR_INVALIDPARAMS,
            "Node '" + child->getName() + "' already was a child of '" +
            child->mParent->getName() + "'.",
            "Node::addChild");
    }

    mChildren.insert(ChildNodeMap::value_type(child->getName(), child));
    child->setParent(this);

}
//-----------------------------------------------------------------------
unsigned short Node::numChildren() const
{
    return static_cast< unsigned short >( mChildren.size() );
}
//-----------------------------------------------------------------------
Node* Node::getChild(unsigned short index) const
{
    if( index < mChildren.size() )
    {
        ChildNodeMap::const_iterator i = mChildren.begin();
        while (index--) ++i;
        return i->second;
    }
    else
        return NULL;
}
//-----------------------------------------------------------------------
Node* Node::removeChild(unsigned short index)
{
    Node* ret;
    if (index < mChildren.size())
    {
        ChildNodeMap::iterator i = mChildren.begin();
        while (index--) ++i;
        ret = i->second;
        // cancel any pending update
        cancelUpdate(ret);

        mChildren.erase(i);
        ret->setParent(NULL);
        return ret;
    }
    else
    {
        DIVERSIA_EXCEPT(
            Exception::ERR_INVALIDPARAMS,
            "Child index out of bounds.",
            "Node::getChild" );
    }
    return 0;
}
//-----------------------------------------------------------------------
Node* Node::removeChild(Node* child)
{
    if (child)
    {
        ChildNodeMap::iterator i = mChildren.find(child->getName());
        // ensure it's our child
        if (i != mChildren.end() && i->second == child)
        {
            // cancel any pending update
            cancelUpdate(child);

            mChildren.erase(i);
            child->setParent(NULL);
        }
    }
    return child;
}
//-----------------------------------------------------------------------
void Node::setOrientation( Quaternion q )
{
    // Do orientation check here, or else it could assert in ogre.
    q.normalise();
    if( q.isNaN() )
    {
        resetOrientation();
        DIVERSIA_EXCEPT( Exception::ERR_INVALID_STATE, "Invalid orientation parameters", 
            "Node::setOrientation" );
    }

    mOrientation = q;
    needUpdate();
}
//-----------------------------------------------------------------------
void Node::setPosition(const Vector3& pos)
{
    //assert(!pos.isNaN() && "Invalid vector supplied as parameter");
    mPosition = pos;
    needUpdate();
}
//-----------------------------------------------------------------------
Matrix3 Node::getLocalAxes() const
{
    Vector3 axisX = Vector3::UNIT_X;
    Vector3 axisY = Vector3::UNIT_Y;
    Vector3 axisZ = Vector3::UNIT_Z;

    axisX = mOrientation * axisX;
    axisY = mOrientation * axisY;
    axisZ = mOrientation * axisZ;

    return Matrix3(axisX.x, axisY.x, axisZ.x,
                   axisX.y, axisY.y, axisZ.y,
                   axisX.z, axisY.z, axisZ.z);
}

//-----------------------------------------------------------------------
void Node::translate(const Vector3& d, TransformSpace relativeTo)
{
    switch(relativeTo)
    {
    case TS_LOCAL:
        // position is relative to parent so transform downwards
        mPosition += mOrientation * d;
        break;
    case TS_WORLD:
        // position is relative to parent so transform upwards
        if (mParent)
        {
            mPosition += (mParent->_getDerivedOrientation().Inverse() * d)
                / mParent->_getDerivedScale();
        }
        else
        {
            mPosition += d;
        }
        break;
    case TS_PARENT:
        mPosition += d;
        break;
    }
    needUpdate();

}
//-----------------------------------------------------------------------
void Node::rotate(const Vector3& axis, const Radian& angle, TransformSpace relativeTo)
{
    Quaternion q;
    q.FromAngleAxis(angle,axis);
    rotate(q, relativeTo);
}

//-----------------------------------------------------------------------
void Node::rotate(const Quaternion& q, TransformSpace relativeTo)
{
    // Normalise quaternion to avoid drift
    Quaternion qnorm = q;
    qnorm.normalise();

    switch(relativeTo)
    {
    case TS_PARENT:
        // Rotations are normally relative to local axes, transform up
        mOrientation = qnorm * mOrientation;
        break;
    case TS_WORLD:
        // Rotations are normally relative to local axes, transform up
        mOrientation = mOrientation * _getDerivedOrientation().Inverse()
            * qnorm * _getDerivedOrientation();
        break;
    case TS_LOCAL:
        // Note the order of the mult, i.e. q comes after
        mOrientation = mOrientation * qnorm;
        break;
    }
    needUpdate();
}


//-----------------------------------------------------------------------
void Node::_setDerivedPosition( const Vector3& pos )
{
    //find where the node would end up in parent's local space
    setPosition( mParent->convertWorldToLocalPosition( pos ) );
}
//-----------------------------------------------------------------------
void Node::_setDerivedOrientation( const Quaternion& q )
{
    //find where the node would end up in parent's local space
    setOrientation( mParent->convertWorldToLocalOrientation( q ) );
}

//-----------------------------------------------------------------------
const Quaternion & Node::_getDerivedOrientation() const
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return mDerivedOrientation;
}
//-----------------------------------------------------------------------
const Vector3 & Node::_getDerivedPosition() const
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return mDerivedPosition;
}
//-----------------------------------------------------------------------
const Vector3 & Node::_getDerivedScale() const
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return mDerivedScale;
}
//-----------------------------------------------------------------------
Vector3 Node::convertWorldToLocalPosition( const Vector3 &worldPos )
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return mDerivedOrientation.Inverse() * (worldPos - mDerivedPosition) / mDerivedScale;
}
//-----------------------------------------------------------------------
Vector3 Node::convertLocalToWorldPosition( const Vector3 &localPos )
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return (mDerivedOrientation * localPos * mDerivedScale) + mDerivedPosition;
}
//-----------------------------------------------------------------------
Quaternion Node::convertWorldToLocalOrientation( const Quaternion &worldOrientation )
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return mDerivedOrientation.Inverse() * worldOrientation;
}
//-----------------------------------------------------------------------
Quaternion Node::convertLocalToWorldOrientation( const Quaternion &localOrientation )
{
    if (mNeedParentUpdate)
    {
        _updateFromParent();
    }
    return mDerivedOrientation * localOrientation;

}
//-----------------------------------------------------------------------
void Node::removeAllChildren()
{
    ChildNodeMap::iterator i, iend;
    iend = mChildren.end();
    for (i = mChildren.begin(); i != iend; ++i)
    {
        i->second->setParent(0);
    }
    mChildren.clear();
    mChildrenToUpdate.clear();
}
//-----------------------------------------------------------------------
void Node::setScale(const Vector3& scale)
{
    //assert(!scale.isNaN() && "Invalid vector supplied as parameter");
    mScale = scale;
    needUpdate();
}
//-----------------------------------------------------------------------
void Node::setInheritOrientation(bool inherit)
{
    mInheritOrientation = inherit;
    needUpdate();
}
//-----------------------------------------------------------------------
void Node::setInheritScale(bool inherit)
{
    mInheritScale = inherit;
    needUpdate();
}
//-----------------------------------------------------------------------
void Node::scale(const Vector3& scale)
{
    mScale = mScale * scale;
    needUpdate();

}
//-----------------------------------------------------------------------
void Node::scale(Real x, Real y, Real z)
{
    mScale.x *= x;
    mScale.y *= y;
    mScale.z *= z;
    needUpdate();

}
//-----------------------------------------------------------------------
void Node::setInitialState()
{
    mInitialPosition = mPosition;
    mInitialOrientation = mOrientation;
    mInitialScale = mScale;
}
//-----------------------------------------------------------------------
void Node::resetToInitialState()
{
    mPosition = mInitialPosition;
    mOrientation = mInitialOrientation;
    mScale = mInitialScale;

    needUpdate();
}
//-----------------------------------------------------------------------
const Vector3& Node::getInitialPosition() const
{
    return mInitialPosition;
}
//-----------------------------------------------------------------------
const Quaternion& Node::getInitialOrientation() const
{
    return mInitialOrientation;

}
//-----------------------------------------------------------------------
const Vector3& Node::getInitialScale() const
{
    return mInitialScale;
}
//-----------------------------------------------------------------------
Node* Node::getChild(const String& name) const
{
    ChildNodeMap::const_iterator i = mChildren.find(name);

    if (i == mChildren.end())
    {
        DIVERSIA_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Child node named " + name +
            " does not exist.", "Node::getChild");
    }
    return i->second;

}
//-----------------------------------------------------------------------
Node* Node::removeChild(const String& name)
{
    ChildNodeMap::iterator i = mChildren.find(name);

    if (i == mChildren.end())
    {
        DIVERSIA_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Child node named " + name +
            " does not exist.", "Node::removeChild");
    }

    Node* ret = i->second;
    // Cancel any pending update
    cancelUpdate(ret);

    mChildren.erase(i);
    ret->setParent(NULL);

    return ret;


}
//-----------------------------------------------------------------------
Node::ChildNodeIterator Node::getChildIterator()
{
    //return ChildNodeIterator(mChildren.begin(), mChildren.end());
    return mChildren.begin();
}
//-----------------------------------------------------------------------
Node::ConstChildNodeIterator Node::getChildIterator() const
{
    //return ConstChildNodeIterator(mChildren.begin(), mChildren.end());
    return mChildren.begin();
}
//-----------------------------------------------------------------------
void Node::needUpdate(bool forceParentUpdate)
{

    mNeedParentUpdate = true;
    mNeedChildUpdate = true;
    mCachedTransformOutOfDate = true;

    // Make sure we're not root and parent hasn't been notified before
    if (mParent && (!mParentNotified || forceParentUpdate))
    {
        mParent->requestUpdate(this, forceParentUpdate);
        mParentNotified = true ;
    }

    // all children will be updated
    mChildrenToUpdate.clear();
    for( ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i )
    {
        i->second->mNeedParentUpdate = true;
    }

    mLocalTransformChangeSignal( *this );
    if( !Node::hasParent() ) mTransformChangeSignal( *this );
}
//-----------------------------------------------------------------------
void Node::requestUpdate(Node* child, bool forceParentUpdate)
{
    // If we're already going to update everything this doesn't matter
    if (mNeedChildUpdate)
    {
        return;
    }

    mChildrenToUpdate.insert(child);
    // Request selective update of me, if we didn't do it before
    if (mParent && (!mParentNotified || forceParentUpdate))
    {
        mParent->requestUpdate(this, forceParentUpdate);
        mParentNotified = true ;
    }

}
//-----------------------------------------------------------------------
void Node::cancelUpdate(Node* child)
{
    mChildrenToUpdate.erase(child);

    // Propogate this up if we're done
    if (mChildrenToUpdate.empty() && mParent && !mNeedChildUpdate)
    {
        mParent->cancelUpdate(this);
        mParentNotified = false ;
    }
}
//-----------------------------------------------------------------------
void Node::queueNeedUpdate(Node* n)
{
    // Don't queue the node more than once
    if (!n->mQueuedForUpdate)
    {
        n->mQueuedForUpdate = true;
        msQueuedUpdates.push_back(n);
    }
}
//-----------------------------------------------------------------------
void Node::processQueuedUpdates()
{
    for (QueuedUpdates::iterator i = msQueuedUpdates.begin();
        i != msQueuedUpdates.end(); ++i)
    {
        // Update, and force parent update since chances are we've ended
        // up with some mixed state in there due to re-entrancy
        Node* n = *i;
        n->mQueuedForUpdate = false;
        n->needUpdate(true);
    }
    msQueuedUpdates.clear();
}
//---------------------------------------------------------------------

} // Namespace Util
} // Namespace Diversia
