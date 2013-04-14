/**
 * @file
 *  QuadTree.hpp - Describes a class containing a number of sectors with
 *  objects inside for fast collision detection.
 *
 * @author      George Kudrayvtsev (halcyon)
 * @version     1.0
 * @copyright   Apache License v2.0
 *  Licensed under the Apache License, Version 2.0 (the "License").         \n
 *  You may not use this file except in compliance with the License.        \n
 *  You may obtain a copy of the License at:
 *  http://www.apache.org/licenses/LICENSE-2.0                              \n
 *  Unless required by applicable law or agreed to in writing, software     \n
 *  distributed under the License is distributed on an "AS IS" BASIS,       \n
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 *  See the License for the specific language governing permissions and     \n
 *  limitations under the License.
 *
 * @addtogroup Game
 * @{
 **/

#ifndef IRON_CLAD__ENTITY__QUAD_TREE_HPP
#define IRON_CLAD__ENTITY__QUAD_TREE_HPP

#include <list>
#include "RigidBody.hpp"

namespace ic
{
    /**
     * A node within a quad tree object.
     **/
    class QTNode
    {
        typedef std::list<obj::CRigidBody*>::iterator       QTListIt_t;
        typedef std::list<obj::CRigidBody*>::const_iterator QTListCIt_t;

    public:
        /**
         * Creates a node at a given depth. The root node is depth 0.
         **/
        QTNode(const uint8_t depth = 0);
        ~QTNode();

        /**
         * Accesses a child of a node.
         *  If the node has no children, returns NULL.
         *  It comes in a const and non-const variety, for various 
         *  situations. The range of the index must be in 
         *  [0, QTNode::s_SPLIT_SIZE), this is capped using math::clamp()
         *
         * @param   uint8_t     Index of the child
         * 
         * @return  NULL if no children, the child QTNode* otherwise.
         * 
         * @see     math::clamp<uint8_t>()
         * @see     QTNode::s_SPLIT_SIZE
         **/
        QTNode* operator[](uint8_t index);
        const QTNode* operator[](uint8_t index) const;

        /**
         * Adds a collidable object to the current node.
         * 
         * @param   obj::CRigidBody*    The object to add to the node
         * @param   bool                Force the node even if full? (optional = false)
         * 
         * @return  TRUE if it was added successfully, 
         *          FALSE if the node is full.
         *          
         * @see QTNode::IsFull()
         * @see QTNode::SetMax()
         **/
        bool AddObject(obj::CRigidBody* pBody, const bool force = false);

        /**
         * Removes an existing object from the current node.
         * 
         * @param   obj::CRigidBody*    The object to remove
         * 
         * @return  TRUE if it was removed successfully, 
         *          FALSE if the object doesn't exist.
         **/
        bool RemoveObject(obj::CRigidBody* pBody);

        /**
         * Splits the node into 4 (default) sub-nodes.
         *  These nodes are evenly spaced between the parent node and are
         *  assigned as the node's children. Any objects in the node are
         *  reconfigured to be in the child nodes.
         **/
        void Split();

        /**
         * Checks if the given body collides with the node.
         *  This is used to check if a body belongs in this node, or 
         *  another. Usually, if this returns TRUE and IsLeaf() is also
         *  TRUE, the implementation will call AddObject() on this node.
         *  
         * @param   obj::CRigidBody*    Object to test collision on
         * 
         * @return  TRUE if collision, FALSE if not.
         **/
        bool NodeCollision(const obj::CRigidBody* pObj) const;
        
        /**
         * Checks if the given body collides with any child node.
         * @param   obj::CRigidBody*    Object to test collision on
         * @return  TRUE if collision with any child, FALSE if not.
         **/
        bool ChildCollision(const obj::CRigidBody* pObj) const;
        bool ChildCollision(const math::rect_t& Rect) const;

        /**
         * Deletes all child nodes.
         *  This basically performs the opposite operation of Split().
         **/
        void KillChildren();

        /**
         * Tells the caller if this Node has any child nodes.
         **/
        bool HasChildren() const;

        /**
         * Gives the opposite result of HasChildren()
         **/
        bool IsLeaf() const;

        /**
         * Tells the caller if the node is full.
         * @return  TRUE if max is reached, FALSE if not.
         * @see     QTNode::SetMax()
         **/
        bool IsFull() const;

        /**
         * Determines whether or not the node can split into 4 sub-nodes.
         * @param   uint8_t     Maximum node depth
         * @return  TRUE if depth < max_size, FALSE otherwise.
         **/
        bool CanSplit(const uint8_t max_size) const;

#ifdef _DEBUG
        void Print() const;
#else
        void Print() const {}
#endif // _DEBUG

        /**
         * Sets the maximum number of objects in a node before it has
         * to split into child nodes.
         **/
        void SetMax(const uint16_t limit);

        const std::list<obj::CRigidBody*>& GetObjects() const;

        friend class CQuadTree;
        static const uint8_t s_SPLIT_SIZE = 4;

    private:
        math::rect_t m_Rect;
        
        QTNode* mp_Parent;
        
        std::vector<QTNode*>        mp_Children;
        std::list<obj::CRigidBody*> mp_nodeObjects;

        uint8_t     m_depth;
        bool        m_full;
    };

    /**
     * A quad tree for optimized collision detection.
     *  Quad trees split areas into 4 (hence quad) equal parts. Then a
     *  recursive algorithm can speed up collision detection for large
     *  quantities of objects.
     **/
    class IRONCLAD_API CQuadTree
    {
    public:
        CQuadTree(const uint16_t w, const uint16_t h);

        inline bool Insert(obj::CRigidBody* pBody)
        {
            if(this->RInsert(pBody, &m_Root))
            {
                mp_allBodies.push_back(pBody);
                return true;
            }
            
            return false;
        }

        inline bool Remove(obj::CRigidBody* pBody)
        { return this->RRemove(pBody, &m_Root); }

        inline obj::CRigidBody* Collides(const obj::CRigidBody* pBody) const
        { return this->CheckLeaf(pBody, &m_Root); }

        void Update();

#ifdef _DEBUG
        void Print() const;
#else
        void Print() const {}
#endif // _DEBUG

        static const uint8_t QT_MAX_OBJECTS = 32;
        static const uint8_t QT_MAX_DEPTH   = 6;

    private:
        bool RInsert(obj::CRigidBody* pBody, QTNode* pStart);
        bool RRemove(obj::CRigidBody* pBody, QTNode* pStart);
        obj::CRigidBody* CheckLeaf(const obj::CRigidBody* pBody,
                                   const QTNode* pStart) const;

#ifdef _DEBUG
        void RPrintNode(const QTNode* pNode) const;
#endif // _DEBUG

        QTNode m_Root;
        std::vector<obj::CRigidBody*> mp_allBodies;
    };
}   // namespace ic

#endif // IRON_CLAD__ENTITY__QUAD_TREE_HPP

/** @} **/
