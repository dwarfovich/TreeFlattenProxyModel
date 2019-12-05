#ifndef TREE_FLATTEN_PROXY_MODEL_HPP
#define TREE_FLATTEN_PROXY_MODEL_HPP

#include <QAbstractProxyModel>
#include <QVector>
#include <QHash>

namespace cc {
namespace gui {

class TreeFlattenProxyModel : public QAbstractProxyModel {
    Q_OBJECT

public:
    explicit TreeFlattenProxyModel (QObject* parent = nullptr);

    void        setSourceModel (QAbstractItemModel* sourceModel) override;
    QModelIndex index (int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent (const QModelIndex& child) const override;
    int         rowCount (const QModelIndex& parent    = {}) const override;
    int         columnCount (const QModelIndex& parent = {}) const override;
    QVariant    headerData (int section, Qt::Orientation orientation, int role) const override;

    QModelIndex mapToSource (const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource (const QModelIndex& sourceIndex) const override;

private slots:
    void onSourceDataChanged (const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = {});
    void onSourceRowsAboutToBeInserted (const QModelIndex& sourceParent, int first, int last);
    void onSourceRowsInserted (const QModelIndex& sourceParent, int first, int last);
    void onSourceRowsAboutToBeRemoved (const QModelIndex& sourceParent, int first, int last);
    void onSourceRowsRemoved (const QModelIndex& sourceParent, int first, int last);
    void onSourceModelReset ();

private:
    void                         populateFromSourceParent (const QModelIndex& parent);
    void                         insertRow (const QModelIndex& sourceIndex, int row);
    void                         remapTemp ();
    void                         clearTemp ();
    QModelIndex                  lastGrandChild (const QModelIndex& parent) const;
    QList<QPersistentModelIndex> getInsertedSourceChildren (const QModelIndex& sourceIndex) const;
    int                          getLastStableRow (const QModelIndex& sourceParent, int first) const;

private:
    int m_levelRole;
    QVector<QPersistentModelIndex>    m_proxyRowToSource;
    QHash<QPersistentModelIndex, int> m_sourceToProxyRow;
    // Temp variables
    int m_lastStableRow;
    QMap<int, QPersistentModelIndex> m_rowsToRemap;
};
}
}

#endif // TREE_FLATTEN_PROXY_MODEL_HPP
