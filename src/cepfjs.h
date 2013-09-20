#ifndef CEPFJS_H
#define CEPFJS_H

#include <QString>
#include <QObject>
#include <QScriptProgram>

class CDocument;
class QScriptEngine;

class CEPFJS : public QObject
{
    Q_OBJECT
public:
    CEPFJS(QString script, CDocument* doc);
    ~CEPFJS();

public slots:

    void run();

private:

    QString parseScript(QString script);

private:
    CDocument* m_pDocument;
    QScriptEngine* m_pEngine;
    QScriptProgram m_sScript;
};

#endif // CEPFJS_H
