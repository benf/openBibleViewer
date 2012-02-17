/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2012 Paul Walger <metaxy@walger.name>
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
#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QtGui/QMdiArea>
#include <QtWebKit/QWebView>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QVBoxLayout>

#include "src/core/basicclass.h"
#include "src/core/link/urlconverter2.h"
#include "src/core/dbghelper.h"
#include "src/api/api.h"
#include "src/ui/interface/advanced/mdiareafilter.h"

#include "bookmarksmanager.h"
#include "notesmanager.h"
#include "src/ui/interface/advanced/form/form.h"
#include "src/ui/interface/advanced/form/dictionaryform.h"
#include "src/ui/interface/advanced/form/bibleform.h"
#include "src/ui/interface/advanced/form/webform.h"
#include "src/ui/interface/advanced/form/bookform.h"
#include "src/ui/interface/advanced/form/commentaryform.h"
#include "src/ui/interface/advanced/form/treebookform.h"
/**
  * WindowManager controlls all subwindows or tabs. It opens them and provides access to the forms.
  * Also it manages the tiling.
  */
class WindowManager : public QObject , public BasicClass
{
    Q_OBJECT
public:
    explicit WindowManager(QObject *parent = 0);
    ~WindowManager();
    void setMdiArea(QMdiArea *area);
    void setApi(Api *api);
    void setNotesManager(NotesManager *notesManager);
    void setBookmarksManager(BookmarksManager *bookmarksManager);
    void init();
    void save();
    void restore();

    Form *activeForm();
    Form* getForm(QMdiSubWindow *w) const;

    QMdiSubWindow *activeSubWindow();
    void activate(QMdiSubWindow *w);


    QMdiSubWindow* needWindow(Form::FormType type);
    QMdiSubWindow* needWindow(Form::FormType type, ModuleTools::ContentType contentType);
    QMdiSubWindow* hasDictWindow(ModuleTools::DefaultModule d);
    QMdiSubWindow* hasDictWindow(const int moduleID);

    ModuleTools::ContentType contentType(QMdiSubWindow* window);
    ModuleTools::ContentType contentType(Form *form);
    ModuleTools::ContentType contentType(DictionaryForm *form);
    ModuleTools::ContentType contentType(WebForm *form);
    ModuleTools::ContentType contentType(BibleForm *form);

public slots:

    void newBibleSubWindow();
    void newWebSubWindow();
    void newDictionarySubWindow();
    void newBookSubWindow();
    void newCommentarySubWindow();
    QMdiSubWindow * newSubWindow(Form::FormType type = Form::BibleForm, bool forceMax = false);


private slots:
    int closingWindow();

    void cascade();
    void tileVertical();
    void tileHorizontal();
    void tile();
    void cascade(bool checked);
    void tileVertical(bool checked);
    void tileHorizontal(bool checked);
    void tile(bool checked);

    void setTitle(const QString &title);

    void setTabbedView();
    void setSubWindowView();
    int reloadWindow(QMdiSubWindow * window);
    void mdiAreaResized();




    void zoomIn();
    void zoomOut();
    void reloadActive();
    void autoLayout();
    void closeSubWindow();

private:

    QList<QMdiSubWindow*> usableWindowList() const;

    void disable();
    void enable();
    void installResizeFilter();
    void setEnableReload(bool enable);

    int m_nameCounter;
    bool m_enableReload;
    int m_lastActiveWindow;
    int *m_currentWindowID;

    QMdiArea *m_area;//not in our control
    Api *m_api;//not in out control

    MdiAreaFilter *m_mdiAreaFilter;
    NotesManager *m_notesManager;
    BookmarksManager *m_bookmarksManager;

};

#endif // WINDOWMANAGER_H
