#include "tree_item.hpp"

TreeItem::TreeItem (TreeItem* parent)
    : parent_(parent) {}

TreeItem* TreeItem::child (int index) {
    if (index < 0 || index >= static_cast<int> (children_.size())) {
        return nullptr;
    }
    return children_[static_cast<size_t> (index)].get();
}

int TreeItem::childCount () const {
    return static_cast<int> (children_.size());
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

void TreeItem::appendChild (ItemUptr item) {
    item->setParent(this);
    children_.push_back(std::move(item));
}

void TreeItem::setParent (TreeItem* parent) {
    parent_ = parent;
}

QVariant TreeItem::data (int column, int role) const {
    return data_.value({ column, role }, {});
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
    data_[{ column, role }] = value;
}
