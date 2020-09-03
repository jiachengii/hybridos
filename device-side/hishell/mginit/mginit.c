///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
** $Id: mginit.c 558 2008-03-04 09:02:19Z wangjian $
**
** The mginit program of MiniGUI Demostration Envirionment.
**
** Copyright (C) 2003 ~ 2017 FMSoft (http://www.fmsoft.cn).
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>

#ifdef _MGRM_THREADS
    #error "*******************************************************************"
    #error "**** Your MiniGUI is configured as MiniGUI-Threads.            ****"
    #error "**** This program is a server of MiniGUI-Processes,            ****"
    #error "****   it can only run on MiniGUI-Processes,                   ****"
    #error "****   not on MiniGUI-Threads.                                 ****"
    #error "**** If you want to configure MiniGUI as MiniGUI-Processes,    ****"
    #error "****   please run `./configure --enable-procs'                 ****"
    #error "****   when configuring MiniGUI                                ****"
    #error "****   and build and reinstall MiniGUI.                        ****"
    #error "**** Note that this is not a fatal error.                      ****"
    #error "*******************************************************************"
#else

#include "../include/sysconfig.h"
#include "config.h"

static const char* new_del_client_info [] =
{
    NULL,
    "New comming in client: %s\n",
    "Disconnecting client: %s\n"
};

static void on_new_del_client (int op, int cli)
{
    static int nr_clients = 0;
    MG_Client* client = mgClients + cli;

    if (op > 0 && op <= LCO_DEL_CLIENT)
        printf (new_del_client_info [op], client?client->name:"NULL");

    if (op == LCO_NEW_CLIENT) {
        nr_clients ++;
    }
    else if (op == LCO_DEL_CLIENT) {
        nr_clients --;
        if (nr_clients == 0) {
#if 0
            SendMessage (hStatusBar, MSG_CLOSE, 0, 0);
#else
            printf ("There is no any client.\n");
#endif
        }
        else if (nr_clients < 0) {
            printf ("Serious error: nr_clients less than zero.\n");
        }
    }
    else
        printf ("Serious error: incorrect operations.\n");
}

static const char* change_layer_info [] =
{
        NULL,
        "New layer created: %s; client: %s\n",
        "Layer deleted: %s; client: %s\n",
        "New client joined layer: (%s, %s)\n",
        "Remove client from layer: (%s, %s)\n",
        "Change topmost layer to %s; client: %s\n",
        "Change active client: (%s, %s)\n",
};

static void adjust_boxes (int width, MG_Layer* layer)
{
    int left = _LEFT_BOXES;
    MG_Layer* cur_layer = mgLayers;

    while (cur_layer) {
        if (cur_layer != layer) {
            MoveWindow ((HWND)cur_layer->dwAddData, left, _MARGIN,
                            width, _HEIGHT_CTRL, TRUE);
            left += width;
        }

        cur_layer = cur_layer->next;
    }
}

static void on_change_topmost (MG_Layer* layer)
{
    MG_Layer* cur_layer = mgLayers;

    if (layer)
        SendMessage ((HWND)layer->dwAddData, BM_SETCHECK, BST_CHECKED, 0);

    while (cur_layer) {
        if (cur_layer != layer) {
            SendMessage ((HWND)cur_layer->dwAddData, BM_SETCHECK, 0, 0);
        }

        cur_layer = cur_layer->next;
    }
}

static void on_change_layer (int op, MG_Layer* layer, MG_Client* client)
{
    static int nr_boxes = 0;
    static int box_width = _MAX_WIDTH_LAYER_BOX;
    int new_width;

    if (op > 0 && op <= LCO_ACTIVE_CHANGED)
        printf (change_layer_info [op], layer?layer->name:"NULL", 
                        client?client->name:"NULL");

    switch (op) {
    case LCO_NEW_LAYER:
        nr_boxes ++;
        if (box_width * nr_boxes > _WIDTH_BOXES) {
            new_width = _WIDTH_BOXES / nr_boxes;
            if (new_width < _MIN_WIDTH_LAYER_BOX) {
                new_width = _MIN_WIDTH_LAYER_BOX;
            }

            if (new_width != box_width) {
                adjust_boxes (new_width, layer);
                box_width = new_width;
            }
        }
/*
        layer->dwAddData = (DWORD)CreateWindow (CTRL_BUTTON, layer->name,
                 WS_CHILD | WS_VISIBLE | BS_CHECKBOX | BS_PUSHLIKE | BS_CENTER,
                 _ID_LAYER_BOX,
                 _LEFT_BOXES + box_width * (nr_boxes - 1), _MARGIN,
                 box_width, _HEIGHT_CTRL, hStatusBar, (DWORD)layer);
*/
        break;

    case LCO_DEL_LAYER:
        DestroyWindow ((HWND)(layer->dwAddData));
        layer->dwAddData = 0;
        nr_boxes --; 
        if (box_width * nr_boxes < _WIDTH_BOXES) {
            if (nr_boxes != 0)
                new_width = _WIDTH_BOXES / nr_boxes;
            else
                new_width = _MAX_WIDTH_LAYER_BOX;

            if (new_width > _MAX_WIDTH_LAYER_BOX)
                new_width = _MAX_WIDTH_LAYER_BOX;

            adjust_boxes (new_width, layer);
            box_width = new_width;
        }
        break;

    case LCO_JOIN_CLIENT:
        break;
    case LCO_REMOVE_CLIENT:
        break;
    case LCO_TOPMOST_CHANGED:
        on_change_topmost (layer);
        break;
    case LCO_ACTIVE_CHANGED:
        break;
    default:
        printf ("Serious error: incorrect operations.\n");
    }
}

static void child_wait (int sig)
{
    int pid;
    int status;

    while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED (status))
            printf ("--pid=%d--status=%x--rc=%d---\n", 
                            pid, status, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf ("--pid=%d--signal=%d--\n", pid, WTERMSIG (status));
    }
}

static LRESULT HelloWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_CREATE:
            break;
        case MSG_TIMER:
            break;
        case MSG_PAINT:
            break;
        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            PostQuitMessage (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}


static int GetStatusBarZnode(int cli, int clifd, void* buff, size_t len)
{
printf("============== in GetStatusBarZnode.\n");
    return 0;
}

static int GetTopMostTitle(int cli, int clifd, void* buff, size_t len)
{
printf("============== in GetTopMostTitle.\n");
    return 0;
}

int MiniGUIMain (int args, const char* arg[])
{
    struct sigaction siga;
    MSG msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    siga.sa_handler = child_wait;
    siga.sa_flags  = 0;
    memset (&siga.sa_mask, 0, sizeof(sigset_t));
    sigaction (SIGCHLD, &siga, NULL);

    OnNewDelClient = on_new_del_client;
    OnChangeLayer = on_change_layer;

    if (!ServerStartup (0, 0, 0)) {
        fprintf (stderr, 
                 "Can not start the server of MiniGUI-Processes: mginit.\n");
        return 1;
    }

    if(!RegisterRequestHandler(GET_STATUSBAR_ZNODE_REQID, GetStatusBarZnode))
    {
        return 2;
    }

    if(!RegisterRequestHandler(GET_TITLE_REQID, GetTopMostTitle))
    {
        return 2;
    }

    CreateInfo.dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
//        WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_WINTYPE_GLOBAL;//WS_EX_NONE;
    CreateInfo.spCaption = "mginit";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = HelloWinProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = g_rcScr.right / 2;
    CreateInfo.by = g_rcScr.bottom / 2;
    CreateInfo.iBkColor = GetWindowElementPixelEx (HWND_NULL, HDC_SCREEN, WE_MAINC_THREED_BODY);
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_HIDE);

    while (GetMessage(&msg, hMainWnd)) {
//        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    MainWindowThreadCleanup (hMainWnd);


    return 0;
}

#endif

