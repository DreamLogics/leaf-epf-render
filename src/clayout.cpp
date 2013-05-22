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

#include "clayout.h"

CLayout::CLayout(QString id, int height, int width, QString style, QString platform, QString language) : m_sID(id), m_iHeight(height),
    m_iWidth(width), m_sStyle(style), m_sPlatform(platform), m_sLanguage(language)
{

}

QString CLayout::id()
{
    return m_sID;
}

int CLayout::height()
{
    return m_iHeight;
}

int CLayout::width()
{
    return m_iWidth;
}

QString CLayout::getStyle()
{
    return m_sStyle;
}

QString CLayout::platform()
{
    return m_sPlatform;
}

QString CLayout::language()
{
    return m_sLanguage;
}
