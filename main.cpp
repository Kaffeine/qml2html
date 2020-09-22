#include <QCoreApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDebug>
#include <QFile>

#include <iostream>

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#define GENERATOR_ENDL endl
#else
#define GENERATOR_ENDL Qt::endl
#endif

namespace Html {

class HtmlNode : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "data")
    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
public:
    QQmlListProperty<QObject> data();

    virtual QString getHtml() const;
    virtual QString tag() const { return QString(); }
    QString openTag() const;
    QString closeTag() const;

private:
    // data property
    static void data_append(QQmlListProperty<QObject> *, QObject *);
    static int data_count(QQmlListProperty<QObject> *);
    static QObject *data_at(QQmlListProperty<QObject> *, int);
    static void data_clear(QQmlListProperty<QObject> *);

    QList<HtmlNode*> m_nodes;
};

class Page : public HtmlNode
{
    Q_OBJECT
    Q_PROPERTY(QString title MEMBER m_title)
public:
    QString getHtml() const override;
    QString tag() const override { return QLatin1String("html"); }

protected:
    QString m_title;
};

class Text : public HtmlNode
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER m_text)
public:
    QString getHtml() const override;
protected:
    QString m_text;
};

class Heading : public Text
{
    Q_OBJECT
    Q_PROPERTY(int level MEMBER m_level)
public:
    QString tag() const override
    {
        return QStringLiteral("h%1").arg(m_level);
    }

protected:
    int m_level = 1;
};

class Paragraph : public Text
{
    Q_OBJECT
public:
    QString tag() const override { return QLatin1String("p"); }
};

class Button : public Text
{
    Q_OBJECT
public:
    QString tag() const override { return QLatin1String("button"); }
};

QQmlListProperty<QObject> HtmlNode::data()
{
    return QQmlListProperty<QObject>(this, nullptr,
                                     data_append,
                                     data_count,
                                     data_at,
                                     data_clear);
}

QString HtmlNode::getHtml() const
{
    QString html;
    for (const HtmlNode *node : m_nodes) {
        html += node->getHtml() + QLatin1Char('\n');
    }
    return html;
}

QString HtmlNode::openTag() const
{
    return QLatin1Char('<') + tag() + QLatin1Char('>');
}

QString HtmlNode::closeTag() const
{
    return QLatin1String("</") + tag() + QLatin1Char('>');
}

void HtmlNode::data_append(QQmlListProperty<QObject> *prop, QObject *o)
{
    if (!o) {
        return;
    }
    HtmlNode *that = static_cast<HtmlNode *>(prop->object);
    HtmlNode *newNode = qobject_cast<HtmlNode *>(o);
    that->m_nodes.append(newNode);
}

int HtmlNode::data_count(QQmlListProperty<QObject> *prop)
{
    HtmlNode *that = static_cast<HtmlNode *>(prop->object);
    return that->m_nodes.count();
}

QObject *HtmlNode::data_at(QQmlListProperty<QObject> *prop, int index)
{
    HtmlNode *that = static_cast<HtmlNode *>(prop->object);
    return that->m_nodes.at(index);
}

void HtmlNode::data_clear(QQmlListProperty<QObject> *prop)
{
    HtmlNode *that = static_cast<HtmlNode *>(prop->object);
    that->m_nodes.clear();
}

QString Page::getHtml() const
{
    QString result;
    QTextStream stream(&result);
    stream << openTag() << GENERATOR_ENDL;
    stream << "<head>" << GENERATOR_ENDL;
    stream << "<title>" << m_title << "</title>" << GENERATOR_ENDL;
    stream << "</head>" << GENERATOR_ENDL;
    stream << "<body>" << GENERATOR_ENDL;
    stream << HtmlNode::getHtml();
    stream << "</body>" << GENERATOR_ENDL;
    stream << closeTag() << GENERATOR_ENDL;
    return result;
}

QString Text::getHtml() const
{
    QString result;
    QTextStream stream(&result);
    stream << openTag();
    stream << m_text;
    stream << closeTag();
    return result;
}

} // Html namespace

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qmlRegisterType<Html::Page>("HTML", 1, 0, "Page");
    qmlRegisterType<Html::Heading>("HTML", 1, 0, "Heading");
    qmlRegisterType<Html::Paragraph>("HTML", 1, 0, "Paragraph");
    qmlRegisterType<Html::Button>("HTML", 1, 0, "Button");

    const QString input = a.arguments().count() > 1 ? a.arguments().at(1) : QLatin1String(":/index.qml");
    QQmlEngine engine;
    QQmlComponent *rootComponent = new QQmlComponent(&engine);
    rootComponent->loadUrl(QUrl::fromLocalFile(input), QQmlComponent::PreferSynchronous);

    if (rootComponent->isError()) {
        const QList<QQmlError> errorList = rootComponent->errors();
        for (const QQmlError &error : errorList) {
            QMessageLogger(error.url().toString().toLatin1().constData(),
                           error.line(), nullptr).warning()
                    << error;
        }
        return -1;
    }

    QObject *rootObject = rootComponent->create();
    Html::Page *page = qobject_cast<Html::Page*> (rootObject);

    std::cout << page->getHtml().toUtf8().constData();

    return 0;
}

#include "main.moc"
