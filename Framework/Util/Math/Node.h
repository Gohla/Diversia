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
#ifndef DIVERSIA_UTIL_NODE_H
#define DIVERSIA_UTIL_NODE_H

#include "Util/Platform/Prerequisites.h"

#include "Util/Math/Quaternion.h"
#include "Util/Math/Vector3.h"
#include "Util/Math/Matrix4.h"

namespace Diversia
{
namespace Util
{
/** \addtogroup Core
*  @{
*/
/** \addtogroup Scene
*  @{
*/
/** Class representing a general-purpose node an articulated scene graph.
    @remarks
        A node in the scene graph is a node in a structured tree. A node contains
        information about the transformation which will apply to
        it and all of it's children. Child nodes can have transforms of their own, which
        are combined with their parent's transformations.
    @par
        This is an abstract class - concrete classes are based on this for specific purposes,
        e.g. SceneNode, Bone
*/
class DIVERSIA_UTIL_API Node
{
public:
    /** Enumeration denoting the spaces which a transform can be relative to.
    */
    enum TransformSpace
    {
        /// Transform is relative to the local space
        TS_LOCAL,
        /// Transform is relative to the space of the parent node
        TS_PARENT,
        /// Transform is relative to world space
        TS_WORLD
    };
    typedef DiversiaHashMap<String, Node*> ChildNodeMap;
    typedef DiversiaHashMap<String, Node*>::iterator ChildNodeIterator;
    typedef DiversiaHashMap<String, Node*>::const_iterator ConstChildNodeIterator;

    /** Listener which gets called back on Node events.
    */
    class DIVERSIA_UTIL_API Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}
        /** Called when a node gets updated.
        @remarks
            Note that this happens when the node's derived update happens,
            not every time a method altering it's state occurs. There may
            be several state-changing calls but only one of these calls,
            when the node graph is fully updated.
        */
        virtual void nodeUpdated(const Node*) {}
        /** Node is being destroyed */
        virtual void nodeDestroyed(const Node*) {};
        /** Node has been attached to a parent */
        virtual void nodeAttached(const Node*) {};
        /** Node has been detached from a parent */
        virtual void nodeDetached(const Node*) {};
    };

protected:
    /// Pointer to parent node
    Node* mParent;
    /// Collection of pointers to direct children; hashmap for efficiency
    ChildNodeMap mChildren;

    typedef std::set<Node*> ChildUpdateSet;
    /// List of children which need updating, used if self is not out of date but children are
    mutable ChildUpdateSet mChildrenToUpdate;
    /// Flag to indicate own transform from parent is out of date
    mutable bool mNeedParentUpdate;
    /// Flag indicating that all children need to be updated
    mutable bool mNeedChildUpdate;
    /// Flag indicating that parent has been notified about update request
    mutable bool mParentNotified ;
    /// Flag indicating that the node has been queued for update
    mutable bool mQueuedForUpdate;

    /// Friendly name of this node, can be automatically generated if you don't care
    String mName;

    /// Stores the orientation of the node relative to it's parent.
    Quaternion mOrientation;

    /// Stores the position/translation of the node relative to its parent.
    Vector3 mPosition;

    /// Stores the scaling factor applied to this node
    Vector3 mScale;

    /// Stores whether this node inherits orientation from it's parent
    bool mInheritOrientation;

    /// Stores whether this node inherits scale from it's parent
    bool mInheritScale;

    /// Only available internally - notification of parent.
    virtual void setParent(Node* parent);

    /** Cached combined orientation.
        @par
            This member is the orientation derived by combining the
            local transformations and those of it's parents.
            This is updated when _updateFromParent is called by the
            SceneManager or the nodes parent.
    */
    mutable Quaternion mDerivedOrientation;

    /** Cached combined position.
        @par
            This member is the position derived by combining the
            local transformations and those of it's parents.
            This is updated when _updateFromParent is called by the
            SceneManager or the nodes parent.
    */
    mutable Vector3 mDerivedPosition;

    /** Cached combined scale.
        @par
            This member is the position derived by combining the
            local transformations and those of it's parents.
            This is updated when _updateFromParent is called by the
            SceneManager or the nodes parent.
    */
    mutable Vector3 mDerivedScale;

    /** Triggers the node to update it's combined transforms.
        @par
            This method is called internally by Diversia to ask the node
            to update it's complete transformation based on it's parents
            derived transform.
    */
    virtual void _updateFromParent() const;

    /** Class-specific implementation of _updateFromParent.
    @remarks
        Splitting the implementation of the update away from the update call
        itself allows the detail to be overridden without disrupting the
        general sequence of updateFromParent (e.g. raising events)
    */
    virtual void updateFromParentImpl() const;

    /// The position to use as a base for keyframe animation
    Vector3 mInitialPosition;
    /// The orientation to use as a base for keyframe animation
    Quaternion mInitialOrientation;
    /// The scale to use as a base for keyframe animation
    Vector3 mInitialScale;

    /// Cached derived transform as a 4x4 matrix
    mutable Matrix4 mCachedTransform;
    mutable bool mCachedTransformOutOfDate;

    /** Node listener - only one allowed (no list) for size & performance reasons. */
    Listener* mListener;

    sigc::signal<void, const Node&> mLocalTransformChangeSignal;
    sigc::signal<void, const Node&> mTransformChangeSignal;

    typedef std::vector<Node*> QueuedUpdates;
    static QueuedUpdates msQueuedUpdates;

public:
    /**
    Constructor, should only be called by parent, not directly.
    */
    Node( const String& name );

    /**
    Destructor.
    **/
    virtual ~Node();

    /** Returns the name of the node. */
    inline const String& getName() const { return mName; }

    /** Gets this node's parent (NULL if this is the root).
    */
    inline virtual Node* getParent() const { return mParent; }

    /**
    Gets the root parent node.
    **/
    Node* getRootParent();

    /**
    Query if this node has a parent.
    **/
    inline bool hasParent() const { return mParent != 0; }

    /** Returns a quaternion representing the nodes orientation.
    */
    inline const Quaternion& getOrientation() const { return mOrientation; }
    inline Quaternion& getOrientation() { return mOrientation; }

    /** Sets the orientation of this node via a quaternion.
    @remarks
        Orientations, unlike other transforms, are not always inherited by child nodes.
        Whether or not orientations affect the orientation of the child nodes depends on
        the setInheritOrientation option of the child. In some cases you want a orientating
        of a parent node to apply to a child node (e.g. where the child node is a part of
        the same object, so you want it to be the same relative orientation based on the
        parent's orientation), but not in other cases (e.g. where the child node is just
        for positioning another object, you want it to maintain it's own orientation).
        The default is to inherit as with other transforms.
    @par
        Note that rotations are oriented around the node's origin.
    */
    void setOrientation( Quaternion q );

    /** Sets the orientation of this node via quaternion parameters.
    @remarks
        Orientations, unlike other transforms, are not always inherited by child nodes.
        Whether or not orientations affect the orientation of the child nodes depends on
        the setInheritOrientation option of the child. In some cases you want a orientating
        of a parent node to apply to a child node (e.g. where the child node is a part of
        the same object, so you want it to be the same relative orientation based on the
        parent's orientation), but not in other cases (e.g. where the child node is just
        for positioning another object, you want it to maintain it's own orientation).
        The default is to inherit as with other transforms.
    @par
        Note that rotations are oriented around the node's origin.
    */
    inline void setOrientation( Real w, Real x, Real y, Real z ) { setOrientation( Quaternion( w, x, y, z ) ); }

    /** Resets the nodes orientation (local axes as world axes, no rotation).
    @remarks
        Orientations, unlike other transforms, are not always inherited by child nodes.
        Whether or not orientations affect the orientation of the child nodes depends on
        the setInheritOrientation option of the child. In some cases you want a orientating
        of a parent node to apply to a child node (e.g. where the child node is a part of
        the same object, so you want it to be the same relative orientation based on the
        parent's orientation), but not in other cases (e.g. where the child node is just
        for positioning another object, you want it to maintain it's own orientation).
        The default is to inherit as with other transforms.
    @par
        Note that rotations are oriented around the node's origin.
    */
    inline void resetOrientation() { setOrientation( Quaternion::IDENTITY ); }

    /** Sets the position of the node relative to it's parent.
    */
    void setPosition( const Vector3& pos );

    /** Sets the position of the node relative to it's parent.
    */
    inline void setPosition( Real x, Real y, Real z ) { setPosition( Vector3( x, y, z ) ); }

    /** Gets the position of the node relative to it's parent.
    */
    inline const Vector3& getPosition() const { return mPosition; }
    inline Vector3& getPosition() { return mPosition; }

    /** Sets the scaling factor applied to this node.
    @remarks
        Scaling factors, unlike other transforms, are not always inherited by child nodes.
        Whether or not scalings affect the size of the child nodes depends on the setInheritScale
        option of the child. In some cases you want a scaling factor of a parent node to apply to
        a child node (e.g. where the child node is a part of the same object, so you want it to be
        the same relative size based on the parent's size), but not in other cases (e.g. where the
        child node is just for positioning another object, you want it to maintain it's own size).
        The default is to inherit as with other transforms.
    @par
        Note that like rotations, scalings are oriented around the node's origin.
    */
    void setScale( const Vector3& scale );

    /** Sets the scaling factor applied to this node.
    @remarks
        Scaling factors, unlike other transforms, are not always inherited by child nodes.
        Whether or not scalings affect the size of the child nodes depends on the setInheritScale
        option of the child. In some cases you want a scaling factor of a parent node to apply to
        a child node (e.g. where the child node is a part of the same object, so you want it to be
        the same relative size based on the parent's size), but not in other cases (e.g. where the
        child node is just for positioning another object, you want it to maintain it's own size).
        The default is to inherit as with other transforms.
    @par
        Note that like rotations, scalings are oriented around the node's origin.
    */
    inline void setScale( Real x, Real y, Real z ) { setScale( Vector3( x, y, z ) ); }

    /** Gets the scaling factor of this node.
    */
    inline const Vector3& getScale() const { return mScale; }
    inline Vector3& getScale() { return mScale; }

    /** Tells the node whether it should inherit orientation from it's parent node.
    @remarks
        Orientations, unlike other transforms, are not always inherited by child nodes.
        Whether or not orientations affect the orientation of the child nodes depends on
        the setInheritOrientation option of the child. In some cases you want a orientating
        of a parent node to apply to a child node (e.g. where the child node is a part of
        the same object, so you want it to be the same relative orientation based on the
        parent's orientation), but not in other cases (e.g. where the child node is just
        for positioning another object, you want it to maintain it's own orientation).
        The default is to inherit as with other transforms.
    @param inherit If true, this node's orientation will be affected by its parent's orientation.
        If false, it will not be affected.
    */
    void setInheritOrientation(bool inherit);

    /** Returns true if this node is affected by orientation applied to the parent node.
    @remarks
        Orientations, unlike other transforms, are not always inherited by child nodes.
        Whether or not orientations affect the orientation of the child nodes depends on
        the setInheritOrientation option of the child. In some cases you want a orientating
        of a parent node to apply to a child node (e.g. where the child node is a part of
        the same object, so you want it to be the same relative orientation based on the
        parent's orientation), but not in other cases (e.g. where the child node is just
        for positioning another object, you want it to maintain it's own orientation).
        The default is to inherit as with other transforms.
    @remarks
        See setInheritOrientation for more info.
    */
    inline bool getInheritOrientation() const { return mInheritOrientation; }

    /** Tells the node whether it should inherit scaling factors from it's parent node.
    @remarks
        Scaling factors, unlike other transforms, are not always inherited by child nodes.
        Whether or not scalings affect the size of the child nodes depends on the setInheritScale
        option of the child. In some cases you want a scaling factor of a parent node to apply to
        a child node (e.g. where the child node is a part of the same object, so you want it to be
        the same relative size based on the parent's size), but not in other cases (e.g. where the
        child node is just for positioning another object, you want it to maintain it's own size).
        The default is to inherit as with other transforms.
    @param inherit If true, this node's scale will be affected by its parent's scale. If false,
        it will not be affected.
    */
    void setInheritScale(bool inherit);

    /** Returns true if this node is affected by scaling factors applied to the parent node.
    @remarks
        See setInheritScale for more info.
    */
    inline bool getInheritScale() const { return mInheritScale; }

    /** Scales the node, combining it's current scale with the passed in scaling factor.
    @remarks
        This method applies an extra scaling factor to the node's existing scale, (unlike setScale
        which overwrites it) combining it's current scale with the new one. E.g. calling this
        method twice with Vector3(2,2,2) would have the same effect as setScale(Vector3(4,4,4)) if
        the existing scale was 1.
    @par
        Note that like rotations, scalings are oriented around the node's origin.
    */
    void scale(const Vector3& scale);

    /** Scales the node, combining it's current scale with the passed in scaling factor.
    @remarks
        This method applies an extra scaling factor to the node's existing scale, (unlike setScale
        which overwrites it) combining it's current scale with the new one. E.g. calling this
        method twice with Vector3(2,2,2) would have the same effect as setScale(Vector3(4,4,4)) if
        the existing scale was 1.
    @par
        Note that like rotations, scalings are oriented around the node's origin.
    */
    void scale(Real x, Real y, Real z);

    /** Moves the node along the Cartesian axes.
        @par
            This method moves the node by the supplied vector along the
            world Cartesian axes, i.e. along world x,y,z
        @param
            d Vector with x,y,z values representing the translation.
        @param
            relativeTo The space which this transform is relative to.
    */
    void translate(const Vector3& d, TransformSpace relativeTo = TS_PARENT);
    /** Returns position after moving the node along the Cartesian axes.
        @par
            This method moves the node by the supplied vector along the
            world Cartesian axes, i.e. along world x,y,z
        @param
            d Vector with x,y,z values representing the translation.
        @param
            relativeTo The space which this transform is relative to.
    */
    void translateUpdate(Vector3& pos, const Vector3& d, TransformSpace relativeTo = TS_PARENT);
    /** Moves the node along the Cartesian axes.
        @par
            This method moves the node by the supplied vector along the
            world Cartesian axes, i.e. along world x,y,z
        @param
            x
        @param
            y
        @param
            z Real x, y and z values representing the translation.
        @param
        relativeTo The space which this transform is relative to.
    */
    inline void translate(Real x, Real y, Real z, TransformSpace relativeTo = TS_PARENT)
    {
        translate( Vector3( x, y, z ), relativeTo );
    }
    /** Moves the node along arbitrary axes.
        @remarks
            This method translates the node by a vector which is relative to
            a custom set of axes.
        @param
            axes A 3x3 Matrix containg 3 column vectors each representing the
            axes X, Y and Z respectively. In this format the standard cartesian
            axes would be expressed as:
            <pre>
            1 0 0
            0 1 0
            0 0 1
            </pre>
            i.e. the identity matrix.
        @param
            move Vector relative to the axes above.
        @param
        relativeTo The space which this transform is relative to.
    */
    inline void translate(const Matrix3& axes, const Vector3& move,
        TransformSpace relativeTo = TS_PARENT) { translate( axes * move, relativeTo ); }
    /** Moves the node along arbitrary axes.
        @remarks
        This method translates the node by a vector which is relative to
        a custom set of axes.
        @param
            axes A 3x3 Matrix containg 3 column vectors each representing the
            axes X, Y and Z respectively. In this format the standard cartesian
            axes would be expressed as
            <pre>
            1 0 0
            0 1 0
            0 0 1
            </pre>
            i.e. the identity matrix.
        @param
            x,y,z Translation components relative to the axes above.
        @param
            relativeTo The space which this transform is relative to.
    */
    inline void translate(const Matrix3& axes, Real x, Real y, Real z,
        TransformSpace relativeTo = TS_PARENT) { translate( axes, Vector3( x, y, z ), relativeTo ); }

    /** Rotate the node around the Z-axis.
    */
    inline void roll(const Radian& angle, TransformSpace relativeTo = TS_LOCAL) { rotate( Vector3::UNIT_Z, angle, relativeTo ); }

    /** Rotate the node around the X-axis.
    */
    inline void pitch(const Radian& angle, TransformSpace relativeTo = TS_LOCAL) { rotate( Vector3::UNIT_X, angle, relativeTo ); }

    /** Rotate the node around the Y-axis.
    */
    inline void yaw(const Radian& angle, TransformSpace relativeTo = TS_LOCAL) { rotate( Vector3::UNIT_Y, angle, relativeTo ); }

    /** Rotate the node around an arbitrary axis.
    */
    void rotate(const Vector3& axis, const Radian& angle, TransformSpace relativeTo = TS_LOCAL);

    /** Rotate the node around an arbitrary axis using a Quaternion.
    */
    void rotate(const Quaternion& q, TransformSpace relativeTo = TS_LOCAL);

    /** Return rotation the node around an arbitrary axis using a Quaternion.
    */
    void rotateUpdate(Quaternion& orientation, const Quaternion& q, TransformSpace relativeTo = TS_LOCAL);

    /** Gets a matrix whose columns are the local axes based on
        the nodes orientation relative to it's parent. */
    Matrix3 getLocalAxes() const;

    /** Adds a (precreated) child scene node to this node. If it is attached to another node,
        it must be detached first.
    @param child The Node which is to become a child node of this one
    */
    void addChild(Node* child);

    /** Reports the number of child nodes under this one.
    */
    unsigned short numChildren() const;

    /** Gets a pointer to a child node.
    @remarks
        There is an alternate getChild method which returns a named child.
    */
    Node* getChild(unsigned short index) const;

    /** Gets a pointer to a named child node.
    */
    Node* getChild(const String& name) const;

    /** Retrieves an iterator for efficiently looping through all children of this node.
    @remarks
        Using this is faster than repeatedly calling getChild if you want to go through
        all (or most of) the children of this node.
        Note that the returned iterator is only valid whilst no children are added or
        removed from this node. Thus you should not store this returned iterator for
        later use, nor should you add / remove children whilst iterating through it;
        store up changes for later. Note that calling methods on returned items in
        the iterator IS allowed and does not invalidate the iterator.
    */
    ChildNodeIterator getChildIterator();

    /** Retrieves an iterator for efficiently looping through all children of this node.
    @remarks
        Using this is faster than repeatedly calling getChild if you want to go through
        all (or most of) the children of this node.
        Note that the returned iterator is only valid whilst no children are added or
        removed from this node. Thus you should not store this returned iterator for
        later use, nor should you add / remove children whilst iterating through it;
        store up changes for later. Note that calling methods on returned items in
        the iterator IS allowed and does not invalidate the iterator.
    */
    ConstChildNodeIterator getChildIterator() const;

    /**
    Gets a copy of the childrens map.
    **/
    inline ChildNodeMap getChildren() const { return mChildren; }

    /** Drops the specified child from this node.
    @remarks
        Does not delete the node, just detaches it from
        this parent, potentially to be reattached elsewhere.
        There is also an alternate version which drops a named
        child from this node.
    */
    virtual Node* removeChild(unsigned short index);
    /** Drops the specified child from this node.
    @remarks
    Does not delete the node, just detaches it from
    this parent, potentially to be reattached elsewhere.
    There is also an alternate version which drops a named
    child from this node.
    */
    virtual Node* removeChild(Node* child);

    /** Drops the named child from this node.
    @remarks
        Does not delete the node, just detaches it from
        this parent, potentially to be reattached elsewhere.
    */
    virtual Node* removeChild(const String& name);
    /** Removes all child Nodes attached to this node. Does not delete the nodes, just detaches them from
        this parent, potentially to be reattached elsewhere.
    */
    virtual void removeAllChildren();

    /** Sets the final world position of the node directly.
    @remarks
        It's advisable to use the local setPosition if possible
    */
    void _setDerivedPosition(const Vector3& pos);

    /** Sets the final world orientation of the node directly.
    @remarks
    It's advisable to use the local setOrientation if possible, this simply does
    the conversion for you.
    */
    void _setDerivedOrientation(const Quaternion& q);

    /** Gets the orientation of the node as derived from all parents.
    */
    const Quaternion & _getDerivedOrientation() const;

    /** Gets the position of the node as derived from all parents.
    */
    const Vector3 & _getDerivedPosition() const;

    /** Gets the scaling factor of the node as derived from all parents.
    */
    const Vector3 & _getDerivedScale() const;

    /** Gets the full transformation matrix for this node.
        @remarks
            This method returns the full transformation matrix
            for this node, including the effect of any parent node
            transformations, provided they have been updated using the Node::_update method.
            This should only be called by a SceneManager which knows the
            derived transforms have been updated before calling this method.
            Applications using Diversia should just use the relative transforms.
    */
    const Matrix4& _getFullTransform() const;

    /** Internal method to update the Node.
        @note
            Updates this node and any relevant children to incorporate transforms etc.
            Don't call this yourself unless you are writing a SceneManager implementation.
        @param
            updateChildren If true, the update cascades down to all children. Specify false if you wish to
            update children separately, e.g. because of a more selective SceneManager implementation.
        @param
            parentHasChanged This flag indicates that the parent xform has changed,
                so the child should retrieve the parent's xform and combine it with its own
                even if it hasn't changed itself.
    */
    virtual void _update(bool updateChildren, bool parentHasChanged);

    /** Sets a listener for this Node.
    @remarks
        Note for size and performance reasons only one listener per node is
        allowed.
    */
    void setListener(Listener* listener) { mListener = listener; }

    /** Gets the current listener for this Node.
    */
    Listener* getListener() const { return mListener; }

    /** Sets the current transform of this node to be the 'initial state' ie that
        position / orientation / scale to be used as a basis for delta values used
        in keyframe animation.
    @remarks
        You never need to call this method unless you plan to animate this node. If you do
        plan to animate it, call this method once you've loaded the node with it's base state,
        ie the state on which all keyframes are based.
    @par
        If you never call this method, the initial state is the identity transform, ie do nothing.
    */
    void setInitialState();

    /** Resets the position / orientation / scale of this node to it's initial state, see setInitialState for more info. */
    void resetToInitialState();

    /** Gets the initial position of this node, see setInitialState for more info.
    @remarks
        Also resets the cumulative animation weight used for blending.
    */
    const Vector3& getInitialPosition() const;

    /** Gets the local position, relative to this node, of the given world-space position */
    Vector3 convertWorldToLocalPosition( const Vector3 &worldPos );

    /** Gets the world position of a point in the node local space
        useful for simple transforms that don't require a child node.*/
    Vector3 convertLocalToWorldPosition( const Vector3 &localPos );

    /** Gets the local orientation, relative to this node, of the given world-space orientation */
    Quaternion convertWorldToLocalOrientation( const Quaternion &worldOrientation );

    /** Gets the world orientation of an orientation in the node local space
        useful for simple transforms that don't require a child node.*/
    Quaternion convertLocalToWorldOrientation( const Quaternion &localOrientation );

    /** Gets the initial orientation of this node, see setInitialState for more info. */
    const Quaternion& getInitialOrientation() const;

    /** Gets the initial position of this node, see setInitialState for more info. */
    const Vector3& getInitialScale() const;

    /** To be called in the event of transform changes to this node that require it's recalculation.
    @remarks
        This not only tags the node state as being 'dirty', it also requests it's parent to
        know about it's dirtiness so it will get an update next time.
    @param forceParentUpdate Even if the node thinks it has already told it's
        parent, tell it anyway
    */
    void needUpdate(bool forceParentUpdate = false);
    /** Called by children to notify their parent that they need an update.
    @param forceParentUpdate Even if the node thinks it has already told it's
        parent, tell it anyway
    */
    void requestUpdate(Node* child, bool forceParentUpdate = false);
    /** Called by children to notify their parent that they no longer need an update. */
    void cancelUpdate(Node* child);

    /** Queue a 'needUpdate' call to a node safely.
    @remarks
        You can't call needUpdate() during the scene graph update, e.g. in
        response to a Node::Listener hook, because the graph is already being
        updated, and update flag changes cannot be made reliably in that context.
        Call this method if you need to queue a needUpdate call in this case.
    */
    static void queueNeedUpdate(Node* n);
    /** Process queued 'needUpdate' calls. */
    static void processQueuedUpdates();

    /**
    Connects a slot to the derived transform changed signal

    @param [in,out] rSlot   The slot (signature: void func(Node&)) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectTransformChange( const sigc::slot<void, const Node&>& rSlot )
    {
        return mTransformChangeSignal.connect( rSlot );
    }
    /**
    Connects a slot to the local transform changed signal

    @param [in,out] rSlot   The slot (signature: void func(Object&)) to connect.

    @return Connection object to block or disconnect the connection.
    **/
    inline sigc::connection connectLocalTransformChange( const sigc::slot<void, const Node&>& rSlot )
    {
        return mLocalTransformChangeSignal.connect( rSlot );
    }
};
/** @} */
/** @} */
} // Namespace Util
} // Namespace Diversia

CAMP_AUTO_TYPE_NONCOPYABLE( Diversia::Util::Node,
    &Diversia::Util::Bindings::CampBindings::bindNode );

CAMP_AUTO_TYPE( Diversia::Util::Node::TransformSpace, 
    &Diversia::Util::Bindings::CampBindings::bindTransformSpace );

#endif // DIVERSIA_UTIL_NODE_H