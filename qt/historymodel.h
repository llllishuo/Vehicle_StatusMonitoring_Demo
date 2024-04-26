#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H
#include <QAbstractItemModel>

class HistoryModel : public QAbstractItemModel{
public:
     explicit HistoryModel(QObject *parent = 0);

     // 构造父节点下子节点的索引
     virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
     // 通过子节点索引获取父节点索引
     virtual QModelIndex parent(const QModelIndex &child) const override;
     // 获取父节点下子节点的行数
     virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
     // 获取父节点下子节点列数
     virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
     // 获取节点数据：包括DisplayRole|TextAlignmentRole等
     virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // HISTORYMODEL_H
