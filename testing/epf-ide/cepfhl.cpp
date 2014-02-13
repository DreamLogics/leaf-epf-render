#include "cepfhl.h"
#include <QVector>

CEPFHL::CEPFHL(QTextDocument* doc) : QSyntaxHighlighter(doc)
{
}

void CEPFHL::setMode(Mode m)
{
    m_Mode = m;
}

void CEPFHL::highlightBlock(const QString &text)
{
    switch (m_Mode) {
    case modeJS:
        return highlightJS(text);
        break;
    case modeXML:
        return highlightXML(text);
        break;
    case modeHTML:
        return highlightHTML(text);
        break;
    case modeCSS:
        return highlightCSS(text);
        break;
    default:
        break;
    }
}

void CEPFHL::highlightJS(const QString &text)
{
    QVector<HighlightStyle> styles;
    QVector<HighlightStyle> tagstyles;

    HighlightStyle style;

    QTextCharFormat commentstyle;
    commentstyle.setForeground(QColor(0,200,0));

    QTextCharFormat xmltagstyle;
    xmltagstyle.setForeground(QColor(120,140,255));

    QRegExp tagreg("<[ /]*[a-zA-Z0-9_-]+");

    //state indpendant

    style.format = QTextCharFormat();
    style.format.setFontWeight(QFont::Bold);
    style.format.setForeground(QColor(0,40,200));
    style.rule = tagreg;
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setFontWeight(QFont::Bold);
    style.format.setForeground(QColor(0,40,200));
    style.rule = QRegExp("[/]*>");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,0,0));
    style.rule = QRegExp("&[^;]+;");
    styles.append(style);

    //inside tag
    style.format = QTextCharFormat();
    style.format.setForeground(QColor(0,200,200));
    style.rule = QRegExp("[a-zA-Z0-9_-]+");
    tagstyles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(200,0,200));
    style.rule = QRegExp("\"[^\"]+\"");
    tagstyles.append(style);

    //inside comment


    //state independant styles
    for (int i=0;i<styles.size();i++)
    {
        int index;
        int offset = 0;
        QRegExp rg(styles[i].rule);
        while ((index=text.indexOf(rg,offset)) >= 0)
        {
            setFormat(index,rg.cap(0).size(),styles[i].format);
            offset = index+rg.cap(0).size();
        }
    }

    int state = previousBlockState();
    int marker=0;

    for (int i=0;i<text.size();i++)
    {
        if (state == InComment)
        {
            if (text.mid(i,3) == "-->")
            {
                i += 3;
                setFormat(marker,i-marker,commentstyle);
                marker = i;
                state = NormalState;
            }
        }
        else if (state == InXmlTag)
        {
            if (text[i] == '>')
            {
                i++;
                setFormat(marker,i-marker,xmltagstyle);
                marker = i;
                state = NormalState;
            }
        }
        else if (state == InTag)
        {
            if (text[i] == '>')
            {
                i++;
                QString intag = text.mid(marker,i-marker);
                int intagi;
                if ((intagi = tagreg.indexIn(intag)) != -1);
                    marker+=intagi+tagreg.cap(0).size();
                for (int n=0;n<tagstyles.size();n++)
                {
                    int index;
                    int offset = marker;
                    //int end = i-marker+;
                    QRegExp rg(tagstyles[n].rule);
                    while ((index=text.indexOf(rg,offset)) >= 0)
                    {
                        if (index > i)
                            break;
                        setFormat(index,rg.cap(0).size(),tagstyles[n].format);
                        offset = index+rg.cap(0).size();
                    }
                }
                marker = i;
                state = NormalState;
            }
        }
        else
        {
            if (text[i] == '<')
            {
                //stateless styles
                marker = i;
                if (text.mid(i,2) == "<?")
                    state = InXmlTag;
                else if (text.mid(i,4) == "<!--")
                    state = InComment;
                else
                    state = InTag;

            }
        }
    }

    if (state == InComment)
    {
        setFormat(marker,text.size()-marker,commentstyle);
    }


    setCurrentBlockState(state);
}

void CEPFHL::highlightXML(const QString &text)
{
    QVector<HighlightStyle> styles;
    QVector<HighlightStyle> tagstyles;

    HighlightStyle style;

    QTextCharFormat commentstyle;
    commentstyle.setForeground(QColor(0,200,0));

    QTextCharFormat xmltagstyle;
    xmltagstyle.setForeground(QColor(120,140,255));

    QTextCharFormat stringstyle;
    stringstyle.setForeground(QColor(200,0,200));

    QRegExp tagreg("<[ /]*[a-zA-Z0-9_-]+");

    //state indpendant

    style.format = QTextCharFormat();
    style.format.setFontWeight(QFont::Bold);
    style.format.setForeground(QColor(0,40,200));
    style.rule = tagreg;
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setFontWeight(QFont::Bold);
    style.format.setForeground(QColor(0,40,200));
    style.rule = QRegExp("[/]*>");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,0,0));
    style.rule = QRegExp("&[^;]+;");
    styles.append(style);

    //inside tag
    style.format = QTextCharFormat();
    style.format.setForeground(QColor(0,200,200));
    style.rule = QRegExp("[a-zA-Z0-9_-]+");
    tagstyles.append(style);

    //inside comment


    //state independant styles
    for (int i=0;i<styles.size();i++)
    {
        int index;
        int offset = 0;
        QRegExp rg(styles[i].rule);
        while ((index=text.indexOf(rg,offset)) >= 0)
        {
            setFormat(index,rg.cap(0).size(),styles[i].format);
            offset = index+rg.cap(0).size();
        }
    }

    int state = previousBlockState();
    int marker=0;

    for (int i=0;i<text.size();i++)
    {
        if (state == InComment)
        {
            if (text.mid(i,3) == "-->")
            {
                i += 3;
                setFormat(marker,i-marker,commentstyle);
                marker = i;
                state = NormalState;
            }
        }
        else if (state == InXmlTag)
        {
            if (text[i] == '>')
            {
                i++;
                setFormat(marker,i-marker,xmltagstyle);
                marker = i;
                state = NormalState;
            }
        }
        else if (state == InString)
        {
            if (text[i] == '"')
            {
                i++;
                setFormat(marker,i-marker,stringstyle);
                marker = i;
                state = InTag;
            }
        }
        else if (state == InTag)
        {
            if (text[i] == '>' || text[i] == '"')
            {
                if (text[i] == '"')
                    state = InString;
                else
                {
                    i++;
                    state = NormalState;
                }
                QString intag = text.mid(marker,i-marker);
                int intagi;
                if ((intagi = tagreg.indexIn(intag)) != -1);
                    marker+=intagi+tagreg.cap(0).size();
                for (int n=0;n<tagstyles.size();n++)
                {
                    int index;
                    int offset = marker;
                    //int end = i-marker+;
                    QRegExp rg(tagstyles[n].rule);
                    while ((index=text.indexOf(rg,offset)) >= 0)
                    {
                        if (index > i)
                            break;
                        setFormat(index,rg.cap(0).size(),tagstyles[n].format);
                        offset = index+rg.cap(0).size();
                    }
                }
                marker = i;
            }
        }
        else
        {
            if (text[i] == '<')
            {
                //stateless styles
                marker = i;
                if (text.mid(i,2) == "<?")
                    state = InXmlTag;
                else if (text.mid(i,4) == "<!--")
                    state = InComment;
                else
                    state = InTag;

            }
        }
    }

    if (state == InComment)
    {
        setFormat(marker,text.size()-marker,commentstyle);
    }
    else if (state == InXmlTag)
    {
        setFormat(marker,text.size()-marker,xmltagstyle);
    }
    else if (state == InTag)
    {
        QString intag = text.mid(marker);
        int intagi;
        if ((intagi = tagreg.indexIn(intag)) != -1);
            marker+=intagi+tagreg.cap(0).size();
        for (int n=0;n<tagstyles.size();n++)
        {
            int index;
            int offset = marker;
            QRegExp rg(tagstyles[n].rule);
            while ((index=text.indexOf(rg,offset)) >= 0)
            {
                setFormat(index,rg.cap(0).size(),tagstyles[n].format);
                offset = index+rg.cap(0).size();
            }
        }
    }

    setCurrentBlockState(state);

}

void CEPFHL::highlightHTML(const QString &text)
{
    QVector<HighlightStyle> styles;
    QVector<HighlightStyle> tagstyles;

    HighlightStyle style;

    QTextCharFormat commentstyle;
    commentstyle.setForeground(QColor(0,200,0));

    QTextCharFormat xmltagstyle;
    xmltagstyle.setForeground(QColor(120,140,255));

    QRegExp tagreg("<[ /]*[a-zA-Z0-9_-]+");

    //state indpendant

    style.format = QTextCharFormat();
    style.format.setFontWeight(QFont::Bold);
    style.format.setForeground(QColor(0,40,200));
    style.rule = tagreg;
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setFontWeight(QFont::Bold);
    style.format.setForeground(QColor(0,40,200));
    style.rule = QRegExp("[/]*>");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,0,0));
    style.rule = QRegExp("&[^;]+;");
    styles.append(style);

    //inside tag
    style.format = QTextCharFormat();
    style.format.setForeground(QColor(0,200,200));
    style.rule = QRegExp("[a-zA-Z0-9_-]+");
    tagstyles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(200,0,200));
    style.rule = QRegExp("\"[^\"]+\"");
    tagstyles.append(style);

    //inside comment


    //state independant styles
    for (int i=0;i<styles.size();i++)
    {
        int index;
        int offset = 0;
        QRegExp rg(styles[i].rule);
        while ((index=text.indexOf(rg,offset)) >= 0)
        {
            setFormat(index,rg.cap(0).size(),styles[i].format);
            offset = index+rg.cap(0).size();
        }
    }

    int state = previousBlockState();
    int marker=0;

    for (int i=0;i<text.size();i++)
    {
        if (state == InComment)
        {
            if (text.mid(i,3) == "-->")
            {
                i += 3;
                setFormat(marker,i-marker,commentstyle);
                marker = i;
                state = NormalState;
            }
        }
        else if (state == InXmlTag)
        {
            if (text[i] == '>')
            {
                i++;
                setFormat(marker,i-marker,xmltagstyle);
                marker = i;
                state = NormalState;
            }
        }
        else if (state == InTag)
        {
            if (text[i] == '>')
            {
                i++;
                QString intag = text.mid(marker,i-marker);
                int intagi;
                if ((intagi = tagreg.indexIn(intag)) != -1);
                    marker+=intagi+tagreg.cap(0).size();
                for (int n=0;n<tagstyles.size();n++)
                {
                    int index;
                    int offset = marker;
                    //int end = i-marker+;
                    QRegExp rg(tagstyles[n].rule);
                    while ((index=text.indexOf(rg,offset)) >= 0)
                    {
                        if (index > i)
                            break;
                        setFormat(index,rg.cap(0).size(),tagstyles[n].format);
                        offset = index+rg.cap(0).size();
                    }
                }
                marker = i;
                state = NormalState;
            }
        }
        else
        {
            if (text[i] == '<')
            {
                //stateless styles
                marker = i;
                if (text.mid(i,2) == "<?")
                    state = InXmlTag;
                else if (text.mid(i,4) == "<!--")
                    state = InComment;
                else
                    state = InTag;

            }
        }
    }

    if (state == InComment)
    {
        setFormat(marker,text.size()-marker,commentstyle);
    }
    else if (state == InXmlTag)
    {
        setFormat(marker,text.size()-marker,xmltagstyle);
    }
    else if (state == InTag)
    {
        QString intag = text.mid(marker);
        int intagi;
        if ((intagi = tagreg.indexIn(intag)) != -1);
            marker+=intagi+tagreg.cap(0).size();
        for (int n=0;n<tagstyles.size();n++)
        {
            int index;
            int offset = marker;
            QRegExp rg(tagstyles[n].rule);
            while ((index=text.indexOf(rg,offset)) >= 0)
            {
                setFormat(index,rg.cap(0).size(),tagstyles[n].format);
                offset = index+rg.cap(0).size();
            }
        }
    }

    setCurrentBlockState(state);
}

void CEPFHL::highlightCSS(const QString &text)
{

}
