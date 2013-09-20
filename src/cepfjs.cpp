#include "cepfjs.h"
#include <QScriptEngine>
#include "cdocument.h"

CEPFJS::CEPFJS(QString script, CDocument* doc) : m_pDocument(doc)
{
    m_pEngine = new QScriptEngine();

    QScriptValue docobj = m_pEngine->newQObject(doc);
    m_pEngine->globalObject().setProperty("document",docobj);

    m_sScript = QScriptProgram(parseScript(script),"main.js");
}

CEPFJS::~CEPFJS()
{
    delete m_pEngine;
}

void CEPFJS::run()
{
    m_pEngine->evaluate(m_sScript);
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
