/************************************
 
   Common Definitions header

   Contains definitions used by both
   Neko95 and NekoCFG applications

 *************************************/

#ifndef _NEKOCOMMON_H
#define _NEKOCOMMON_H

//class & window names
#define szNekoClassName   "Neko95Class"
#define szNekoWindowTitle "Neko95"

//registry key for Neko's settings
#define szNekoRegKey      "Software\\DHSoft\\Neko95"

//sub-key names
#define szNekoTaskbarKey        "Taskbar Icon"

#define szNekoSpeedKey          "Speed"
#define szNekoSenseKey          "Sensitivity"
#define szNekoLibraryKey        "Library"
#define szNekoSndIdle1Key       "Snd_Idle1"
#define szNekoSndIdle2Key       "Snd_Idle2"
#define szNekoSndIdle3Key       "Snd_Idle3"
#define szNekoSndSleepKey       "Snd_Sleep"
#define szNekoSndAwakeKey       "Snd_Awake"
#define szNekoSndFreqKey        "Sound Frequency"
#define szNekoScaleKey          "Size"
#define szNekoOnTopKey          "On Top"
#define szNekoFootprintLibKey   "Footprint Library"
#define szNekoFootprintKey      "Footprints"

#define szNekoActionKey         "Action"

#define szNekoNumCatsKey        "NumNekos"


//identifiers for different Neko actions
#define CHASE_MOUSE             0
#define RUN_AWAY_FROM_MOUSE     1
#define RUN_AROUND_RANDOMLY     2
#define PACE_AROUND_SCREEN      3
#define RUN_AROUND              4


//message sent from NekoCFG to Neko to signal settings have changed
#define MY_UPDATENEKO (WM_USER+33)

//maximum length of a neko 'Name'
#define MAX_NEKO_NAME (64)

#endif
