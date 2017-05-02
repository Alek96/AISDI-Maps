#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  struct Node
  {
    value_type Data;
    Node *Left, *Right, *Parent;
    int Height;
    Node(key_type key, mapped_type mapped)
      : Data(std::make_pair( key, mapped )), Left(nullptr), Right(nullptr), Parent(nullptr), Height(1)
    {}
    Node(Node* it) : Node(it->Data.first,it->Data.second) {}
    Node(const Node* it) : Node(it->Data.first,it->Data.second) {}
    Node(value_type it) : Node(it.first,it.second) {}

    ~Node() { delete Left; delete Right; }
  };
  Node* Head;
  size_type count;

public:
  TreeMap() : Head(nullptr), count(0) {}

  TreeMap(std::initializer_list<value_type> list) : TreeMap()
  {
    for (auto it = list.begin(); it != list.end(); ++it) {
      insert( new Node(*it) );
    }
  }

  TreeMap(const TreeMap& other) : TreeMap()
  {
    for (auto it = other.begin(); it != other.end(); ++it) {
      insert(new Node(*it));
    }
  }

  TreeMap(TreeMap&& other) : TreeMap()
  {
    *this = std::move(other);
  }

  ~TreeMap()
  {
    erase();
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(this != &other) {
      erase();
      for (auto it = other.begin(); it != other.end(); ++it) {
        insert( new Node(*it) );
      }
    }
    return *this;
  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(this != &other) {
      erase();
      Head = other.Head;
      count = other.count;
      other.Head = nullptr;
      other.count = 0;
    }
    return *this;
  }

  bool isEmpty() const
  {
    return count == 0;
  }

private:
  void erase()
  {
    delete Head;
    Head = nullptr;
    count = 0;
  }

  void insert(Node* newNode)
  {
    if (Head == nullptr) {
      Head = newNode;
      count++;
      return;
    }

    Node* current = Head;
    while (true) {
      if (newNode->Data.first > current->Data.first) {
        if (current->Right == nullptr) {
          current->Right = newNode;
          current->Right->Parent = current;
          break;
        }
        else
          current = current->Right;
      }
      else if (newNode->Data.first < current->Data.first) {
        if (current->Left == nullptr) {
          current->Left = newNode;
          current->Left->Parent = current;
          break;
        }
        else
          current = current->Left;
      }
      else {
        delete newNode;
        return;
      }
    }
    count++;
    repairsTree(current);
    return;
  }
  void repairsTree(Node* current)
  {
    while (current != nullptr)
    {
      if (!isBalance(current)) {
        if (getHeight(current->Left) <= getHeight(current->Right)) {
          if (getHeight(current->Right->Left) <= getHeight(current->Right->Right)) {
            switch_Right_With_Parent(current);
          }
          else {
            switch_Left_With_Parent(current->Right);
            upgradeHeight(current->Right);
            switch_Right_With_Parent(current);
          }
        }
        else {
          if (getHeight(current->Left->Left) >= getHeight(current->Left->Right)) {
            switch_Left_With_Parent(current);
          }
          else {
            switch_Right_With_Parent(current->Left);
            upgradeHeight(current->Left);
            switch_Left_With_Parent(current);
          }
        }
      }

      upgradeHeight(current);
      current = current->Parent;
    }
  }

  int getHeight(Node* current)
  {
    if (current == nullptr)		return 0;
    else						return current->Height;
  }
  int maxHeight(Node* A, Node* B)
  {
    return getHeight(A) > getHeight(B) ? getHeight(A) : getHeight(B);
  }
  void upgradeHeight(Node* current)
  {
    current->Height = maxHeight(current->Left, current->Right) + 1;
  }
  bool isBalance(Node* current)
  {
    if (current == nullptr)
      return false;
    if (getHeight(current->Left) +0 == 0+ getHeight(current->Right) ||
        getHeight(current->Left) +1 == 0+ getHeight(current->Right) ||
        getHeight(current->Left) +0 == 1+ getHeight(current->Right))
      return true;
    return false;
  }

  void switch_Right_With_Parent(Node* current)
  {
    Node *P, *R, *RL;
    P = current->Parent;
    R = current->Right;
    RL = current->Right->Left;

    if (P != nullptr) {
      if (P->Right == current)	P->Right = R;
      else						P->Left = R; }
    else			Head = R;
      R->Parent = P;

    R->Left = current;
    current->Parent = R;

    current->Right = RL;
    if(RL != nullptr)	RL->Parent = current;

    upgradeHeight(current);
  }
  void switch_Left_With_Parent(Node* current)
  {
    Node *P, *L, *LR;
    P = current->Parent;
    L = current->Left;
    LR = current->Left->Right;

    if (P != nullptr) {
      if (P->Right == current)	P->Right = L;
      else						P->Left = L; }
    else			Head = L;
      L->Parent = P;

    L->Right = current;
    current->Parent = L;

    current->Left = LR;
    if(LR != nullptr)	LR->Parent = current;

    upgradeHeight(current);
  }

  Node* findNode(const key_type& key) const
  {
    Node* current = Head;
    while ( current!=nullptr ) {
      if (key > current->Data.first) {
        current = current->Right;
      }
      else if (key < current->Data.first) {
        current = current->Left;
      }
      else
        break;
    }
    return current;
  }

  Node* findLowNode(Node* current)
  {
    if(current == nullptr)
      return nullptr;
    while(current->Left != nullptr)
      current = current->Left;
    return current;
  }


public:
  mapped_type& operator[](const key_type& key)
  {
    Node* current = findNode(key);
    if( current == nullptr ) {
      current = new Node(key,mapped_type());
      insert( current );
    }
    return current->Data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
    const Node* current = findNode(key);
    if(current == nullptr)
      throw std::out_of_range("valueOf");
    return current->Data.second;
  }

  mapped_type& valueOf(const key_type& key)
  {
    Node* current = findNode(key);
    if(current == nullptr)
      throw std::out_of_range("valueOf");
    return current->Data.second;
  }

  const_iterator find(const key_type& key) const        ///czy moge uzyc zwyklego findNode bez const, wyskakuje blad
  {
    return const_iterator(this,findNode(key));;
  }

  iterator find(const key_type& key)                    ///uzycie findNode
  {
    return iterator(this,findNode(key));;
  }

private:
  void remove(Node* old)
  {
    if(old->Left == nullptr && old->Right == nullptr) {
      if(old->Parent != nullptr ) {
        if(old->Parent->Left == old)
          old->Parent->Left = nullptr;
        else
          old->Parent->Right = nullptr;
      }
      else
        Head = nullptr;
    }
    else if(old->Left == nullptr) {
      if(old->Parent != nullptr ) {
        if(old->Parent->Left == old)
          old->Parent->Left = old->Right;
        else
          old->Parent->Right = old->Right;
      }
      else
        Head = old->Right;
      old->Right->Parent = old->Parent;
    }
    else if(old->Right == nullptr) {
      if(old->Parent != nullptr ) {
        if(old->Parent->Left == old)
          old->Parent->Left = old->Left;
        else
          old->Parent->Right = old->Left;
      }
      else
        Head = old->Left;
      old->Left->Parent = old->Parent;
    }
    else {
      Node* current = old;
      current = current->Right;
      while(current->Left != nullptr)
        current = current->Left;

      current->Parent->Left = current->Right;
      if(current->Right != nullptr)
        current->Right = current->Parent;

      if(old->Parent != nullptr) {
        if(old->Parent->Left == old)
          old->Parent->Left = current;
        else
          old->Parent->Right = current;
      }
      else
        Head = current;
      current->Parent = old->Parent;

      current->Left = old->Left;
      current->Left->Parent = current;
    }

    count--;
    old->Left = nullptr;
    old->Right = nullptr;
    delete old;
  }

public:
  void remove(const key_type& key)
  {
    Node* current = findNode(key);
    if( current == nullptr )
      throw std::out_of_range ("remove");
    remove(current);
  }

  void remove(const const_iterator& it)
  {
    Node* current = findNode(it->first);
    if(this != it.tree || current != it.node || current == nullptr)
      throw std::out_of_range ("remove");
    remove(current);
  }

  size_type getSize() const
  {
    return count;
  }

  bool operator==(const TreeMap& other) const
  {
    if( count != other.count )
      return false;

    for(auto it = begin(), it2 = other.begin(); it != end(); ++it, ++it2) {
      if(*it != *it2)
        return false;
    }
    return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()                  ///??????????
  {
    iterator it(this, findLowNode(Head));
    return it;
  }

  iterator end()
  {
    iterator it(this);
    return it;
  }

  const_iterator cbegin() const     ///?????????
  {
    Node* current = Head;
    if(current != nullptr)
      while(current->Left != nullptr)
        current = current->Left;
    const_iterator it(this, current);
    return it;
  }

  const_iterator cend() const
  {
    const_iterator it(this);
    return it;
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type*;

private:
  const TreeMap *tree;
  Node *node;
  friend void TreeMap<KeyType, ValueType>::remove(const const_iterator&);

public:
  explicit ConstIterator(const TreeMap *tree = nullptr, Node *node = nullptr) : tree(tree), node(node) {}

  ConstIterator(const ConstIterator& other) : ConstIterator(other.tree, other.node) {}

  ConstIterator& operator++()
  {
    if(tree == nullptr || node == nullptr) {
      throw std::out_of_range("operator++");
    }
    else if( node->Right != nullptr ) {
        node = node->Right;
        while(node->Left != nullptr)
          node = node->Left;
    }
    else {
        while(true) {
          if(node->Parent == nullptr) {
            node = nullptr;
            break;
          }
          if(node->Parent->Left == node) {
            node = node->Parent;
            break;
          }
          node = node->Parent;
        }
    }

    return *this;
  }

  ConstIterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  ConstIterator& operator--()
  {
    if(tree == nullptr || tree->Head == nullptr) {
      throw std::out_of_range("operator--");
    }
    else if(node == nullptr) {
        node = tree->Head;
        while(node->Right != nullptr)
          node = node->Right;
    }
    else if(node->Left != nullptr ) {
        node = node->Left;
        while(node->Right != nullptr)
          node = node->Right;
    }
    else {
        while(true) {
          if(node->Parent == nullptr) {
            throw std::out_of_range("operator--");
          }
          if(node->Parent->Right == node) {
            node = node->Parent;
            break;
          }
          node = node->Parent;
        }
    }

    return *this;
    //throw std::runtime_error("TODO");
  }

  ConstIterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  reference operator*() const
  {
    if(node == nullptr)
      throw std::out_of_range("operator*");
    return node->Data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return tree == other.tree && node == other.node;
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type*;

  explicit Iterator(TreeMap *tree = nullptr, Node *node = nullptr)
    : ConstIterator(tree, node)
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif /* AISDI_MAPS_MAP_H */
