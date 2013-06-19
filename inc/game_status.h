#ifndef __GAME_STATUS_H__
#define __GAME_STATUS_H__
extern unsigned long game_status;
extern int current_slide_start;
extern int current_tick_id;

#define SET_GAME_STATUS(__bits) (game_status |= (__bits))
#define RESET_GAME_STATUS(__bits) (game_status &= ~(__bits))

#define GAME_SF_ONSLIDER 0x1
#define GAME_SF_STRAY 0x2
#define GAME_SF_MOUSE_DOWN 0x4
#endif
