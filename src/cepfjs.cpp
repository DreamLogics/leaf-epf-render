#include "cepfjs.h"
#include <QScriptEngine>
#include "cdocument.h"
#include <QDebug>

static QScriptValue jsAlert(QScriptContext *context, QScriptEngine *engine)
{
    //qDebug() << "javascript:" << context->argument(0).toString();
    return QScriptValue();
}

CEPFJS::CEPFJS(QString script, CDocument* doc) : m_pDocument(doc)
{
    m_pEngine = new QScriptEngine();

    QScriptValue docobj = m_pEngine->newQObject(doc);
    m_pEngine->globalObject().setProperty("document",docobj);
    m_pEngine->globalObject().setProperty("alert",m_pEngine->newFunction(jsAlert));

    m_sScript = QScriptProgram(parseScript(script),"main.js");
}

CEPFJS::~CEPFJS()
{
    delete m_pEngine;
}

void CEPFJS::run()
{
    m_pEngine->evaluate(m_sScript);
    if (m_pEngine->hasUncaughtException())
    {
        //qDebug() << "javascript:" << m_pEngine->uncaughtExceptionBacktrace() << m_pEngine->uncaughtException().toString();
    }
}

QString CEPFJS::parseScript(QString script)
{
    //includes
    QString ns = script;
    QByteArray data;
    QString scr;
    int off=0;
    QRegExp incfinder("^[\\t ]*#include +\"([^\"]+)\"");
    int index = incfinder.indexIn(script,off);
    while (index!=-1)
    {
        data = m_pDocument->resource(incfinder.cap(1));
        scr = QString::fromUtf8(data);

        ns.replace(incfinder.cap(0),scr);

        off = index + incfinder.cap(1).size();
        index = incfinder.indexIn(script,off);
    }
    ns += "main();";
    return ns;
}
