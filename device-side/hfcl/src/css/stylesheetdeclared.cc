/*
** HFCL - HybridOS Foundation Class Library
**
** Copyright (C) 2018 Beijing FMSoft Technologies Co., Ltd.
**
** This file is part of HFCL.
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "css/stylesheetdeclared.h"

namespace hfcl {

bool StyleSheetDeclared::getProperty (PropertyIds pid, DWORD32 *value,
        HTData *data)
{
    PropertyValueMap::iterator it = m_map.find (pid);
    if (it == m_map.end ()) {
        return false;
    }

    if (value)
        *value = m_map [pid]->getValue();
    if (data)
        *data = m_map [pid]->getData();

    return true;
}

bool StyleSheetDeclared::setProperty (PropertyIds pid, DWORD32 value,
    HTData data)
{
    PropertyValue* pv;
    PropertyValueMap::iterator it = m_map.find (pid);

    if (it == m_map.end ()) {
        pv = HFCL_NEW_EX(PropertyValue, (value, data));
        m_map [pid] = pv;
        return true;
    }
    else {
        PropertyValue* pv = it->second;
        pv->setValue(value, data);
    }

    return false;
}

} // namespace hfcl
