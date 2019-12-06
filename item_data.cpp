#include "item_data.hpp"

QVariant ItemData::value (int role) const {
    switch (role) {
    case Qt::DisplayRole: return text;
    default: return {};
    }
}

void ItemData::setValue (QVariant value, int role) {
    switch (role) {
    case Qt::DisplayRole: text = value.toString();
        break;
    default: break;
    }
}
