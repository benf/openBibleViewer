/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2010 Paul Walger
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program; if not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
#ifndef BOOKDOCKWIDGET_H
#define BOOKDOCKWIDGET_H

#include <QtGui/QDockWidget>
#include <QtCore/QStringList>
#include "dockwidget.h"
namespace Ui
{
class BookDockWidget;
}
/*!
 BockDockWidget represents a dock widget, to navigate through the bible

*/
class BookDockWidget : public DockWidget
{
    Q_OBJECT
private slots:
    void readBook(QModelIndex index);
    void readChapter(QModelIndex index);

public:
    BookDockWidget(QWidget *parent = 0);
    ~BookDockWidget();
    void setChapters(const QStringList &chapters);
    void setBooks(const QHash<int, QString> &books);
    void setCurrentBook(const int &bookID);
    void setCurrentChapter(const int &chapterID);
    void clearBooks();
    void clearChapters();

protected:
    void changeEvent(QEvent *e);
private:
    Ui::BookDockWidget *ui;
    QStandardItemModel *m_bookModel;
    QStandardItemModel *m_chapterModel;
    QItemSelectionModel *m_bookSelection;
    QItemSelectionModel *m_chapterSelection;

};

#endif // BOOKDOCKWIDGET_H
