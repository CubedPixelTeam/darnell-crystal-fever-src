#include <PA9.h>
#include "player.h"
#include "all_gfx.h"
bool touchingGround = true;
u8 jumptimer = 0;
bool jump = false;
int upDistance = 0;
void APlayer::hflipSprite(bool flip){
	this->hflip = flip;
	if(flip == true) PA_SetRotsetNoAngle(1,0,-194,192);
	else PA_SetRotsetNoAngle(1,0,194,192);
}
u8 APlayer::checkCollisionRight(s32 x, s32 y, s8 speed){
	for(s8 i = -1; i < 31; i ++) {
		for(s8 px = 0; px < speed; px ++){
		u8 pixel = PA_EasyBgGetPixel(this->screenc,this->cbg,x + (22 + px), y + i + this->g);
		if(pixel != 0) return pixel;
		}
	}
	return 0;
}
u8 APlayer::checkCollisionLeft(s32 x, s32 y, s8 speed){
	for(s8 i = -1; i < 31; i ++) {
		for(s8 px = 0; px < speed; px ++){
			u8 pixel = PA_EasyBgGetPixel(this->screenc,this->cbg,x + (8 - px),y+i+this->g);
			if(pixel != 0) return pixel;
		}
	}
	return 0;
}
u8 APlayer::checkCollisionBottom(s32 x, s32 y){
	for(s8 i = 10; i < 21-this->speedcap; i ++) {
		u8 pixel = PA_EasyBgGetPixel(this->screenc,this->cbg,x+i,y);
		if(pixel != 0) return pixel;
		//else if(PA_EasyBgGetPixel(this->screenc,this->cbg,x+i,y) == 2) return 2;
	}
	return 0;
}
void APlayer::Create(u8 id, s32 x, s32 y,u8 c,u8 b,u8 t, u8 d, u8 ut){
    this->id = id;
	this->x = x;
	this->y = y;
	this->hflip = false;
	this->speedcap = 2;
	this->trampoline = t;
	this->death = d;
	this->utrampoline = ut;
	this->co = c;
	this->bp = b;
	this->idlekillswitch = false;
	this->triggerkillswitch = false;
	this->looser_grav = false;
	this->airdash_killswitch = true;
	PA_LoadSpritePal(1,id,(void*)dar_Pal);
	PA_CreateSprite(1,id,(void*)dar_Sprite,OBJ_SIZE_16X32,1,id,x,y);
	PA_SetSpriteDblsize(1,id,1);
	PA_SetSpriteRotEnable(1,id,0);
	PA_SetRotsetNoAngle(1,0,194,192);
	PA_StartSpriteAnim(1,this->id,1,4,15);
}
int distance = 0;
u16 idletimer = 0;

u8 APlayer::Move(){
	u8 pixel = checkCollisionRight(this->sx,this->sy + 1,this->speedcap);
	if(pixel != 0) this->speedcap = 2;
	if(Pad.Held.Right && ((pixel == 0) || (pixel == this->bp))) {
		this->hflipSprite(false);
		this->x += this->speedcap;
	}	
	else if(Pad.Held.Right && (pixel == this->death))return 1;

	pixel = checkCollisionLeft(this->sx,this->sy + 1,this->speedcap);
	if(pixel != 0) this->speedcap = 2;

	if(Pad.Held.Left && ((pixel == 0) || (pixel == bp))) {
		this->hflipSprite(true);
		this->x -= this->speedcap;
	}
	else if(Pad.Held.Left && (pixel == this->death)) return 1;
	
	if(Pad.Newpress.Left||Pad.Newpress.Right){
		PA_StartSpriteAnim(1,this->id,1,4,15);
		this->hflipSprite(Pad.Newpress.Left);
	}
	if(Pad.Held.Y || Pad.Held.X)this->speedcap = 4;
	
	if(!(Pad.Held.Y || Pad.Held.X)||
	checkCollisionRight(this->sx,this->sy + 1,this->speedcap)!=0||
	checkCollisionLeft(this->sx,this->sy + 1,this->speedcap)!=0) this->speedcap = 2;

	if(!(Pad.Held.Right || Pad.Held.Left)) {
		PA_SpriteAnimPause(1,this->id,1);
		PA_SetSpriteAnimFrame(1,this->id,0);
	}
	this->hflipSprite(hflip);
	if(Pad.Held.Anykey) idletimer = 0;
	else if(!Pad.Held.Anykey && idlekillswitch == false) idletimer ++; 
	return 0;
}
u8 delay_timer = 0;
u16 idleanimtimer = 0;
bool idlesprite = false;
bool airdash = true; 
u8 airdash_timer = 0;
u8 APlayer::Gravity(){
	this->hflipSprite(hflip);
	u8 pixel = checkCollisionBottom(this->sx,this->sy + 32 + this->g);
	if(pixel == 0){
		PA_SpriteAnimPause(1,this->id,1);
		if(this->looser_grav == false) this->g += 0.4;
		else this->g += 0.2;
		if(delay_timer < 10) delay_timer ++;
		if(delay_timer < 9 && this->g > 0){
			if(Pad.Newpress.A || Pad.Newpress.B) {
				touchingGround = true;
				jumptimer = 10;
				this->g = -1;
				//mmEffect(SFX_JUMP);
				AS_SoundQuickPlay(jump_soundfx);
			}
		}
		PA_SetSpriteAnimFrame(1,this->id,5);
	}
	else if(pixel == this->co||pixel == this->death||
	(pixel == this->bp && this->g >= 0)){
		for(int i = 0; i < this->g; i ++){
			if(checkCollisionBottom(this->sx,this->sy + 32 + distance) == 0) distance ++;
		}
		this->y += distance;
		distance = 0;
		this->g = 0;
		delay_timer = 0;
		airdash = true;
		airdash_timer = 0;
		if(PA_GetSpriteAnimFrame(1,this->id) == 5) PA_StartSpriteAnim(1,this->id,1,4,15);
		
		if((PA_EasyBgGetPixel(this->screenc,this->cbg,this->sx + 25,this->sy + 32) == 0) && pixel == 1 
		&& !(Pad.Held.Right || Pad.Held.Left)) {
			PA_SetRotsetNoAngle(1,0,194,192);
			PA_SetSpriteAnimFrame(1,this->id,6);
			idletimer = 0;
		}
		else if((PA_EasyBgGetPixel(this->screenc,this->cbg,this->sx + 6,this->sy + 32) == 0) && pixel == 1 
		&& !(Pad.Held.Right || Pad.Held.Left)) {
			PA_SetRotsetNoAngle(1,0,-194,192);
			PA_SetSpriteAnimFrame(1,this->id,6);
			idletimer = 0;
		}
		
		if(idletimer > 300) {
			idleanimtimer ++;
			if(idleanimtimer > 30){
				idlesprite = !idlesprite;
				idleanimtimer = 0;
			}
			PA_SetSpriteAnimFrame(1,this->id,8-idlesprite);
		}
		if(Pad.Newpress.A || Pad.Newpress.B) {
		touchingGround = true;
			jumptimer = 0;
			this->g = -7;
			//mmEffect(SFX_JUMP);
			AS_SoundQuickPlay(jump_soundfx);
		}
	}
	if(checkCollisionBottom(this->sx,this->sy + 31)!= 0){
		for(int i = 0; i < 22; i ++){
			if(checkCollisionBottom(this->sx,this->sy+31+upDistance)!=0)upDistance --;
		}
		this->y += upDistance + 2;
		upDistance = 0;
	}

	if(pixel == this->trampoline) this->g = -13; 
	else if(pixel == this->death) return 1; 
	else if(pixel == this->utrampoline) this->g = -19; 
	
/*	if(pixel == 3) this->g = -13;
	else if(pixel == 4) return 1;
	else if(pixel == 5) this->g = -19;
*/
	pixel = checkCollisionBottom(this->sx,this->sy + this->g);
	if(pixel == this->co && pixel != this->death){
			for(int i = 0; i < this->g; i ++){
				if(checkCollisionBottom(this->sx,this->sy + 32 + distance) == 0) distance ++;
			}
		this->y += distance;
		distance = 0;
		this->g = 0;
		touchingGround = false;
    }

	if((Pad.Held.A || Pad.Held.B) && touchingGround == true){
		jumptimer ++;
		if(jumptimer < 12){
		this->g = -4;
		}
		else touchingGround = false;
	}
	else touchingGround = false;
	if(airdash_killswitch == false && airdash == true && (Pad.Held.X||Pad.Held.Y)){
		this->g = 0;
		airdash_timer ++;
		if(airdash_timer < 60) this->g = 0;
		else airdash = false;
	}
	if(this->g > 10) this->g = 10;
	this->y += this->g;
    return 0;
}
s32 player_bgx = 0;
s32 player_bgy = 0;
void APlayer::UpdateBg(){
	u8 xminus = 128;
	if(Pad.Held.R) xminus = 40;
	else if(Pad.Held.L) xminus = 190;
    player_bgx = this->x - xminus;
    player_bgy = this->y - 96;
}
void APlayer::Update(){
    this->sx = (this->x - player_bgx) - 4;
    this->sy = (this->y - player_bgy) - 4;
    PA_SetSpriteXY(1,this->id,this->sx,this->sy - 22);
    PA_EasyBgScrollXY(this->screenc,this->cbg,player_bgx,player_bgy);
}