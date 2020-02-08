/**
 * Bookmark header
 */


#pragma once


#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

class Bookmark {
public:
    Bookmark();
    explicit Bookmark(QString url, QString name = QString(),
            QStringList tagList = QStringList(), QString description = QString());
    friend QDebug operator<<(QDebug dbg, const Bookmark &bookmark);
    QJsonObject jsonObject() const;
    static QList<Bookmark> parseBookmarks(const QString& text, bool withBasicUrls = false);
    static QString bookmarksWebServiceJsonText(const QList<Bookmark>& bookmarks);
    static QString parsedBookmarksWebServiceJsonText(const QString& text, bool withBasicUrls = false);
    bool operator==(const Bookmark &bookmark) const;
    static void mergeInList(QList<Bookmark> &bookmarks, Bookmark &bookmark);
    static void mergeListInList(const QList<Bookmark> &sourceBookmarks,
            QList<Bookmark> &destinationBookmarks);
    void mergeInList(QList<Bookmark> &bookmarks);
    void merge(Bookmark &bookmark);

protected:
    QString name;
    QString url;
    QStringList tags;
    QString description;
};
