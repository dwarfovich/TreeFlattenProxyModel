#ifndef TREE_ITEM_HPP
#define TREE_ITEM_HPP

#include <QVariant>
#include <QHash>

#include <memory>
#include <vector>

class TreeItem {
public:
    using ItemUptr = std::unique_ptr<TreeItem>;

    explicit TreeItem (TreeItem* parent = nullptr);

    TreeItem* parent ();
    TreeItem* child (int index);
    int       childCount () const;
    int       row () const;
    void      insertChild (int index, ItemUptr item);
    void      appendChild (ItemUptr item);
    QVariant  data (int column, int role = Qt::DisplayRole) const;
    void      setData (int column, int role, const QVariant& value);

private:
    // methods
    void setParent (TreeItem* parent);

    // data
    TreeItem* parent_;
    std::vector<ItemUptr> children_;
    QHash<QPair<int, int>, QVariant> data_;
};

#endif // TREE_ITEM_HPP
