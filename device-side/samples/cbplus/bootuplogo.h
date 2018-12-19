/*
** HFCL Samples - Samples for HybridOS Foundation Class Library
**
** Copyright (C) 2018 Beijing FMSoft Technologies Co., Ltd.
**
** This file is part of HFCL Samples.
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

#ifndef HFCL_SAMPLES_BOOTUPLOGO_H_
#define HFCL_SAMPLES_BOOTUPLOGO_H_

#include <hfcl/activity.h>
#include <hfcl/view.h>

using namespace hfcl;

class BootupLogo : public ControllerClient, public TimerEventListener {
public:
    BootupLogo (Controller* owner, int view_id, View* parent,
        HTData param1, HTData param2);
    virtual ~BootupLogo();

    DECLARE_VIEWCONTEXT

private:
#if 0
    bool onKey(int keyCode, KeyEvent* event);
    unsigned int onControllerCommand(unsigned int cmd_id,
        HTData param1, HTData param2);
#endif

    bool handleEvent(Event* event);

    void onMenuCommand(CustomEvent* event);
    bool onGifAnimateNotify(CustomEvent* event);

    GifAnimateView* m_animateview;
    int m_timer_id;
};

#endif /* HFCL_SAMPLES_BOOTUPLOGO_H_ */
