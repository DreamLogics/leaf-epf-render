/****************************************************************************
**
** LEAF EPF Render engine
** http://leaf.dreamlogics.com/
**
** Copyright (C) 2013 DreamLogics
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef CLAYOUT_H
#define CLAYOUT_H

#include <QString>

class CLayout
{
public:
    CLayout(QString id, int height, int width, QString style, QString platform, QString language);

    QString id();
    int height();
    int width();
    QString css();
    QString platform();
    QString language();

private:

    QString m_sID;
    int m_iHeight;
    int m_iWidth;
    QString m_sStyle;
    QString m_sPlatform;
    QString m_sLanguage;
};

#endif // CLAYOUT_H
