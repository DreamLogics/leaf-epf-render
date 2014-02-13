#ifndef CEPFHL_H
#define CEPFHL_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>

class CEPFHL : public QSyntaxHighlighter
{
public:
    CEPFHL(QTextDocument* doc);

    enum Mode
    {
        modeJS=0,
        modeXML,
        modeHTML,
        modeCSS
    };

    void setMode(Mode m);

protected:
    enum State
    {
        NormalState = -1,
        InComment,
        InTag,
        InXmlTag,
        InString
    };

    virtual void highlightBlock(const QString &text);

private:

    struct HighlightStyle
    {
        QTextCharFormat format;
        QRegExp rule;
    };

    void highlightJS(const QString &text);
    void highlightXML(const QString &text);
    void highlightHTML(const QString &text);
    void highlightCSS(const QString &text);

private:
    Mode m_Mode;
};

#endif // CEPFHL_H
