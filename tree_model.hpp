#ifndef TREEMODEL_HPP
#define TREEMODEL_HPP

#include "tree_item.hpp"

#include <QAbstractItemModel>

#include <memory>

class TreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    TreeModel (int columns = 1, QObject* parent = nullptr);

    QVariant      data (const QModelIndex& index, int role) const override;
    QVariant      headerData (int section, Qt::Orientation orientation, int role) const override;
    QModelIndex   index (int row, int column, const QModelIndex& parent = {}) const override;
    QModelIndex   parent (const QModelIndex& index) const override;
    Qt::ItemFlags flags (const QModelIndex& index) const override;
    int           rowCount (const QModelIndex& parent    = QModelIndex()) const override;
    int           columnCount (const QModelIndex& parent = QModelIndex()) const override;
    bool          setData (const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool          setHeaderData (int section, Qt::Orientation orientation, const QVariant& value, int role) override;
    bool          insertRows (int row, int count, const QModelIndex& parent = QModelIndex()) override;

private:
    // methods
    TreeItem* itemFromIndex (const QModelIndex& index) const;
    void      createTestItems ();
    // data
    static int insertions_;
    std::unique_ptr<TreeItem> rootItem;
    int columns_ = 1;
};

#endif // TREEMODEL_HPP
