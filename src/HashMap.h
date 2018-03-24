#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <functional>
#include <vector>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
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
        Node* next;
        Node *prev;
        value_type value;

        Node(): next(nullptr), prev(nullptr){}
        Node(const key_type& key): next(nullptr), prev(nullptr), value(std::make_pair(key, mapped_type())){}


    };

     Node* tail;
    size_type size = 0;
    static const size_type TABLE_SIZE = 16384;
    std::pair<Node*, Node*>* table;



public:

  HashMap()
  {

       table = new std::pair<Node*, Node*>[TABLE_SIZE];
       tail = new Node;
      tail->next = tail->prev = tail;
      size= 0;
  }

  HashMap(std::initializer_list<value_type> list):HashMap()
  {
      for(auto it = list.begin();it!=list.end();it++)
        operator[]((*it).first)=(*it).second;

  }
    ~HashMap()
    {
        delete [] table;
        delete tail;
    }

  HashMap(const HashMap& other):HashMap()
  {

      for(auto it = other.begin(); it!=other.end();it++)
          operator[]((*it).first)=(*it).second;

  }

  HashMap(HashMap&& other):HashMap()
  {
      size = other.size;
      auto swappingTail = tail;
      tail = other.tail;
        for(size_type i = 0;i < TABLE_SIZE;i++)
        {
            table[i].first = other.table[i].first;
            table[i].second = other.table[i].second;
            other.table[i].first = nullptr;
            other.table[i].second = nullptr;
        }
      other.size = 0;
      other.tail = swappingTail;


  }

  HashMap& operator=(const HashMap& other)
  {
      if(*this == other)
          return *this;

      for(auto it = begin();it!=end();)
      {
          remove(it++);
      }

      if(other.isEmpty())
          return *this;
      for(auto it = other.begin(); it!=other.end();++it)
               operator[]((*it).first)=(*it).second; ///?

    return *this;
  }

  HashMap& operator=(HashMap&& other)
  {
      if(*this == other )
          return *this;
      for(auto it = begin();it!=end();)
          remove(it++);
      size = other.size;
        auto swappingPtr = tail;

      tail = other.tail;
      for(size_type i = 0;i < TABLE_SIZE;i++)
      {
          table[i].first = other.table[i].first;
          table[i].second = other.table[i].second;
          other.table[i].first = nullptr;
          other.table[i].second = nullptr;
      }
      other.size = 0;
      other.tail = swappingPtr;

      return *this;
  }

  bool isEmpty() const
  {
      return size == 0;
  }

  mapped_type& operator[](const key_type& key)
  {
      if(isEmpty())
      {

          auto newKey =(std::hash<key_type>{}(key)%TABLE_SIZE);
          table[newKey].first=tail->next = tail->prev = new Node(key);
          table[newKey].second = tail;
          tail->prev->next=tail->next->prev=tail;

          size++;
          return tail->next->value.second;
      }

    if(find(key) == end())
    {
       auto  newNode = new Node(key);
        tail->prev->next=newNode;
        auto changeHashField = tail->prev->value.first;
        newNode->prev=  tail->prev;
        newNode->next = tail;
        tail->prev = newNode;
        table[(std::hash<key_type>{} (changeHashField)%TABLE_SIZE)].second=newNode;
        auto newKey =(std::hash<key_type>{} (key)%TABLE_SIZE);
        if(table[newKey].first==table[newKey].second)
        {
            table[newKey].first = newNode;
            table[newKey].second = tail;

        }
        size++;
        return newNode->value.second;
    }else
        return valueOf(key);
  }


  const mapped_type& valueOf(const key_type& key) const
  {
      if(isEmpty())
          throw std::out_of_range("map is empty");
      const_iterator position = find(key);
      if(position == cend())
          throw std::out_of_range("key does not exist");
      return (*position).second;

  }

  mapped_type& valueOf(const key_type& key)
  {
      if(isEmpty())
          throw std::out_of_range("map is empty");
      iterator position = find(key);
      if(position == end())
          throw std::out_of_range("key does not exist");
      return (*position).second;
  }

  const_iterator find(const key_type& key) const
  {
      if(isEmpty())
          return cend();
      auto newKey =(std::hash<key_type>{} (key)%TABLE_SIZE);
      if(table[newKey].first==table[newKey].second)
        return cend();
      for(auto ptr = table[newKey].first; ptr!=table[newKey].second;ptr = ptr->next)
      {
          if(ptr->value.first ==key)
              return const_iterator(ptr,tail);
      }
      return cend();
  }

  iterator find(const key_type& key)
  {
      if(isEmpty())
          return end();
      auto newKey =(std::hash<key_type>{} (key)%TABLE_SIZE);
      if(table[newKey].first==table[newKey].second)
          return end();
      for(auto ptr = table[newKey].first; ptr!=table[newKey].second;ptr = ptr->next)
      {
          if(ptr->value.first ==key)
              return iterator(const_iterator(ptr,tail));
      }
      return end();
  }

  void remove(const key_type& key)
  {
      if(isEmpty())
          throw std::out_of_range("cannot remove, empty list");
      if(find(key)==end())
          throw std::out_of_range("cannot remove, no such element");
      remove(find(key));


  }

  void remove(const const_iterator& it)
  {
      if(isEmpty())
          throw std::out_of_range("cannot remove, empty list");
      if(it==cend())
          throw std::out_of_range("cannot remove, no such element");
      auto oldKey = std::hash<key_type>{}((*it).first) % TABLE_SIZE;
      auto deletingNode = it.currentNode;


      if(it == cbegin())
        {
            it.itTail->next = it.currentNode->next;
            it.currentNode->next->prev = it.itTail;

            if(table[oldKey].first==it.currentNode)
            {
                if(table[oldKey].second==it.currentNode->next)
                    table[oldKey].second = table[oldKey].first = nullptr;
                else
                    table[oldKey].first = it.currentNode->next;
            }


        }else
      {
          if(table[oldKey].first==it.currentNode)
          {
              if(table[oldKey].second==it.currentNode->next)
                  table[oldKey].second = table[oldKey].first = nullptr;
              else
                  table[oldKey].first = it.currentNode->next;
              auto prevKey = std::hash<key_type>{}(it.currentNode->prev->value.first) % TABLE_SIZE;
              table[prevKey].second = it.currentNode->next;
          }

          it.currentNode->prev->next = it.currentNode->next;
          it.currentNode->next->prev = it.currentNode->prev;
      }

      delete deletingNode;
      size--;


  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap& other) const
  {
      for(auto it = begin();it!= end();it++)
      {
          value_type value = *it;
          if(other.find(value.first)==other.end())
              return false;
          else if(other.valueOf(value.first)!=value.second)
              return false;
      }

      for(auto otherIt = other.begin();otherIt!= other.end();otherIt++)
      {
          value_type value = *otherIt;
          if(find(value.first)==end())
              return false;
          else if(valueOf(value.first)!=value.second)
              return false;
      }

      return true;



  }

  bool operator!=(const HashMap& other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {

    return Iterator(const_iterator(tail->next,tail));
  }

  iterator end()
  {
    return Iterator(const_iterator(tail,tail));
  }

  const_iterator cbegin() const
  {
    return const_iterator(tail->next,tail);
  }

  const_iterator cend() const
  {
      return const_iterator(tail,tail);
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
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type*;
    Node* currentNode, *itTail;

  explicit ConstIterator():currentNode(nullptr), itTail(nullptr)
  {}

    ConstIterator(Node *current,Node *tableTail):currentNode(current),  itTail(tableTail)
    {}

  ConstIterator(const ConstIterator& other)
  {
    currentNode=other.currentNode;
      itTail = other.itTail;
  }

  ConstIterator& operator++()
  {
      if(currentNode== nullptr)
          throw std::out_of_range(" uniniclazad++");
    if(currentNode == itTail)
        throw std::out_of_range("cannot increment end");
      currentNode=currentNode->next;
      return *this;
  }

  ConstIterator operator++(int)
  {
      if(currentNode== nullptr)
           throw std::out_of_range(" uniniclazad++");
      auto tmp=*this;
      operator++();
      return  tmp;
  }

  ConstIterator& operator--()
  {
    if(currentNode == itTail->next)
        throw std::out_of_range("cannot decrement begginig");
      currentNode=currentNode->prev;
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
      if(itTail == nullptr )
          throw std::out_of_range(" tail");
           if(currentNode == itTail)
          throw std::out_of_range(" end");
    return currentNode->value;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator& other) const
  {
    return (currentNode==other.currentNode);
  }

  bool operator!=(const ConstIterator& other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type*;

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

#endif /* AISDI_MAPS_HASHMAP_H */
