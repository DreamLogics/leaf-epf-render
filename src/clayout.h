#ifndef CLAYOUT_H
#define CLAYOUT_H

#include <QString>

class CLayout
{
public:
    CLayout(QString id, int height, int width, QString style, QString platform, QString language);

    virtual QString getID();
    virtual int height();
    virtual int width();
    virtual QString getStyle();
    virtual QString platform();
    virtual QString language();

private:

    QString m_sID;
    int m_iHeight;
    int m_iWidth;
    QString m_sStyle;
    QString m_sPlatform;
    QString m_sLanguage;
};

#endif // CLAYOUT_H
