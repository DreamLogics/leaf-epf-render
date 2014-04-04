#include "cepfhl.h"
#include <QVector>
#include <QDebug>

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

    HighlightStyle style;


    QTextCharFormat commentstyle;
    commentstyle.setForeground(QColor(0,200,0));

    QTextCharFormat stringstyle;
    stringstyle.setForeground(QColor(200,0,200));

    //state indpendant

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,0,0));
    style.rule = QRegExp("&[^;]+;");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(20,100,200));
    style.rule = QRegExp("\\b(abstract|boolean|break|byte|case|catch|char|class|const|continue|debugger|default|delete|do|double|else|enum|export|extends|false|final|finally|float|for|function|goto|if|implements|import|in|instanceof|int|interface|long|native|new|null|package|private|protected|public|return|short|static|super|switch|synchronized|this|throw|throws|transient|true|try|typeof|var|void|volatile|while|with)\\b");
    styles.append(style);

    style.format = commentstyle;
    style.rule = QRegExp("^[ \t]*//.*");
    styles.append(style);


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
            if (text.mid(i,2) == "*/")
            {
                i += 2;
                setFormat(marker,i-marker,commentstyle);
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
        else
        {
            if (text[i] == '"')
            {
                marker = i;
                state = InString;
            }
            else if (text.mid(i,2) == "/*")
            {
                //stateless styles
                marker = i;
                state = InComment;

            }

        }
    }

    if (state == InComment)
    {
        setFormat(marker,text.size()-marker,commentstyle);
    }
    else if (state == InString)
    {
        setFormat(marker,text.size()-marker,stringstyle);
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
                        if (index >= i)
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
    else if (state == InString)
    {
        setFormat(marker,text.size()-marker,stringstyle);
    }

    setCurrentBlockState(state);

}

void CEPFHL::highlightHTML(const QString &text)
{
    highlightXML(text);
}

void CEPFHL::highlightCSS(const QString &text)
{
    QVector<HighlightStyle> styles;

    HighlightStyle style;

    QStringList props = QStringList() << "ascent" << "azimuth" << "background-attachment"
                                      << "background-color" << "background-image" << "background-position"
                                      << "background-repeat" << "background" << "baseline" << "bbox"
                                      << "border-collapse" << "border-color" << "border-spacing" <<
                                         "border-style" << "border-top" << "border-right" << "border-bottom"
                                      << "border-left" << "border-top-color" << "border-right-color" <<
                                         "border-bottom-color" << "border-left-color" << "border-top-style"
                                      << "border-right-style" << "border-bottom-style" << "border-left-style"
                                      << "border-top-width" << "border-right-width" << "border-bottom-width"
                                      << "border-left-width" << "border-width" << "border" << "bottom"
                                      << "cap-height" << "caption-side" << "centerline" << "clear" <<
                                         "clip" << "color" << "content" << "counter-increment" <<
                                         "counter-reset" << "cue-after" << "cue-before" << "cue" <<
                                         "cursor" << "definition-src" << "descent" << "direction" <<
                                         "display" << "elevation" << "empty-cells" << "float" <<
                                         "font-size-adjust" << "font-family" << "font-size" <<
                                         "font-stretch" << "font-style" << "font-variant" <<
                                         "font-weight" << "font" << "height" << "left" << "letter-spacing"
                                      << "line-height" << "list-style-image" << "list-style-position" <<
                                         "list-style-type" << "list-style" << "margin-top" << "margin-right"
                                      << "margin-bottom" << "margin-left" << "margin" << "marker-offset" <<
                                         "marks" << "mathline" << "max-height" << "max-width" << "min-height"
                                      << "min-width" << "orphans" << "outline-color" << "outline-style" <<
                                         "outline-width" << "outline" << "overflow" << "padding-top" <<
                                         "padding-right" << "padding-bottom" << "padding-left" << "padding"
                                      << "page" << "page-break-after" << "page-break-before" <<
                                         "page-break-inside" << "pause" << "pause-after" << "pause-before"
                                      << "pitch" << "pitch-range" << "play-during" << "position" << "quotes"
                                      << "right" << "richness" << "size" << "slope" << "src" << "speak-header"
                                      << "speak-numeral" << "speak-punctuation" << "speak" << "speech-rate" <<
                                         "stemh" << "stemv" << "stress" << "table-layout" << "text-align" <<
                                         "top" << "text-decoration" << "text-indent" << "text-shadow" <<
                                         "text-transform" << "unicode-bidi" << "unicode-range" << "units-per-em"
                                      << "vertical-align" << "visibility" << "voice-family" << "volume" <<
                                         "white-space" << "widows" << "width" << "widths" << "word-spacing"
                                      << "x-height" << "z-index" << "render-mode" << "inner-glow" << "outer-glow" << "color-overlay" << "drop-shadow";

    QTextCharFormat commentstyle;
    commentstyle.setForeground(QColor(0,200,0));

    QTextCharFormat propstyle;
    propstyle.setForeground(QColor(20,100,200));

    QTextCharFormat stringstyle;
    stringstyle.setForeground(QColor(200,0,200));

    //state indpendant

    /*style.format = QTextCharFormat();
    style.format.setForeground(QColor(20,100,200));
    style.rule = QRegExp("\\b(ascent|azimuth|background-attachment|background-color|background-image|background-position|background-repeat|background|baseline|bbox|border-collapse|border-color|border-spacing|border-style|border-top|border-right|border-bottom|border-left|border-top-color|border-right-color|border-bottom-color|border-left-color|border-top-style|border-right-style|border-bottom-style|border-left-style|border-top-width|border-right-width|border-bottom-width|border-left-width|border-width|border|bottom|cap-height|caption-side|centerline|clear|clip|color|content|counter-increment|counter-reset|cue-after|cue-before|cue|cursor|definition-src|descent|direction|display|elevation|empty-cells|float|font-size-adjust|font-family|font-size|font-stretch|font-style|font-variant|font-weight|font|height|left|letter-spacing|line-height|list-style-image|list-style-position|list-style-type|list-style|margin-top|margin-right|margin-bottom|margin-left|margin|marker-offset|marks|mathline|max-height|max-width|min-height|min-width|orphans|outline-color|outline-style|outline-width|outline|overflow|padding-top|padding-right|padding-bottom|padding-left|padding|page|page-break-after|page-break-before|page-break-inside|pause|pause-after|pause-before|pitch|pitch-range|play-during|position|quotes|right|richness|size|slope|src|speak-header|speak-numeral|speak-punctuation|speak|speech-rate|stemh|stemv|stress|table-layout|text-align|top|text-decoration|text-indent|text-shadow|text-transform|unicode-bidi|unicode-range|units-per-em|vertical-align|visibility|voice-family|volume|white-space|widows|width|widths|word-spacing|x-height|z-index|section-transition|background-gradient|color-overlay|text-source)\\b");
    styles.append(style);*/

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(200,0,0));
    style.rule = QRegExp("!(scale|important) *;");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,0,200));
    style.rule = QRegExp("\\$[a-zA-Z0-9_-]+");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(200,0,100));
    style.rule = QRegExp("\\@[a-zA-Z0-9_-]+");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,100,200));
    style.rule = QRegExp("\\#[a-zA-Z0-9_-]+");
    styles.append(style);

    style.format = QTextCharFormat();
    style.format.setForeground(QColor(100,200,200));
    style.rule = QRegExp("\\::[a-zA-Z0-9_-]+");
    styles.append(style);

    //find props

    for (int i=0;i<text.size();i++)
    {
        if (i == 0 || !(text[i-1].category() == QChar::Punctuation_Dash || text[i-1].category() == QChar::Letter_Lowercase) )
        {
            QString word;
            while (i<text.size())
            {
                const QChar c=text[i];
                if (!(c.category() == QChar::Punctuation_Dash || c.category() == QChar::Letter_Lowercase))
                {
                    //qDebug() << word;
                    if (props.contains(word))
                    {
                        setFormat(i-word.size(),word.size(),propstyle);
                    }
                    break;
                }
                word += c;
                i++;
            }
        }
    }


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
            if (text.mid(i,2) == "*/")
            {
                i += 2;
                setFormat(marker,i-marker,commentstyle);
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
        else
        {
            if (text[i] == '"')
            {
                marker = i;
                state = InString;
            }
            else if (text.mid(i,2) == "/*")
            {
                //stateless styles
                marker = i;
                state = InComment;

            }

        }
    }

    if (state == InComment)
    {
        setFormat(marker,text.size()-marker,commentstyle);
    }
    else if (state == InString)
    {
        setFormat(marker,text.size()-marker,stringstyle);
    }

    setCurrentBlockState(state);

}

CEPFHL::Mode CEPFHL::mode()
{
    return m_Mode;
}
