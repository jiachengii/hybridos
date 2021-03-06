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


#ifndef HFCL_ACTIVITY_ACTIVITYMANAGER_H_
#define HFCL_ACTIVITY_ACTIVITYMANAGER_H_

#include "../mgcl/mgcl.h"
#include "../common/common.h"
#include "../common/event.h"
#include "../common/stlalternative.h"
#include "../activity/intent.h"
#include "../activity/activitystack.h"

namespace hfcl {

class BaseActivity;
class ActivityFactory;

/***
 * hook key msg from actManager
 *
 * @param MSG *msg - msg we get from msg queue
 * @return bool
 *      - GOON_DISPATCH continue message loop;
 *      - STOP_DISPATCH stop for process this msg.
 */
typedef bool (*KeyHookCallback)(MSG *msg);

class ActivityManager
{
public:
    MAPCLASSKEY(utf8string, ActivityFactory*, ActivityFactoryMap);
    PAIR(utf8string, ActivityFactory*, ActivityFactoryPair);

    ActivityManager() : m_hostingWnd(HWND_INVALID) { init(); }
    ~ActivityManager();

    static ActivityManager* getInstance();
    void run();

    BaseActivity* getCurrentActivity();
    BaseActivity* getTopActivity(unsigned int top = 0);
    BaseActivity* getActivityByName(const char * name);
    BaseActivity* getActivityFromFactory(utf8string name);
    ActivityFactory* getActivityFactory(utf8string name) {
        return m_acts[name];
    }
    int actNumOnRun() { return m_actstack.size(); }
    const ActivityFactoryMap& actlications() const { return m_acts; }

    void registerActivity(utf8string name, ActivityFactory *actFactory);

    bool actIsExist(BaseActivity *obj);
    bool actIsExist(const char * actName);

    void onBoot();
    BaseActivity* startActivity(utf8string act_name, Intent *intent);
    bool exit(BaseActivity* act);
    bool isExist(BaseActivity *act) { return m_actstack.isExist(act); }
    bool moveActivity2Top(const char * name);
    bool moveActivity2Bottom(const char * name);
    bool isActivityRunningInBackground(const char * name);
    bool pushActivityRunningInBackground(const char * name);
    BaseActivity* popActivityRunningToFrontdesk(const char * name);

    void changeSysLanguage(int langId);
    void addDisableLockFrameTick() { m_disableLockTick ++; }
    void subDisableLockFrameTick() {
        m_disableLockTick--;
        if (m_disableLockTick < 0)
            m_disableLockTick = 0;
    }
    int disableLockFrameTicks() { return m_disableLockTick; }

    HWND hosting() { return m_hostingWnd; }
    bool ActivityBeStarted(BaseActivity* act) {
        return m_actstack.isExist(act);
    }
    int broadcastMessage(int msg, int wParam, int lParam);

    void registerKeyMsgHook(KeyHookCallback callback) {
        m_key_hook = callback;
    }
    /***
     * hook key
     *
     * @param MSG *msg - msg we get from msg queue.
     *
     * @return bool - GOON_DISPATCH continue message loop,
     *                - STOP_DISPATCH stop for process this msg.
     ***/
    bool processKeyHook(MSG* msg);

    void startTimerService();
    void stopTimerService();

    void freezeChar(bool f) { m_charFreezon = f; }
    bool isCharFreezon() {return m_charFreezon; }

    int m_gValue;
    bool m_standby;

private:
    bool init();
    static LRESULT defaultHostingProc(HWND hWnd, UINT message,
            WPARAM wParam, LPARAM lParam);

    HWND m_hostingWnd;
    ActivityStack m_actstack;
    static ActivityManager* s_actManager;

    ActivityFactoryMap m_acts;
    KeyHookCallback m_key_hook;
    bool m_charFreezon;
    int  m_disableLockTick;
};

} // namespace hfcl

#endif /* HFCL_ACTIVITY_ACTIVITYMANAGER_H_ */
