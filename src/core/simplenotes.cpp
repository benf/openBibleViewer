#include "simplenotes.h"
#include <QSortFilterProxyModel>
#include "src/core/dbghelper.h"
#include <QtGui/QClipboard>
#include <QtGui/QMenu>
#include <QtGui/QColorDialog>
#include <QApplication>
#include "src/ui/dialog/biblepassagedialog.h"
#include "src/core/urlconverter.h"
SimpleNotes::SimpleNotes()
{
}
void SimpleNotes::setSettings(Settings *settings)
{
    m_settings = settings;
}
void SimpleNotes::setModuleManager(ModuleManager *moduleManager)
{
    m_moduleManager = moduleManager;
}
void SimpleNotes::setNotes(Notes *notes)
{
    m_notes = notes;
}
void SimpleNotes::setDataWidget(QTextEdit *data)
{
    m_textEdit_note = data;
}
void SimpleNotes::setTitleWidget(QLineEdit *title)
{
    m_lineEdit_title = title;
}
void SimpleNotes::setViewWidget(QTreeView *treeView)
{
    m_treeView = treeView;
}

void SimpleNotes::init()
{
    m_itemModel = new QStandardItemModel(m_treeView);
    connect(m_treeView,SIGNAL(activated(QModelIndex)),this,SLOT(showNote(QModelIndex)));
    connect(m_treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(notesContextMenu()));
    connect(m_textEdit_note, SIGNAL(undoAvailable(bool)), this, SLOT(fastSave()));
    connect(m_textEdit_note, SIGNAL(redoAvailable(bool)), this, SLOT(fastSave()));

    if (!m_notes->isLoaded()) {
        m_notes->init(m_settings->homePath + "notes.xml");
        m_notes->loadNotes();
        m_notes->readNotes();
    }

    m_notes->loadingNewInstance();
    connect(m_notes, SIGNAL(saveAll()), this, SLOT(saveNote()));
    connect(m_notes,SIGNAL(titleChanged(QString,QString)), this, SLOT(changeTitle(QString,QString)));
    connect(m_notes,SIGNAL(dataChanged(QString,QString)),this,SLOT(changeData(QString,QString)));
    connect(m_notes,SIGNAL(refChanged(QString,QMap<QString,QString>)),this,SLOT(changeRef(QString,QMap<QString,QString>)));

    m_itemModel->clear();
    QStringList id = m_notes->getIDList();

    QStandardItem *parentItem = m_itemModel->invisibleRootItem();
    myDebug() << " id = " << id;
    for (int i = 0; i < id.size(); i++) {
        if (m_notes->getType(id.at(i)) == "text") {

            QStandardItem *bibleItem = new QStandardItem;
            bibleItem->setText(m_notes->getTitle(id.at(i)));
            bibleItem->setData(id.at(i));
            parentItem->appendRow(bibleItem);
        }
    }
    m_noteID = "";

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_itemModel);
    m_selectionModel = new QItemSelectionModel(m_proxyModel);

    m_treeView->setSortingEnabled(true);
    m_treeView->setModel(m_proxyModel);
    m_treeView->setSelectionModel(m_selectionModel);
}
void SimpleNotes::showNote(QModelIndex index)
{
    showNote(index.data(Qt::UserRole + 1).toString());
}
void SimpleNotes::showNote(const QString &noteID)
{
    //DEBUG_FUNC_NAME
    //save current notes
    select(noteID);

    m_notes->setData(m_noteID, m_textEdit_note->toHtml());
    m_notes->setTitle(m_noteID, m_lineEdit_title->text());
    m_notes->setRef(m_noteID, currentNoteRef);


    //load new notes
    m_noteID = noteID;
    currentNoteRef = m_notes->getRef(m_noteID);

    setTitle(m_notes->getTitle(m_noteID));
    setData(m_notes->getData(m_noteID));
    setRef(currentNoteRef);
}
void SimpleNotes::setTitle(QString title)
{
    m_lineEdit_title->setText(title);
}
void SimpleNotes::setData(QString data)
{
     m_textEdit_note->setHtml(data);
}
void SimpleNotes::setRef(QMap<QString, QString> ref)
{
    /*if(!ref["link"].isEmpty()) {
        ui->label_noteLink->setText(m_moduleManager->notePos2Text(ref["link"]));
        ui->pushButton_editNoteLink->setEnabled(true);
    } else {
        ui->label_noteLink->setText("");
        ui->pushButton_editNoteLink->setEnabled(false);
    }*/
}
void SimpleNotes::changeData(QString id, QString data)
{
    DEBUG_FUNC_NAME
    if(m_noteID == id) {
        setData(data);
    }
}
void SimpleNotes::changeTitle(QString id, QString title)
{
    DEBUG_FUNC_NAME
    if(m_noteID == id) {
         setTitle(title);
    }
    //todo: reload list
}
void SimpleNotes::changeRef(QString id, QMap<QString, QString> ref)
{
    if(m_noteID == id) {
         setRef(ref);
    }
}

void SimpleNotes::copyNote(void)
{
    QModelIndexList list = m_selectionModel->selectedRows(0);
    QClipboard *clipboard = QApplication::clipboard();
    QString text;
    for(int i = 0; i < list.size();i++) {
         QTextDocument doc;
         doc.setHtml(m_notes->getData(list.at(i).data(Qt::UserRole+1).toString()));
         if(i != 0)
             text += "\n";
         text += doc.toPlainText();
    }
    if(!text.isEmpty()) {
        clipboard->setText(text);
    }
}
void SimpleNotes::saveNote(void)
{
    //DEBUG_FUNC_NAME
    aktNote();
    fastSave();
    m_notes->saveNotes();
}
void SimpleNotes::fastSave(void)
{
    m_notes->setData(m_noteID, m_textEdit_note->toHtml());
    m_notes->setTitle(m_noteID, m_lineEdit_title->text());
    m_notes->setRef(m_noteID, currentNoteRef);
}
void SimpleNotes::aktNote()
{
    QModelIndexList list = m_treeView->model()->match(m_treeView->model()->index(0, 0), Qt::UserRole + 1, m_noteID);
    if (list.size() != 1) {
        myDebug() << "invalid noteID = " << m_noteID;
        return;
    }
    QModelIndex index = list.at(0);
    if(index.data(Qt::DisplayRole) != m_notes->getTitle(m_noteID)) {
        m_treeView->model()->setData(index,m_notes->getTitle(m_noteID),Qt::DisplayRole);
    }
}
void SimpleNotes::select(QString noteID)
{
    QModelIndexList list = m_treeView->model()->match(m_treeView->model()->index(0, 0), Qt::UserRole + 1, noteID);
    if (list.size() != 1) {
        return;
    }
    QModelIndex index = list.at(0);
    m_selectionModel->clearSelection();
    m_selectionModel->setCurrentIndex(index, QItemSelectionModel::Select);
}

void SimpleNotes::newNote(void)
{
    aktNote();
    fastSave();

    QString newID = m_notes->generateNewID();
    m_notes->setData(newID, "");
    m_notes->setTitle(newID, tr("(unnamed)"));
    m_notes->setType(newID, "text");

    currentNoteRef = QMap<QString, QString>();
    m_notes->setRef(newID, currentNoteRef);

    m_notes->insertID(newID);
    m_noteID = newID;

    QStandardItem *parentItem = m_itemModel->invisibleRootItem();
    QStandardItem *newItem = new QStandardItem;
    newItem->setText(m_notes->getTitle(m_noteID));
    newItem->setData(m_noteID);
    parentItem->appendRow(newItem);

    select(m_noteID);
    setTitle(tr("(unnamed)"));
    setData("");
    setRef(currentNoteRef);

}
void SimpleNotes::notesContextMenu(void)
{
    QMenu *contextMenu = new QMenu(m_treeView);
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
void SimpleNotes::removeNote(void)
{
    DEBUG_FUNC_NAME
    QModelIndexList list = m_selectionModel->selectedRows(0);
    for(int i = 0; i < list.size();i++) {
        QString id = list.at(i).data(Qt::UserRole+1).toString();
        if(id == m_noteID) {
            setTitle("");
            setData("");
            setRef(QMap<QString,QString>());
        }
        m_notes->removeNote(id);
        m_treeView->model()->removeRow(list.at(i).row());
    }
}