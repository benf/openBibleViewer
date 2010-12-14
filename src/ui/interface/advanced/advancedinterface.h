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
#ifndef ADVANCEDINTERFACE_H
#define ADVANCEDINTERFACE_H

#include <QWidget>
#include "src/ui/interface/interface.h"


#include "src/ui/dock/bookmarksdockwidget.h"
#include "src/ui/dock/dictionarydockwidget.h"

#include "src/core/windowcache.h"
#include "src/api/api.h"
#include "mdiareafilter.h"
#include "windowmanager.h"
#include "biblemanager.h"
#include "notesmanager.h"
#include <QtGui/QMdiSubWindow>
#include <QtGui/QCloseEvent>
#include <QtGui/QContextMenuEvent>
namespace Ui
{
class AdvancedInterface;
}

class AdvancedInterface : public Interface
{
    Q_OBJECT
public slots:
    void closing();
    void settingsChanged(Settings oldSettings, Settings newSettings, bool modifedModuleSettings);
    void restoreSession();
private slots:


    void pharseUrl(QUrl url);
    void pharseUrl(QString url);

    void search(SearchQuery query);
    void search();

    void saveFile();
    void printFile();
    void onlineHelp();

    void copy();
    void selectAll();


    void showBookmarksDock();
    void showNotesDock();

    void showSearchDialog();
    int showAboutDialog();
    void showMarkCategories();
    void showMarkList();
    void showNotesEditor();

    void setTabView();
    void setSubWindowView();
    void setTitle(const QString &title);


public:
    AdvancedInterface(QWidget *parent = 0);
    ~AdvancedInterface();
    void init();
    bool hasMenuBar();
    QMenuBar* menuBar();
    bool hasToolBar();
    QList<QToolBar*> toolBars();
    QHash<DockWidget *, Qt::DockWidgetArea> docks();

    void createDocks();
    void createToolBars();
    void createMenu();



protected:
    void changeEvent(QEvent *e);
signals:
    void historySetUrl(QString url);
    void reloadInterface();
private:
    Ui::AdvancedInterface *ui;

    BibleDisplaySettings *m_bibleDisplaySettings;
    Api *m_api;
    //Menu
    QAction *m_actionTabView;
    QAction *m_actionSubWindowView;
    void searchInText(SearchQuery query);

    QToolBar *m_mainBar;
    QToolBar *m_searchBar;

    QAction *m_mainBarActionSearch;
    QAction *m_mainBarActionBookmarks;
    QAction *m_mainBarActionNotes;
    QAction *m_mainBarActionNewWindow;
    QAction *m_mainBarActionZoomIn;
    QAction *m_mainBarActionZoomOut;
    QAction *m_mainBarActionModule;

    BookmarksDockWidget * m_bookmarksDockWidget;
    DictionaryDockWidget * m_dictionaryDockWidget;

    WindowManager *m_windowManager;
    BibleManager *m_bibleManager;
    NotesManager *m_notesManager;
};

#endif // ADVANCEDINTERFACE_H
