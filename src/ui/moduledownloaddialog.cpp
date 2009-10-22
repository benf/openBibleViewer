#include "moduledownloaddialog.h"
#include "ui_moduledownloaddialog.h"
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
#include <QtXml/QDomElement>
#include <QtCore/QFile>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QStyle>
#include <QtGui/QIcon>
#include <QtCore/QList>
#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QMessageBox>
#include <QtCore/QtDebug>
moduleDownloadDialog::moduleDownloadDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::moduleDownloadDialog)
{
    ui->setupUi(this);
    progressDialog = new QProgressDialog(this);

    http = new QHttp(this);
    connect(ui->pushButton_download, SIGNAL(clicked()), this, SLOT(downloadNext()));//set httpRequestAborted = false
    connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(item(QTreeWidgetItem*)));
    connect(http, SIGNAL(requestFinished(int, bool)),
            this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(updateDataReadProgress(int, int)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
            this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    currentDownload = -1;
    httpRequestAborted = false;

}
void moduleDownloadDialog::readModules()
{
    qDebug() << "moduleDownloadDialog::readModules()";
    //load list from file
    //show it in treewidget
    QDomDocument doc;
    QFile file(":/data/modules.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "moduleDownloadDialog::readModules() cant read the file";
        return;
    }
    if (!doc.setContent(&file)) {
        qDebug() << "moduleDownloadDialog::readModules() the file isnt valid";
        return;
    }
    QList<QTreeWidgetItem *> items;

    QDomNode id = doc.documentElement().firstChild();
    for (; !id.isNull();) {
        QMap<QString, QStringList> map2;
        QMap<QString, QStringList> map;
        QDomNode item = id.firstChild();
        for (; !item.isNull();) {
            QDomElement e = item.toElement();
            map[e.attribute("lang", "unkown")].append(e.firstChild().toText().data());
            map2[e.attribute("lang", "unkown")].append(e.attribute("link", ""));
            item = item.nextSibling();
        }
        QTreeWidgetItem *top = new QTreeWidgetItem(ui->treeWidget);

        QStyle *style = ui->treeWidget->style();
        QIcon folderIcon;
        folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon), QIcon::Normal, QIcon::Off);
        folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon), QIcon::Normal, QIcon::On);
        top->setIcon(0, folderIcon);
        QDomElement e = id.toElement();
        QString typ = e.attribute("typ", "bible");
        if (typ == "bible") {
            top->setText(0, "Bibles");
        } else if (typ == "strong") {
            top->setText(0, "Strong Modules");
        }
        items.append(top);

        QMapIterator<QString, QStringList> i(map);
        while (i.hasNext()) {
            i.next();
            if (i.value().size() > 0) {
                QTreeWidgetItem *langItem = new QTreeWidgetItem();
                langItem->setText(0, i.key());
                langItem->setCheckState(0, Qt::Unchecked);
                langItem->setData(1, 0, "lang");
                top->addChild(langItem);
                for (int a = 0; a < i.value().size(); a++) {
                    QTreeWidgetItem *bibleItem = new QTreeWidgetItem();
                    bibleItem->setData(1, 0, "module");
                    bibleItem->setData(2, 0, map2[i.key()].at(a));
                    bibleItem->setData(2, 1, map[i.key()].at(a));
                    bibleItem->setCheckState(0, Qt::Unchecked);
                    bibleItem->setText(0, i.value().at(a));
                 /*   QIcon bibleIcon;
                    bibleIcon.addPixmap(QPixmap(":/icons/16x16/text-xml.png"), QIcon::Normal, QIcon::Off);
                    bibleItem->setIcon(0, bibleIcon);*/
                    langItem->addChild(bibleItem);
                }
            }
        }
        top->setExpanded(true);
        id = id.nextSibling();
    }
}
void moduleDownloadDialog::item(QTreeWidgetItem* i)
{
    qDebug() << "moduleDownloadDialog::i i = " << i->data(1, 0) << " downloadList = " << downloadList;
    httpRequestAborted = false;
    if (i->data(1, 0) == "lang") {
        if (i->checkState(0) == Qt::Checked) {
            for(int c = 0; c< i->childCount();c++) {
                i->child(c)->setCheckState(0,Qt::Checked);
                item(i->child(c));
            }
            //check also all childrem of this
        } else if (i->checkState(0) == Qt::Unchecked) {
             for(int c = 0; c< i->childCount();c++) {
                i->child(c)->setCheckState(0,Qt::Unchecked);
                item(i->child(c));
            }
            //uncheck all children
        }
    } else if (i->data(1, 0) == "module") {

        if (i->checkState(0) == Qt::Checked) {
            downloadList.append(i->data(2, 0).toString());
            nameList.append(i->data(2, 1).toString());
            // add to download list
        } else if (i->checkState(0) == Qt::Unchecked) {
            downloadList.removeOne(i->data(2, 0).toString());
            nameList.removeOne(i->data(2, 1).toString());
            //remove from download list
        }
    }
    //if(i->checkState(0) == Qt::
}
void moduleDownloadDialog::setSettings(settings_s settings)
{
    m_set = settings;
}
void moduleDownloadDialog::downloadNext()
{
    if (httpRequestAborted)
        return;
    qDebug() << "moduleDownloadDialog::downloadNext() currentDownload = " << currentDownload << " size = " << downloadList.size();
    QDir dir(m_set.homePath);
    dir.mkdir(m_set.homePath + "modules");
    if (currentDownload + 1 == downloadList.size() && downloadList.size() != 0) {
        qDebug() << "moduleDownloadDialog::downloadNext() emit";
        emit downloaded(downloadedList, downNames);
        return;
    }
    if (currentDownload < downloadList.size() && downloadList.size() != 0) {
        currentDownload++;
        QUrl url(downloadList.at(currentDownload));
        QFileInfo fileInfo(url.path());
        QDir d(m_set.homePath + "modules/");
        dir.mkpath(m_set.homePath + "modules/" + fileInfo.fileName() +"/");
        QString fileName = m_set.homePath + "modules/" + fileInfo.fileName() +"/"+fileInfo.fileName();
        downloadedList << fileName;
        downNames << nameList.at(currentDownload);
        qDebug() << "moduleDownloadDialog::downloadNext() fileName = " << fileName;

        if (QFile::exists(fileName)) {
            QFile::remove(fileName);
        }
        file = new QFile(fileName);
        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("HTTP"), tr("Unable to save the file %1: %2.").arg(fileName).arg(file->errorString()));
            delete file;
            file = 0;
            return;
        }
        QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
        http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

        httpRequestAborted = false;
        QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
        if (path.isEmpty())
            path = "/";
        httpGetId = http->get(path, file);
        progressDialog->setWindowTitle(tr("HTTP"));
        progressDialog->setLabelText(tr("Downloading %1.").arg(nameList.at(currentDownload)));
        progressDialog->setModal(true);
    } else {
        qDebug() << "moduleDownloadDialog::downloadNext() nothing selected";
    }
}
void moduleDownloadDialog::cancelDownload()
{
    httpRequestAborted = true;
    http->abort();
}

void moduleDownloadDialog::httpRequestFinished(int requestId, bool error)
{
    if (requestId != httpGetId)
        return;
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }
        progressDialog->hide();
        return;
    }

    if (requestId != httpGetId)
        return;
    if (currentDownload > downloadList.size() - 2 || downloadList.size() == 1) {
        progressDialog->hide();
    }
    file->close();

    if (error) {
        file->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(http->errorString()));
    } else {
        qDebug() << "moduleDownloadDialog::httpRequestFinished() Downloaded";
        file->close();
    }

    //downloadButton->setEnabled(true);
    delete file;
    file = 0;
    downloadNext();
}

void moduleDownloadDialog::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    switch (responseHeader.statusCode()) {
    case 200:                   // Ok
    case 301:                   // Moved Permanently
    case 302:                   // Found
    case 303:                   // See Other
    case 307:                   // Temporary Redirect
        // these are not error conditions
        break;

    default:
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(responseHeader.reasonPhrase()));
        httpRequestAborted = true;
        progressDialog->hide();
        http->abort();
    }
}

void moduleDownloadDialog::updateDataReadProgress(int bytesRead, int totalBytes)
{
    if (httpRequestAborted)
        return;
    //qDebug() << "moduleDownloadDialog::updateDataReadProgress() bytesRead = " << bytesRead;
    if (totalBytes == 0) {
        if (progressDialog->maximum() != 0) {
            progressDialog->setMaximum(0);
            progressDialog->setMinimum(0);
            progressDialog->setValue(1);
        }
    } else {
        if (progressDialog->maximum() != totalBytes) {
            progressDialog->setMaximum(totalBytes);
        }
        progressDialog->setValue(bytesRead);
    }
}

moduleDownloadDialog::~moduleDownloadDialog()
{
    delete ui;
}

void moduleDownloadDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}