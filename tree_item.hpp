#ifndef TREE_ITEM_HPP
#define TREE_ITEM_HPP

#include "item_data.hpp"

#include <QVariant>
#include <QHash>

#include <memory>
#include <vector>

class TreeItem {
public:
    using ItemUptr = std::unique_ptr<TreeItem>;

    explicit TreeItem (int columns = 0, TreeItem* parent = nullptr);

    TreeItem* parent ();
    TreeItem* child (int index);
    int       childCount () const;
    int       columns () const;
    int       row () const;
    void      insertChild (int index, ItemUptr item);
    void      appendChild (ItemUptr item);
    void      insertColumns (int start, int count);
    void      removeColumns (int start, int count);
    QVariant  data (int column, int role = Qt::DisplayRole) const;
    void      setData (int column, int role, const QVariant& value);

private:
    // methods
    void setParent (TreeItem* parent);

    // data
    TreeItem* parent_;
    std::vector<ItemUptr> children_;
    std::vector<ItemData> data_;
};

#endif // TREE_ITEM_HPP
