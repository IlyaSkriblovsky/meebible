#include "CacheInfo.h"

#include <QDebug>

#include "Paths.h"

#include "Translation.h"
#include "Languages.h"
#include "Language.h"

#include "Cache.h"


CacheInfo::CacheInfo()
{
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "code";
    roleNames[TranslationRole] = "translation";
    roleNames[LanguageRole] = "language";
    roleNames[DataSizeRole] = "dataSize";
    roleNames[IndexSizeRole] = "indexSize";
    setRoleNames(roleNames);
}

CacheInfo::~CacheInfo()
{
}


int CacheInfo::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return _items.size();
}

int CacheInfo::count() { return _items.size(); }


QVariant CacheInfo::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _items.size())
        return QVariant();

    const CacheItem& item = _items.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole:
            return QString("%1_%2").arg(item.translation->code()).arg(item.translation->language()->code());

        case TranslationRole: return QVariant::fromValue((QObject*)item.translation);
        case LanguageRole   : return QVariant::fromValue((QObject*)item.language);

        case DataSizeRole: return item.dataSize;
        case IndexSizeRole: return item.indexSize;
    }

    return QVariant();
}


void CacheInfo::update()
{
    beginResetModel();

    // Aaah, spaghetti code...
    Cache::instance()->closeIndex();

    _items.clear();

    QDir cacheDir = Paths::cacheDir();

    QStringList asts = cacheDir.entryList(QStringList("*.ast"));

    foreach (const QString& ast, asts)
    {
        QString basename = ast.mid(0, ast.length()-4);

        int underscorePos = basename.lastIndexOf('_');
        if (underscorePos == -1) continue;

        // QStringList parts = basename.split('_');
        // if (parts.length() != 2) continue;

        QString transCode = basename.left(underscorePos);
        QString langCode = basename.right(basename.length() - underscorePos - 1);

        Language* lang = Languages::instance()->langByCode(langCode);
        if (! lang) continue;
        Translation* trans = lang->translationByCode(transCode);
        if (! trans) continue;


        int dataSize = QFileInfo(cacheDir.filePath(ast)).size();
        dataSize += QFileInfo(cacheDir.filePath(basename + ".asd")).size();
        int indexSize = QFileInfo(cacheDir.filePath(basename + ".idx")).size();


        CacheItem item;
        item.language = lang;
        item.translation = trans;
        item.dataSize = dataSize;
        item.indexSize = indexSize;

        _items.append(item);
    }

    endResetModel();
    countChanged();
}


void CacheInfo::deleteTranslation(QString transCode, QString langCode)
{
    qDebug() << "deleteTranslation" << transCode << langCode;

    Cache::instance()->closeStorage();
    Cache::instance()->closeIndex();

    QDir cacheDir = Paths::cacheDir();

    QString basename = QString("%1_%2").arg(transCode).arg(langCode);

    QFile(cacheDir.filePath(basename + ".idx")).remove();
    QFile(cacheDir.filePath(basename + ".ast")).remove();
    QFile(cacheDir.filePath(basename + ".asd")).remove();
    QFile(cacheDir.filePath("trans_" + basename + ".xml")).remove();

    update();
}


void CacheInfo::deleteIndexOnly(QString transCode, QString langCode)
{
    qDebug() << "deleteIndexOnly" << transCode << langCode;

    Cache::instance()->closeIndex();
    QDir cacheDir = Paths::cacheDir();

    QString basename = QString("%1_%2").arg(transCode).arg(langCode);

    QFile(cacheDir.filePath(basename + ".idx")).remove();

    update();
}
