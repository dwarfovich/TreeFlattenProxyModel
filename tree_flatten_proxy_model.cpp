#include "tree_flatten_proxy_model.hpp"
// #include "custom_roles.hpp"

namespace cc {
namespace gui {

TreeFlattenProxyModel::TreeFlattenProxyModel (QObject* parent)
    : QAbstractProxyModel{parent}
    , m_lastStableRow{-1} {}

QModelIndex TreeFlattenProxyModel::index (int row, int column, const QModelIndex& parent) const {
    if (parent.isValid()) {
        return {};
    }

    const auto& sourceIndex = m_proxyRowToSource.at(row);
    return createIndex(row, column, sourceIndex.internalPointer());
}

QModelIndex TreeFlattenProxyModel::parent (const QModelIndex& child) const {
    Q_UNUSED(child);

    return {};
}

int TreeFlattenProxyModel::rowCount (const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return m_proxyRowToSource.size();
}

int TreeFlattenProxyModel::columnCount (const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return sourceModel()->columnCount();
}

QVariant TreeFlattenProxyModel::headerData (int section, Qt::Orientation orientation, int role) const {
    return sourceModel()->headerData(section, orientation, role);
// if (orientation == Qt::Vertical && role == Qt::DisplayRole)
// return section;
// else
// return sourceModel()->headerData(section, orientation, role);
}

QModelIndex TreeFlattenProxyModel::mapToSource (const QModelIndex& proxyIndex) const {
    if (!proxyIndex.isValid()) {
        return {};
    }

    const auto& sourceIndex = m_proxyRowToSource.at(proxyIndex.row());
    return sourceIndex.sibling(sourceIndex.row(), proxyIndex.column());
}

QModelIndex TreeFlattenProxyModel::mapFromSource (const QModelIndex& sourceIndex) const {
    if (!sourceIndex.isValid()) {
        return {};
    }

    const auto& index0   = sourceIndex.sibling(sourceIndex.row(), 0);
    auto        proxyRow = m_sourceToProxyRow.value(index0, -1);
    return index(proxyRow, sourceIndex.column(), {});
}

void TreeFlattenProxyModel::onSourceDataChanged (const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
    Q_ASSERT(topLeft.parent() == bottomRight.parent());
    Q_ASSERT(topLeft.row() <= bottomRight.row());
    Q_ASSERT(topLeft.column() <= bottomRight.column());

    const auto& proxyTopLeft     = mapFromSource(topLeft);
    const auto& proxyBottomRight = mapFromSource(bottomRight);
    emit        dataChanged(proxyTopLeft, proxyBottomRight, roles);
}

void TreeFlattenProxyModel::onSourceRowsAboutToBeInserted (const QModelIndex& sourceParent, int first, int last) {
    Q_UNUSED(last);

    // Saving affected rows
    m_lastStableRow = getLastStableRow(sourceParent, first);
    for (int i = m_lastStableRow + 1; i < rowCount(); ++i) {
        m_rowsToRemap.insert(i, m_proxyRowToSource[i]);
    }
}

void TreeFlattenProxyModel::onSourceRowsInserted (const QModelIndex& sourceParent, int first, int last) {
    QList<QPersistentModelIndex> list;
    for (int i = first; i <= last; ++i) {
        auto sourceIndex = sourceModel()->index(i, 0, sourceParent);
        list.append(getInsertedSourceChildren(sourceIndex));
    }

    for (const auto& index : list) {
        insertRow(index, ++m_lastStableRow);
    }

    remapTemp();
    clearTemp();
}

void TreeFlattenProxyModel::onSourceRowsAboutToBeRemoved (const QModelIndex& sourceParent, int first, int last) {
    m_lastStableRow = getLastStableRow(sourceParent, first);

    // Get last row to remove
    const auto& sourceIndex     = sourceModel()->index(last, 0, sourceParent);
    const auto& lastChild       = lastGrandChild(sourceIndex);
    const auto& proxyIndex      = mapFromSource(lastChild);
    int         lastRowToRemove = proxyIndex.row();

    // Save affected rows
    for (int i = lastRowToRemove + 1; i < rowCount(); ++i) {
        m_rowsToRemap.insert(i, m_proxyRowToSource[i]);
    }
}

void TreeFlattenProxyModel::onSourceRowsRemoved (const QModelIndex& sourceParent, int first, int last) {
    Q_UNUSED(sourceParent);
    Q_UNUSED(first);
    Q_UNUSED(last);

    int firstRowToRemove = m_lastStableRow + 1;
    int lastRowToRemove  = m_rowsToRemap.empty()
                           ? rowCount() - 1
                           : m_rowsToRemap.firstKey() - 1;
    int count = lastRowToRemove - firstRowToRemove + 1;

    beginRemoveRows({}, firstRowToRemove, lastRowToRemove);
    m_proxyRowToSource.remove(firstRowToRemove, count);
    endRemoveRows();

    remapTemp();
    clearTemp();
}

void TreeFlattenProxyModel::onSourceModelReset () {
    beginResetModel();
    m_proxyRowToSource.clear();
    m_sourceToProxyRow.clear();
    // Temp variables
// int m_lastStableRow = -1;
    m_rowsToRemap.clear();
    populateFromSourceParent(sourceModel()->parent({}));
    endResetModel();
}

void TreeFlattenProxyModel::populateFromSourceParent (const QModelIndex& parent) {
    int rows = sourceModel()->rowCount(parent);
    for (int i = 0; i < rows; ++i) {
        auto sourceChild = sourceModel()->index(i, 0, parent);
        insertRow(sourceChild, ++m_lastStableRow);
        populateFromSourceParent(sourceChild);
    }
}

void TreeFlattenProxyModel::insertRow (const QModelIndex& sourceIndex, int row) {
    beginInsertRows({}, row, row);
    m_proxyRowToSource.insert(row, sourceIndex);
    m_sourceToProxyRow.insert(sourceIndex, row);
    endInsertRows();
}

void TreeFlattenProxyModel::remapTemp () {
    for (const auto& row : m_rowsToRemap.keys()) {
        const auto& index = m_rowsToRemap.value(row);
        m_sourceToProxyRow.insert(index, ++m_lastStableRow);
    }
}

void TreeFlattenProxyModel::clearTemp () {
    m_lastStableRow = -1;
    m_rowsToRemap.clear();
}

QModelIndex TreeFlattenProxyModel::lastGrandChild (const QModelIndex& parent) const {
    int rows = sourceModel()->rowCount(parent);
    if (rows == 0) {
        return parent;
    } else {
        Q_ASSERT(rows - 1 >= 0);
        const auto& lastChild = sourceModel()->index(rows - 1, 0, parent);
        return lastGrandChild(lastChild);
    }
}

QList<QPersistentModelIndex> TreeFlattenProxyModel::getInsertedSourceChildren (const QModelIndex& sourceIndex) const {
    QList<QPersistentModelIndex> list;
    list.append(sourceIndex);
    {
        for (int i = 0; i < sourceModel()->rowCount(sourceIndex); ++i) {
            auto schild = sourceModel()->index(i, 0, sourceIndex);
            list.append(getInsertedSourceChildren(schild));
        }
    }
    return list;
}

int TreeFlattenProxyModel::getLastStableRow (const QModelIndex& sourceParent, int first) const {
    int         lastStableRow = -1;
    QModelIndex si;
    if (sourceModel()->rowCount(sourceParent) == 0) {
        si = sourceParent;
        auto pi = mapFromSource(si);
        lastStableRow = pi.row();
    } else if (first == 0) {
        si = sourceParent;
        auto pi = mapFromSource(si);
        lastStableRow = pi.row();
    } else {
        si = sourceModel()->index(first - 1, 0, sourceParent);
        auto lastChild = lastGrandChild(si);
        auto pi        = mapFromSource(lastChild);
        lastStableRow = pi.row();
    }

    // int row = -1;
    // if (first == 0) {
    // row = proxyParent.row();
    // } else {
    // auto ls = sourceModel()->index(first, 0, sourceParent);
    // row = mapFromSource(ls).row() - 1;
    // }

    // m_lastStableRow = row;

    return lastStableRow;
}

void TreeFlattenProxyModel::setSourceModel (QAbstractItemModel* sourceModel) {
    Q_ASSERT(sourceModel);

    // FIXME: Disconnect old model.
    beginResetModel();
    QAbstractProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &QAbstractItemModel::dataChanged,
            this, &TreeFlattenProxyModel::onSourceDataChanged);
    connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted,
            this, &TreeFlattenProxyModel::onSourceRowsAboutToBeInserted);
    connect(sourceModel, &QAbstractItemModel::rowsInserted,
            this, &TreeFlattenProxyModel::onSourceRowsInserted);
    connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &TreeFlattenProxyModel::onSourceRowsAboutToBeRemoved);
    connect(sourceModel, &QAbstractItemModel::rowsRemoved,
            this, &TreeFlattenProxyModel::onSourceRowsRemoved);
    connect(sourceModel, &QAbstractItemModel::modelReset,
            this, &TreeFlattenProxyModel::onSourceModelReset);
    endResetModel();

    populateFromSourceParent(sourceModel->parent({}));
}
}
}
