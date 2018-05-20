﻿#ifndef STORAGE_H
#define STORAGE_H
#include <QtCore>

template<class T>
struct IDatas {
    virtual ~IDatas() {}
    virtual unsigned int get_size() {
        return 1;
    }
};

template<class T = double>
struct DataNode : public IDatas<T> {
    DataNode() : data_ptr(nullptr), uid(0) {}
    virtual ~DataNode() {
        reset();
    }

    virtual void reset() {
        if(data_ptr != nullptr) delete data_ptr;
    }
    unsigned int uid;
    T* data_ptr;
};

template<class T = double>
struct DataNodeBank : public IDatas<T> {
    virtual ~DataNodeBank() {
        reset();
    }

    virtual void reset() {
        if(node_bank.isEmpty()) return;
        foreach(QSharedPointer<DataNode<T>> item, node_bank) {
            item.reset();
        }
        node_bank.clear();
    }
    virtual unsigned int get_size() {
        return node_bank.size();
    }
    QHash<unsigned int, QSharedPointer<DataNode<T>>> node_bank;
};

template<class T>
class TStruct {
public:
    virtual ~TStruct() {}

    virtual unsigned int calc_first_key_part(const unsigned int key) {
        unsigned int mask = 0xFFFFFF00;
        return (unsigned int)(key & mask) >> 8;
    }
    virtual unsigned int calc_second_key_part(const unsigned int key) {
        unsigned int mask = 0x000000FF;
        return (unsigned int)(key & mask);
    }
    virtual void add_item(QSharedPointer<DataNode<T>>& new_item_ptr) {
        unsigned int first_key_part = calc_first_key_part(new_item_ptr->uid);
        if(!storage.contains(first_key_part)) {
            storage.insert(first_key_part, new_item_ptr);
        }
        else {
            unsigned int second_key_part = calc_second_key_part(new_item_ptr->uid);
            QSharedPointer<IDatas<T>> utype_ptr = storage[first_key_part];
            if(utype_ptr->get_size() == 1) {
                QSharedPointer<DataNode<T>> old_node_ptr = qSharedPointerCast<DataNode<T>>(utype_ptr);
                QSharedPointer<DataNodeBank<T>> data_node_bank(new DataNodeBank<T>());
                data_node_bank->node_bank.insert(calc_second_key_part(old_node_ptr->uid), old_node_ptr);
                data_node_bank->node_bank.insert(second_key_part, new_item_ptr);
                storage.insert(first_key_part, data_node_bank);
            }
            else {
                QSharedPointer<DataNodeBank<T>> data_node_bank = qSharedPointerCast<DataNodeBank<T>>(utype_ptr);
                data_node_bank->node_bank.insert(second_key_part, new_item_ptr);
            }
        }
    }
private:
    QHash<unsigned int, QSharedPointer<IDatas<T>>> storage;
};

#endif // STORAGE_H
