#include "historymodel.h"

HistoryModel::HistoryModel(QObject *parent /*= 0*/)
    : QAbstractItemModel(parent)
{
}
QModelIndex HistoryModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
    return createIndex(row, column);
}
QModelIndex HistoryModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}
int HistoryModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    if (parent.row() == -1)
    {
        return 5;
    }
    return 0;
}
int HistoryModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return 2;
}

QVariant HistoryModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::TextAlignmentRole)
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    else if (role == Qt::DisplayRole)
        return QString("row=%1,col=%2").arg(index.row()).arg(index.column());
    else
        return QVariant();
}
