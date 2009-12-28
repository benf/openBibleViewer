/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009 Paul Walger
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

#ifndef SEARCHINFODIALOG_H
#define SEARCHINFODIALOG_H

#include <QtGui/QDialog>
#include "src/core/searchresult.h"
namespace Ui
{
class SearchInfoDialog;
}
/*!
 SearchInfoDialog represents a dialog  to show additional information for the search like statistics

*/
class SearchInfoDialog : public QDialog
{
    Q_OBJECT
private slots:
    void tabChanged(int index);
public:
    explicit SearchInfoDialog(QWidget *parent = 0);
    void setInfo(SearchResult, QStringList bookNames, QString searchText, QStringList textList);
    virtual ~SearchInfoDialog();
    double mRound(double Zahl, int Stellen);
    int d2i(double d);
protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::SearchInfoDialog *m_ui;
    bool m_textShown;
    QStringList m_textList;

};

#endif // SEARCHINFODIALOG_H
