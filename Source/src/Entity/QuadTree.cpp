#include "IronClad/Entity/QuadTree.hpp"

using namespace ic;

/************************************************************************/
/* Begin QTNode struct definitions.                                     */
/************************************************************************/

QTNode::QTNode(const uint8_t depth) : mp_Parent(NULL), m_full(false),
                                      m_depth(depth)
{
    mp_Children.reserve(4);
    mp_nodeObjects.clear();
}

QTNode::~QTNode()
{
    // This is how we do things around these parts.
    if(this->HasChildren()) this->KillChildren();
    mp_nodeObjects.clear();
}

QTNode* QTNode::operator[](uint8_t index)
{
    if(IsLeaf()) return NULL;

    math::clamp<uint8_t>(index, 0, s_SPLIT_SIZE);
    return mp_Children[index];
}

const QTNode* QTNode::operator[](uint8_t index) const
{
    if(IsLeaf()) return NULL;

    math::clamp<uint8_t>(index, 0, s_SPLIT_SIZE - 1);
    return mp_Children[index];
}

bool QTNode::AddObject(obj::CRigidBody* pBody, const bool force)
{
    if(m_full && !force) return false;

    mp_nodeObjects.push_back(pBody);
    m_full = mp_nodeObjects.size() >= CQuadTree::QT_MAX_OBJECTS;
    return true;
}

bool QTNode::RemoveObject(obj::CRigidBody* pBody)
{
    for(QTNode::QTListIt_t i = mp_nodeObjects.begin();
        i != mp_nodeObjects.end(); ++i)
    {
        if(*i == pBody)
        {
            mp_nodeObjects.erase(i);
            return true;
        }
        
        ++i;
    }

    return false;
}


void QTNode::Split()
{
    // I feel so bad :(
    if(!IsLeaf())   this->KillChildren();

    mp_Children.resize(s_SPLIT_SIZE);

    // The corner coordinates of the parent node.
    math::vector2_t Start = math::vector2_t(m_Rect.x, m_Rect.y);

    for(size_t i = 0; i < s_SPLIT_SIZE; ++i)
    {
        uint8_t d = (mp_Parent == NULL) ? 0 : mp_Parent->m_depth + 1;

        QTNode* pCurrent    = new QTNode(d);   
        pCurrent->mp_Parent = this;
        pCurrent->m_Rect    = math::rect_t(m_Rect.x, m_Rect.y,
                                           m_Rect.w, m_Rect.h);

        // Node becomes half the size. The ++ is to account for
        // integer division making errors when nodes get small.
        pCurrent->m_Rect.w /= 2; pCurrent->m_Rect.w++;
        pCurrent->m_Rect.h /= 2; pCurrent->m_Rect.h++;

        pCurrent->m_Rect.x = Start.x;
        pCurrent->m_Rect.y = Start.y;

        // Move the proper objects into the new leaf nodes that
        // they belong in, removing them from the current node.
        for(QTListIt_t j = mp_nodeObjects.begin(); 
            j != mp_nodeObjects.end(); /* no third */)
        {
            if(pCurrent->NodeCollision(*j))
            {
                pCurrent->AddObject(*j);
                j = mp_nodeObjects.erase(j);
            }
            else
            {
                ++j;
            }
        }

        mp_Children[i] = pCurrent;
        Start.x += pCurrent->m_Rect.w;

        if(Start.x >= m_Rect.x + m_Rect.w)
        {
            Start.x = m_Rect.x;
            Start.y+= pCurrent->m_Rect.h;
        }
    }

    mp_nodeObjects.clear();
}

bool QTNode::NodeCollision(const obj::CRigidBody* pObj) const
{
    return pObj->CheckCollision(m_Rect);
}

bool QTNode::ChildCollision(const obj::CRigidBody* pObj) const
{
    if(!this->HasChildren()) return false;

    for(size_t i = 0; i < s_SPLIT_SIZE; ++i)
    {
        if(pObj->CheckCollision(mp_Children[i]->m_Rect))
            return true;
    }

    return false;
}

bool QTNode::ChildCollision(const math::rect_t& Rect) const
{
    if(!this->HasChildren()) return false;

    for(size_t i = 0; i < s_SPLIT_SIZE; ++i)
        if(Rect.Collides(mp_Children[i]->m_Rect)) return true;

    return false;
}

void QTNode::KillChildren()
{
    for(size_t i = 0; i < s_SPLIT_SIZE; ++i)
    {
        for(QTListIt_t j = mp_Children[i]->mp_nodeObjects.begin();
            j != mp_Children[i]->mp_nodeObjects.end(); ++j)
            mp_nodeObjects.push_back(*j);

        delete mp_Children[i];
    }
    mp_Children.clear();
}

bool QTNode::HasChildren() const
{
    return !mp_Children.empty();
}

bool QTNode::IsLeaf() const 
{
    return mp_Children.empty();
}

bool QTNode::IsFull() const
{
    return m_full;
}

bool QTNode::CanSplit(const uint8_t max_size) const 
{ return m_depth < max_size; }

#ifdef _DEBUG
void QTNode::Print() const
{
    std::cout << "QTNode (" << this << "): ";
    std::cout << "parent (" << mp_Parent << "), ";
    std::cout << "hasChildren (" << (!mp_Children.empty()) << "), ";
    std::cout << "objCount (" << mp_nodeObjects.size() << "), ";
    std::cout << "rect (" << m_Rect.x << ", " << m_Rect.y << ", ";
    std::cout << m_Rect.w << ", " << m_Rect.h << ");";
    std::cout << std::endl;
}
#endif // _DEBUG

const std::list<obj::CRigidBody*>& QTNode::GetObjects() const
{
    return mp_nodeObjects;
}

/************************************************************************/
/* Begin CQuadTree class definitions.                                   */
/************************************************************************/

CQuadTree::CQuadTree(const uint16_t w, const uint16_t h) : m_Root(0)
{
    m_Root.m_Rect = math::rect_t(0, 0, w, h);
}

#ifdef _DEBUG
void CQuadTree::Print() const
{
    this->RPrintNode(&m_Root);
}
#endif // _DEBUG

void CQuadTree::Update()
{
    for(size_t i = 0; i < mp_allBodies.size(); ++i)
    {
        if(mp_allBodies[i]->NeedsUpdate())
        {
            RRemove(mp_allBodies[i], mp_allBodies[i]->pNode);
            RInsert(mp_allBodies[i],
                (mp_allBodies[i]->pNode->mp_Parent == NULL) ? 
                 mp_allBodies[i]->pNode : 
                 mp_allBodies[i]->pNode->mp_Parent);

            mp_allBodies[i]->m_update = false;
        }
    }
}

bool CQuadTree::RInsert(obj::CRigidBody* pBody, QTNode* pStart)
{
    if(pBody == NULL || pStart == NULL) return false;

    // Is this a leaf node?
    if(pStart->IsLeaf())
    {
        // Completely full quad
        if(!pStart->AddObject(pBody))
        {
            // Can we add another branch?

            // If not, force object into full quad regardless.
            if(!pStart->CanSplit(QT_MAX_DEPTH))
            {
                pStart->AddObject(pBody, true);
                pBody->pNode = pStart;
                return true;
            }

            // Otherwise, create another branch.
            else
            {
                pStart->Split();
                return RInsert(pBody, pStart);
            }
        }

        pBody->pNode = pStart;
        return true;
    }

    // Just a branch, has babies.
    else
    {
        for(uint8_t i = 0; i < QTNode::s_SPLIT_SIZE; ++i)
        {
            if(pStart->operator[](i)->NodeCollision(pBody))
                return RInsert(pBody, pStart->operator[](i));
        }
    }

    // This shit shouldn't happen, unless called by Update().
    // If it does, though, we go up the tree to the root (makes sense lol),
    // to find a node that we can fit in.

    // Return false if we've reached the root node and
    // still can't insert.
    return (pStart->mp_Parent == NULL) ? false :
            RInsert(pBody, pStart->mp_Parent);
}

obj::CRigidBody* CQuadTree::CheckLeaf(const obj::CRigidBody* pBody,
                                          const QTNode* pStart) const
{
    if(pStart->IsLeaf())
    {
        for(QTNode::QTListCIt_t i = pStart->GetObjects().cbegin();
            i != pStart->GetObjects().cend(); ++i)
        {
            if((*i)->CheckCollision(pBody) && (*i) != pBody) return *i;
        }
    }

    else
    {
        for(size_t i = 0; i < QTNode::s_SPLIT_SIZE; ++i)
        {
            obj::CRigidBody* pResult = this->CheckLeaf(pBody,
                                                       pStart->operator[](i));
            
            if(pResult != NULL) return pResult;
        }
    }

    return NULL;
}

bool CQuadTree::RRemove(obj::CRigidBody* pBody, QTNode* pStart)
{
    if(pStart->IsLeaf())
    {
        return pStart->RemoveObject(pBody);
    }

    else
    {
        for(size_t i = 0; i < QTNode::s_SPLIT_SIZE; ++i)
        {
            if(this->RRemove(pBody, pStart->operator[](i))) return true;
        }
    }

    return false;
}

#ifdef _DEBUG
void CQuadTree::RPrintNode(const QTNode* pNode) const
{
    if(pNode == NULL) return;

    pNode->Print();

    if(pNode->HasChildren())
    {
        for(size_t i = 0; i < QTNode::s_SPLIT_SIZE; ++i)
            RPrintNode(pNode->operator[](i));
    }
}
#endif // _DEBUG
