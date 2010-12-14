#ifndef ACTIONS_H
#define ACTIONS_H

#include <QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include "src/core/bible/bibleurl.h"
class Actions : public QObject
{
    Q_OBJECT
public:
    explicit Actions(QObject *parent = 0);

signals:
    void _showChapter(const int &moduleID, const int &bookID, const int &chapterID);
    /**
      By default the Interface pharse all urls.
      */
    void _get(const QString &url);
    void _get(const BibleUrl &url);

    void _showNote(const QString &nodeID);

    void _setCurrentBook(const QSet<int> &bookID);
    void _setCurrentChapter(const QSet<int> &chapterID);

    void _reloadActiveBible();

    void _setTitle(const QString &title);
    void _updateChapters(const QStringList &chapters);
    void _updateBooks(const QHash<int, QString> &books, QList<int> ids);

    void _setCurrentModule(const int &moduleID);
public slots:
    /**
      Show a chapter in current SubWindow.
      */
    void showChapter(const int &moduleID, const int &bookID, const int &chapterID);

    void get(const QString &url);
    void get(const QUrl &url);
    void get(const BibleUrl &url);

    void showNote(const QString &noteID);

    void setCurrentBook(const QSet<int> &bookID);
    void setCurrentChapter(const QSet<int> &chapterID);

    void reloadActiveBible();

    void setTitle(const QString &title);
    void updateChapters(const QStringList &chapters);
    void updateBooks(const QHash<int, QString> &books, QList<int> ids);

    void setCurrentModule(const int &moduleID);

};

#endif // ACTIONS_H
