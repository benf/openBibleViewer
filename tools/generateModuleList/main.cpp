#include <QtCore/QCoreApplication>
#include <QDomDocument>
#include <QFile>
#include <iostream>
#include <QtDebug>
using namespace std;
void readSourceforge()
{
    QDomDocument doc;
    QFile file("doc.xml");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "connot open file";
        return;
    }
    QString error;
    int line; int c;
    if(!doc.setContent(&file, true, &error, &line, &c)) {
        qDebug() << "cannot set content" << error  << line << c;
        file.close();
        return;
    }
    file.close();
    QFile outFile("modules.xml");
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&outFile);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    out << "<MODULES version=\"0.1\">\n";
    out << "<ID typ=\"bible\">\n";

    qDebug() << "file loaded";
    int counter = 0;
    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) { // all tr nodes
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            //cout << qPrintable(e.tagName()) << endl; // the node really is an element.
        }
        if(e.hasChildNodes()) {
            QDomNode n2 = e.firstChild();
            bool pharseIt = false;
            QString link = "";
            QString name = "";
            QString size = "";
            QString lang = "";

            while(!n2.isNull()) { // all tr nodes
                QDomElement e2 = n2.toElement(); // try to convert the node to an element.
                if(!e2.isNull()) {
                    if(e2.attribute("class") == "tree")
                        pharseIt = true;
                    if(pharseIt) {
                        if(e2.attribute("class") == "tree") {
                            QDomElement e3 = e2.firstChild().toElement();
                            link = e3.attribute("class");
                            name = e3.attribute("title");
                        } else if(e2.text().contains("MB") || e2.text().contains("KB")) {
                            size = e2.text();
                        }
                    }
                }
                if(pharseIt == false)
                    break;
                n2 = n2.nextSibling();
            }
            // qDebug() << "link = " << link << " name = " << name << " size = " << size;
            if(link.size() > 0) {
                counter++;
                QString url = link.remove(0, link.lastIndexOf(" { url:") + 9);
                url = url.remove(url.size() - 3, 4);
                QString a = name.remove(0, name.indexOf("/", 2) + 1);
                a = a.remove(a.lastIndexOf("/"), a.size());
                QString b = a;
                QString c = a;
                b = b.remove(0, b.indexOf("/") + 1);
                c = c.remove(c.indexOf("/"), c.size());
                name = b;
                lang = c;
                qDebug() << name << lang;
                out << "<ITEM link=\"" + url + "\" lang=\"" + lang + "\" size = \"" + size + "\">" + name + "</ITEM>\n";

            }
        }
        n = n.nextSibling();
    }
    out << "</ID>\n</MODULES>";
    qDebug() << "count = " << counter;
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "stating sf";
    readSourceforge();
    qDebug() << "ended";
    return a.exec();
}
