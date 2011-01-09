/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2011 Paul Walger
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
#ifndef QUICKJUMPDOCKWIDGET_H
#define QUICKJUMPDOCKWIDGET_H

#include <QDockWidget>
#include <QCompleter>
#include "dockwidget.h"
namespace Ui
{
class QuickJumpDockWidget;
}
/*!
 ModuleDockWidget represents a dock widget from which you can jump fast to certain positions in the module

*/
class QuickJumpDockWidget : public DockWidget
{
    Q_OBJECT
private slots:
    void goToPos();
public:
    QuickJumpDockWidget(QWidget *parent = 0);
    ~QuickJumpDockWidget();
    void init();
    void setBooks(QStringList list);

protected:
    void changeEvent(QEvent *e);
private:
    Ui::QuickJumpDockWidget *ui;
    QStringList m_hist;
    QStringList m_books;

    QCompleter *m_completer;
};

#endif // QUICKJUMPDOCKWIDGET_H
