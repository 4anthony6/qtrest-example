#include "baserestlistmodel.h"
#include "detailsmodel.h"

BaseRestListModel::BaseRestListModel(QObject *parent) : QAbstractListModel(parent), m_sort("-id"),
    m_perPage(20), m_currentPage(0), m_roleNamesIndex(0)
{
    api.setAccept(accept());
    m_detailsModel = new DetailsModel();
    setLoadingStatus(LoadingStatus::Idle);
    connect(&api,SIGNAL(replyError(QNetworkReply *, QNetworkReply::NetworkError, QString)), this, SLOT(replyError(QNetworkReply *, QNetworkReply::NetworkError, QString)));
}

void BaseRestListModel::declareQML()
{
    qRegisterMetaType<DetailsModel*>("DetailsModel*");
}

void BaseRestListModel::reload()
{
    setLoadingStatus(LoadingStatus::RequestToReload);
    this->fetchMore(QModelIndex());
}

bool BaseRestListModel::canFetchMore(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (currentPage() < totalCount()) {
        return true;
    } else {
        return false;
    }
}

void BaseRestListModel::fetchMore(const QModelIndex &parent)
{
    Q_UNUSED(parent)

    switch (loadingStatus()) {
    case LoadingStatus::RequestToReload:
        setCurrentPage(0);
        setLoadingStatus(LoadingStatus::FullReloadProcessing);
        break;
    case LoadingStatus::Idle:
        setLoadingStatus(LoadingStatus::LoadMoreProcessing);
        break;
    default:
        return;
        break;
    }

    qDebug() << "fetchMore";

    int nextPage = currentPage()+1;
    setCurrentPage(nextPage);

    QNetworkReply *reply = fetchMoreImpl(parent);
    connect(reply, SIGNAL(finished()), this, SLOT(fetchMoreFinished()));
}

void BaseRestListModel::fetchMoreFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (api.checkReplyIsError(reply) || !reply->isFinished()) {
        return;
    }

    if (this->loadingStatus() == LoadingStatus::Idle) {
        return;
    }

    qDebug() << "fetchMoreFinished";

    //TODO check is reply for me

    updateHeadersData(reply);

    QVariantList values = getVariantList(reply->readAll());

    //prepare vars
    int insertFrom = rowCount();
    int insertCount = rowCount()+values.count()-1;

    //check if we need to full reload
    if (this->loadingStatus() == LoadingStatus::FullReloadProcessing) {
        clearForReload();
        insertFrom = rowCount();
        insertCount = values.count()-1;
    }

    //check for assertion or empty data
    if (insertCount < insertFrom) { insertCount = insertFrom; }

    if (insertCount == 0) {
        setLoadingStatus(LoadingStatus::Error);
        emit countChanged();
        qDebug() << "Nothing to insert! Please check your parser!" << count() << loadingStatus();
        return;
    }

    //append rows to model
    beginInsertRows(this->index(rowCount(), 0), insertFrom, insertCount);

    QListIterator<QVariant> i(values);
    while (i.hasNext()) {
        Item item = createItem(i.next().toMap());
        append(item);
    }

    //get all role names
    generateRoleNames();

    endInsertRows();

    detailsModel()->setSourceModel(this);

    setLoadingStatus(LoadingStatus::Idle);

    emit countChanged();
}

void BaseRestListModel::fetchDetail(QString id)
{
    m_fetchDetailId = id;
    Item item = findItemById(id);
    if (item.isUpdated()) {
        return;
    }

    switch (loadingStatus()) {
    case LoadingStatus::Idle:
        setLoadingStatus(LoadingStatus::LoadDetailsProcessing);
        break;
    default:
        return;
        break;
    }

    m_detailsModel->invalidateModel();

    QNetworkReply *reply = fetchDetailImpl(id);
    connect(reply, SIGNAL(finished()), this, SLOT(fetchDetailFinished()));
}

void BaseRestListModel::fetchDetailFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (api.checkReplyIsError(reply) || !reply->isFinished()) {
        return;
    }

    if (this->loadingStatus() == LoadingStatus::Idle) {
        return;
    }

    QVariantMap item = getVariantMap(reply->readAll());

    updateItem(item);

    detailsModel()->setSourceModel(this);

    //todo wtf?
    setLoadingStatus(LoadingStatus::IdleDetails);
}

void BaseRestListModel::replyError(QNetworkReply *reply, QNetworkReply::NetworkError error, QString errorString)
{
    Q_UNUSED(reply)
    setLoadingErrorCode(error);
    setLoadingErrorString(errorString);
    setLoadingStatus(LoadingStatus::Error);
}

Item BaseRestListModel::createItem(QVariantMap value)
{
    return Item(preProcessItem(value),idField());
}

Item BaseRestListModel::findItemById(QString id)
{
    QListIterator<Item> i(m_items);
    while (i.hasNext()) {
        Item item = i.next();
        if (item.id() == id) {
            return item;
        }
    }
}

void BaseRestListModel::updateItem(QVariantMap value)
{
    Item item = findItemById(value.value(idField()).toString());
    int row = m_items.indexOf(item);
    item.update(value);
    emit dataChanged(index(row),index(row));
    //qDebug() << 'detail updated' << row << item.id();
}

QVariant BaseRestListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_items.count()) {
        qDebug() << "Row not found" << index.row();
        return QVariant();
    }

    Item item = m_items.at(index.row());
    return item.value(m_roleNames[role]);
}

QHash<int, QByteArray> BaseRestListModel::roleNames() const
{
    return m_roleNames;
}

void BaseRestListModel::updateHeadersData(QNetworkReply *reply)
{
    //update headers data
    this->setCurrentPage(reply->rawHeader("X-Pagination-Current-Page").toInt());
    this->setTotalCount(reply->rawHeader("X-Pagination-Total-Count").toInt());
    this->setPageCount(reply->rawHeader("X-Pagination-Page-Count").toInt());
    reply->deleteLater();
}

void BaseRestListModel::clearForReload()
{
    beginResetModel();

    m_items.clear();

    endResetModel();
}

void BaseRestListModel::append(Item item)
{
    m_items.append(item);
}

void BaseRestListModel::generateRoleNames()
{
    if (m_roleNamesIndex > 0) {
        return;
    }

    if (rowCount() > 0) {
        QStringList keys = m_items[0].keys();

        foreach (QString key, keys) {
            m_roleNamesIndex++;
            m_roleNames[m_roleNamesIndex] = QByteArray().append(key);
        }
    }
}

int BaseRestListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}
