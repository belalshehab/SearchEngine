#ifndef TRIE_H
#define TRIE_H
#include <QString>
#include <QLinkedList>
#include <QVector>
#include <QMap>
#include <QDebug>

#include <utility>
#include <algorithm>

template<class ValueType>
class Trie
{
    struct Node
    {
        QChar key;

        QLinkedList<Node *> childrens;

        ValueType data;

        bool hasData;


        Node(QChar c): key(c), hasData(false)
        {
        }

        bool operator==(const Node &other)
        {
            return key == other.key;
        }

        bool operator==(QChar c)
        {
            return key == c;
        }

    };

public:
    Trie();

    ValueType &insert(const QString &key);

    ValueType &insert(const QString &key, const ValueType &value);

    const ValueType &operator[](const QString &key) const;

    ValueType &operator[](const QString &key);

    std::pair<bool, ValueType> search(const QString &key) const;
    ValueType searchPrefix(const QString &key) const;

    QVector<std::pair<QString, ValueType> > getData() const;

    QVector<std::pair<QString, ValueType> > traverse(QString word = "", Node *node = nullptr);

private:
    Node *m_root;

    QVector<std::pair<QString, Node *> > m_vector;

    QVector<std::pair<QString, ValueType> > m_vector2;
    int m_size;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class ValueType>
Trie<ValueType>::Trie():
    m_root(new Node(' ')), m_size(0)
{
}


template<class ValueType>
ValueType &Trie<ValueType>::insert(const QString &key)
{
    Node *node = m_root;

    bool newFlag = false;

    for(const QChar &c: key)
    {
        auto itr = std::find_if(node->childrens.begin(), node->childrens.end(), [=](Node *node){
            return *node == c;
        });

        if(itr == node->childrens.end())
        {
            node->childrens.push_back((new Node(c)));
            node = node->childrens.back();
            newFlag = true;
        }

        else
        {
            node = *itr;
        }
    }

    if(newFlag)
    {
        node->hasData = true;
        m_vector.push_back({key, node});
    }

    return node->data;
}

template<class ValueType>
ValueType &Trie<ValueType>::insert(const QString &key, const ValueType &value)
{
    ValueType data = insert(key);

    data = value;
    return data;
}

template<class ValueType>
const ValueType &Trie<ValueType>::operator[](const QString &key) const
{
    Node *node = m_root;

    for(const QChar &c: key)
    {
        auto itr = std::find_if(node->childrens.begin(), node->childrens.end(), [=](Node *node){
            return node->key == c;
        });

        if(itr == node->childrens.end())
        {
            return {};
        }
        else
        {
            node = *itr;
        }
    }
    return node->data;
}

template<class ValueType>
ValueType &Trie<ValueType>::operator[](const QString &key)
{
    return insert(key);
}


template<class ValueType>
std::pair<bool, ValueType> Trie<ValueType>::search(const QString &key) const
{
    Node *node = m_root;

    for(const QChar &c: key)
    {
        auto itr = std::find_if(node->childrens.begin(), node->childrens.end(), [=](Node *node){
            return *node == c;
        });

        if(itr == node->childrens.end())
        {
            return {false, {}};
        }
        else
        {
            node = *itr;
        }
    }

    return {node->hasData, node->data};
}

template<class ValueType>
ValueType Trie<ValueType>::searchPrefix(const QString &key) const
{
    return {};
}

template<class ValueType>
QVector<std::pair<QString, ValueType> > Trie<ValueType>::getData() const
{
    QVector<std::pair<QString, ValueType> > vec;
    for(const std::pair<QString, Node*> &elemnt: m_vector)
    {
        vec.push_back({elemnt.first, elemnt.second->data});
    }
    return vec;
}

template<class ValueType>
QVector<std::pair<QString, ValueType> > Trie<ValueType>::traverse(QString word, Node *node)
{
    if(node == nullptr || node == m_root)
    {
        node = m_root;
        word = "";
    }

    for(Node *child : node->childrens)
    {
        if(child->hasData)
        {
//            qInfo() << word << ": " << child->data;
            m_vector2.push_back({word, child->data});
        }
        return traverse(word + child->key, child);
    }
}

#endif // TRIE_H
