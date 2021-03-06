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
#include "bibleform.h"
#include "ui_bibleform.h"
#include "src/core/verse/reftext.h"
#include <QtCore/QPointer>
#include "src/core/link/urlconverter2.h"
BibleForm::BibleForm(QWidget *parent) : Form(parent), m_ui(new Ui::BibleForm)
{
    //DEBUG_FUNC_NAME
    m_id = -1;
    m_ui->setupUi(this);

    m_view = new WebView(this);
    m_view->setObjectName("webView");
    m_view->setUrl(QUrl("about:blank"));
    m_ui->verticalLayout->addWidget(m_view);

    m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_ui->toolButton_backward, SIGNAL(clicked()), this, SLOT(backward()));
    connect(m_ui->toolButton_forward, SIGNAL(clicked()), this, SLOT(forward()));
    connect(m_ui->toolButton_bibleList, SIGNAL(clicked()), this, SLOT(showBibleListMenu()));

    m_ui->toolButton_backward->setShortcut(QKeySequence::Back);
    m_ui->toolButton_forward->setShortcut(QKeySequence::Forward);

    connect(m_ui->comboBox_books, SIGNAL(activated(int)), this, SLOT(readBook(int)));
    connect(m_ui->comboBox_chapters, SIGNAL(activated(int)), this, SLOT(readChapter(int)));

    m_api = NULL;
    m_verseTable = NULL;
    setButtons();

}
BibleForm::~BibleForm()
{
    delete m_ui;
    m_ui = NULL;
    delete m_verseTable;
    m_verseTable = NULL;
}
Form::FormType BibleForm::type() const
{
    return Form::BibleForm;
}

void BibleForm::init()
{
    //DEBUG_FUNC_NAME
    m_verseTable = m_moduleManager->newVerseTable();
    attachApi();

    connect(m_view->page(), SIGNAL(linkClicked(QUrl)), m_actions, SLOT(get(QUrl)));
    connect(m_view, SIGNAL(linkMiddleOrCtrlClicked(QUrl)), m_actions, SLOT(newGet(QUrl)));

    m_view->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    m_view->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
#if QT_VERSION >= 0x040700
    m_view->settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    m_view->settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
    m_view->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    m_view->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    m_view->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
#endif

    connect(m_view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachApi()));

    connect(m_actions, SIGNAL(_updateChapters(int, QSharedPointer<Versification> )), this, SLOT(forwardSetChapters(int, QSharedPointer<Versification> )));
    connect(m_actions, SIGNAL(_updateBooks(QSharedPointer<Versification> )), this, SLOT(forwardSetBooks(QSharedPointer<Versification> )));

    connect(m_actions, SIGNAL(_clearBooks()), this, SLOT(forwardClearBooks()));
    connect(m_actions, SIGNAL(_clearChapters()), this, SLOT(forwardClearChapters()));

    connect(m_actions, SIGNAL(_setCurrentBook(QSet<int>)), this, SLOT(forwardSetCurrentBook(QSet<int>)));
    connect(m_actions, SIGNAL(_setCurrentChapter(QSet<int>)), this, SLOT(forwardSetCurrentChapter(QSet<int>)));
    connect(m_actions, SIGNAL(_showTextRanges(QString, TextRanges, VerseUrl)), this, SLOT(forwardShowTextRanges(QString, TextRanges, VerseUrl)));
    connect(m_actions, SIGNAL(_searchInText(SearchResult*)), this, SLOT(forwardSearchInText(SearchResult*)));
    connect(m_actions, SIGNAL(_reloadIf(VerseUrl)), this, SLOT(reloadIf(VerseUrl)));
    connect(m_actions, SIGNAL(_moduleChanged(int)), this, SLOT(moduleChanged(int)));

    connect(m_view, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this, SLOT(showContextMenu(QContextMenuEvent*)));
    createDefaultMenu();
}
void BibleForm::newModule(const int moduleID)
{
    myDebug() << moduleID;
    m_moduleManager->newVerseModule(moduleID, QPoint(0, 0), m_verseTable);
    if(m_verseTable->verseModule())
        m_verseTable->verseModule()->setModuleID(moduleID);

}
int BibleForm::newModule()
{
    DEBUG_FUNC_NAME
    int defaultModuleID = -1;
    QMapIterator<int, ModuleSettings*> i(m_settings->m_moduleSettings);
    while(i.hasNext()) {
        i.next();
        if(i.value()->defaultModule == OBVCore::DefaultBibleModule) {
            defaultModuleID = i.key();
            break;
        }

    }
    myDebug() << "default" << defaultModuleID;
    if(defaultModuleID == -1) {
        QMapIterator<int, Module*> i2(m_moduleManager->m_moduleMap->data);
        while(i2.hasNext() && defaultModuleID == -1) {
            i2.next();
            if(i2.value()->moduleClass() == OBVCore::BibleModuleClass) {
                defaultModuleID = i2.key();
                break;
            }
        }
    }
    myDebug() << "some" << defaultModuleID;
    newModule(defaultModuleID);
    return defaultModuleID;
}

void BibleForm::pharseUrl(const VerseUrl &url)
{
    myDebug() << url.toString();
    if(m_url.isValid()) {
        m_url = m_url.applyUrl(url);
    } else {
        m_url = url;
        if(verseTableLoaded() && !m_url.hasModuleID()) {
            m_url.setModuleID(m_verseTable->verseModule()->moduleID());
        }
    }

    Ranges ranges;
    foreach(VerseUrlRange range, m_url.ranges()) {
        ranges.addRange(range.toRange());
    }
    ranges.setSource(m_url);
    showRanges(ranges, m_url);
    if(m_url.hasParam("searchInCurrentText")) {
        m_actions->searchInText();
    }
}

void BibleForm::pharseUrl(const QString &string)
{
    myDebug() << string;
    const QString bible = "verse:/";
    const QString bq = "go";

    if(string.startsWith(bible)) {

        VerseUrl url;
        Ranges ranges;
        if(!url.fromString(string)) {
            return;
        }

        if(m_url.isValid()) {
            m_url = m_url.applyUrl(url);
        } else {
            m_url = url;
            if(verseTableLoaded() && !m_url.hasModuleID()) {
                m_url.setModuleID(m_verseTable->verseModule()->moduleID());
            }
        }

        foreach(VerseUrlRange range, m_url.ranges()) {
            ranges.addRange(range.toRange());
        }
        ranges.setSource(m_url);
        showRanges(ranges, m_url);
        if(m_url.hasParam("searchInCurrentText")) {
            m_actions->searchInText();
        }
    } else if(string.startsWith(bq)) {
        //its a biblequote internal link, but i dont have the specifications!!!
        QStringList internal = string.split(" ");
        const QString bibleID = internal.at(1);//todo: use it
        myDebug() << "bibleID = " << bibleID;

        int bookID = internal.at(2).toInt() - 1;
        int chapterID = internal.at(3).toInt() - 1;
        int verseID = internal.at(4).toInt();
        VerseUrlRange range;
        range.setModule(VerseUrlRange::LoadCurrentModule);
        range.setBook(bookID);
        range.setChapter(chapterID);
        range.setStartVerse(verseID);
        VerseUrl url(range);
        m_actions->get(url);
    }
}
void BibleForm::showRanges(const Ranges &ranges, const VerseUrl &url)
{
    DEBUG_FUNC_NAME
    std::pair<QString, TextRanges> r;
    m_verseTable->currentVerseTableID();
    const int moduleID = ranges.getList().first().moduleID();

    if(!verseTableLoaded()) {
        m_moduleManager->newVerseModule(moduleID, QPoint(0, 0), m_verseTable);
    }
    //todo: its very important to fix this bug
    /*if(m_verseTable->verseModule()->moduleID() != ranges.getList().first().moduleID()) {

        const QPoint p = m_verseTable->m_points.value(m_verseTable->currentVerseTableID());
        VerseModule *m;
        if(m_moduleManager->getModule(moduleID)->moduleClass() == OBVCore::BibleModuleClass) {
            m = new Bible();
            m_moduleManager->initVerseModule(m);
        } else {
            myWarning() << "trying to load an non bible module";
            return;
        }
        OBVCore::ModuleType type = m_moduleManager->getModule(moduleID)->moduleType();
        m->setModuleType(type);
        m->setModuleID(moduleID);
        m_verseTable->addModule(m, p);
    }*/
    r = m_verseTable->readRanges(ranges);

    if(!r.second.failed()) {
        showTextRanges(r.first, r.second, url);
        m_actions->updateChapters(r.second.minBookID(), m_verseTable->verseModule()->versification());
        m_actions->updateBooks(m_verseTable->verseModule()->versification());
        m_actions->setCurrentBook(r.second.bookIDs());
        m_actions->setCurrentChapter(r.second.chapterIDs());
        m_actions->setTitle(m_verseTable->verseModule()->moduleTitle());
    } else {
        showTextRanges(r.first, r.second, url);
        m_actions->clearBooks();
        m_actions->clearChapters();
    }
}

void BibleForm::nextChapter()
{
    if(!verseTableLoaded())
        return;
    if(m_verseTable->verseModule()->lastTextRanges()->minChapterID() <
            m_verseTable->verseModule()->versification()->maxChapter().value(m_verseTable->verseModule()->lastTextRanges()->minBookID()) - 1) {
        VerseUrl bibleUrl;
        VerseUrlRange range;
        range.setOpenToTransformation(false);
        range.setModule(VerseUrlRange::LoadCurrentModule);
        range.setBook(VerseUrlRange::LoadCurrentBook);
        range.setChapter(m_verseTable->verseModule()->lastTextRanges()->minChapterID() + 1);
        range.setWholeChapter();
        bibleUrl.addRange(range);
        m_actions->get(bibleUrl);
    } else if(m_verseTable->verseModule()->lastTextRanges()->minBookID() < m_verseTable->verseModule()->versification()->bookCount() - 1) {
        VerseUrl bibleUrl;
        VerseUrlRange range;
        range.setOpenToTransformation(false);
        range.setModule(VerseUrlRange::LoadCurrentModule);
        range.setBook(m_verseTable->verseModule()->lastTextRanges()->minBookID() + 1);
        range.setChapter(VerseUrlRange::LoadFirstChapter);
        range.setWholeChapter();
        bibleUrl.addRange(range);
        m_actions->get(bibleUrl);
    }
}

void BibleForm::previousChapter()
{
    if(!verseTableLoaded())
        return;
    if(m_verseTable->verseModule()->lastTextRanges()->minChapterID() > 0) {
        VerseUrl bibleUrl;
        VerseUrlRange range;
        range.setOpenToTransformation(false);
        range.setModule(VerseUrlRange::LoadCurrentModule);
        range.setBook(VerseUrlRange::LoadCurrentBook);
        range.setChapter(m_verseTable->verseModule()->lastTextRanges()->minChapterID() - 1);
        range.setWholeChapter();
        bibleUrl.addRange(range);
        m_actions->get(bibleUrl);
    } else if(m_verseTable->verseModule()->lastTextRanges()->minBookID() > 0) {
        VerseUrl bibleUrl;
        VerseUrlRange range;
        range.setOpenToTransformation(false);
        range.setModule(VerseUrlRange::LoadCurrentModule);
        range.setBook(m_verseTable->verseModule()->lastTextRanges()->minBookID() - 1);
        range.setChapter(VerseUrlRange::LoadLastChapter);
        range.setWholeChapter();
        bibleUrl.addRange(range);
        m_actions->get(bibleUrl);
    }
}

void BibleForm::restore(const QString &key)
{
    const QString a = m_settings->session.id() + "/windows/" + key + "/";
    const qreal zoom = m_settings->session.file()->value(a + "zoom").toReal();
    const QPoint scroll = m_settings->session.file()->value(a + "scrollPosition").toPoint();

    //load verse module

    m_verseTable->clear();
    const QStringList urls = m_settings->session.file()->value(a + "urls").toStringList();
    const QStringList points = m_settings->session.file()->value(a + "biblePoints").toStringList();

    for(int j = 0; j < urls.size() && j < points.size(); j++) {
        const QString url = urls.at(j);
        QStringList tmp = points.at(j).split("|");
        QPoint point;
        point.setX(tmp.first().toInt());
        point.setY(tmp.last().toInt());

        UrlConverter2 urlConverter(UrlConverter::PersistentUrl, UrlConverter::InterfaceUrl, url);
        urlConverter.setSM(m_settings, m_moduleManager->m_moduleMap.data());
        urlConverter.convert();

        if(urlConverter.moduleID() != -1) {
            m_moduleManager->newVerseModule(urlConverter.moduleID(), point, m_verseTable);
            pharseUrl(urlConverter.url());//these urls are handeld by this Form
        }
    }

    m_view->page()->mainFrame()->setScrollPosition(scroll);
    m_view->setZoomFactor(zoom);
}

void BibleForm::save()
{
    VerseTable *list = m_verseTable;

    QStringList urls;
    QStringList points;
    if(list > 0) {
        QHashIterator<int, VerseModule *> i(list->m_modules);
        while(i.hasNext()) {
            i.next();
            VerseModule *b = i.value();
            if(b != NULL && b->moduleID() >= 0) {
                myDebug() << b->moduleID() << b->moduleTitle();
                VerseUrl bibleUrl = b->lastTextRanges()->source().source();
                bibleUrl.setModuleID(b->moduleID());
                myDebug() << bibleUrl.toString();

                UrlConverter urlConverter(UrlConverter::InterfaceUrl, UrlConverter::PersistentUrl, bibleUrl);
                urlConverter.setSettings(m_settings);
                urlConverter.setModuleMap(m_moduleManager->m_moduleMap.data());
                VerseUrl newUrl = urlConverter.convert();

                const QString url = newUrl.toString();
                const QPoint point = list->m_points.value(i.key());
                myDebug() << url;

                points << QString::number(point.x()) + "|" + QString::number(point.y());
                urls << url;
            }
        }
    }
    const QString a = m_settings->session.id() + "/windows/" + QString::number(m_id) + "/";
    m_settings->session.file()->setValue(a + "urls", urls);
    m_settings->session.file()->setValue(a + "biblePoints", QVariant(points));
    m_settings->session.file()->setValue(a + "scrollPosition", m_view->page()->mainFrame()->scrollPosition());
    m_settings->session.file()->setValue(a + "zoom", m_view->zoomFactor());

    m_settings->session.file()->setValue(a + "type", "bible");
}

void BibleForm::attachApi()
{
    //DEBUG_FUNC_NAME
    QWebFrame * frame = m_view->page()->mainFrame();
    {
        QFile file(":/data/js/tools.js");
        if(!file.open(QFile::ReadOnly | QFile::Text))
            return;
        QTextStream stream(&file);
        QString jquery = stream.readAll();
        file.close();
        frame->evaluateJavaScript(jquery);
    }

    frame->addToJavaScriptWindowObject("Module", m_api->moduleApi());
    m_api->moduleApi()->setFrame(frame);
}

void BibleForm::historySetUrl(QString url)
{
    m_browserHistory.setCurrent(url);
    setButtons();
}
void BibleForm::backward()
{
    m_actions->get(m_browserHistory.backward());
    setButtons();
}
void BibleForm::forward()
{
    m_actions->get(m_browserHistory.forward());
    setButtons();
}
void BibleForm::setButtons()
{
    if(m_browserHistory.backwardAvailable()) {
        m_ui->toolButton_backward->setDisabled(false);
    } else {
        m_ui->toolButton_backward->setDisabled(true);
    }
    if(m_browserHistory.forwardAvailable()) {
        m_ui->toolButton_forward->setDisabled(false);
    } else {
        m_ui->toolButton_forward->setDisabled(true);
    }
}

void BibleForm::showBibleListMenu()
{
    QPointer<VerseTableWidget> w = new VerseTableWidget(this);
    setAll(w);
    w->setVerseTable(m_verseTable);
    w->init();
    w->exec();
    delete w;
}

void BibleForm::readChapter(int id)
{
    VerseUrlRange r;
    r.setOpenToTransformation(false);
    r.setModule(VerseUrlRange::LoadCurrentModule);
    r.setBook(VerseUrlRange::LoadCurrentBook);
    r.setChapter(id);
    r.setWholeChapter();

    VerseUrl url(r);
    m_actions->get(url);
}

void BibleForm::readBook(int id)
{
    const int i = m_bookIDs.at(id);

    VerseUrlRange r;
    r.setOpenToTransformation(false);
    r.setModule(VerseUrlRange::LoadCurrentModule);
    r.setBook(i);
    r.setChapter(VerseUrlRange::LoadFirstChapter);
    r.setWholeChapter();

    VerseUrl url(r);
    m_actions->get(url);
}
void BibleForm::zoomIn()
{
    m_view->setZoomFactor(m_view->zoomFactor() + 0.1);
}

void BibleForm::zoomOut()
{
    m_view->setZoomFactor(m_view->zoomFactor() - 0.1);
}

void BibleForm::setChapters(int bookID, QSharedPointer<Versification> v11n)
{
    if(v11n == NULL)
        return;
    const int count = v11n->maxChapter().value(bookID, 0);
    QStringList chapters;
    int add = 0;
    if(v11n->extendedData.hasChapterZero()) {
        add = -1;
    }
    for(int i = 1; i <= count; ++i) {
        chapters << QString::number(i + add);
    }
    bool same = true;
    if(m_ui->comboBox_chapters->count() == chapters.count()) {
        for(int i = 0; i < chapters.count(); ++i) {
            if(m_ui->comboBox_chapters->itemText(i) != chapters.at(i)) {
                same = false;
                break;
            }
        }
    } else {
        same = false;
    }
    if(!same) {
        m_ui->comboBox_chapters->clear();
        m_ui->comboBox_chapters->insertItems(0, chapters);
    }

}

void BibleForm::clearChapters()
{
    //DEBUG_FUNC_NAME
    m_ui->comboBox_chapters->clear();
}
void BibleForm::setCurrentChapter(const QSet<int> &chapterID)
{
    //DEBUG_FUNC_NAME
    //myDebug() << " windowID = " << m_id;
    //myDebug() << chapterID;
    disconnect(m_ui->comboBox_chapters, SIGNAL(activated(int)), this, SLOT(readChapter(int)));
    int min = -1;
    foreach(int c, chapterID) {
        if(c < min || min == -1)
            min = c;
    }
    //myDebug() << "min = " << min;
    m_ui->comboBox_chapters->setCurrentIndex(min);
    connect(m_ui->comboBox_chapters, SIGNAL(activated(int)), this, SLOT(readChapter(int)));
}

void BibleForm::setBooks(QSharedPointer<Versification> v11n)
{
    //DEBUG_FUNC_NAME
    bool same = true;
    QHash<int, QString> books = v11n->bookNames();
    QHashIterator<int, QString> i(books);
    int count = 0;
    if(m_ui->comboBox_books->count() == books.count()) {
        while(i.hasNext()) {
            i.next();
            if(m_ui->comboBox_books->itemText(count) != i.value()) {
                same = false;
                break;
            }
            ++count;
        }
    } else {
        same = false;
    }
    if(!same) {
        m_ui->comboBox_books->clear();
        m_ui->comboBox_books->insertItems(0, books.values());
        m_bookIDs = books.keys();
    }

}
void BibleForm::clearBooks()
{
    //DEBUG_FUNC_NAME
    m_ui->comboBox_books->clear();
}
void BibleForm::setCurrentBook(const QSet<int> &bookID)
{
    //DEBUG_FUNC_NAME
    //todo: is there a better way then disconnect and connect?
    disconnect(m_ui->comboBox_books, SIGNAL(activated(int)), this, SLOT(readBook(int)));
    int min = -1;
    foreach(int b, bookID) {
        if(b < min || min == -1)
            min = b;
    }
    m_ui->comboBox_books->setCurrentIndex(m_verseTable->verseModule()->versification()->bookIDs().indexOf(min));
    connect(m_ui->comboBox_books, SIGNAL(activated(int)), this, SLOT(readBook(int)));
}
void BibleForm::forwardClear()
{
    if(!active())
        return;
    clear();
}
void BibleForm::clear()
{
    clearChapters();
    clearBooks();
    m_view->setHtml("");
    m_actions->setTitle("");
    m_verseTable->clear();
}

void BibleForm::activated()
{
    //DEBUG_FUNC_NAME
    //myDebug() << " windowID = " << m_id;
    m_api->moduleApi()->setFrame(m_view->page()->mainFrame());
    if(m_verseTable == NULL) {
        clearChapters();
        clearBooks();
        m_actions->setTitle("");
        m_verseTable = m_moduleManager->newVerseTable();
        return;
    } else if(m_verseTable->verseModule() == NULL) {
        m_moduleManager->newVerseModule(-1, QPoint(0, 0), m_verseTable);
        return;
    }

    if(m_verseTable->verseModule()->moduleID() < 0) {
        myWarning() << "invalid moduleID";
        clearChapters();
        clearBooks();
        m_actions->setTitle("");
        return;
    }


    m_actions->setTitle(m_verseTable->verseModule()->moduleTitle());
    m_actions->setCurrentModule(m_verseTable->verseModule()->moduleID());

    if(m_verseTable->verseModule()->lastTextRanges() != NULL) {
        m_actions->updateChapters(m_verseTable->verseModule()->lastTextRanges()->minBookID(), m_verseTable->verseModule()->versification());
        m_actions->updateBooks(m_verseTable->verseModule()->versification());

        if(m_lastTextRanges.verseCount() != 0 && !m_lastTextRanges.failed()) {
            m_actions->setCurrentChapter(m_lastTextRanges.chapterIDs());
            m_actions->setCurrentBook(m_lastTextRanges.bookIDs());
        }

        m_verseTable->setLastTextRanges(&m_lastTextRanges);
        m_verseTable->setLastUrl(m_lastUrl);
    }
}

void BibleForm::showText(const QString &text)
{
    //DEBUG_FUNC_NAME
    //myDebug() << " windowID = " << m_id;
    QWebFrame * frame = m_view->page()->mainFrame();
    {
        QString cssFile;
        if(m_moduleManager->verseTableLoaded(m_verseTable))
            cssFile = m_settings->getModuleSettings(m_verseTable->verseModule()->moduleID())->styleSheet;
        if(cssFile.isEmpty())
            cssFile = ":/data/css/default.css";
        m_view->settings()->setUserStyleSheetUrl(QUrl::fromLocalFile(cssFile));
    }
    //todo: often it isn't real html but some fragments and sometimes it's a whole html page
    //eg biblequote

    //this make it probably a bit better than append it every attachapi()
    //but does not work with biblequote see above

    /*QString showText;
    if(!text.contains("<html>")) {
        showText = "<html><head><script type='text/javascript' src='qrc:/data/js/tools.js'></script></head><body>"+text + "</body></html>";
    }*/

    m_view->setHtml(text);


    if(m_lastTextRanges.verseCount() > 1) {
        m_view->scrollToAnchor("currentEntry");
        if(m_verseTable->hasTopBar())
            frame->scroll(0, -40); //due to the biblelist bar on top
        //todo: it could be that the top bar has a width more than 40px
        //because the user zoomed in.
    }

    if(m_verseTable->verseModule()->moduleType() == OBVCore::BibleQuoteModule) {
        QWebElementCollection collection = frame->documentElement().findAll("img");
        const QStringList searchPaths = ((Bible*) m_verseTable->verseModule())->getSearchPaths();

        foreach(QWebElement paraElement, collection) {
            QString url = paraElement.attribute("src");
            if(url.startsWith(":/") || url.startsWith("http") || url.startsWith("qrc:/"))
                continue;
            foreach(const QString & pre, searchPaths) {
                QFileInfo i(pre + url);
                if(i.exists()) {
                    paraElement.setAttribute("src", "file://" + pre + url);
                    break;
                } else {
                    QDir d(pre);
                    QStringList list = d.entryList();
                    foreach(const QString & f, list) {
                        QFileInfo info2(f);
                        if(info2.baseName().compare(i.baseName(), Qt::CaseInsensitive) == 0) {
                            paraElement.setAttribute("src", "file://" + pre + f);
                            break;
                        }
                    }
                }
            }
        }
    }

}


void BibleForm::forwardShowTextRanges(const QString &html, const TextRanges &range, const VerseUrl &url)
{
    if(!active())
        return;
    showTextRanges(html, range, url);
}

void BibleForm::showTextRanges(const QString &html, const TextRanges &range, const VerseUrl &url)
{
    showText(html);
    m_lastTextRanges = range;
    m_lastUrl = url;
    m_verseTable->setLastTextRanges(&m_lastTextRanges);
    m_verseTable->setLastUrl(m_lastUrl);
    historySetUrl(url.toString());
}
void BibleForm::evaluateJavaScript(const QString &js)
{
    m_view->page()->mainFrame()->evaluateJavaScript(js);
}
void BibleForm::print()
{
    QPrinter printer;
    QPointer<QPrintDialog> dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print"));
    if(dialog->exec() == QDialog::Accepted) {
        m_view->page()->mainFrame()->print(&printer);
    }
    delete dialog;
}
void BibleForm::printPreview()
{
    QPrinter printer;
    QPointer<QPrintPreviewDialog> preview = new QPrintPreviewDialog(&printer, this);
    connect(preview, SIGNAL(paintRequested(QPrinter *)), m_view, SLOT(print(QPrinter *)));
    preview->exec();
    delete preview;
}

void BibleForm::saveFile()
{
    const QString lastPlace = m_settings->session.getData("lastSaveFilePlace").toString();
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save output"), lastPlace, tr("Open Document (*.odt);;PDF (*.pdf);;Html (*.html *.htm);;Plain (*.txt)"));
    QFileInfo fi(fileName);
    m_settings->session.setData("lastSaveFilePlace", fi.path());
    if(fi.suffix().compare("html", Qt::CaseInsensitive) == 0 ||
            fi.suffix().compare("htm", Qt::CaseInsensitive) == 0) {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        out << m_view->page()->mainFrame()->toHtml();
        file.close();
    } else if(fi.suffix().compare("pdf", Qt::CaseInsensitive) == 0) {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        m_view->print(&printer);
    } else if(fi.suffix().compare("odt", Qt::CaseInsensitive) == 0) {
        QTextDocumentWriter writer;
        writer.setFormat("odf");
        writer.setFileName(fileName);
        QTextDocument doc;
        doc.setHtml(m_view->page()->mainFrame()->toHtml());
        writer.write(&doc);
    } else {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream out(&file);
        out << m_view->page()->mainFrame()->toPlainText();
        file.close();
    }
}

QString BibleForm::selectedText()
{
    return m_view->selectedText();
}

void BibleForm::copy()
{
    m_view->page()->triggerAction(QWebPage::Copy);
}

void BibleForm::selectAll()
{
    m_view->page()->triggerAction(QWebPage::SelectAll);
}
void BibleForm::forwardSetChapters(int bookID, QSharedPointer<Versification> v11n)
{
    if(!active())
        return;
    setChapters(bookID, v11n);
}

void BibleForm::forwardSetBooks(QSharedPointer<Versification> v11n)
{
    if(!active())
        return;
    setBooks(v11n);
}

void BibleForm::forwardClearBooks()
{
    if(!active())
        return;
    clearBooks();
}

void BibleForm::forwardClearChapters()
{
    if(!active())
        return;
    clearChapters();
}

void BibleForm::forwardSetCurrentBook(const QSet<int> &bookID)
{
    if(!active())
        return;
    setCurrentBook(bookID);
}

void BibleForm::forwardSetCurrentChapter(const QSet<int> &chapterID)
{
    if(!active())
        return;
    setCurrentChapter(chapterID);
}

void BibleForm::forwardShowText(const QString &text)
{
    if(!active())
        return;
    showText(text);
}

void BibleForm::forwardSearchInText(SearchResult *res)
{
    if(!active())
        return;
    searchInText(res);
}



void BibleForm::searchInText(SearchResult *res)
{
    DEBUG_FUNC_NAME
    if(res == NULL)
        return;
    if(res->searchQuery.queryType == SearchQuery::Simple) {
        QString s = res->searchQuery.searchText;
        //todo: hacky
        s.remove('*');
        s.remove('?');
        m_view->findText(s, QWebPage::HighlightAllOccurrences);
    }
}

void BibleForm::createDefaultMenu()
{
    m_actionCopy = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/icons/16x16/edit-copy.png")), tr("Copy"), this);
    connect(m_actionCopy, SIGNAL(triggered()), this, SLOT(copy()));

    m_actionSelect = new QAction(QIcon::fromTheme("edit-select-all", QIcon(":/icons/16x16/edit-select-all.png")), tr("Select All"), this);
    connect(m_actionSelect, SIGNAL(triggered()), this , SLOT(selectAll()));

    m_menuMark = new QMenu(this);
    m_menuMark->setTitle(tr("Mark this"));
    m_menuMark->setIcon(QIcon::fromTheme("format-fill-color.png", QIcon(":/icons/16x16/format-fill-color.png")));

    QAction *actionYellowMark = new QAction(QIcon(":/icons/16x16/mark-yellow.png"), tr("Yellow"), m_menuMark);
    actionYellowMark->setObjectName("yellowMark");
    connect(actionYellowMark, SIGNAL(triggered()), this , SLOT(newColorMark()));
    m_menuMark->addAction(actionYellowMark);

    QAction *actionGreenMark = new QAction(QIcon(":/icons/16x16/mark-green.png"), tr("Green"), m_menuMark);
    actionGreenMark->setObjectName("greenMark");
    connect(actionGreenMark, SIGNAL(triggered()), this , SLOT(newColorMark()));
    m_menuMark->addAction(actionGreenMark);

    QAction *actionBlueMark = new QAction(QIcon(":/icons/16x16/mark-blue.png"), tr("Blue"), m_menuMark);
    actionBlueMark->setObjectName("blueMark");
    connect(actionBlueMark, SIGNAL(triggered()), this , SLOT(newColorMark()));
    m_menuMark->addAction(actionBlueMark);

    QAction *actionOrangeMark = new QAction(QIcon(":/icons/16x16/mark-orange.png"), tr("Orange"), m_menuMark);
    actionOrangeMark->setObjectName("orangeMark");
    connect(actionOrangeMark, SIGNAL(triggered()), this , SLOT(newColorMark()));
    m_menuMark->addAction(actionOrangeMark);

    QAction *actionVioletMark = new QAction(QIcon(":/icons/16x16/mark-violet.png"), tr("Violet"), m_menuMark);
    actionVioletMark->setObjectName("violetMark");
    connect(actionVioletMark, SIGNAL(triggered()), this , SLOT(newColorMark()));
    m_menuMark->addAction(actionVioletMark);

    QAction *actionCustomMark  = new QAction(QIcon(":/icons/16x16/format-fill-color.png"), tr("Custom Color"), m_menuMark);
    connect(actionCustomMark, SIGNAL(triggered()), this , SLOT(newCustomColorMark()));
    m_menuMark->addAction(actionCustomMark);
    m_menuMark->addSeparator();

    QAction *actionBoldMark  = new QAction(QIcon::fromTheme("format-text-bold", QIcon(":/icons/16x16/format-text-bold.png")), tr("Bold"), m_menuMark);
    connect(actionBoldMark, SIGNAL(triggered()), this , SLOT(newBoldMark()));
    m_menuMark->addAction(actionBoldMark);

    QAction *actionItalicMark  = new QAction(QIcon::fromTheme("format-text-italic", QIcon(":/icons/16x16/format-text-italic.png")), tr("Italic"), m_menuMark);
    connect(actionItalicMark, SIGNAL(triggered()), this , SLOT(newItalicMark()));
    m_menuMark->addAction(actionItalicMark);

    QAction *actionUnderlineMark  = new QAction(QIcon::fromTheme("format-text-underline", QIcon(":/icons/16x16/format-text-underline.png")), tr("Underline"), m_menuMark);
    connect(actionUnderlineMark, SIGNAL(triggered()), this , SLOT(newUnderlineMark()));
    m_menuMark->addAction(actionUnderlineMark);


    m_actionRemoveMark = new QAction(QIcon(":/icons/16x16/mark-yellow.png"), tr("Remove Mark"), this);
    connect(m_actionRemoveMark, SIGNAL(triggered()), this , SLOT(removeMark()));

    m_actionBookmark = new QAction(QIcon::fromTheme("bookmark-new", QIcon(":/icons/16x16/bookmark-new.png")), tr("Add Bookmark"), this);
    connect(m_actionBookmark, SIGNAL(triggered()), this, SLOT(newBookmark()));

    m_actionNote = new QAction(QIcon::fromTheme("view-pim-notes", QIcon(":/icons/16x16/view-pim-notes.png")), tr("Add Note"), this);
    connect(m_actionNote, SIGNAL(triggered()), this , SLOT(newNoteWithLink()));
}
void BibleForm::deleteDefaultMenu()
{
    delete m_actionCopy;
    m_actionCopy = 0;
    delete m_actionSelect;
    m_actionSelect = 0;
    delete m_menuMark;
    m_menuMark = 0;
    delete m_actionRemoveMark;
    m_actionRemoveMark = 0;
    delete m_actionBookmark;
    m_actionBookmark = 0;
    delete m_actionNote;
    m_actionNote = 0;
}
void BibleForm::showContextMenu(QContextMenuEvent* ev)
{
    //DEBUG_FUNC_NAME
    QScopedPointer<QMenu> contextMenu(new QMenu(this));

    QAction *actionCopyWholeVerse = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/icons/16x16/edit-copy.png")), tr("Copy Verse"), contextMenu.data());
    VerseSelection selection = verseSelection();
    lastSelection = selection;
    if(selection.startVerse != -1) {
        QString addText;
        if(selection.startVerse != selection.endVerse)
            addText = " " + QString::number(selection.startVerse + 1) + " - " + QString::number(selection.endVerse + 1);
        else
            addText = " " + QString::number(selection.startVerse + 1);
        if(selection.startVerse < 0 || selection.endVerse < 0) {
            actionCopyWholeVerse->setText(tr("Copy Verse"));
            actionCopyWholeVerse->setEnabled(false);
        } else {
            actionCopyWholeVerse->setText(tr("Copy Verse %1", "e.g Copy Verse 1-2 or Copy Verse 2").arg(addText));
            actionCopyWholeVerse->setEnabled(true);

            connect(actionCopyWholeVerse, SIGNAL(triggered()), this , SLOT(copyWholeVerse()));
        }
    } else {
        actionCopyWholeVerse->setEnabled(false);
    }

    QAction *dbg = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/icons/16x16/edit-copy.png")), tr("Debugger"), contextMenu.data());
    connect(dbg, SIGNAL(triggered()), this, SLOT(debugger()));

    contextMenu->addAction(m_actionCopy);
    contextMenu->addAction(actionCopyWholeVerse);
    contextMenu->addAction(m_actionSelect);
    contextMenu->addSeparator();
    contextMenu->addMenu(m_menuMark);
    contextMenu->addAction(m_actionRemoveMark);
    contextMenu->addSeparator();
    contextMenu->addAction(m_actionBookmark);
    contextMenu->addAction(m_actionNote);
    contextMenu->addAction(dbg);
    contextMenu->exec(ev->globalPos());
}
void BibleForm::newNoteWithLink()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }
    m_notesManager->newNoteWithLink(lastSelection, verseModule()->versification());
}
void BibleForm::newBookmark()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }
    m_bookmarksManager->newBookmark(lastSelection, verseModule()->versification());
}

void BibleForm::copyWholeVerse(void)
{
    //DEBUG_FUNC_NAME
    VerseSelection selection = lastSelection;
    if(selection.startVerse != -1) {
        Ranges ranges;
        if(selection.startChapterID == selection.endChapterID) {
            Range r;
            r.setModule(selection.moduleID);
            r.setBook(selection.bookID);
            r.setChapter(selection.startChapterID);
            r.setStartVerse(selection.startVerse);
            r.setEndVerse(selection.endVerse);
            ranges.addRange(r);
        } else {
            for(int i = selection.startChapterID; i <= selection.endChapterID; i++) {
                Range r;
                r.setModule(selection.moduleID);
                r.setBook(selection.bookID);
                r.setChapter(i);
                if(i == selection.startChapterID)
                    r.setStartVerse(selection.startVerse);
                else
                    r.setStartVerse(0);

                if(i == selection.endChapterID)
                    r.setEndVerse(selection.endVerse);
                else
                    r.setEndVerse(m_settings->getModuleSettings(selection.moduleID)->getV11n()->maxVerse().value(selection.bookID).at(selection.startChapterID));

                ranges.addRange(r);
            }
        }
        QString stext = m_verseTable->verseModule()->readRanges(ranges).join(" ");

        QTextDocument doc2;
        doc2.setHtml(stext);
        stext = doc2.toPlainText();
        RefText refText;
        refText.setSettings(m_settings);
        refText.setShowModuleName(true);
        //todo: new line on windows
        const QString text = refText.toString(ranges) + "\n" + stext;
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
    }
}

void BibleForm::debugger()
{
    //DEBUG_FUNC_NAME
    m_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebInspector *i = new QWebInspector;
    i->setPage(m_view->page());
    i->showNormal();

}

void BibleForm::newColorMark()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }
    const QString colorName = sender()->objectName();
    QColor color;
    if(colorName == "yellowMark") {
        color = QColor(255, 255, 0);
    } else if(colorName == "greenMark") {
        color = QColor(146, 243, 54);
    } else if(colorName == "blueMark") {
        color = QColor(77, 169, 243);
    } else if(colorName == "orangeMark") {
        color = QColor(243, 181, 57);
    } else if(colorName == "violetMark") {
        color = QColor(169, 102, 240);
    } else {
        color = QColor(255, 255, 0);
    }

    VerseSelection selection = lastSelection;
    m_notesManager->newCustomColorMark(selection, color, verseModule()->versification());
}

void BibleForm::newCustomColorMark()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }

    VerseSelection selection = lastSelection;
    const QColor color = QColorDialog::getColor(QColor(255, 255, 0), this);
    if(color.isValid()) {
        m_notesManager->newCustomColorMark(selection, color, verseModule()->versification());
    }
}

void BibleForm::newBoldMark()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }

    VerseSelection selection = lastSelection;
    m_notesManager->newBoldMark(selection, verseModule()->versification());
}

void BibleForm::newItalicMark()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }

    VerseSelection selection = lastSelection;
    m_notesManager->newItalicMark(selection, verseModule()->versification());
}

void BibleForm::newUnderlineMark()
{
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }

    VerseSelection selection = lastSelection;
    m_notesManager->newUnderlineMark(selection, verseModule()->versification());
}

void BibleForm::removeMark()
{
    //DEBUG_FUNC_NAME;
    if(!m_moduleManager->verseTableLoaded(m_verseTable)) {
        return;
    }
    VerseSelection selection = lastSelection;
    m_notesManager->removeMark(selection, verseModule()->versification());
}

VerseSelection BibleForm::verseSelection()
{
    QWebFrame *f = m_view->page()->mainFrame();
    VerseSelection s;
    if(!f)
        return s;
    myDebug() << "running verselection";

    f->evaluateJavaScript("var vS = new VerseSelection(); vS.getSelection();");
    s.moduleID = f->evaluateJavaScript("vS.moduleID;").toInt();
    s.bookID  = f->evaluateJavaScript("vS.bookID;").toInt();
    s.startChapterID = f->evaluateJavaScript("vS.startChapterID;").toInt();
    s.endChapterID = f->evaluateJavaScript("vS.endChapterID;").toInt();
    s.startVerse = f->evaluateJavaScript("vS.startVerse;").toInt();
    s.endVerse = f->evaluateJavaScript("vS.endVerse;").toInt();
    myDebug() << "start verse = " << s.startVerse << " end verse = " << s.endVerse;

    const QString startVerseText = m_lastTextRanges.getVerse(s.bookID, s.startChapterID, s.startVerse).data();
    QString endVerseText;
    if(s.startVerse != s.endVerse || s.startChapterID != s.endChapterID)
        endVerseText = m_lastTextRanges.getVerse(s.bookID, s.endChapterID, s.endVerse).data();
    else
        endVerseText = startVerseText;

    const QString selectedText = f->evaluateJavaScript("vS.selectedText;").toString();
    myDebug() << "selected text = " << selectedText;
    myDebug() << "startVerseText = " << startVerseText;
    myDebug() << "endVerseText = " << endVerseText;
    {
        QString sText;
        for(int i = 0; i < selectedText.size() - 1; i++) {
            sText += selectedText.at(i);
            const int pos = startVerseText.indexOf(sText);
            if(pos != -1 && startVerseText.lastIndexOf(sText) == pos) {
                s.shortestStringInStartVerse = sText;
                break;
            }
        }
        if(s.shortestStringInStartVerse.isEmpty() && s.startVerse != s.endVerse) {
            //find the last long string if the selection is over more than one verse long
            //todo: it isn't alway unique
            QString lastLongest = selectedText;
            int lastPos = -2;
            for(int i = selectedText.size() - 1; i > 0; i--) {
                const int pos = startVerseText.lastIndexOf(lastLongest);
                if(pos != -1) {
                    lastPos = pos;
                    break;
                }
                lastLongest.remove(i, selectedText.size());
            }
            //and shorten it
            sText.clear();
            for(int i = 0; i < selectedText.size() - 1; i++) {
                sText += selectedText.at(i);
                const int pos = startVerseText.lastIndexOf(sText);
                if(pos != -1 && lastPos == pos) {
                    s.shortestStringInStartVerse = sText;
                    break;
                }
            }


        }
        sText.clear();
        for(int i = 0; i < selectedText.size() - 1; i++) {
            sText.prepend(selectedText.at(selectedText.size() - i - 1));
            const int pos = endVerseText.indexOf(sText);
            if(pos != -1 && endVerseText.lastIndexOf(sText) == pos) {
                s.shortestStringInEndVerse = sText;
                break;
            }
        }
        if(s.shortestStringInEndVerse.isEmpty() && s.startVerse != s.endVerse) {
            //find the first longest string if the selection is over more than one verse long
            QString firstLongest = selectedText;
            int firstPos = -2;
            for(int i = 0; i < selectedText.size(); i++) {
                const int pos = endVerseText.indexOf(firstLongest);
                if(pos != -1) {
                    firstPos = pos;
                    break;
                }
                firstLongest.remove(0, 1);
            }
            //and shorten it
            sText.clear();
            for(int i = 0; i < selectedText.size() - 1; i++) {
                sText.prepend(selectedText.at(selectedText.size() - i - 1));
                const int pos = endVerseText.indexOf(sText);
                if(pos != -1 && firstPos == pos) {
                    s.shortestStringInEndVerse = sText;
                    break;
                }
            }
        }
        s.type = VerseSelection::ShortestString;
        if(s.shortestStringInStartVerse.isEmpty() || s.shortestStringInEndVerse.isEmpty()) {
            s.setCanBeUsedForMarks(false);
        } else {
            s.setCanBeUsedForMarks(true);
        }
    }
    myDebug() << s.shortestStringInStartVerse << s.shortestStringInEndVerse;
    //do not this stuff with BibleQuote because some modules have wired html stuff.
    if(s.canBeUsedForMarks() == false && m_verseTable->verseModule()->moduleType() != OBVCore::BibleQuoteModule) {
        //now the ultimative alogrithm
        f->evaluateJavaScript("var adVerseSelection = new AdVerseSelection();adVerseSelection.getSelection();");
        const QString startVerseText2 = f->evaluateJavaScript("adVerseSelection.startVerseText;").toString();

        const QString uniqueString = "!-_OPENBIBLEVIEWER_INSERT_-!";
        const int posOfInsert = startVerseText2.lastIndexOf(uniqueString);

        QString back = selectedText;
        QString longestString;
        for(int i = selectedText.size() - 1; i > 0; --i) {
            const int pos = startVerseText2.indexOf(back);
            if(pos != -1) {
                longestString = back;
                break;
            }
            back.remove(i, selectedText.size());
        }

        int count = 0;
        int currentPos = 0;
        while(true) {
            currentPos = startVerseText2.indexOf(longestString, currentPos + 1);
            if(currentPos > posOfInsert || currentPos == -1) {
                break;
            }
            count++;
        }
        s.type = VerseSelection::RepeatOfLongestString;
        s.repeat = count;
        s.longestString = longestString;
        if(!s.longestString.isEmpty())
            s.setCanBeUsedForMarks(true);
        //todo: end
        myDebug() << "longest = " << longestString << " count = " << count;

    }
    return s;
}

bool BibleForm::verseTableLoaded()
{
    return m_moduleManager->verseTableLoaded(m_verseTable);
}

SearchableModule * BibleForm::searchableModule() const
{
    return (SearchableModule*)(m_verseTable->verseModule());
}

VerseTable * BibleForm::verseTable() const
{
    return m_verseTable;
}

VerseModule * BibleForm::verseModule() const
{
    return m_verseTable->verseModule();
}
void BibleForm::moduleChanged(const int moduleID)
{
	if(!m_verseTable)
		return;

    if(m_verseTable->contains(moduleID)) {
        reload(true);
    }
}
void BibleForm::reload(bool full)
{
    const QPoint p = m_view->page()->mainFrame()->scrollPosition();
    if(full) {
        m_verseTable->clearData();
    }
    if(!m_url.ranges().isEmpty())
        pharseUrl(m_url);
    m_view->page()->mainFrame()->setScrollPosition(p);
}
void BibleForm::reloadIf(const VerseUrl &url)
{
	if(!m_verseTable || !m_verseTable->verseModule() || !m_verseTable->verseModule()->lastTextRanges()) {
		return;
	}

	VerseUrlRange r = url.ranges().first();
    if(m_verseTable->verseModule()->moduleID() == r.moduleID() && m_verseTable->verseModule()->lastTextRanges()->contains(r.bookID(), r.chapterID())) {
        reload(false);
    }
}

void BibleForm::changeEvent(QEvent *e)
{
    switch(e->type()) {
    case QEvent::LanguageChange:
        deleteDefaultMenu();
        createDefaultMenu();
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
