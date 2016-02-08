#ifndef DETAILSMODEL_H
#define DETAILSMODEL_H

#include <QSortFilterProxyModel>

class DetailsModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    DetailsModel();

    bool filterAcceptsRow( int source_row, const QModelIndex& source_parent ) const override;
    void invalidateModel() { invalidateFilter(); }
};

#endif // DETAILSMODEL_H
