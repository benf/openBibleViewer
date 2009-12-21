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
#include "mainwindow.h"
#include "../../core/xbelreader.h"
#include "../../core/xbelwriter.h"
#include "../../core/dbghelper.h"
#include "../biblepassagedialog.h"
#include <QtCore/QtDebug>
#include <QtCore/QString>
#include <QtGui/QAction>
#include <QtGui/QTextBrowser>
#include <QtGui/QClipboard>
#include <QtGui/QMessageBox>

int MainWindow::loadBookmarks(void)
{
    bookmarksFileName = homeDataPath + "bookmarks.xml";
    if (ui->dockWidget_bookmarks->isVisible()) {
        ui->dockWidget_bookmarks->hide();
    } else {
        ui->dockWidget_bookmarks->show();
    }
    ui->treeWidget_bookmarks->clear();

    QFile file(bookmarksFileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return 1;
    XbelReader reader(ui->treeWidget_bookmarks);
    if (!reader.read(&file))
        return 1;
    return 0;
}
void MainWindow::newBookmark(void)
{
    if (m_bible.currentBibleID < 0)
        return;
    QTreeWidgetItem *bookmark = new QTreeWidgetItem();
    bookmark->setFlags(bookmark->flags() | Qt::ItemIsEditable);
    QIcon bookmarkIcon;
    QStyle *style = ui->treeWidget_bookmarks->style();
    bookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileLinkIcon));
    bookmark->setIcon(0, bookmarkIcon);
    QTextCursor cursor = currentTextCursor;
    int startverse = verseFromCursor(cursor);

    bookmark->setText(0,
                      m_bible.bookFullName.at(m_bible.currentBookID) +
                      " " +
                      QString::number(m_bible.currentChapterID - m_bible.chapterAdd + 1, 10) +
                      "," +
                      QString::number(startverse, 10));

    bookmark->setText(1,
                      biblesIniPath.at(m_bible.currentBibleID) +
                      ";" +
                      QString::number(m_bible.currentBookID, 10) +
                      ";" +
                      QString::number(m_bible.currentChapterID - m_bible.chapterAdd + 1, 10) +
                      ";" +
                      QString::number(startverse, 10));//auch aus cursor

    bookmark->setData(0, Qt::UserRole, "bookmark");
    if (ui->treeWidget_bookmarks->currentItem() && ui->treeWidget_bookmarks->currentItem()->data(0, Qt::UserRole).toString() == "folder") {
        ui->treeWidget_bookmarks->currentItem()->addChild(bookmark);
    } else {
        ui->treeWidget_bookmarks->insertTopLevelItem(0, bookmark);
    }
    saveBookmarks();
}
void MainWindow::saveBookmarks(void)
{
    QString fileName = bookmarksFileName;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return;
    XbelWriter writer(ui->treeWidget_bookmarks);
    if (writer.writeFile(&file))
        statusBar()->showMessage(tr("Bookmarks saved"), 5000);
}
void MainWindow::newBookmarksFolder(void)
{
    QTreeWidgetItem *folder = new QTreeWidgetItem();
    folder->setFlags(folder->flags() | Qt::ItemIsEditable);
    QStyle *style = ui->treeWidget_bookmarks->style();
    QIcon folderIcon;
    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
    folder->setIcon(0, folderIcon);
    folder->setText(0, tr("new folder"));
    folder->setText(1, "");
    folder->setData(0, Qt::UserRole, "folder");
    if (ui->treeWidget_bookmarks->currentItem() && ui->treeWidget_bookmarks->currentItem()->data(0, Qt::UserRole).toString() == "folder") {
        ui->treeWidget_bookmarks->currentItem()->addChild(folder);
    } else {
        ui->treeWidget_bookmarks->insertTopLevelItem(0, folder);
    }
    saveBookmarks();
}

void MainWindow::bookmarksContextMenu(void)
{
    QMenu *contextMenu = new QMenu(this);
    contextMenu->setObjectName("contextMenu");

    QAction *actionGoTo = new QAction(this);
    actionGoTo->setObjectName("actionGoTo");
    QIcon iconGoTo;
    iconGoTo.addPixmap(QPixmap(":/icons/16x16/go-jump.png"), QIcon::Normal, QIcon::Off);
    actionGoTo->setIcon(iconGoTo);
    actionGoTo->setText(tr("Go to"));

    connect(actionGoTo, SIGNAL(triggered()), this, SLOT(bookmarksGo()));

    QAction *actionNewFolder = new QAction(this);
    actionNewFolder->setObjectName("actionNewFolder");
    connect(actionNewFolder, SIGNAL(triggered()), this, SLOT(newBookmarksFolder()));
    QIcon iconNewFolder;
    iconNewFolder.addPixmap(QPixmap(":/icons/16x16/folder-new.png"), QIcon::Normal, QIcon::Off);
    actionNewFolder->setIcon(iconNewFolder);
    actionNewFolder->setText(tr("New Folder"));

    QAction *actionEdit = new QAction(this);
    actionEdit->setObjectName("actionEdit");
    QIcon iconEdit;
    iconEdit.addPixmap(QPixmap(":/icons/16x16/document-edit.png"), QIcon::Normal, QIcon::Off);
    actionEdit->setIcon(iconEdit);
    actionEdit->setText(tr("Edit"));
    connect(actionEdit, SIGNAL(triggered()), this, SLOT(editBookmark()));

    QAction *actionRemove = new QAction(this);
    actionRemove->setObjectName("actionRemove");
    QIcon iconRemove;
    iconRemove.addPixmap(QPixmap(":/icons/16x16/list-remove.png"), QIcon::Normal, QIcon::Off);
    actionRemove->setIcon(iconRemove);
    actionRemove->setText(tr("Remove"));
    connect(actionRemove, SIGNAL(triggered()), this, SLOT(removeBookmark()));

    contextMenu->addAction(actionGoTo);
    contextMenu->addAction(actionNewFolder);
    contextMenu->addAction(actionEdit);
    contextMenu->addAction(actionRemove);

    contextMenu->exec(QCursor::pos());
}
void MainWindow::removeBookmark()
{
    if (ui->treeWidget_bookmarks->currentItem())
        delete ui->treeWidget_bookmarks->currentItem();
    else
        myDebug() << "nothing selected";
}
void MainWindow::editBookmark()
{
    if (!ui->treeWidget_bookmarks->currentItem()) {
        myDebug() << "nothing selected";
        return;
    }

    QString pos = ui->treeWidget_bookmarks->currentItem()->text(1);
    if (internalOpenPos(pos) != 0) {
        QMessageBox::critical(0, tr("Error"), tr("This Bookmark is invalid."));
        myDebug() << "invalid bookmark";
    }

    QStringList list = pos.split(";");
    if (list.size() < 4) {
        return ;
    }
    QString dirname = list.at(0);
    QString sbookID = list.at(1);
    QString schapterID = list.at(2);
    QString sverseID = list.at(3);
    int bibleID = 0;
    int bookID = sbookID.toInt();
    int chapterID = schapterID.toInt();
    int verseID = sverseID.toInt();
    //get bibleID
    for (int i = 0; i < biblesIniPath.size(); i++) {
        if (biblesIniPath.at(i) == dirname) {
            bibleID = i;
            break;
        }
    }

    BiblePassageDialog *passageDialog = new  BiblePassageDialog(this);
    passageDialog->setWindowModality(Qt::WindowModal);
    connect(passageDialog, SIGNAL(updated(QString)), this, SLOT(updateBookmark(QString)));
    passageDialog->setSettings(m_settings);
    passageDialog->setCurrent(bibleID, dirname, bookID, chapterID, verseID);
    passageDialog->show();
    passageDialog->exec();
}
void MainWindow::bookmarksGo()
{
    QString pos = ui->treeWidget_bookmarks->currentItem()->text(1);
    if (internalOpenPos(pos) != 0)
        QMessageBox::critical(0, tr("Error"), tr("This Bookmark is invalid."));
}
void MainWindow::updateBookmark(QString pos)
{
    ui->treeWidget_bookmarks->currentItem()->setText(1, pos);
}
void MainWindow::bookmarksGo(QTreeWidgetItem * item)
{
    if (m_settings->onClickBookmarkGo == true) {
        QString pos = item->text(1);
        if (internalOpenPos(pos) != 0)
            QMessageBox::critical(0, tr("Error"), tr("This Bookmark is invalid."));
    }
}

