#include "tree_model.hpp"
#include "tree_item.hpp"

#include <QModelIndex>

#include <QDebug>
#define DEB qDebug()

int TreeModel::insertions_ = 0;

TreeModel::TreeModel (int columns, QObject* parent)
    : QAbstractItemModel{parent}
    , root_{std::make_unique<TreeItem> (columns)} {
    createTestItems();
}

int TreeModel::columnCount (const QModelIndex& parent) const {
    Q_UNUSED(parent)

    return root_->columns();
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

    return root_.get();
}

QVariant TreeModel::headerData (int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal) {
        return root_->data(section, role);
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
        auto item = std::make_unique<TreeItem> (columnCount(), parent_item);
        item->setData(0, Qt::DisplayRole, "Insertion " + QString::number(++insertions_));
        parent_item->insertChild(row, std::move(item));
    }
    endInsertRows();

    return true;
}

bool TreeModel::insertColumns (int column, int count, const QModelIndex& parent) {
    beginInsertColumns(parent, column, column + count - 1);
    root_->insertColumns(column, count);
    endInsertColumns();
    return true;
}

bool TreeModel::removeRows (int row, int count, const QModelIndex& parent) {
    if (row < 0) {
        return false;
    }

    auto* parent_item = itemFromIndex(parent);
    beginRemoveRows(parent, row, row + count - 1);
    parent_item->removeChildren(row, count);
    endRemoveRows();
    return true;
}

bool TreeModel::removeColumns (int column, int count, const QModelIndex& parent) {
    if (column < 0) {
        return false;
    }

    beginRemoveColumns(parent, column, column + count - 1);
    root_->removeColumns(column, count);
    endRemoveColumns();
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

    if (parent_item == root_.get() || !parent_item) {
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
    if (orientation == Qt::Vertical) {
        return QAbstractItemModel::setHeaderData(section, orientation, value, role);
    }

    if (role == Qt::EditRole) {
        role = Qt::DisplayRole;
    }

    root_->setData(section, role, value);
    emit headerDataChanged(orientation, section, section);

    return true;
}

void TreeModel::createTestItems () {
    auto item1 = std::make_unique<TreeItem> (1);
    item1->setData(0, Qt::DisplayRole, "row 1");
    auto item2 = std::make_unique<TreeItem> (1);
    item2->setData(0, Qt::DisplayRole, "row 2");
    auto item3 = std::make_unique<TreeItem> (1);
    item3->setData(0, Qt::DisplayRole, "row 3");
    auto item4 = std::make_unique<TreeItem> (1);
    item4->setData(0, Qt::DisplayRole, "row 4");

    item3->appendChild(std::move(item4));
    item1->appendChild(std::move(item2));
    item1->appendChild(std::move(item3));
    root_->appendChild(std::move(item1));
    setHeaderData(0, Qt::Horizontal, "Column 1", Qt::DisplayRole);
}

void TreeModel::dumpModel () {
    std::function<void(TreeItem*)> printItems;
    printItems = [&printItems](TreeItem* item) {
                     for (int i = 0; i < item->columns(); ++i) {
                         DEB << i << ":" << item->data(i);
                     }
                     for (int i = 0; i < item->childCount(); ++i) {
                         DEB << "print next child";
                         printItems(item->child(i));
                     }
                 };
    printItems(root_.get());
}
