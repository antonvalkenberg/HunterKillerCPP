#pragma once
#include <iostream>
#include <vector>

// T must provide int T::GetHashCode()
template<class T>
class Node
{
public:
    Node();
    explicit Node(T* pPayload);
    Node(T* pPayload, Node<T>* pParent);
    Node(T* pPayload, Node<T>* pParent, std::vector<Node<T>*>* pChildren);
    ~Node();
    bool operator==(Node<T>& rNode);
    bool operator!=(Node<T>& rNode);
    bool Equals(Node<T>& rNode);
    int GetHashCode();
    bool IsRoot();
    bool IsLeaf();
    bool IsAncestorOf(Node<T>& rNode);
    bool IsDescendantOf(Node<T>& rNode);
    int CalculateDepth();
    void AddChild(Node<T>* pChild);
    T* GetPayload();
    void SetPayload(T* pPayload);
    Node<T>* GetParent();
    void SetParent(Node<T>* pParent);
private:
    // This Node's subject matter. Pointer is not owned, do not delete.
    T* Payload;
    // This Node's parent node, if any. Pointer is not owned, do not delete.
    Node<T>* Parent;
    std::vector<Node<T>*>* Children;
};

template<class T>
Node<T>::Node() {
    Payload = nullptr;
    Parent = nullptr;
    Children = new std::vector<Node<T>*>();
}

template<class T>
Node<T>::Node(T* pPayload) {
    Payload = pPayload;
    Parent = nullptr;
    Children = new std::vector<Node<T>*>();
}

template<class T>
Node<T>::Node(T* pPayload, Node<T>* pParent) {
    Payload = pPayload;
    Parent = pParent;
    Children = new std::vector<Node<T>*>();
}

template<class T>
Node<T>::Node(T* pPayload, Node<T>* pParent, std::vector<Node<T>*>* pChildren) {
    Payload = pPayload;
    Parent = pParent;
    Children = pChildren;
}

template<class T>
Node<T>::~Node() {
    Payload = nullptr; // Not owned, do not delete.
    Parent = nullptr; // Not owned, do not delete.
    delete Children;
    Children = nullptr;
}

template<class T>
bool Node<T>::operator==(Node<T>& rNode) {
    return this->Equals(rNode);
}

template<class T>
bool Node<T>::operator!=(Node<T>& rNode) {
    return !this->Equals(rNode);
}

template<class T>
bool Node<T>::Equals(Node<T>& rNode) {
    return this->GetHashCode() == rNode.GetHashCode();
}

template<class T>
int Node<T>::GetHashCode()
{
    constexpr uint32_t hashOffsetBasis = 2166136261;
    constexpr uint32_t hashFNVPrime = 16777619;

    //See: http://www.isthe.com/chongo/tech/comp/fnv/#FNV-1
    uint32_t hash = hashOffsetBasis;
    
    if (Payload) {
        hash = hash ^ Payload->GetHashCode();
        hash = hash * hashFNVPrime;
    }

    if (Children) {
        for (Node<T>* child : *Children)
        {
            hash = hash ^ child->GetHashCode();
            hash = hash * hashFNVPrime;
        }
    }
    // Note that Parent isn't included in this hash, because that would cause an infinite loop of hashing

    return hash;
}

template<class T>
bool Node<T>::IsRoot()
{
    return !Parent;
}

template<class T>
bool Node<T>::IsLeaf()
{
    return Children->empty();
}

template<class T>
bool Node<T>::IsAncestorOf(Node<T>& rNode)
{
    while (!rNode.IsRoot()) {
        if (rNode.Equals(*this))
            return true;
        rNode = &rNode.GetParent();
    }
    return false;
}

template<class T>
bool Node<T>::IsDescendantOf(Node<T>& rNode)
{
    if (this->Equals(rNode))
        return false;

    Node<T>* node = this;
    while (!node->IsRoot()) {
        node = node->GetParent();
        if (node->Equals(rNode)) {
            node = 0;
            return true;
        }
    }

    node = 0;
    return false;
}

template<class T>
int Node<T>::CalculateDepth()
{
    int depth = 0;
    Node<T>* node = this;
    while (!node->IsRoot()) {
        depth++;
        node = node->GetParent();
    }
    
    node = 0;
    return depth;
}

template<class T>
void Node<T>::AddChild(Node<T>* pChild)
{
    Children->push_back(pChild);
}

template<class T>
T* Node<T>::GetPayload()
{
    return Payload;
}

template<class T>
void Node<T>::SetPayload(T* pPayload)
{
    Payload = pPayload;
}

template<class T>
Node<T>* Node<T>::GetParent()
{
    return Parent;
}

template<class T>
void Node<T>::SetParent(Node<T>* pParent)
{
    Parent = pParent;
}