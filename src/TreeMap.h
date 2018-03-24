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
        value_type data;
        Node *left, *right, *parent;
        Node() {}
        Node(key_type key):data(std::make_pair(key, mapped_type{} )), left(nullptr), right(nullptr), parent(
                nullptr){}
        Node(value_type data):Node(data.first, data.second){}


    };
     Node* root;
    size_type size;
    void init()
    {
        root = new Node();
        root->right = root;
        root->left = root;
        root->parent = nullptr;
        size = 0;

    }
public:
  TreeMap()
  {
      init();
  }

  TreeMap(std::initializer_list<value_type> list)
  {
    init();
    for(auto element : list)
        operator[](element.first) = element.second;

  }

  TreeMap(const TreeMap& other)
  {
      init();
      for(auto element : other)
          operator[](element.first) = element.second;
  }

  TreeMap(TreeMap&& other)
  {
      init();
      root=other.root;
      size=other.size;
      other.root= nullptr;
      other.size=0;
  }

  TreeMap& operator=(const TreeMap& other)
  {
    if(root==other.root)
        return *this;
      if(!isEmpty())
      for(auto it = begin();it!=end();++it)
          remove(it);

         for(auto element : other)
             operator[](element.first) = element.second;
      return  *this;

  }

  TreeMap& operator=(TreeMap&& other)
  {
    if(root==other.root)
        return *this;
      for(auto it = begin();it!=end();++it)
          remove(it);
      root=other.root;
      size=other.size;
      other.root= nullptr;
      other.size=0;
      return *this;
  }

  bool isEmpty() const
  {
    return(size == 0);
  }

  mapped_type& operator[](const key_type& key)
  {
    if(isEmpty())
    {
         Node *newNode = new Node(key);
        newNode->parent = root;
        root->left = newNode;
        root->right = nullptr;
        size++;
        return newNode->data.second;
    }
      Node* next = root->left;
      Node *current = next;
      while(next!= nullptr)
      {
          current = next;
          if(key == current->data.first)
              return current->data.second;
          if(key < current->data.first)
              next = current->left;
          else
              next = current->right;
      }
      Node *newNode = new Node(key);
      newNode->parent = current;
      if(key < current->data.first)
           current->left = newNode;
      else
          current->right = newNode;
      size++;
      return newNode->data.second;
  }

  const mapped_type& valueOf(const key_type& key) const
  {
      if(isEmpty())
          throw std::out_of_range("map is empty");
      const_iterator position = find(key);
      if(position == cend())
          throw std::out_of_range("key does not exist");
      return position->second;
  }

  mapped_type& valueOf(const key_type& key)
  {
      if(isEmpty())
          throw std::out_of_range("map is empty");
      iterator position = find(key);
      if(position == end())
          throw std::out_of_range("key does not exist");
      return position->second;
  }

  const_iterator find(const key_type& key) const
  {
      if(isEmpty())
          return cend();
      return lookfor(root->left, key);

  }

  iterator find(const key_type& key)
  {
      if(isEmpty())
          return end();
      return lookfor(root->left, key);
  }

  void remove(const key_type& key)
  {
      if(isEmpty())
          throw std::out_of_range("cannot remove, empty list");
      auto removingNode = find(key).currentNode;
      if(removingNode==root)
          throw std::out_of_range("cannot remove, no such element");
      if(removingNode->left == nullptr)
      {
          disconnectNode(removingNode,removingNode->right);

      }else if(removingNode->right == nullptr)
          disconnectNode(removingNode,removingNode->left);
      else
      {
          auto tmp = removingNode->right;

          while (tmp->left != nullptr)
              tmp = tmp->left;
          if(tmp->parent != removingNode) {
              disconnectNode(tmp, tmp->right);
              tmp->right = removingNode->right;
              tmp->right->parent = tmp;
          }
          disconnectNode(removingNode, tmp);
          tmp->left = removingNode->left;
          tmp->left->parent = tmp;


      }
      delete removingNode;
      size--;
      if(isEmpty())
      {
          root->left=root;

      }


  }

  void disconnectNode(Node *disconnected, Node *son)
  {
      if(disconnected->parent == root)
          root->left = son;
      else if(disconnected->parent->left == disconnected)
      {
          disconnected->parent->left = son;


      }
      else if(disconnected->parent->right == disconnected)
      {

          disconnected->parent->right = son;

      }
      if(son!= nullptr)
          son->parent = disconnected->parent;

  }

  void remove(const const_iterator& it)
  {
        remove(it.currentNode->data.first);
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const TreeMap& other) const
  {
    if(size != other.size)
        return false;
      auto otherIt = other.begin();
      auto thisIt = begin();
      for(size_type i = 0; i < size; i++, otherIt++, thisIt++)
          if(!(otherIt->first == thisIt->first && otherIt->second == thisIt->second))
              return false;
      return true;
  }

  bool operator!=(const TreeMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
        return cbegin();
  }

  iterator end()
  {
    return cend();
  }

  const_iterator cbegin() const
  {
    if(isEmpty())
        return const_iterator(root->left);
    Node *serching = root->left;
      while(serching->left != nullptr)
          serching = serching->left;
      return const_iterator(serching);
  }

  const_iterator cend() const
  {
      return const_iterator(root);

  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

  const_iterator lookfor(Node *starting, const key_type& key) const
  {
      while(starting != nullptr)
      {
          if(starting->data.first == key)
              return const_iterator(starting);
          if(starting->data.first > key)
              starting = starting->left;
          else
              starting = starting->right;

      }
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

    Node *currentNode;

  explicit ConstIterator()
  {}

  ConstIterator(Node *pointer) :currentNode(pointer) {}

  ConstIterator(const ConstIterator& other):currentNode(other.currentNode)
  {}

  ConstIterator& operator++()
  {
      if(currentNode==currentNode->right)
          throw std::out_of_range("cannot increment end");
      if(currentNode->right == nullptr)
      {
          auto nextNode = currentNode->parent;
          while(nextNode!= nullptr && currentNode == nextNode->right)
          {
              currentNode = nextNode;
              nextNode = currentNode->parent;
          }
          currentNode=nextNode;
      } else
      {
          currentNode=currentNode->right;
          while(currentNode->left!= nullptr)
              currentNode=currentNode->left;
      }
      return *this;

  }

  ConstIterator operator++(int)
  {
      auto tmp=*this;
      operator++();
      return  tmp;
  }

  ConstIterator& operator--()
  {
      if(currentNode->right==currentNode)
          throw std::out_of_range("Cannot decrement, empty map");
      if(currentNode->left != nullptr) {
          currentNode = currentNode->left;
          while(currentNode->right != nullptr)
              currentNode = currentNode->right;
          return *this;
      }
     Node *tmp = currentNode->parent;
      while(tmp != nullptr && currentNode == tmp->left) {
          if(tmp->parent == nullptr)
              throw std::out_of_range("Cannot decrement begin");
          currentNode = tmp;
          tmp = tmp->parent;
      }
      currentNode = tmp;
      return *this;

  }

  ConstIterator operator--(int)
  {
      auto tmp=*this;
      operator--();
      return  tmp;
  }

  reference operator*() const
  {
      if(currentNode->right == currentNode)
          throw std::out_of_range("Cannot dereference end");
      return currentNode->data;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
      return this->currentNode == other.currentNode;
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

  explicit Iterator()
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
