#include "tree_item.hpp"

#include <stdexcept>

#include <QDebug>
#define DEB qDebug()

TreeItem::TreeItem (int columns, TreeItem* parent)
    : parent_(parent)
    , data_{std::vector<ItemData> (columns)} {}

TreeItem* TreeItem::child (int index) {
    if (index < 0 || index >= static_cast<int> (children_.size())) {
        return nullptr;
    }
    return children_[static_cast<size_t> (index)].get();
}

int TreeItem::childCount () const {
    return static_cast<int> (children_.size());
}

int TreeItem::columns () const {
    return data_.size();
}

int TreeItem::row () const {
    if (parent_) {
        // TODO: remake method
        const auto& siblings = parent_->children_;
        for (int i = 0; i < siblings.size(); ++i) {
            if (siblings[i].get() == this) {
                return i;
            }
        }
    }
    return 0;
}

void TreeItem::insertColumns (int start, int count) {
    // FIXME: check count too; handle exceptions
    if (start < 0 || start > data_.size()) {
        DEB << "start" << start;
        throw std::invalid_argument("Wrong start argument is out of range");
    }

    for (int i = start; i < start + count; ++i) {
        data_.insert(data_.begin() + i, ItemData {});
    }

    for (auto& child : children_) {
        child->insertColumns(start, count);
    }
}

void TreeItem::appendChild (ItemUptr item) {
    item->setParent(this);
    children_.push_back(std::move(item));
}

void TreeItem::removeChildren (int first, int count) {
    auto iter = children_.begin() + first;
    children_.erase(iter, iter + count);
}

void TreeItem::removeColumns (int start, int count) {
    if (start < 0 || start >= data_.size()) {
        return;
    }

    data_.erase(data_.begin() + start, data_.begin() + start + count);
    for (auto& child : children_) {
        child->removeColumns(start, count);
    }
}

void TreeItem::setParent (TreeItem* parent) {
    parent_ = parent;
}

QVariant TreeItem::data (int column, int role) const {
    if (column >= 0 && column < data_.size()) {
        return data_[column].value(role);
    } else {
        return {};
    }
}

void TreeItem::insertChild (int index, ItemUptr item) {
    if (index < 0) {
        return;
    }
    if (index > static_cast<int> (children_.size())) {
        index = static_cast<int> (children_.size());
    }

    children_.insert(children_.begin() + index, std::move(item));
}

TreeItem* TreeItem::parent () {
    return parent_;
}

void TreeItem::setData (int column, int role, const QVariant& value) {
    if (column < 0 || column >= data_.size()) {
        return;
    } else {
        data_[column].setValue(value, role);
    }
}
