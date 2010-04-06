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
#ifndef ADVANCEDINTERFACE_H
#define ADVANCEDINTERFACE_H

#include <QWidget>
#include "src/ui/interface/interface.h"
#include "src/ui/dock/bookdockwidget.h"
#include "src/ui/dock/moduledockwidget.h"
#include "src/ui/dock/searchresultdockwidget.h"
#include "src/ui/dock/notesdockwidget.h"
#include "src/ui/dock/bookmarksdockwidget.h"
#include "src/ui/dock/strongdockwidget.h"
#include "src/ui/dock/quickjumpdockwidget.h"
#include "src/core/windowcache.h"
#include <QtGui/QMdiSubWindow>
#include <QtGui/QCloseEvent>
#include "mdiform.h"
namespace Ui
{
class AdvancedInterface;
}

class AdvancedInterface : public Interface
{
    Q_OBJECT
public slots:
    void closing();
    void settingsChanged(Settings oldSettings,Settings newSettings);
    void restoreSession();
private slots:
    void myCascade();
    void myTileVertical();
    void myTileHorizontal();
    void myTile();
    void newSubWindow(bool doAutoLayout = true);
    void closeSubWindow();
    int closingWindow();
    int reloadWindow(QMdiSubWindow * window);
    void pharseUrl(QUrl url);
    void pharseUrl(QString url);
    void showText(const QString &text);
    void zoomIn();
    void zoomOut();
    void readChapter(const int &id);
    void readBook(const int &id);
    void nextChapter();
    void previousChapter();
    int textBrowserContextMenu(QPoint);
    int copyWholeVerse();
    void newYellowMark();
    void newGreenMark();
    void newBlueMark();
    void newOrangeMark();
    void newVioletMark();
    void newCustomMark();
    void reloadChapter();
    void removeMark();
    void showSearchDialog();
    void search(SearchQuery query);
    void showBookmarksDock();
    void showNotesDock();
    void newBookmark();
    void newNoteWithLink();
    int showAboutDialog();
    int saveFile();
    int printFile();
    void onlineHelp();
    void copy();
    void selectAll();
    void nextVerse();
    void previousVerse();
    void showMarkCategories();
    void showMarkList();
    void showNotesEditor();
    void setTabView();
    void setSubWindowView();


public:
    AdvancedInterface(QWidget *parent = 0);
    ~AdvancedInterface();
    void init();
    bool hasMenuBar();
    QMenuBar* menuBar();
    bool hasToolBar();
    QToolBar* toolBar();
    void setBookDockWidget(BookDockWidget *bookDockWidget);
    void setModuleDockWidget(ModuleDockWidget *moduleDockWidget);
    void setSearchResultDockWidget(SearchResultDockWidget *searchResultDockWidget);
    void setNotesDockWidget(NotesDockWidget *notesDockWidget);
    void setBookmarksDockWidget(BookmarksDockWidget *boockmarksDockWidget);
    void setStrongDockWidget(StrongDockWidget *strongDockWidget);
    void setQuickJumpDockWidget(QuickJumpDockWidget *quickJumpDockWidget);



protected:
    void changeEvent(QEvent *e);
signals:
    void get(QString);
    void historySetUrl(QString url);
    void reloadInterface();
private:
    Ui::AdvancedInterface *ui;
    bool m_enableReload;
    QMdiSubWindow *activeMdiChild();
    QList<QMdiSubWindow*> usableWindowList();
    QList<QMdiSubWindow *> m_internalWindows;
    int currentWindowID();
    int tabIDof(QMdiSubWindow* window);
    WindowCache m_windowCache;
    int m_lastActiveWindow;
    QTextBrowser* getCurrentTextBrowser();
    void setEnableReload(bool enable);
    void loadModuleDataByID(int id);

    void setTitle(const QString &title);
    void setChapters(const QStringList &chapters);
    void setBooks(const QStringList &books);
    void setCurrentBook(const int &bookID);
    void setCurrentChapter(const int &chapterID);

    void readBookByID(int id);
    VerseSelection verseSelectionFromCursor(QTextCursor cursor);

    void showChapter(const int &chapterID, const int &verseID);
    void autoLayout();
    QTextCursor m_textCursor;

    //Menu
    QAction *actionTabView;
    QAction *actionSubWindowView;

};

#endif // ADVANCEDINTERFACE_H
