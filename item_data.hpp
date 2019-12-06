#ifndef ITEM_DATA_HPP
#define ITEM_DATA_HPP

#include <QString>
#include <QVariant>

struct ItemData {

    QVariant value (int role = Qt::DisplayRole) const;
    void     setValue (QVariant value, int role = Qt::DisplayRole);

    QString text;
};

#endif // ITEM_DATA_HPP
