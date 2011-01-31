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
#include "insertlinkdialog.h"
#include "ui_insertlinkdialog.h"

InsertLinkDialog::InsertLinkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertLinkDialog)
{
    ui->setupUi(this);
    m_itemModel = new QStandardItemModel(ui->treeView);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(save()));
}

InsertLinkDialog::~InsertLinkDialog()
{
    delete ui;
}
void InsertLinkDialog::init()
{
    m_itemModel->clear();
    QStandardItem *parentItem = m_itemModel->invisibleRootItem();

    const QStringList ids = m_notes->getIDList("text");

    foreach(const QString & id, ids) {
        QStandardItem *noteItem = new QStandardItem;
        noteItem->setText(m_notes->getTitle(id));
        noteItem->setData(id);
        parentItem->appendRow(noteItem);
    }

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_itemModel);
    m_proxyModel->setHeaderData(0, Qt::Horizontal, tr("Note Title"));
    m_selectionModel = new QItemSelectionModel(m_proxyModel);

    ui->treeView->setSortingEnabled(true);
    ui->treeView->setModel(m_proxyModel);
    ui->treeView->setSelectionModel(m_selectionModel);

    m_frame = new BiblePassageFrame(this);
    setAll(m_frame);
    m_frame->init();
    ui->page->layout()->addWidget(m_frame);
}

void InsertLinkDialog::setCurrent(const QList<VerseUrlRange> &ranges)
{
    m_frame->setVerseUrlRanges(ranges);

}


void InsertLinkDialog::save()
{
    if(ui->toolBox->currentIndex() == 0) {
        /*const QString link = m_path
                             + ";" + QString::number(ui->comboBox_books->currentIndex())
                             + ";" + QString::number(ui->spinBox_chapter->value() - 1)
                             + ";" + QString::number(ui->spinBox_verse->value() - 1)
                             + ";" + m_settings->getModuleCache(m_path).bookNames.values().at(ui->comboBox_books->currentIndex());
        //todo: new modulecache and new link
        emit newLink("persistent://" + link);*/
    } else if(ui->toolBox->currentIndex() == 1) {
        const QModelIndexList list = m_selectionModel->selectedRows(0);
        if(!list.isEmpty()) {
            const QString id = list.at(0).data(Qt::UserRole + 1).toString();
            emit newLink("note://" + id);
        }
        ///get note id
    } else if(ui->toolBox->currentIndex() == 2) {
        emit newLink(ui->lineEdit->text());
    }
    close();
}
void InsertLinkDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        m_proxyModel->setHeaderData(0, Qt::Horizontal, tr("Note Title"));
        break;
    default:
        break;
    }
}
