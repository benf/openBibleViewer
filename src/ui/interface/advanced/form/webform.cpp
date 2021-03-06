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
#include "webform.h"
#include "ui_webform.h"
#include <QtGui/QToolBar>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrintPreviewDialog>
#include <QtWebKit/QWebHistory>
#include <QtCore/QPointer>

WebForm::WebForm(QWidget *parent) :
    Form(parent),
    m_ui(new Ui::WebForm)
{
    m_ui->setupUi(this);

    connect(m_ui->lineEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));
    connect(m_ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(adjustLocation()));
    connect(m_ui->webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));

    m_ui->toolButton_back->setIcon(m_ui->webView->pageAction(QWebPage::Back)->icon());
    m_ui->toolButton_back->setToolTip(m_ui->webView->pageAction(QWebPage::Back)->toolTip());

    m_ui->toolButton_forward->setIcon(m_ui->webView->pageAction(QWebPage::Forward)->icon());
    m_ui->toolButton_forward->setToolTip(m_ui->webView->pageAction(QWebPage::Forward)->toolTip());

    m_ui->toolButton_reloadStop->setIcon(m_ui->webView->pageAction(QWebPage::Reload)->icon());
    m_ui->toolButton_reloadStop->setToolTip(m_ui->webView->pageAction(QWebPage::Reload)->toolTip());

    connect(m_ui->toolButton_forward, SIGNAL(clicked()), m_ui->webView, SLOT(forward()));
    connect(m_ui->toolButton_back, SIGNAL(clicked()), m_ui->webView, SLOT(back()));
    connect(m_ui->toolButton_reloadStop, SIGNAL(clicked()), m_ui->webView, SLOT(reload()));


    m_page = NULL;
}

WebForm::~WebForm()
{
    delete m_ui;
    if(m_page != NULL) {
        delete m_page;
        m_page = NULL;
    }
}

Form::FormType WebForm::type() const
{
    return Form::WebForm;
}

// shamelessly copied from Qt Demo Browser
QUrl WebForm::guessUrlFromString(const QString &string)
{
    QString urlStr = string.trimmed();
    QRegExp test(QLatin1String("^[a-zA-Z]+\\:.*"));

    // Check if it looks like a qualified URL. Try parsing it and see.
    bool hasSchema = test.exactMatch(urlStr);
    if(hasSchema) {
        QUrl url(urlStr, QUrl::TolerantMode);
        if(url.isValid())
            return url;
    }

    // Might be a file.
    if(QFile::exists(urlStr))
        return QUrl::fromLocalFile(urlStr);

    // Might be a shorturl - try to detect the schema.
    if(!hasSchema) {
        int dotIndex = urlStr.indexOf(QLatin1Char('.'));
        if(dotIndex != -1) {
            QString prefix = urlStr.left(dotIndex).toLower();
            QString schema = (prefix == QLatin1String("ftp")) ? prefix : QLatin1String("http");
            QUrl url(schema + QLatin1String("://") + urlStr, QUrl::TolerantMode);
            if(url.isValid())
                return url;
        }
    }

    // Fall back to QUrl's own tolerant parser.
    return QUrl(string, QUrl::TolerantMode);
}

void WebForm::pharseUrl(QString url)
{
    DEBUG_FUNC_NAME;
    const QString webPage = "webpage:/";
    url = url.remove(0, webPage.size());
    openModule(url.toInt());
}

void WebForm::openModule(const int moduleID)
{
    DEBUG_FUNC_NAME;
    if(m_page == NULL) {
        m_page = new WebPage();
        //m_moduleManager->initSimpleModule(m_page);
        m_page->setSettings(m_settings);
        m_page->setModuleMap(m_moduleManager->m_moduleMap);
        m_page->setModuleID(moduleID);

        QUrl url = m_page->getUrl();
        m_ui->webView->load(url);
    } else {
        m_page->setModuleID(moduleID);

        QUrl url = m_page->getUrl();
        m_ui->webView->load(url);
    }
}

void WebForm::init()
{
}

void WebForm::changeLocation()
{
    m_ui->webView->load(guessUrlFromString(m_ui->lineEdit->text()));
}

void WebForm::adjustLocation()
{
    m_ui->lineEdit->setText(m_ui->webView->url().toString());
}

void WebForm::copy()
{
    m_ui->webView->triggerPageAction(QWebPage::Copy);
}

void WebForm::selectAll()
{
    m_ui->webView->triggerPageAction(QWebPage::SelectAll);
}

void WebForm::print()
{
    QPrinter printer;
    QPointer<QPrintDialog> dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print"));
    if(dialog->exec() == QDialog::Accepted) {
        m_ui->webView->page()->mainFrame()->print(&printer);
    }
    delete dialog;
}

void WebForm::printPreview()
{
    QPrinter printer;
    QPointer<QPrintPreviewDialog> preview = new QPrintPreviewDialog(&printer, this);
    connect(preview, SIGNAL(paintRequested(QPrinter *)), m_ui->webView, SLOT(print(QPrinter *)));
    preview->exec();
    delete preview;
}

void WebForm::saveFile()
{

}

QString WebForm::selectedText()
{
    return m_ui->webView->selectedText();
}

void WebForm::zoomIn()
{
    m_ui->webView->setZoomFactor(m_ui->webView->zoomFactor() + 0.1);
}

void WebForm::zoomOut()
{
    m_ui->webView->setZoomFactor(m_ui->webView->zoomFactor() - 0.1);
}

void WebForm::activated()
{
    m_actions->clearBooks();
    m_actions->clearChapters();
    if(m_page != NULL) {
        m_actions->setTitle(m_page->moduleTitle());
        m_actions->setCurrentModule(m_page->moduleID());
    }
}

void WebForm::save()
{
    const QString a = m_settings->session.id() + "/windows/" + QString::number(m_id) + "/";
    m_settings->session.file()->setValue(a + "type", "web");
    m_settings->session.file()->setValue(a + "url", m_ui->webView->url());

    QByteArray history;
    QDataStream historyStream(&history, QIODevice::ReadWrite);
    QWebHistory *hist  = m_ui->webView->history();
    historyStream << *(hist);
    m_settings->session.file()->setValue(a + "history", history.toBase64());
}

void WebForm::restore(const QString &key)
{
    const QString a = m_settings->session.id() + "/windows/" + key + "/";

    QUrl url = m_settings->session.file()->value(a + "url").toUrl();
    if(url.isValid())
        m_ui->webView->load(url);

    QByteArray history = QByteArray::fromBase64(m_settings->session.file()->value(a + "history").toByteArray());
    QDataStream readingStream(&history, QIODevice::ReadOnly);
    QWebHistory *hist  = m_ui->webView->history();
    readingStream >> *(hist);
}

void WebForm::loadStarted()
{
    if(m_ui->webView->history()->canGoBack()) {
        m_ui->toolButton_back->setDisabled(false);
    } else {
        m_ui->toolButton_back->setDisabled(true);
    }

    if(m_ui->webView->history()->canGoForward()) {
        m_ui->toolButton_forward->setDisabled(false);
    } else {
        m_ui->toolButton_forward->setDisabled(true);
    }

}
