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

#ifndef CSS_DEFAULT_H
#define CSS_DEFAULT_H

const char* pszDefaultCSS = "document"
        "{"
        "background-color: #ffffff;"
        "scale: 1;"
        "render-mode: normal;"
        "}"
        "section"
        "{"
        "position: fixed;"
        "opacity: 1.0;"
        "rotation: 0;"
        "scale: 0;"
        "render-mode: normal;"
        "section-transition: slide;"
        "}"
        "overlay object"
        "{"
        "position: static;"
        "opacity: 1.0;"
        "rotation: 0;"
        "scale: 1;"
        "render-mode: normal;"
        "}"
        "object"
        "{"
        "position: static;"
        "opacity: 1.0;"
        "rotation: 0;"
        "scale: 1;"
        "render-mode: normal;"
        "}";

#endif // CSS_DEFAULT_H
