#include "tree_model.hpp"
#include "tree_item.hpp"

#include <QModelIndex>

int TreeModel::insertions_ = 0;

TreeModel::TreeModel (int columns, QObject* parent)
    : QAbstractItemModel{parent}
    , rootItem{std::make_unique<TreeItem> ()} {
    createTestItems();
}

int TreeModel::columnCount (const QModelIndex& parent) const {
    Q_UNUSED(parent)

    return columns_;
}

QVariant TreeModel::data (const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    auto* item = itemFromIndex(index);
    return item->data(index.column(), role);
}

Qt::ItemFlags TreeModel::flags (const QModelIndex& index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

TreeItem* TreeModel::itemFromIndex (const QModelIndex& index) const {
    if (index.isValid()) {
        auto* item = static_cast<TreeItem*> (index.internalPointer());
        if (item) {
            return item;
        }
    }

    return rootItem.get();
}

QVariant TreeModel::headerData (int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        return rootItem->data(section, role);
    } else {
        return {};
    }
}

QModelIndex TreeModel::index (int row, int column, const QModelIndex& parent) const {
    if (parent.isValid() && parent.column() != 0) {
        return {};
    }

    auto* parent_item = itemFromIndex(parent);
    if (!parent_item) {
        return {};
    }

    auto* child_item = parent_item->child(row);
    if (child_item) {
        return createIndex(row, column, child_item);
    } else {
        return {};
    }
}

bool TreeModel::insertRows (int row, int count, const QModelIndex& parent) {
    auto* parent_item = itemFromIndex(parent);
    if (!parent_item) {
        return false;
    }

    int last_row = row + count - 1;
    beginInsertRows(parent, row, last_row);
    for (int i = row; i <= last_row; ++i) {
        auto item = std::make_unique<TreeItem> (parent_item);
        item->setData(0, Qt::DisplayRole, "Insertion " + QString::number(++insertions_));
        parent_item->insertChild(row, std::move(item));
    }
    endInsertRows();

    return true;
}

QModelIndex TreeModel::parent (const QModelIndex& index) const {
    if (!index.isValid()) {
        return {};
    }

    auto* child_item  = itemFromIndex(index);
    auto* parent_item = child_item
                        ? child_item->parent()
                        : nullptr;

    if (parent_item == rootItem.get() || !parent_item) {
        return QModelIndex();
    }

    return createIndex(parent_item->row(), 0, parent_item);
}

int TreeModel::rowCount (const QModelIndex& parent) const {
    const TreeItem* parentItem = itemFromIndex(parent);

    return parentItem
           ? parentItem->childCount()
           : 0;
}

bool TreeModel::setData (const QModelIndex& index, const QVariant& value, int role) {
    if (role == Qt::EditRole) {
        role = Qt::DisplayRole;
    }
    TreeItem* item = itemFromIndex(index);
    item->setData(index.column(), role, value);

    emit dataChanged(index, index, { Qt::DisplayRole });

    return true;
}

bool TreeModel::setHeaderData (int section, Qt::Orientation orientation, const QVariant& value, int role) {
    if (role != Qt::EditRole || orientation != Qt::Horizontal) {
        return false;
    }

    rootItem->setData(section, role, value);
    emit headerDataChanged(orientation, section, section);

    return true;
}

void TreeModel::createTestItems () {
    auto item1 = std::make_unique<TreeItem> ();
    item1->setData(0, Qt::DisplayRole, "row 1");
    auto item2 = std::make_unique<TreeItem> ();
    item2->setData(0, Qt::DisplayRole, "row 2");
    auto item3 = std::make_unique<TreeItem> ();
    item3->setData(0, Qt::DisplayRole, "row 3");
    auto item4 = std::make_unique<TreeItem> ();
    item4->setData(0, Qt::DisplayRole, "row 4");

    item3->appendChild(std::move(item4));
    item1->appendChild(std::move(item2));
    item1->appendChild(std::move(item3));
    rootItem->appendChild(std::move(item1));
}
