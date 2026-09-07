// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

#include "doomtype.h"
#include "doomdef.h"
#include "doomstat.h"
#include "d_ticcmd.h"
#include <stdarg.h>
#include <time.h>

extern player_t players[4];
extern int consoleplayer;
extern void M_QuitResponse(int ch);

extern char con_buffer[128][64];
extern int  con_head;
extern int  con_scroll;

void Con_Execute(char* cmd) {
	char buffer[80];
	strncpy(buffer, cmd, sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	char* token = strtok(buffer, " ");
	if(!token) return;

	// COMMANDS SECTION
	if(!strcmp(token, "god")) {
		players[consoleplayer].cheats ^= CF_GODMODE;
		con_print("Degreelessless mode toggled!");
	}
	else if(!strcmp(token, "noclip")) {
		players[consoleplayer].cheats ^= CF_NOCLIP;
		con_print("no clip toggled");
	}
	else if (strcmp(token, "map") == 0) {
        char* map_arg = strtok(NULL, " ");
        if (map_arg) {

            int ep = 1;
            int map = 1;
            int valid_format = 0;

            if (gamemode == commercial) { // DOOM 2 MODE: Reject e#m# formats
                if ((map_arg[0] == 'e' || map_arg[0] == 'E') && 
                    (map_arg[2] == 'm' || map_arg[2] == 'M')) {
                    con_print("Error: Doom 2 uses 'map <number>' format (e.g., map01 or 1).");
                } 
                else {
                    // Accept map## or raw ##
                    if (strncmp(map_arg, "map", 3) == 0 || strncmp(map_arg, "MAP", 3) == 0) {
                        map = atoi(&map_arg[3]);
                    } else {
                        map = atoi(map_arg);
                    }
                    valid_format = 1;
                }
            } 
            else if (gamemode == shareware || gamemode == registered || gamemode == retail) {
                // DOOM 1 MODE: Require e#m# format
                // Added a strlen check to prevent out-of-bounds reads on short typos like "e1"
                if ((map_arg[0] == 'e' || map_arg[0] == 'E') && 
                    (map_arg[2] == 'm' || map_arg[2] == 'M') &&
                    strlen(map_arg) >= 4) {
                    
                    ep = map_arg[1] - '0';
                    map = map_arg[3] - '0';
                    valid_format = 1;
                } 
                else {
                    con_print("Error: Doom 1 uses 'e#m#' format (e.g., e1m1).");
                }
            } 
            else {
                con_print("Error: Cannot warp, indetermined game mode.");
            }

            // Only queue the level change if the syntax matched the current IWAD
            if (valid_format) {
                char map_msg[64];
                snprintf(map_msg, sizeof(map_msg), "Warping to %s...", map_arg);
                con_print(map_msg);

                extern skill_t gameskill;
                G_DeferedInitNew(gameskill, ep, map);

                // con_active = 0;  // Optional: Automatically close the console
            }
        } else {
            // Update the generic help text to reflect that both formats exist
            con_print("Usage: map <mapname> (e.g., e1m1 for Doom 1, map01 for Doom 2)");
        }
    }
	else if(!strcmp(token, "exit")) {
		M_QuitResponse('y');
	}
	else if(!strcmp(token, "killall")) {
		extern int numsectors;
		extern sector_t* sectors;
		int kill_count = 0;

		// Iterate through every sector in the map
		for (int i = 0; i < numsectors; i++) {
			mobj_t* mo = sectors[i].thinglist;
			
			// Traverse the linked list of THINGS in this sector
			while (mo) {
				// Check if the entity counts as a kill (monsters) and is currently alive
				if ((mo->flags & MF_COUNTKILL) && mo->health > 0) {
					
					// Apply massive damage. 
					// The console player is passed as the inflictor and source so you get credit.
					P_DamageMobj(mo, players[consoleplayer].mo, players[consoleplayer].mo, 10000);
					kill_count++;
				}
				mo = mo->snext;
			}
		}

		// Output the result to the console
		char msg[64];
		snprintf(msg, sizeof(msg), "Killed %d monsters", kill_count);
		con_print(msg);
	}
	else if(!strcmp(token, "clear")) {
		for(int i = 0; i < 128; i++) {
			con_buffer[i][0] = '\0';
		}
		con_scroll = 0;
	}
	else if(!strcmp(token, "quit")) {
		I_Quit();
	}
	else {
		char cmd_nop[64];
		snprintf(cmd_nop, sizeof(cmd_nop), "Invalid/Undefined command: \"%s\"", token);
		con_print(cmd_nop);
	}
}

void I_Init (void)
{
    I_InitSound();
    I_InitMusic();
    I_InitGraphics();
}


int	mb_used = 6;

// Called by startup code
// to get the ammount of memory to malloc
// for the zone management.
byte*	I_ZoneBase (int *size)
{
    *size = mb_used*1024*1024;
    return (byte *) malloc (*size);
}



#ifndef _WIN32
#include <sys/time.h>
long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}
#endif


// Called by D_DoomLoop,
// returns current time in tics.
int I_GetTime (void)
{
    #ifdef _WIN32
        return clock()/(CLOCKS_PER_SEC/TICRATE);
    #else 
        return timeInMilliseconds()/(1000/TICRATE);
    #endif
}


//
// Called by D_DoomLoop,
// called before processing any tics in a frame
// (just after displaying a frame).
// Time consuming syncronous operations
// are performed here (joystick reading).
// Can call D_PostEvent.
//
void I_StartFrame (void)
{
}


//
// Called by D_DoomLoop,
// called before processing each tic in a frame.
// Quick syncronous operations are performed here.
// Can call D_PostEvent.
void I_StartTic (void)
{
    static int prev[ KEYCOUNT ] = { 0 };
    int keys[ KEYCOUNT ];
    for( int i = 0; i < KEYCOUNT; ++i ) keys[ i ] = keystate( i );

    for( int i = 0; i < KEYCOUNT; ++i ) 
    {
        int key = 0;

        switch( i )
        {
            case 97: key = KEY_F1; break;
            case 98: key = KEY_F2; break;
            case 99: key = KEY_F3; break;
            case 100: key = KEY_F4; break;
            case 101: key = KEY_F5; break;
            case 102: key = KEY_F6; break;
            case 103: key = KEY_F7; break;
            case 104: key = KEY_F8; break;
            case 105: key = KEY_F9; break;
            case 106: key = KEY_F10; break;
            case 107: key = KEY_F11; break;
            case 108: key = KEY_F12; break;
            case 8: key = KEY_TAB; break;
            case KEY_OEM_MINUS: key = KEY_MINUS; break;
            case KEY_OEM_PLUS: key = KEY_EQUALS; break;
            case KEY_BACK: key = KEY_BACKSPACE; break;
            case KEY_UP: key = KEY_UPARROW; break;
            case KEY_DOWN: key = KEY_DOWNARROW; break;
            case KEY_LEFT: key = KEY_LEFTARROW; break;
            case KEY_RIGHT: key = KEY_RIGHTARROW; break;
            case 20: key = KEY_ESCAPE; break;
            case KEY_SHIFT: key = KEY_RSHIFT; break;
            case KEY_CONTROL: key = KEY_RCTRL; break;
            case KEY_RMENU: key = KEY_RALT; break;
            case KEY_LMENU: key = KEY_LALT; break;
            case KEY_RETURN: key = KEY_ENTER; break;
			case 14: key = KEY_PAUSE; break;
		    case KEY_SPACE: key = ' '; break;
        }
        if( i >= KEY_A && i <= KEY_Z ) {
            key = 'a' + ( i - KEY_A );
        }
        if( i >= KEY_0 && i <= KEY_9 ) {
            key = '0' + ( i - KEY_0 );
        }

        if( keys[ i ] && !prev[ i ] )
            {
				if(i == KEY_OEM_3) { // tilde
					con_active = !con_active;
					prev[i] = keys[i];
					continue; // don't pass to game
				}

				if(con_active) {
					if(i == KEY_UP || i == KEY_PRIOR) {
						con_scroll += 5;
						if(con_scroll > CONSOLE_LINES - 20) con_scroll = CONSOLE_LINES - 20;
					}
					else if(i == KEY_DOWN || i == KEY_NEXT) {
						con_scroll -= 5;
						if (con_scroll < 0) con_scroll = 0;
					}
					else if(i == KEY_BACK || key == 127) {	
						if(con_cursor > 0)
							con_input[--con_cursor] = '\0';
					}
					else if(i == KEY_RETURN || key == 13) {
						if (con_cursor > 0) {
							con_print(con_input);
							Con_Execute(con_input); //pass to parser

							con_input[0] = '\0';
							con_cursor = 0;
							con_scroll = 0; // snap view to bottom
						}
					}
					else if(key >= ' ' && key <= 'z' && i != KEY_OEM_3) {
						if(con_cursor < 78) {
							con_input[con_cursor++] = (char)key;
							con_input[con_cursor] = '\0';
						}
					}

					prev[i] = keys[i];
					continue;
				}

				event_t ev;
				ev.type = ev_keydown;
				ev.data1 = key;
				D_PostEvent( &ev );
            }
        else if( !keys[ i ] && prev[ i ] )
            {       
            event_t ev;
            ev.type = ev_keyup;
            ev.data1 = key;
            D_PostEvent( &ev );
            }
        prev[ i ] = keys[ i ];
	}

    int relx = mouserelx();
    int rely = mouserely();

    event_t ev;
	ev.type = ev_mouse;
    ev.data1 = 0;
	ev.data2 = relx << 2;
	ev.data3 = -rely << 2;
    D_PostEvent( &ev );
}

// Asynchronous interrupt functions should maintain private queues
// that are read by the synchronous functions
// to be converted into events.

// Either returns a null ticcmd,
// or calls a loadable driver to build it.
// This ticcmd will then be modified by the gameloop
// for normal input.

ticcmd_t	emptycmd;

ticcmd_t* I_BaseTiccmd (void)
{
    return &emptycmd;
}


// Called by M_Responder when quit is selected.
// Clean exit, displays sell blurb.
void I_Quit (void)
{
    D_QuitNetGame ();
    I_ShutdownSound();
    I_ShutdownMusic();
    M_SaveDefaults ();
    I_ShutdownGraphics();

	remove("temp.mid");
    exit(0);
}



// Allocates from low memory under dos,
// just mallocs under unix
byte* I_AllocLow (int length)
{
    byte*	mem;
        
    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}

void I_Tactile (int on, int off, int total)
{
  // UNUSED.
  on = off = total = 0;
}


void I_Error (char *error, ...)
{
    va_list	argptr;

    // Message first.
    va_start (argptr,error);
    fprintf (stderr, "Error: ");
    vfprintf (stderr,error,argptr);
    fprintf (stderr, "\n");
    va_end (argptr);

    fflush( stderr );
    waitvbl();
    swapbuffers();
    waitvbl();

    // Shutdown. Here might be other errors.
    if (demorecording)
	G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();
    
    exit(-1);
}

