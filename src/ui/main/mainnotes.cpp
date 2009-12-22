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
#include "../../core/stelle.h"
#include "../../core/notes.h"
#include "../../core/dbghelper.h"
#include "../../core/urlconverter.h"
#include "../biblepassagedialog.h"
#include "ui_mainwindow.h"

#include <QtCore/QString>
#include <QtGui/QTreeView>
#include <QtGui/QClipboard>
#include <QtGui/QColorDialog>
#include <QtGui/QTextDocumentFragment>

void MainWindow::loadNotes(void)
{
    // myDebug();
    DEBUG_FUNC_NAME
    if (ui->dockWidget_notes->isVisible()) {
        ui->dockWidget_notes->hide();
    } else {
        ui->dockWidget_notes->show();
    }
    m_note = new Notes(homeDataPath + "notes.xml");
    m_note->loadNotes();
    m_note->readNotes();
    ui->listWidget_notes->clear();
    QStringList id = m_note->getIDList();
    QStringList titles;
    for (int i = 0; i < id.size(); i++) {
        if (m_note->getType(id.at(i)) == "text")
            titles << m_note->getTitle(id.at(i));
    }
    ui->listWidget_notes->insertItems(0, titles);
    m_noteID = "";
    m_bible.setNotes(m_note);
}
void MainWindow::showNote(QListWidgetItem *item)
{
    int id = ui->listWidget_notes->row(item);
    if (id >= 0 && id < m_note->getIDList().size()) {
        showNote(m_note->getIDList().at(id));
    }
}
void MainWindow::showNote(const QString &noteID)
{
    DEBUG_FUNC_NAME
    m_note->setData(m_noteID, ui->textEdit_note->toHtml());
    m_note->setTitle(m_noteID, ui->lineEdit_note_titel->text());
    m_note->setRef(m_noteID, currentNoteRef);
    m_note->saveNotes();
    if (!m_note->getIDList().contains(noteID)) {
        myDebug() << "invalid noteID = " << noteID;
        return;
    }
    m_noteID = noteID;
    myDebug() << "id = " << noteID << " m_noteID = " << m_noteID;
    ui->lineEdit_note_titel->setText(m_note->getTitle(m_noteID));
    ui->textEdit_note->setHtml(m_note->getData(m_noteID));
    currentNoteRef = m_note->getRef(m_noteID);
    myDebug() << "link = " << m_note->getRef(m_noteID, "link");
    if (!m_note->getRef(m_noteID, "link").isEmpty())
        ui->label_noteLink->setText(notePos2Text(m_note->getRef(m_noteID, "link")));
    else
        ui->label_noteLink->setText("");

}

void MainWindow::copyNote(void)
{
    DEBUG_FUNC_NAME

    int id = ui->listWidget_notes->currentRow();
    if (id < m_note->getIDList().size() && id >= 0) {
        QClipboard *clipboard = QApplication::clipboard();
        QTextDocument doc;
        doc.setHtml(m_note->getData(m_note->getIDList().at(id)));
        clipboard->setText(doc.toPlainText());
    } else {
        myDebug() << "no note";
    }
}
void MainWindow::saveNote(void)
{
    DEBUG_FUNC_NAME
    myDebug() << "m_noteID = " << m_noteID;
    m_note->setData(m_noteID, ui->textEdit_note->toHtml());
    m_note->setTitle(m_noteID, ui->lineEdit_note_titel->text());
    m_note->setRef(m_noteID, currentNoteRef);
    m_note->saveNotes();
    reloadNotes();
}
void MainWindow::newNote(void)
{
    DEBUG_FUNC_NAME

    saveNote();
    reloadNotes();
    QString newID = m_note->generateNewID();
    m_note->setData(newID, "");
    m_note->setTitle(newID, tr("(unnamed)"));
    m_note->setType(newID, "text");
    QMap<QString, QString> ref;
    currentNoteRef = ref;
    //m["link"] = "";
    m_note->setRef(newID, ref);
    m_note->insertID(newID);
    m_noteID = newID;
    reloadNotes();
    ui->listWidget_notes->setCurrentRow(m_note->getIDList().size() - 1);
    //showNote(ui->listWidget_notes->currentItem());

    ui->label_noteLink->setText("");
    ui->lineEdit_note_titel->setText(tr("(unnamed)"));
    ui->textEdit_note->setHtml("");
    if (!m_note->getRef(m_noteID, "link").isEmpty())
        ui->label_noteLink->setText(notePos2Text(m_note->getRef(m_noteID, "link")));
    else
        ui->label_noteLink->setText("");

    myDebug() << " newID = " << newID << " m_noteID = " << m_noteID;
}
void MainWindow::newNoteWithLink()
{
    /*DEBUG_FUNC_NAME
    if (m_bible.currentBibleID < 0) {
        newNote();
        return;
    }
    int startverse = verseFromCursor(cursor);
    QString link;
    UrlConverter urlConverter(UrlConverter::None,UrlConverter::PersistentUrl,"");
    urlConverter.m_biblesIniPath = biblesIniPath;
    urlConverter.m_bibleID =  QString::number(m_bible.currentBibleID);
    urlConverter.m_bookID = m_bible.currentBookID;
    urlConverter.m_chapterID = m_bible.currentChapterID - m_bible.chapterAdd;
    urlConverter.m_verseID = startverse - 1;
    urlConverter.m_bookName = m_bible.bookFullName.at(m_bible.currentBookID);
    link = urlConverter.convert();
    //link = biblesIniPath.at(m_bible.currentBibleID) + ";" + QString::number(m_bible.currentBookID, 10) + ";" + QString::number(m_bible.currentChapterID + 1 - m_bible.chapterAdd, 10) + ";" + QString::number(startverse, 10) + ";" + ;

    saveNote();
    reloadNotes();
    QString newID = m_note->generateNewID();
    m_note->setData(newID, "");
    m_note->setTitle(newID, tr("(unnamed)"));
    m_note->setType(newID, "text");
    QMap<QString, QString> ref;
    ref["link"] = link;
    currentNoteRef = ref;
    m_note->setRef(newID, ref);
    m_note->insertID(newID);
    m_noteID = newID;
    reloadNotes();
    ui->listWidget_notes->setCurrentRow(m_note->getIDList().size() - 1);
    ui->label_noteLink->setText("");
    ui->lineEdit_note_titel->setText(tr("(unnamed)"));
    ui->textEdit_note->setHtml("");
    if (!m_note->getRef(m_noteID, "link").isEmpty())
        ui->label_noteLink->setText(notePos2Text(m_note->getRef(m_noteID, "link")));
    else
        ui->label_noteLink->setText("");*/
}
void MainWindow::removeNote(void)
{
    DEBUG_FUNC_NAME
    m_note->removeNote(m_noteID);
    ui->lineEdit_note_titel->setText(tr(""));
    ui->textEdit_note->setHtml("");
    ui->label_noteLink->setText("");
    reloadNotes();
}
void MainWindow::reloadNotes(void)
{
    DEBUG_FUNC_NAME
    ui->listWidget_notes->clear();
    QStringList id = m_note->getIDList();
    QStringList titles;
    myDebug() << " idList = " << id;
    for (int i = 0; i < id.size(); i++) {
        if (m_note->getType(id.at(i)) == "text") {
            //myDebug() << "id.at(i) = " << id.at(i) << " title = " << m_note->getTitle(id.at(i));
            titles << m_note->getTitle(id.at(i));
        }
    }
    ui->listWidget_notes->insertItems(0, titles);
    ui->listWidget_notes->setCurrentRow(id.lastIndexOf(m_noteID));
}
void MainWindow::notesContextMenu(void)
{
    QMenu *contextMenu = new QMenu(this);
    contextMenu->setObjectName("contextMenu");

    QAction *actionCopy = new QAction(this);
    actionCopy->setObjectName("actionCopy");
    actionCopy->setText(tr("Copy"));
    QIcon iconCopy;
    iconCopy.addPixmap(QPixmap(":/icons/16x16/edit-copy.png"), QIcon::Normal, QIcon::Off);
    actionCopy->setIcon(iconCopy);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copyNote()));

    QAction *actionNew = new QAction(this);
    actionNew->setObjectName(QString::fromUtf8("actionNew"));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(newNote()));
    QIcon iconNew;
    iconNew.addPixmap(QPixmap(":/icons/16x16/document-new.png"), QIcon::Normal, QIcon::Off);
    actionNew->setIcon(iconNew);
    actionNew->setText(tr("New"));

    QAction *actionRemove = new QAction(this);
    actionRemove->setObjectName("actionRemove");
    actionRemove->setText(tr("Delete"));
    QIcon iconRemove;
    iconRemove.addPixmap(QPixmap(":/icons/16x16/edit-delete.png"), QIcon::Normal, QIcon::Off);
    actionRemove->setIcon(iconRemove);
    connect(actionRemove, SIGNAL(triggered()), this, SLOT(removeNote()));

    contextMenu->addAction(actionCopy);
    contextMenu->addAction(actionNew);
    contextMenu->addAction(actionRemove);
    contextMenu->exec(QCursor::pos());
}
void MainWindow::noteSetTextBold(void)
{
    if (ui->textEdit_note->fontWeight() == QFont::Bold) {
        ui->textEdit_note->setFontWeight(QFont::Normal);
    } else {
        ui->textEdit_note->setFontWeight(QFont::Bold);
    }
}
void MainWindow::noteSetTextItalic(void)
{
    if (ui->textEdit_note->fontItalic()) {
        ui->textEdit_note->setFontItalic(false);
    } else {
        ui->textEdit_note->setFontItalic(true);
    }
}
void MainWindow::noteSetTextUnderline(void)
{
    if (ui->textEdit_note->fontUnderline()) {
        ui->textEdit_note->setFontUnderline(false);
    } else {
        ui->textEdit_note->setFontUnderline(true);
    }
}
void MainWindow::noteSetTextColor(void)
{
    QColor color = QColorDialog::getColor(Qt::green, this);
    if (color.isValid()) {
        ui->textEdit_note->setTextColor(color);
    }
}

QString MainWindow::notePos2Text(const QString &pos)
{
    DEBUG_FUNC_NAME

    myDebug() << "start pos = " << pos;
    QString string = "";
    UrlConverter urlConverter(UrlConverter::PersistentUrl,UrlConverter::InterfaceUrl,pos);
    urlConverter.m_biblesIniPath = biblesIniPath;//not nice, i know
    urlConverter.pharse();
    QString link = urlConverter.convert();

    string =  urlConverter.m_bookName + " " + QString::number(urlConverter.m_chapterID+1) + "," + QString::number(urlConverter.m_verseID+1);
    return  "<a href=\"" + link + "\" > " + string + "</a>";
}
void MainWindow::editNoteLink()
{
    DEBUG_FUNC_NAME
    /*int id = ui->listWidget_notes->currentRow();
    QString pos = note->notesPos.at(id);

    QStringList list = pos.split(";");
    if (list.size() < 5) {
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
    BiblePassageDialog *passageDialog = new BiblePassageDialog(this);
    //connect( passageDialog, SIGNAL( searched( QString,bool,bool,bool ) ), this, SLOT( showSearchResults( QString,bool,bool,bool ) ) );
    connect(passageDialog, SIGNAL(updated(QString)), this, SLOT(updateNote(QString)));
    qDebug() << "MainWindow::editNoteLink() m_bible.chapterData.size() = " << m_bible.chapterData.size(),
    passageDialog->setData(bibles, m_bible.bookFullName);
    passageDialog->setCurrent(bibleID, dirname, bookID, chapterID, verseID);
    passageDialog->show();
    passageDialog->exec();*/
}
void MainWindow::updateNote(QString link)
{
    DEBUG_FUNC_NAME
    currentNoteRef["link"] = link;
    showNote(ui->listWidget_notes->currentItem());
    return;
}
void MainWindow::newMark()
{
   /* DEBUG_FUNC_NAME
    if (m_bible.currentBibleID < 0) {
        //newNote();
        return;
    }
    int startverse = verseFromCursor(cursor);
    QString link;
    UrlConverter urlConverter(UrlConverter::None,UrlConverter::PersistentUrl,"");
    urlConverter.m_biblesIniPath = biblesIniPath;
    urlConverter.m_bibleID = QString::number(m_bible.currentBibleID);
    urlConverter.m_bookID = m_bible.currentBookID;
    urlConverter.m_chapterID = m_bible.currentChapterID - m_bible.chapterAdd;
    urlConverter.m_verseID = startverse - 1;
    urlConverter.m_bookName = m_bible.bookFullName.at(m_bible.currentBookID);
    link = urlConverter.convert();

    saveNote();
    reloadNotes();
    QString newID = m_note->generateNewID();
    m_note->setData(newID, "");
    m_note->setTitle(newID, tr("(unnamed)"));
    m_note->setType(newID, "mark");
    QMap<QString, QString> ref;
    ref["link"] = link;
    ref["start"] = QString::number(cursor.selectionStart());
    ref["end"] = QString::number(cursor.selectionEnd());
    myDebug() << cursor.selection().toHtml();
    m_note->setRef(newID, ref);
    m_note->insertID(newID);*/

}
