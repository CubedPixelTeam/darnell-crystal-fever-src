#include <PA9.h>
#include "all_gfx.h"
#include "fdz.h"
#include "player.h"
#include "objects.h"
#include "imz.h"

static ACrystal Crystals[60];
static APlayer Player;
static ACannon Cannon[9];
static u8 crystal_counter = 0;
static int timerms = 0;
static int timer = 0;

static int death(){
    PA_CFadeOutSys();
    crystal_counter = 0;
    Player.x = 30;
    Player.y = 420;
    Player.g = 0;
    Player.UpdateBg();
    Player.Update();
    timer = 0;
    timerms =0;
    for(u8 i = 0; i <= 59; i ++) {
        Crystals[i].ReActivate();
        if(i <= 8) Cannon[i].ReActivate();
    }
    PA_CFadeInSys();
    return 0;
}
u8 imz2Func(){
    PA_ResetAll();
    PA_SetBrightness(1,-31);
    PA_SetBrightness(0,-31);
    PA_InitText(1,0);
    PA_ResetAll();
    FAT_LoadBackground(1,2,1344, 480,PA_BgLarge,"data/graphics/levels/bgs/lp/lp2");
    FAT_LoadBackground(0,2,1344, 480,PA_BgLarge,"data/graphics/levels/bgs/lp/lp2c");
    FAT_LoadBackground(0,1,256, 192,PA_BgNormal,"nitro:/data/graphics/TitleScreen2");
    
    PA_HideBg(0,2);
    PA_InitText(0,0);
    Player.Create(0,30,420,1,3,4,2,5);
    Player.cbg = 2;
    Player.screenc = 0;
    Player.UpdateBg();
    Player.Update();
    AS_MP3StreamPlay("nitro:/data/music/cc.mp3");
    PA_CFadeInSys();
    while(1){
        if(Player.Move())death();
        if(Player.Gravity()) death();
        Player.UpdateBg();
        if(player_bgx < 0) player_bgx = 0;
        else if(player_bgx > 1088) player_bgx = 1088;
        if(player_bgy < 0) player_bgy = 0;
        else if(player_bgy > 288) player_bgy = 288;
        Player.Update();

        if(Player.y > 840) death();
        if(Player.x > 1276) {
            PA_CFadeOutSys();
            imStoryFunc();
            return 0;
        }
        PA_EasyBgScrollXY(1,2,PA_BgInfo[0][2].ScrollX,PA_BgInfo[0][2].ScrollY);
        timerms ++;
        if(timerms >= 60){
            timer ++;
            timerms = 0;
        }
        if(Pad.Newpress.Start) {
            PA_WaitForVBL();
            PA_WaitFor(Pad.Newpress.Start);
        }
        PA_ClearTextBg(0);
        PA_OutputText(0,10,10,"Crystals: %d\n\n\nTimer:%d ",crystal_counter,timer);
        PA_WaitForVBL();
    }
    return 0;
}