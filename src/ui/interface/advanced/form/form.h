/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2011 Paul Walger <metaxy@walger.name>
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
#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "src/core/basicclass.h"
#include "src/ui/interface/advanced/manager/bookmarksmanager.h"
#include "src/ui/interface/advanced/manager/notesmanager.h"
#include "src/api/api.h"
#include "src/core/dbghelper.h"
#include "src/module/searchablemodule.h"
#include <QtGui/QMdiSubWindow>
class Form : public QWidget, public BasicClass
{
    Q_OBJECT
public:
    enum FormType {
        BibleForm = 0,
        DictionaryForm = 1,
        WebForm = 2
    };

    explicit Form(QWidget *parent = 0);
    virtual void init() = 0;
    virtual void setID(const int id);
    virtual int id();

    void setApi(Api *api);
    void setNotesManager(NotesManager *notesManager);
    void setBookmarksManager(BookmarksManager *bookmarksManager);
    void setParentSubWindow(QMdiSubWindow *window);

    int *currentWindowID;

    virtual void restore(const QString &key) = 0;
    virtual void save() = 0;

    virtual FormType type() const = 0;

    virtual SearchableModule * searchableModule() const;

signals:

public slots:

    virtual void copy() = 0;
    virtual void selectAll() = 0;
    virtual void print() = 0;
    virtual void printPreview() = 0;
    virtual void saveFile() = 0;
    virtual QString selectedText() = 0;

    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;

    virtual void activated() = 0;
protected:

    bool active();
    int m_id;
    Api *m_api;//not in out control
    NotesManager *m_notesManager;
    BookmarksManager *m_bookmarksManager;
    QMdiSubWindow *m_parentSubWindow;

};

#endif // FORM_H
