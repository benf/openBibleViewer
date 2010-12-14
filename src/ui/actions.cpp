#include "actions.h"
#include "src/core/dbghelper.h"
Actions::Actions(QObject *parent) :
    QObject(parent)
{
}
void Actions::showChapter(const int &moduleID, const int &bookID, const int &chapterID)
{
    emit _showChapter(moduleID, bookID, chapterID);
}
void Actions::get(const QString &url)
{
    emit _get(url);
}
void Actions::get(const QUrl &url)
{
    emit _get(url.toString());
}
void Actions::get(const BibleUrl &url)
{
    emit _get(url);
}
void Actions::showNote(const QString &noteID)
{
    emit _showNote(noteID);
}
void Actions::setCurrentBook(const QSet<int> &bookID)
{
    DEBUG_FUNC_NAME
    emit _setCurrentBook(bookID);
}
void Actions::setCurrentChapter(const QSet<int> &chapterID)
{
    DEBUG_FUNC_NAME
    emit _setCurrentChapter(chapterID);
}
void Actions::reloadActiveBible()
{
    emit _reloadActiveBible();
}
void Actions::setTitle(const QString &title)
{
    emit _setTitle(title);
}

void Actions::updateChapters(const QStringList &chapters)
{
    emit _updateChapters(chapters);
}

void Actions::updateBooks(const QHash<int, QString> &books, QList<int> ids)
{
    emit _updateBooks(books,ids);
}
void Actions::setCurrentModule(const int &moduleID)
{
    emit _setCurrentModule(moduleID);
}