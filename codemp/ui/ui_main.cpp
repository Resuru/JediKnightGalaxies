// Copyright (C) 1999-2000 Id Software, Inc.
//
/*
=======================================================================

USER INTERFACE MAIN

=======================================================================
*/

// use this to get a demo build without an explicit demo build, i.e. to get the demo ui files to build
//#define PRE_RELEASE_TADEMO

#include "../ghoul2/G2.h"
#include "ui_local.h"
#include "qcommon/qfiles.h"
#include "qcommon/game_version.h"
#include "ui_force.h"
#include "../cgame/animtable.h" //we want this to be compiled into the module because we access it in the shared module.
#include "../game/bg_saga.h"
#include "../game/jkg_gangwars.h"

#include "cgame/holocronicons.h"
// JKG
#include "jkg_ui_auxlib.h"
#include "jkg_conversations.h"
#include "jkg_pazaak.h"
#include "jkg_partymanager.h"
#include "jkg_slice.h"
#include "jkg_lootui.h"
#include "jkg_inventory.h"
#include "jkg_gangwars_ui.h"

#include "qcommon/game_version.h"

#include "..\cgame\holocronicons.h"

#define strnicmp Q_stricmpn

extern void UI_SaberAttachToChar( itemDef_t *item );

char *forcepowerDesc[NUM_FORCE_POWERS] = 
{
	"@MENUS_OF_EFFECT_JEDI_ONLY_NEFFECT",
	"@MENUS_DURATION_IMMEDIATE_NAREA",
	"@MENUS_DURATION_5_SECONDS_NAREA",
	"@MENUS_DURATION_INSTANTANEOUS",
	"@MENUS_INSTANTANEOUS_EFFECT_NAREA",
	"@MENUS_DURATION_VARIABLE_20",
	"@MENUS_DURATION_INSTANTANEOUS_NAREA",
	"@MENUS_OF_EFFECT_LIVING_PERSONS",
	"@MENUS_DURATION_VARIABLE_10",
	"@MENUS_DURATION_VARIABLE_NAREA",
	"@MENUS_DURATION_CONTINUOUS_NAREA",
	"@MENUS_OF_EFFECT_JEDI_ALLIES_NEFFECT",
	"@MENUS_EFFECT_JEDI_ALLIES_NEFFECT",
	"@MENUS_VARIABLE_NAREA_OF_EFFECT",
	"@MENUS_EFFECT_NAREA_OF_EFFECT",
	"@SP_INGAME_FORCE_SABER_OFFENSE_DESC",
	"@SP_INGAME_FORCE_SABER_DEFENSE_DESC",
	"@SP_INGAME_FORCE_SABER_THROW_DESC"
};

void JKG_Draw_Tooltip(itemDef_t *tooltip);

// Movedata Sounds
typedef enum
{
	MDS_NONE = 0,
	MDS_FORCE_JUMP,
	MDS_ROLL,
	MDS_SABER,
	MDS_MOVE_SOUNDS_MAX
};

typedef enum
{
	MD_ACROBATICS = 0,
	MD_SINGLE_FAST,
	MD_SINGLE_MEDIUM,
	MD_SINGLE_STRONG,
	MD_DUAL_SABERS,
	MD_SABER_STAFF,
	MD_MOVE_TITLE_MAX
};

// Some hard coded badness
// At some point maybe this should be externalized to a .dat file
char *datapadMoveTitleData[MD_MOVE_TITLE_MAX] =
{
	"@MENUS_ACROBATICS",
	"@MENUS_SINGLE_FAST",
	"@MENUS_SINGLE_MEDIUM",
	"@MENUS_SINGLE_STRONG",
	"@MENUS_DUAL_SABERS",
	"@MENUS_SABER_STAFF",
};

char *datapadMoveTitleBaseAnims[MD_MOVE_TITLE_MAX] =
{
	"BOTH_RUN1",
	"BOTH_SABERFAST_STANCE",
	"BOTH_STAND2",
	"BOTH_SABERSLOW_STANCE",
	"BOTH_SABERDUAL_STANCE",
	"BOTH_SABERSTAFF_STANCE",
};

#define MAX_MOVES 16

typedef struct 
{
	char	*title;	
	char	*desc;	
	char	*anim;
	short	sound;
} datpadmovedata_t;

static datpadmovedata_t datapadMoveData[MD_MOVE_TITLE_MAX][MAX_MOVES] = 
{
	// Acrobatics
	"@MENUS_FORCE_JUMP1",				"@MENUS_FORCE_JUMP1_DESC",			"BOTH_FORCEJUMP1",				MDS_FORCE_JUMP,
	"@MENUS_FORCE_FLIP",				"@MENUS_FORCE_FLIP_DESC",			"BOTH_FLIP_F",					MDS_FORCE_JUMP,
	"@MENUS_ROLL",						"@MENUS_ROLL_DESC",					"BOTH_ROLL_F",					MDS_ROLL,
	"@MENUS_BACKFLIP_OFF_WALL",			"@MENUS_BACKFLIP_OFF_WALL_DESC",	"BOTH_WALL_FLIP_BACK1",			MDS_FORCE_JUMP,
	"@MENUS_SIDEFLIP_OFF_WALL",			"@MENUS_SIDEFLIP_OFF_WALL_DESC",	"BOTH_WALL_FLIP_RIGHT",			MDS_FORCE_JUMP,
	"@MENUS_WALL_RUN",					"@MENUS_WALL_RUN_DESC",				"BOTH_WALL_RUN_RIGHT",			MDS_FORCE_JUMP,
	"@MENUS_WALL_GRAB_JUMP",			"@MENUS_WALL_GRAB_JUMP_DESC",		"BOTH_FORCEWALLREBOUND_FORWARD",MDS_FORCE_JUMP,
	"@MENUS_RUN_UP_WALL_BACKFLIP",		"@MENUS_RUN_UP_WALL_BACKFLIP_DESC",	"BOTH_FORCEWALLRUNFLIP_START",	MDS_FORCE_JUMP,
	"@MENUS_JUMPUP_FROM_KNOCKDOWN",		"@MENUS_JUMPUP_FROM_KNOCKDOWN_DESC","BOTH_KNOCKDOWN3",				MDS_NONE,
	"@MENUS_JUMPKICK_FROM_KNOCKDOWN",	"@MENUS_JUMPKICK_FROM_KNOCKDOWN_DESC","BOTH_KNOCKDOWN2",			MDS_NONE,
	"@MENUS_ROLL_FROM_KNOCKDOWN",		"@MENUS_ROLL_FROM_KNOCKDOWN_DESC",	"BOTH_KNOCKDOWN1",				MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,

	//Single Saber, Fast Style
	"@MENUS_STAB_BACK",					"@MENUS_STAB_BACK_DESC",			"BOTH_A2_STABBACK1",			MDS_SABER,
	"@MENUS_LUNGE_ATTACK",				"@MENUS_LUNGE_ATTACK_DESC",			"BOTH_LUNGE2_B__T_",			MDS_SABER,
	"@MENUS_FAST_ATTACK_KATA",			"@MENUS_FAST_ATTACK_KATA_DESC",		"BOTH_A1_SPECIAL",				MDS_SABER,
	"@MENUS_ATTACK_ENEMYONGROUND",		"@MENUS_ATTACK_ENEMYONGROUND_DESC", "BOTH_STABDOWN",				MDS_FORCE_JUMP,
	"@MENUS_CARTWHEEL",					"@MENUS_CARTWHEEL_DESC",			"BOTH_ARIAL_RIGHT",				MDS_FORCE_JUMP,
	"@MENUS_BOTH_ROLL_STAB",			"@MENUS_BOTH_ROLL_STAB2_DESC",		"BOTH_ROLL_STAB",				MDS_SABER,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,

	//Single Saber, Medium Style
	"@MENUS_SLASH_BACK",				"@MENUS_SLASH_BACK_DESC",			"BOTH_ATTACK_BACK",				MDS_SABER,
	"@MENUS_FLIP_ATTACK",				"@MENUS_FLIP_ATTACK_DESC",			"BOTH_JUMPFLIPSLASHDOWN1",		MDS_FORCE_JUMP,
	"@MENUS_MEDIUM_ATTACK_KATA",		"@MENUS_MEDIUM_ATTACK_KATA_DESC",	"BOTH_A2_SPECIAL",				MDS_SABER,
	"@MENUS_ATTACK_ENEMYONGROUND",		"@MENUS_ATTACK_ENEMYONGROUND_DESC", "BOTH_STABDOWN",				MDS_FORCE_JUMP,
	"@MENUS_CARTWHEEL",					"@MENUS_CARTWHEEL_DESC",			"BOTH_ARIAL_RIGHT",				MDS_FORCE_JUMP,
	"@MENUS_BOTH_ROLL_STAB",			"@MENUS_BOTH_ROLL_STAB2_DESC",		"BOTH_ROLL_STAB",				MDS_SABER,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,

	//Single Saber, Strong Style
	"@MENUS_SLASH_BACK",				"@MENUS_SLASH_BACK_DESC",			"BOTH_ATTACK_BACK",				MDS_SABER,
	"@MENUS_JUMP_ATTACK",				"@MENUS_JUMP_ATTACK_DESC",			"BOTH_FORCELEAP2_T__B_",		MDS_FORCE_JUMP,
	"@MENUS_STRONG_ATTACK_KATA",		"@MENUS_STRONG_ATTACK_KATA_DESC",	"BOTH_A3_SPECIAL",				MDS_SABER,
	"@MENUS_ATTACK_ENEMYONGROUND",		"@MENUS_ATTACK_ENEMYONGROUND_DESC", "BOTH_STABDOWN",				MDS_FORCE_JUMP,
	"@MENUS_CARTWHEEL",					"@MENUS_CARTWHEEL_DESC",			"BOTH_ARIAL_RIGHT",				MDS_FORCE_JUMP,
	"@MENUS_BOTH_ROLL_STAB",			"@MENUS_BOTH_ROLL_STAB2_DESC",		"BOTH_ROLL_STAB",				MDS_SABER,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,

	//Dual Sabers
	"@MENUS_SLASH_BACK",				"@MENUS_SLASH_BACK_DESC",			"BOTH_ATTACK_BACK",				MDS_SABER,
	"@MENUS_FLIP_FORWARD_ATTACK",		"@MENUS_FLIP_FORWARD_ATTACK_DESC",	"BOTH_JUMPATTACK6",				MDS_FORCE_JUMP,
	"@MENUS_DUAL_SABERS_TWIRL",			"@MENUS_DUAL_SABERS_TWIRL_DESC",	"BOTH_SPINATTACK6",				MDS_SABER,
	"@MENUS_ATTACK_ENEMYONGROUND",		"@MENUS_ATTACK_ENEMYONGROUND_DESC", "BOTH_STABDOWN_DUAL",				MDS_FORCE_JUMP,
	"@MENUS_DUAL_SABER_BARRIER",		"@MENUS_DUAL_SABER_BARRIER_DESC",	"BOTH_A6_SABERPROTECT",			MDS_SABER,
	"@MENUS_DUAL_STAB_FRONT_BACK",		"@MENUS_DUAL_STAB_FRONT_BACK_DESC", "BOTH_A6_FB",					MDS_SABER,
	"@MENUS_DUAL_STAB_LEFT_RIGHT",		"@MENUS_DUAL_STAB_LEFT_RIGHT_DESC", "BOTH_A6_LR",					MDS_SABER,
	"@MENUS_CARTWHEEL",					"@MENUS_CARTWHEEL_DESC",			"BOTH_ARIAL_RIGHT",				MDS_FORCE_JUMP,
	"@MENUS_BOTH_ROLL_STAB",			"@MENUS_BOTH_ROLL_STAB_DESC",		"BOTH_ROLL_STAB",				MDS_SABER,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,

	// Saber Staff
	"@MENUS_STAB_BACK",					"@MENUS_STAB_BACK_DESC",			"BOTH_A2_STABBACK1",			MDS_SABER,
	"@MENUS_BACK_FLIP_ATTACK",			"@MENUS_BACK_FLIP_ATTACK_DESC",		"BOTH_JUMPATTACK7",				MDS_FORCE_JUMP,
	"@MENUS_SABER_STAFF_TWIRL",			"@MENUS_SABER_STAFF_TWIRL_DESC",	"BOTH_SPINATTACK7",				MDS_SABER,
	"@MENUS_ATTACK_ENEMYONGROUND",		"@MENUS_ATTACK_ENEMYONGROUND_DESC", "BOTH_STABDOWN_STAFF",			MDS_FORCE_JUMP,
	"@MENUS_SPINNING_KATA",				"@MENUS_SPINNING_KATA_DESC",		"BOTH_A7_SOULCAL",				MDS_SABER,
	"@MENUS_KICK1",						"@MENUS_KICK1_DESC",				"BOTH_A7_KICK_F",				MDS_FORCE_JUMP,
	"@MENUS_JUMP_KICK",					"@MENUS_JUMP_KICK_DESC",			"BOTH_A7_KICK_F_AIR",			MDS_FORCE_JUMP,
	"@MENUS_BUTTERFLY_ATTACK",			"@MENUS_BUTTERFLY_ATTACK_DESC",		"BOTH_BUTTERFLY_FR1",			MDS_SABER,
	"@MENUS_BOTH_ROLL_STAB",			"@MENUS_BOTH_ROLL_STAB2_DESC",		"BOTH_ROLL_STAB",				MDS_SABER,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
	NULL, NULL, 0,	MDS_NONE,
};

/*
================
vmMain

This is the only way control passes into the module.
!!! This MUST BE THE VERY FIRST FUNCTION compiled into the .qvm file !!!
================
*/
vmCvar_t  ui_debug;
vmCvar_t  ui_initialized;
vmCvar_t	ui_char_color_red;
vmCvar_t	ui_char_color_green;
vmCvar_t	ui_char_color_blue;
vmCvar_t	ui_PrecacheModels;
vmCvar_t	ui_char_anim;
// Jedi Knight Galaxies
vmCvar_t	jkgver;
vmCvar_t	ui_hidehud;


void _UI_Init( qboolean );
void _UI_Shutdown( void );
void _UI_KeyEvent( int key, qboolean down );
void _UI_MouseEvent( int dx, int dy );
void _UI_Refresh( int realtime );
qboolean _UI_IsFullscreen( void );
extern qboolean UI_SaberModelForSaber( const char *saberName, char *saberModel );
void UI_ClampMaxPlayers(void);
static void UI_CheckServerName( void );
static qboolean UI_CheckPassword( void );
static void UI_JoinServer( void );

// Functions in BG or ui_shared
void Menu_ShowGroup (menuDef_t *menu, char *itemName, qboolean showFlag);
void Menu_ItemDisable(menuDef_t *menu, char *name,int disableFlag);
int Menu_ItemsMatchingGroup(menuDef_t *menu, const char *name);
itemDef_t *Menu_GetMatchingItemByNumber(menuDef_t *menu, int index, const char *name);

int BG_GetUIPortrait(const int team, const short classIndex, const short cntIndex);
char *BG_GetUIPortraitFile(const int team, const short classIndex, const short cntIndex);

siegeClass_t *BG_GetClassOnBaseClass(const int team, const short classIndex, const short cntIndex);

const char *JAMD5Check();
void CheckEngineDll();

extern uiCrossoverExports_t *UI_InitializeCrossoverAPI( cgCrossoverExports_t *cg );
extern "C" {
/*Q_EXPORT_C*/ Q_EXPORT intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  ) 
{
	switch ( command ) {
	case UI_GETAPIVERSION:
		return UI_API_VERSION;

	case UI_INIT:
		_UI_Init(arg0);
		return 0;

	case UI_SHUTDOWN:
		_UI_Shutdown();
		return 0;

	case UI_KEY_EVENT:
		_UI_KeyEvent( arg0, arg1 );
		return 0;

	case UI_MOUSE_EVENT:
		_UI_MouseEvent( arg0, arg1 );
		return 0;

	case UI_REFRESH:
		_UI_Refresh( arg0 );
		return 0;

	case UI_IS_FULLSCREEN:
		return _UI_IsFullscreen();

	case UI_SET_ACTIVE_MENU:
		_UI_SetActiveMenu( arg0 );
		return 0;

	case UI_CONSOLE_COMMAND:
		return UI_ConsoleCommand(arg0);

	case UI_DRAW_CONNECT_SCREEN:
		UI_DrawConnectScreen( arg0 );
		return 0;
	case UI_MENU_RESET:
		Menu_Reset();
		return 0;

	case UI_CROSSOVER_API:
		return (int)UI_InitializeCrossoverAPI( (cgCrossoverExports_t *)arg0 );
	}

	return -1;
}
}

// TODO: remove
siegeClassDesc_t g_UIClassDescriptions[MAX_SIEGE_CLASSES];
siegeTeam_t *siegeTeam1 = NULL;
siegeTeam_t *siegeTeam2 = NULL;
int g_UIGloballySelectedSiegeClass = -1;

//Cut down version of the stuff used in the game code
//This is just the bare essentials of what we need to load animations properly for ui ghoul2 models.
//This function doesn't need to be sync'd with the BG_ version in bg_panimate.c unless some sort of fundamental change
//is made. Just make sure the variables/functions accessed in ui_shared.c exist in both modules.
qboolean	UIPAFtextLoaded = qfalse;
animation_t	uiHumanoidAnimations[MAX_TOTALANIMATIONS]; //humanoid animations are the only ones that are statically allocated.

bgLoadedAnim_t bgAllAnims[MAX_ANIM_FILES];
int uiNumAllAnims = 1; //start off at 0, because 0 will always be assigned to humanoid.

animation_t *UI_AnimsetAlloc(void)
{
	assert (uiNumAllAnims < MAX_ANIM_FILES);
	bgAllAnims[uiNumAllAnims].anims = (animation_t *) malloc(sizeof(animation_t)*MAX_TOTALANIMATIONS);

	return bgAllAnims[uiNumAllAnims].anims;
}

int UI_SourceForLAN()
{
	if(ui_netSource.integer > AS_FAVORITES)
	{
		return AS_GLOBAL;
	}
	return ui_netSource.integer;
}

/*
======================
UI_ParseAnimationFile

Read a configuration file containing animation coutns and rates
models/players/visor/animation.cfg, etc

======================
*/
static char UIPAFtext[100000];
int UI_ParseAnimationFile(const char *filename, animation_t *animset, qboolean isHumanoid) 
{
	char		*text_p;
	int			len;
	int			i;
	char		*token;
	float		fps;
	int			skip;
	int			usedIndex = -1;
	int			nextIndex = uiNumAllAnims;

	fileHandle_t	f;
	int				animNum;

	if (!isHumanoid)
	{
		i = 1;
		while (i < uiNumAllAnims)
		{ //see if it's been loaded already
			if (!Q_stricmp(bgAllAnims[i].filename, filename))
			{
				animset = bgAllAnims[i].anims;
				return i; //alright, we already have it.
			}
			i++;
		}

		//Looks like it has not yet been loaded. Allocate space for the anim set if we need to, and continue along.
		if (!animset)
		{
			if (strstr(filename, "players/_humanoid/"))
			{ //then use the static humanoid set.
				animset = uiHumanoidAnimations;
				isHumanoid = qtrue;
				nextIndex = 0;
			}
			else if ( strstr (filename, "players/_humanoidJKG/") )
			{
				animset = UI_AnimsetAlloc();
				nextIndex = 1;

				if ( !animset )
				{
					assert (!"Animation set allocation failed!");
					return -1;
				}
			}
			else
			{
				animset = UI_AnimsetAlloc();

				if (!animset)
				{
					assert(!"Anim set alloc failed!");
					return -1;
				}
			}
		}
	}
#ifdef _DEBUG
	else
	{
		assert(animset);
	}
#endif

	// load the file
	if (!UIPAFtextLoaded || !isHumanoid)
	{ //rww - We are always using the same animation config now. So only load it once.
		len = trap_FS_FOpenFile( filename, &f, FS_READ );
		if ( (len <= 0) || (len >= sizeof( UIPAFtext ) - 1) ) 
		{
			if (len > 0)
			{
				Com_Error(ERR_DROP, "%s exceeds the allowed ui-side animation buffer!", filename);
			}
			return -1;
		}

		trap_FS_Read( UIPAFtext, len, f );
		UIPAFtext[len] = 0;
		trap_FS_FCloseFile( f );
	}
	else
	{
		return 0; //humanoid index
	}

	// parse the text
	text_p = UIPAFtext;
	skip = 0;	// quiet the compiler warning

	//FIXME: have some way of playing anims backwards... negative numFrames?

	//initialize anim array so that from 0 to MAX_ANIMATIONS, set default values of 0 1 0 100
	for(i = 0; i < MAX_ANIMATIONS; i++)
	{
		animset[i].firstFrame = 0;
		animset[i].numFrames = 0;
		animset[i].loopFrames = -1;
		animset[i].frameLerp = 100;
//		animset[i].initialLerp = 100;
	}

	// read information for each frame
	while(1) 
	{
		token = COM_Parse( (const char **)(&text_p) );

		if ( !token || !token[0]) 
		{
			break;
		}

		animNum = GetIDForString(animTable, token);
		if(animNum == -1)
		{
			continue;
		}

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		animset[animNum].firstFrame = atoi( token );

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		animset[animNum].numFrames = atoi( token );

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		animset[animNum].loopFrames = atoi( token );

		token = COM_Parse( (const char **)(&text_p) );
		if ( !token ) 
		{
			break;
		}
		fps = atof( token );
		if ( fps == 0 ) 
		{
			fps = 1;//Don't allow divide by zero error
		}
		if ( fps < 0 )
		{//backwards
			animset[animNum].frameLerp = floor(1000.0f / fps);
		}
		else
		{
			animset[animNum].frameLerp = ceil(1000.0f / fps);
		}

//		animset[animNum].initialLerp = ceil(1000.0f / fabs(fps));
	}

	if (isHumanoid)
	{
		bgAllAnims[0].anims = animset;
		strcpy(bgAllAnims[0].filename, filename);
		UIPAFtextLoaded = qtrue;

		usedIndex = 0;
	}
	else
	{
		bgAllAnims[nextIndex].anims = animset;
		strcpy(bgAllAnims[nextIndex].filename, filename);

		usedIndex = nextIndex;

		if (nextIndex > 1)
		{ //don't bother increasing the number if this ended up as a humanoid load, or a JKG humanoid
			uiNumAllAnims++;
		}
		else
		{
			UIPAFtextLoaded = qtrue;
			usedIndex = 0;
		}
	}

	return usedIndex;
}

//menuDef_t *Menus_FindByName(const char *p);
void Menu_ShowItemByName(menuDef_t *menu, const char *p, qboolean bShow);

char holdSPString[MAX_STRING_CHARS]={0};
char holdSPString2[MAX_STRING_CHARS]={0};

uiInfo_t uiInfo;

static void UI_StartServerRefresh(qboolean full);
static void UI_StopServerRefresh( void );
static void UI_DoServerRefresh( void );
static void UI_BuildServerDisplayList(int force);
static void UI_BuildServerStatus(qboolean force);
static void UI_BuildFindPlayerList(qboolean force);
static int QDECL UI_ServersQsortCompare( const void *arg1, const void *arg2 );
static int UI_MapCountByGameType(qboolean singlePlayer);
static int UI_HeadCountByColor( void );
static void UI_ParseGameInfo(const char *teamFile);
static const char *UI_SelectedMap(int index, int *actual);
static int UI_GetIndexFromSelection(int actual);
static void UI_SiegeClassCnt( const int team );


int ProcessNewUI( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6 );
int	uiSkinColor=TEAM_FREE;
int	uiHoldSkinColor=TEAM_FREE;	// Stores the skin color so that in non-team games, the player screen remembers the team you chose, in case you're coming back from the force powers screen.

static const serverFilter_t serverFilters[] = {
	{"JKG", "JKG" },
	{"Non-JKG", "" },
	{"base", "base" },
};
static const int numServerFilters = sizeof(serverFilters) / sizeof(serverFilter_t);

static const char *skillLevels[] = {
	"SKILL1",//"I Can Win",
	"SKILL2",//"Bring It On",
	"SKILL3",//"Hurt Me Plenty",
	"SKILL4",//"Hardcore",
	"SKILL5"//"Nightmare"
};
static const int numSkillLevels = sizeof(skillLevels) / sizeof(const char*);



static const char *teamArenaGameTypes[] = {
	"FFA",
	"Duel",
	"PowerDuel",
	"SP",
	"Team FFA",
	"CTF",
	"CTY",
	"TeamTournament",
	"Warzone",
};
static int const numTeamArenaGameTypes = sizeof(teamArenaGameTypes) / sizeof(const char*);



static char* netnames[] = {
	"???",
	"UDP",
	"IPX",
	NULL
};

static int gamecodetoui[] = {4,2,3,0,5,1,6};
static int uitogamecode[] = {4,6,2,3,1,5,7};

const char *UI_GetStringEdString(const char *refSection, const char *refName);

const char *UI_TeamName(int team)  {
	if (team==TEAM_RED)
		return "RED";
	else if (team==TEAM_BLUE)
		return "BLUE";
	else if (team==TEAM_SPECTATOR)
		return "SPECTATOR";
	return "FREE";
}

// returns either string or NULL for OOR...
//
static const char *GetCRDelineatedString( const char *psStripFileRef, const char *psStripStringRef, int iIndex)
{
	static char sTemp[256];
	const char *psList = UI_GetStringEdString(psStripFileRef, psStripStringRef);
	char *p;

	while (iIndex--)
	{
		psList = strchr(psList,'\n');
		if (!psList){
			return NULL;	// OOR
		}
		psList++;
	}

	strcpy(sTemp,psList);
	p = strchr(sTemp,'\n');
	if (p) {
		*p = '\0';
	}

	return sTemp;
}


static const char *GetMonthAbbrevString( int iMonth )
{
	const char *p = GetCRDelineatedString("MP_INGAME","MONTHS", iMonth);

	return p ? p : "Jan";	// sanity
}




/*
static const char *netSources[] = {
	"Local",
	"Internet",
	"Favorites"
//	"Mplayer"
};
static const int numNetSources = sizeof(netSources) / sizeof(const char*);
*/
//[MasterServer]
static const int numNetSources = AS_GLOBAL5+1;	// now hard-entered in StringEd file
//[/MasterServer]
static const char *GetNetSourceString(int iSource)
{
	const char *p = GetCRDelineatedString("MP_INGAME","NET_SOURCES", iSource);

	return p ? p : "??";
}




void AssetCache() {
	int n;
	//if (Assets.textFont == NULL) {
	//}
	//Assets.background = trap_R_RegisterShaderNoMip( ASSET_BACKGROUND );
	//Com_Printf("Menu Size: %i bytes\n", sizeof(Menus));
	uiInfo.uiDC.Assets.gradientBar = trap_R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
	uiInfo.uiDC.Assets.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
	uiInfo.uiDC.Assets.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	uiInfo.uiDC.Assets.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_ORANGE );//trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	uiInfo.uiDC.Assets.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );//trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	uiInfo.uiDC.Assets.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );//trap_R_RegisterShaderNoMip( ART_FX_TEAL );
	uiInfo.uiDC.Assets.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	uiInfo.uiDC.Assets.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_PURPLE );//trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	uiInfo.uiDC.Assets.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
	uiInfo.uiDC.Assets.scrollBar = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
	uiInfo.uiDC.Assets.scrollBarArrowDown = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
	uiInfo.uiDC.Assets.scrollBarArrowUp = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
	uiInfo.uiDC.Assets.scrollBarArrowLeft = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
	uiInfo.uiDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
	uiInfo.uiDC.Assets.scrollBarThumb = trap_R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
	uiInfo.uiDC.Assets.sliderBar = trap_R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
	uiInfo.uiDC.Assets.sliderThumb = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );

	uiInfo.uiDC.Assets.arrow = trap_R_RegisterShaderNoMip( "gfx/2d/convo/arrow" );

	// Icons for various server settings.
	uiInfo.uiDC.Assets.needPass = trap_R_RegisterShaderNoMip( "gfx/menus/needpass" );
	uiInfo.uiDC.Assets.noForce = trap_R_RegisterShaderNoMip( "gfx/menus/noforce" );
	uiInfo.uiDC.Assets.forceRestrict = trap_R_RegisterShaderNoMip( "gfx/menus/forcerestrict" );
	uiInfo.uiDC.Assets.saberOnly = trap_R_RegisterShaderNoMip( "gfx/menus/saberonly" );

	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
		uiInfo.uiDC.Assets.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
	}

	uiInfo.newHighScoreSound = 0;//trap_S_RegisterSound("sound/feedback/voc_newhighscore.wav");
}

void _UI_DrawSides(float x, float y, float w, float h, float size) {
	size /= uiInfo.uiDC.xscale;
	trap_R_DrawStretchPic( x, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x + w - size, y, size, h, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}

void _UI_DrawTopBottom(float x, float y, float w, float h, float size) {
	size /= uiInfo.uiDC.yscale;
	// JKG - Fix: Avoid double-rendering the corners
	trap_R_DrawStretchPic( x+size, y, w-size*2, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
	trap_R_DrawStretchPic( x+size, y + h - size, w-size*2, size, 0, 0, 0, 0, uiInfo.uiDC.whiteShader );
}
/*
================
UI_DrawRect

Coordinates are 640*480 virtual values
=================
*/
void _UI_DrawRect( float x, float y, float width, float height, float size, const float *color ) {
	trap_R_SetColor( color );

	_UI_DrawTopBottom(x, y, width, height, size);
	_UI_DrawSides(x, y, width, height, size);

	trap_R_SetColor( NULL );
}

int MenuFontToHandle(int iMenuFont)
{
	switch (iMenuFont)
	{
		//Jedi Knight Galaxies note: this switch was replaced with enum values
	case FONT_SMALL: return uiInfo.uiDC.Assets.qhSmallFont;
	case FONT_MEDIUM: return uiInfo.uiDC.Assets.qhMediumFont;
	case FONT_LARGE: return uiInfo.uiDC.Assets.qhBigFont;
	case FONT_SMALL2: return uiInfo.uiDC.Assets.qhSmall2Font;
	case FONT_SMALL3: return uiInfo.uiDC.Assets.qhSmall3Font;
	case FONT_SMALL4: return uiInfo.uiDC.Assets.qhSmall4Font;
	}

	return uiInfo.uiDC.Assets.qhMediumFont;	// 0;
}

int Text_Width(const char *text, float scale, int iMenuFont) 
{	
	int iFontIndex = MenuFontToHandle(iMenuFont);

	return trap_R_Font_StrLenPixels(text, iFontIndex, scale);
}

int Text_Height(const char *text, float scale, int iMenuFont) 
{
	int iFontIndex = MenuFontToHandle(iMenuFont);

	return trap_R_Font_HeightPixels(iFontIndex, scale);
}

void Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style, int iMenuFont)
{
	int iStyleOR = 0;

	int iFontIndex = MenuFontToHandle(iMenuFont);
	//
	// kludge.. convert JK2 menu styles to SOF2 printstring ctrl codes...
	//	
	switch (style)
	{
	case  ITEM_TEXTSTYLE_NORMAL:			iStyleOR = 0;break;					// JK2 normal text
	case  ITEM_TEXTSTYLE_BLINK:				iStyleOR = (int)STYLE_BLINK;break;		// JK2 fast blinking
	case  ITEM_TEXTSTYLE_PULSE:				iStyleOR = (int)STYLE_BLINK;break;		// JK2 slow pulsing
	case  ITEM_TEXTSTYLE_SHADOWED:			iStyleOR = (int)STYLE_DROPSHADOW;break;	// JK2 drop shadow
	case  ITEM_TEXTSTYLE_OUTLINED:			iStyleOR = (int)STYLE_DROPSHADOW;break;	// JK2 drop shadow
	case  ITEM_TEXTSTYLE_OUTLINESHADOWED:	iStyleOR = (int)STYLE_DROPSHADOW;break;	// JK2 drop shadow
	case  ITEM_TEXTSTYLE_SHADOWEDMORE:		iStyleOR = (int)STYLE_DROPSHADOW;break;	// JK2 drop shadow
	}

	trap_R_Font_DrawString(	x,		// int ox
		y,		// int oy
		text,	// const char *text
		color,	// paletteRGBA_c c
		iStyleOR | iFontIndex,	// const int iFontHandle
		!limit?-1:limit,		// iCharLimit (-1 = none)
		scale	// const float scale = 1.0f
		);
}


void Text_PaintWithCursor(float x, float y, float scale, vec4_t color, const char *text, int cursorPos, char cursor, int limit, int style, int iMenuFont) 
{
	Text_Paint(x, y, scale, color, text, 0, limit, style, iMenuFont);

	// now print the cursor as well...  (excuse the braces, it's for porting C++ to C)
	//
	{
		char sTemp[1024];
		int iCopyCount = limit ? min( (signed)strlen( text ), limit ) : (signed)strlen( text );
			iCopyCount = min( iCopyCount, cursorPos );
			iCopyCount = min( iCopyCount, sizeof( sTemp ) );

			// copy text into temp buffer for pixel measure...
			//			
			strncpy(sTemp,text,iCopyCount);
					sTemp[iCopyCount] = '\0';

			{
				int iFontIndex = MenuFontToHandle( iMenuFont );	
				int iNextXpos  = trap_R_Font_StrLenPixels(sTemp, iFontIndex, scale );

				Text_Paint(x+iNextXpos, y, scale, color, va("%c",cursor), 0, limit, style|ITEM_TEXTSTYLE_BLINK, iMenuFont);
			}
	}
}


// maxX param is initially an X limit, but is also used as feedback. 0 = text was clipped to fit within, else maxX = next pos
//
static void Text_Paint_Limit(float *maxX, float x, float y, float scale, vec4_t color, const char* text, float adjust, int limit, int iMenuFont) 
{
	// this is kinda dirty, but...
	//
	int iFontIndex = MenuFontToHandle(iMenuFont);
	
	//float fMax = *maxX;
	int iPixelLen = trap_R_Font_StrLenPixels(text, iFontIndex, scale);
	if (x + iPixelLen > *maxX)
	{
		// whole text won't fit, so we need to print just the amount that does...
		//  Ok, this is slow and tacky, but only called occasionally, and it works...
		//
		char sTemp[4096]={0};	// lazy assumption
		const char *psText = text;
		char *psOut = &sTemp[0];
		char *psOutLastGood = psOut;
		unsigned int uiLetter;

		while (*psText && (x + trap_R_Font_StrLenPixels(sTemp, iFontIndex, scale)<=*maxX) 
			&& psOut < &sTemp[sizeof(sTemp)-1]	// sanity
		)
		{
			int iAdvanceCount;
			psOutLastGood = psOut;

			uiLetter = trap_AnyLanguage_ReadCharFromString(psText, &iAdvanceCount, NULL);
			psText += iAdvanceCount;

			if (uiLetter > 255)
			{
				*psOut++ = uiLetter>>8;
				*psOut++ = uiLetter&0xFF;
			}
			else
			{
				*psOut++ = uiLetter&0xFF;
			}
		}
		*psOutLastGood = '\0';

		*maxX = 0;	// feedback
		Text_Paint(x, y, scale, color, sTemp, adjust, limit, ITEM_TEXTSTYLE_NORMAL, iMenuFont);
	}
	else
	{
		// whole text fits fine, so print it all...
		//
		*maxX = x + iPixelLen;	// feedback the next position, as the caller expects		
		Text_Paint(x, y, scale, color, text, adjust, limit, ITEM_TEXTSTYLE_NORMAL, iMenuFont);
	}
}

/*
=================
_UI_Refresh
=================
*/

void UI_DrawCenteredPic(qhandle_t image, int w, int h) {
	int x, y;
	x = (SCREEN_WIDTH - w) / 2;
	y = (SCREEN_HEIGHT - h) / 2;
	UI_DrawHandlePic(x, y, w, h, image);
}

int frameCount = 0;
int startTime;

static void UI_BuildPlayerList();
char parsedFPMessage[1024];

extern int FPMessageTime;

void Text_PaintCenter(float x, float y, float scale, vec4_t color, const char *text, float adjust, int iMenuFont);

const char *UI_GetStringEdString(const char *refSection, const char *refName)
{
	static char text[1024]={0};

	trap_SP_GetStringTextString(va("%s_%s", refSection, refName), text, sizeof(text));
	return text;
}

const char *UI_GetStringEdString2(const char *refName)
{
	static char text[1024]={0};
	if(refName[0] == '@')
	{
		trap_SP_GetStringTextString(refName+1, text, sizeof(text));
	}
	else
	{
		Q_strncpyz(text, refName, 1024);
	}
	return text;
}

#define	UI_FPS_FRAMES	4
void _UI_Refresh( int realtime )
{
	static int index;
	static int	previousTimes[UI_FPS_FRAMES];

	//if ( !( trap_Key_GetCatcher() & KEYCATCH_UI ) ) {
	//	return;
	//}

	trap_G2API_SetTime(realtime, 0);
	trap_G2API_SetTime(realtime, 1);
	//ghoul2 timer must be explicitly updated during ui rendering.

	uiInfo.uiDC.frameTime = realtime - uiInfo.uiDC.realTime;
	uiInfo.uiDC.realTime = realtime;

	previousTimes[index % UI_FPS_FRAMES] = uiInfo.uiDC.frameTime;
	index++;
	if ( index > UI_FPS_FRAMES ) {
		int i, total;
		// average multiple frames together to smooth changes out a bit
		total = 0;
		for ( i = 0 ; i < UI_FPS_FRAMES ; i++ ) {
			total += previousTimes[i];
		}
		if ( !total ) {
			total = 1;
		}
		uiInfo.uiDC.FPS = 1000 * UI_FPS_FRAMES / total;
	}



	UI_UpdateCvars();

	if (Menu_Count() > 0) {
		// paint all the menus
		Menu_PaintAll();
		// refresh server browser list
		UI_DoServerRefresh();
		// refresh server status
		UI_BuildServerStatus(qfalse);
		// refresh find player list
		UI_BuildFindPlayerList(qfalse);
	} 
	// draw cursor
	UI_SetColor( NULL );
	if (Menu_Count() > 0 && !uiInfo.hideCursor) {
		// JKG - Dont draw the cursor during the loading screen
		uiClientState_t	cstate;
		trap_GetClientState( &cstate );
		if( cstate.connState <= CA_DISCONNECTED || cstate.connState >= CA_ACTIVE ) {
			UI_DrawHandlePic( uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory, 48, 48, uiInfo.uiDC.Assets.cursor);
		}
	}

#ifndef NDEBUG
	if (uiInfo.uiDC.debug)
	{
		// cursor coordinates
		//FIXME
		//UI_DrawString( 0, 0, va("(%d,%d)",uis.cursorx,uis.cursory), UI_LEFT|UI_SMALLFONT, colorRed );
	}
#endif

	if (ui_freeSaber.integer)
	{
		bgForcePowerCost[FP_SABER_OFFENSE][FORCE_LEVEL_1] = 0;
		bgForcePowerCost[FP_SABER_DEFENSE][FORCE_LEVEL_1] = 0;
	}
	else
	{
		bgForcePowerCost[FP_SABER_OFFENSE][FORCE_LEVEL_1] = 1;
		bgForcePowerCost[FP_SABER_DEFENSE][FORCE_LEVEL_1] = 1;
	}

	/*
	if (parsedFPMessage[0] && FPMessageTime > realtime)
	{
		vec4_t txtCol;
		int txtStyle = ITEM_TEXTSTYLE_SHADOWED;

		if ((FPMessageTime - realtime) < 2000)
		{
			txtCol[0] = colorWhite[0];
			txtCol[1] = colorWhite[1];
			txtCol[2] = colorWhite[2];
			txtCol[3] = (((float)FPMessageTime - (float)realtime)/2000);

			txtStyle = 0;
		}
		else
		{
			txtCol[0] = colorWhite[0];
			txtCol[1] = colorWhite[1];
			txtCol[2] = colorWhite[2];
			txtCol[3] = colorWhite[3];
		}

		Text_Paint(10, 0, 1, txtCol, parsedFPMessage, 0, 1024, txtStyle, FONT_MEDIUM);
	}
	*/
	//For now, don't bother.
}

/*
=================
_UI_Shutdown
=================
*/
void UI_CleanupGhoul2(void);

void _UI_Shutdown( void ) {
	trap_LAN_SaveCachedServers();
	UI_CleanupGhoul2();
	trap_Cvar_Set("connmsg", "");	// Clear the connection message override
}

char *defaultMenu = NULL;

char *GetMenuBuffer(const char *filename) {
	int	len;
	fileHandle_t	f;
	static char buf[MAX_MENUFILE];

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( !f ) {
		trap_Print( va( S_COLOR_RED "menu file not found: %s, using default\n", filename ) );
		return defaultMenu;
	}
	if ( len >= MAX_MENUFILE ) {
		trap_Print( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i\n", filename, len, MAX_MENUFILE ) );
		trap_FS_FCloseFile( f );
		return defaultMenu;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );
	//COM_Compress(buf);
	return buf;

}

qboolean Asset_Parse(int handle) {
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (Q_stricmp(token.string, "{") != 0) {
		return qfalse;
	}

	while ( 1 ) {

		memset(&token, 0, sizeof(pc_token_t));

		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if (Q_stricmp(token.string, "}") == 0) {
			return qtrue;
		}

		// font
		if (Q_stricmp(token.string, "font") == 0) {
			int pointSize;
			if (!trap_PC_ReadToken(handle, &token) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}			
			//trap_R_RegisterFont(tempStr, pointSize, &uiInfo.uiDC.Assets.textFont);
			uiInfo.uiDC.Assets.qhMediumFont = trap_R_RegisterFont(token.string);
			uiInfo.uiDC.Assets.fontRegistered = qtrue;
			continue;
		}

		if (Q_stricmp(token.string, "smallFont") == 0) {
			int pointSize;
			if (!trap_PC_ReadToken(handle, &token) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			//trap_R_RegisterFont(token, pointSize, &uiInfo.uiDC.Assets.smallFont);
			uiInfo.uiDC.Assets.qhSmallFont = trap_R_RegisterFont(token.string);
			continue;
		}

		if (Q_stricmp(token.string, "small2Font") == 0) {
			int pointSize;
			if (!trap_PC_ReadToken(handle, &token) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			//trap_R_RegisterFont(token, pointSize, &uiInfo.uiDC.Assets.smallFont);
			uiInfo.uiDC.Assets.qhSmall2Font = trap_R_RegisterFont(token.string);
			continue;
		}

		//Jedi Knight Galaxies add
		if (!Q_stricmp(token.string, "small3Font")) {
			int pointSize;
			if (!trap_PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.qhSmall3Font = trap_R_RegisterFont(token.string);
			continue;
		}
		if (!Q_stricmp(token.string, "small4Font")) {
			int pointSize;
			if (!trap_PC_ReadToken(handle, &token) || !PC_Int_Parse(handle, &pointSize)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.qhSmall4Font = trap_R_RegisterFont(token.string);
			continue;
		}

		if (Q_stricmp(token.string, "bigFont") == 0) {
			int pointSize;
			if (!trap_PC_ReadToken(handle, &token) || !PC_Int_Parse(handle,&pointSize)) {
				return qfalse;
			}
			//trap_R_RegisterFont(token, pointSize, &uiInfo.uiDC.Assets.bigFont);
			uiInfo.uiDC.Assets.qhBigFont = trap_R_RegisterFont(token.string);
			continue;
		}

		if (Q_stricmp(token.string, "cursor") == 0) 
		{
			if (!PC_String_Parse(handle, &uiInfo.uiDC.Assets.cursorStr))
			{
				Com_Printf(S_COLOR_YELLOW,"Bad 1st parameter for keyword 'cursor'");
				return qfalse;
			}
			uiInfo.uiDC.Assets.cursor = trap_R_RegisterShaderNoMip( uiInfo.uiDC.Assets.cursorStr);
			continue;
		}

		// gradientbar
		if (Q_stricmp(token.string, "gradientbar") == 0) {
			if (!trap_PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.gradientBar = trap_R_RegisterShaderNoMip(token.string);
			continue;
		}

		// enterMenuSound
		if (Q_stricmp(token.string, "menuEnterSound") == 0) {
			if (!trap_PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuEnterSound = trap_S_RegisterSound( token.string );
			continue;
		}

		// exitMenuSound
		if (Q_stricmp(token.string, "menuExitSound") == 0) {
			if (!trap_PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuExitSound = trap_S_RegisterSound( token.string );
			continue;
		}

		// itemFocusSound
		if (Q_stricmp(token.string, "itemFocusSound") == 0) {
			if (!trap_PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.itemFocusSound = trap_S_RegisterSound( token.string );
			continue;
		}

		// menuBuzzSound
		if (Q_stricmp(token.string, "menuBuzzSound") == 0) {
			if (!trap_PC_ReadToken(handle, &token)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.menuBuzzSound = trap_S_RegisterSound( token.string );
			continue;
		}

		if (Q_stricmp(token.string, "fadeClamp") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.fadeClamp)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeCycle") == 0) {
			if (!PC_Int_Parse(handle, &uiInfo.uiDC.Assets.fadeCycle)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "fadeAmount") == 0) {
			if (!PC_Float_Parse(handle, &uiInfo.uiDC.Assets.fadeAmount)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowX") == 0) {
			if (!PC_Screenspace_Parse(handle, &uiInfo.uiDC.Assets.shadowX, qtrue)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowY") == 0) {
			if (!PC_Screenspace_Parse(handle, &uiInfo.uiDC.Assets.shadowY, qfalse)) {
				return qfalse;
			}
			continue;
		}

		if (Q_stricmp(token.string, "shadowColor") == 0) {
			if (!PC_Color_Parse(handle, &uiInfo.uiDC.Assets.shadowColor)) {
				return qfalse;
			}
			uiInfo.uiDC.Assets.shadowFadeClamp = uiInfo.uiDC.Assets.shadowColor[3];
			continue;
		}

		if (Q_stricmp(token.string, "moveRollSound") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.moveRollSound = trap_S_RegisterSound( token.string );
			}
			continue;
		}

		if (Q_stricmp(token.string, "moveJumpSound") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.moveJumpSound = trap_S_RegisterSound( token.string );
			}

			continue;
		}
		if (Q_stricmp(token.string, "datapadmoveSaberSound1") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.datapadmoveSaberSound1 = trap_S_RegisterSound( token.string );
			}

			continue;
		}

		if (Q_stricmp(token.string, "datapadmoveSaberSound2") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.datapadmoveSaberSound2 = trap_S_RegisterSound( token.string );
			}

			continue;
		}

		if (Q_stricmp(token.string, "datapadmoveSaberSound3") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.datapadmoveSaberSound3 = trap_S_RegisterSound( token.string );
			}

			continue;
		}

		if (Q_stricmp(token.string, "datapadmoveSaberSound4") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.datapadmoveSaberSound4 = trap_S_RegisterSound( token.string );
			}

			continue;
		}

		if (Q_stricmp(token.string, "datapadmoveSaberSound5") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.datapadmoveSaberSound5 = trap_S_RegisterSound( token.string );
			}

			continue;
		}

		if (Q_stricmp(token.string, "datapadmoveSaberSound6") == 0) 
		{
			if (trap_PC_ReadToken(handle,&token))
			{
				uiInfo.uiDC.Assets.datapadmoveSaberSound6 = trap_S_RegisterSound( token.string );
			}

			continue;
		}


		// precaching various sound files used in the menus
		if (Q_stricmp(token.string, "precacheSound") == 0)
		{
			const char *tempStr;
			if (PC_Script_Parse(handle, &tempStr)) 
			{
				char *soundFile;
				do
				{
					soundFile = COM_ParseExt(&tempStr, qfalse);	
					if (soundFile[0] != 0 && soundFile[0] != ';') {
						trap_S_RegisterSound( soundFile);
					}
				} while (soundFile[0]);
			}
			continue;
		}
	}
	return qfalse;
}


void UI_Report() {
	String_Report();
	//Font_Report();

}

void UI_ParseMenu(const char *menuFile) {
	int handle;
	pc_token_t token;

	//Com_Printf("Parsing menu file: %s\n", menuFile);

	handle = trap_PC_LoadSource(menuFile);
	if (!handle) {
		return;
	}

	while ( 1 ) {
		memset(&token, 0, sizeof(pc_token_t));
		if (!trap_PC_ReadToken( handle, &token )) {
			break;
		}

		//if ( Q_stricmp( token, "{" ) ) {
		//	Com_Printf( "Missing { in menu file\n" );
		//	break;
		//}

		//if ( menuCount == MAX_MENUS ) {
		//	Com_Printf( "Too many menus!\n" );
		//	break;
		//}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "assetGlobalDef") == 0) {
			if (Asset_Parse(handle)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token.string, "menudef") == 0) {
			// start a new menu
			Menu_New(handle);
		}
	}
	trap_PC_FreeSource(handle);
}

qboolean Load_Menu(int handle) {
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
		return qfalse;
	if (token.string[0] != '{') {
		return qfalse;
	}

	while ( 1 ) {

		if (!trap_PC_ReadToken(handle, &token))
			return qfalse;

		if ( token.string[0] == 0 ) {
			return qfalse;
		}

		if ( token.string[0] == '}' ) {
			return qtrue;
		}

		UI_ParseMenu(token.string); 
	}
	return qfalse;
}

void UI_LoadMenus(const char *menuFile, qboolean reset) {
	pc_token_t token;
	int handle;
	int start;

	start = trap_Milliseconds();

	trap_PC_LoadGlobalDefines ( "ui/jamp/menudef.h" );
	trap_PC_LoadGlobalDefines ( "ui/jkg/menudef.h" );

	handle = trap_PC_LoadSource( menuFile );
	if (!handle) {
		Com_Printf( S_COLOR_YELLOW "menu file not found: %s, using default\n", menuFile );
		handle = trap_PC_LoadSource( "ui/jampmenus.txt" );
		if (!handle) {
			trap_Error( va( S_COLOR_RED "default menu file not found: ui/menus.txt, unable to continue!\n", menuFile ) );
		}
	}

	if (reset) {
		Menu_Reset();
	}

	while ( 1 ) {
		if (!trap_PC_ReadToken(handle, &token))
			break;
		if( token.string[0] == 0 || token.string[0] == '}') {
			break;
		}

		if ( token.string[0] == '}' ) {
			break;
		}

		if (Q_stricmp(token.string, "loadmenu") == 0) {
			if (Load_Menu(handle)) {
				continue;
			} else {
				break;
			}
		}
	}

//	Com_Printf("UI menu load time = %d milli seconds\n", trap_Milliseconds() - start);

	trap_PC_FreeSource( handle );

	trap_PC_RemoveAllGlobalDefines ( );
}

void UI_Load() {
	char *menuSet;
	char lastName[1024];
	menuDef_t *menu = Menu_GetFocused();

	if (menu && menu->window.name) {
		strcpy(lastName, menu->window.name);
	}
	else
	{
		lastName[0] = 0;
	}

	if (uiInfo.inGameLoad)
	{
		menuSet= "ui/jampingame.txt";
	}
	else
	{
		menuSet= UI_Cvar_VariableString("ui_menuFilesMP");
	}
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/jampmenus.txt";
	}

	String_Init();

	UI_ParseGameInfo("ui/jamp/gameinfo.txt");
	UI_LoadArenas();
	UI_LoadBots();

	UI_LoadMenus(menuSet, qtrue);
	// JEdi Knight Galaxies
	UI_LoadMenus("ui/jkgmenus.txt", qfalse);

	Menus_CloseAll();
	Menus_ActivateByName(lastName);

}


static void UI_SetCapFragLimits(qboolean uiVars) {
	int cap = 5;
	int frag = 10;

	if (uiVars) {
		trap_Cvar_Set("ui_captureLimit", va("%d", cap));
		trap_Cvar_Set("ui_fragLimit", va("%d", frag));
	} else {
		trap_Cvar_Set("capturelimit", va("%d", cap));
		trap_Cvar_Set("fraglimit", va("%d", frag));
	}
}

static const char* UI_GetGameTypeName(int gtEnum)
{
	switch ( gtEnum ) 
	{
	case GT_FFA:
		return UI_GetStringEdString("MENUS", "FREE_FOR_ALL");//"Free For All";
	case GT_SINGLE_PLAYER:
		return UI_GetStringEdString("MENUS", "SAGA");//"Team FFA";
	case GT_DUEL:
		return UI_GetStringEdString("MENUS", "DUEL");//"Team FFA";
	case GT_POWERDUEL:
		return UI_GetStringEdString("MENUS", "POWERDUEL");//"Team FFA";
	case GT_TEAM:
		return UI_GetStringEdString("MENUS", "TEAM_FFA");//"Team FFA";
	case GT_CTF:
		return UI_GetStringEdString("MENUS", "CAPTURE_THE_FLAG");//"Capture the Flag";
	case GT_WARZONE:
		return UI_GetStringEdString("MENUS", "WARZONE");//"Warzone";
	}
	return UI_GetStringEdString("MENUS", "SAGA");//"Team FFA";
}



// ui_gameType assumes gametype 0 is -1 ALL and will not show
static void UI_DrawGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) 
{
	Text_Paint(rect->x, rect->y, scale, color, UI_GetGameTypeName(uiInfo.gameTypes[ui_gameType.integer].gtEnum), 0, 0, textStyle, iMenuFont);
}

static void UI_DrawNetGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) 
{
	if (ui_netGameType.integer < 0 || ui_netGameType.integer >= uiInfo.numGameTypes) 
	{
		trap_Cvar_Set("ui_netGameType", "0");
		trap_Cvar_Set("ui_actualNetGameType", "0");
	}
	Text_Paint(rect->x, rect->y, scale, color, UI_GetGameTypeName(uiInfo.gameTypes[ui_netGameType.integer].gtEnum) , 0, 0, textStyle, iMenuFont);
}

static void UI_DrawAutoSwitch(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) {
	int switchVal = trap_Cvar_VariableValue("cg_autoswitch");
	const char *switchString = "AUTOSWITCH1";
	const char *stripString = NULL;

	switch(switchVal)
	{
	case 2:
		switchString = "AUTOSWITCH2";
		break;
	case 3:
		switchString = "AUTOSWITCH3";
		break;
	case 0:
		switchString = "AUTOSWITCH0";
		break;
	default:
		break;
	}

	stripString = UI_GetStringEdString("MP_INGAME", (char *)switchString);

	if (stripString)
	{
		Text_Paint(rect->x, rect->y, scale, color, stripString, 0, 0, textStyle, iMenuFont);
	}
}

static void UI_DrawJoinGameType(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) 
{
	if (ui_joinGameType.integer < 0 || ui_joinGameType.integer > uiInfo.numJoinGameTypes) 
	{
		trap_Cvar_Set("ui_joinGameType", "0");
	}

	Text_Paint(rect->x, rect->y, scale, color, UI_GetGameTypeName(uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum) , 0, 0, textStyle, iMenuFont);
}



static int UI_TeamIndexFromName(const char *name) {
	int i;

	if (name && *name) {
		for (i = 0; i < uiInfo.teamCount; i++) {
			if (Q_stricmp(name, uiInfo.teamList[i].teamName) == 0) {
				return i;
			}
		}
	} 

	return 0;

}

static void UI_DrawClanLogo(rectDef_t *rect, float scale, vec4_t color) {
	int i;
	i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	if (i >= 0 && i < uiInfo.teamCount) {
		trap_R_SetColor( color );

		if (uiInfo.teamList[i].teamIcon == -1) {
			uiInfo.teamList[i].teamIcon = trap_R_RegisterShaderNoMip(uiInfo.teamList[i].imageName);
			uiInfo.teamList[i].teamIcon_Metal = trap_R_RegisterShaderNoMip(va("%s_metal",uiInfo.teamList[i].imageName));
			uiInfo.teamList[i].teamIcon_Name = trap_R_RegisterShaderNoMip(va("%s_name", uiInfo.teamList[i].imageName));
		}

		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon);
		trap_R_SetColor(NULL);
	}
}

static void UI_DrawClanCinematic(rectDef_t *rect, float scale, vec4_t color) {
	int i;
	i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
	if (i >= 0 && i < uiInfo.teamCount) {

		if (uiInfo.teamList[i].cinematic >= -2) {
			if (uiInfo.teamList[i].cinematic == -1) {
				uiInfo.teamList[i].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.teamList[i].imageName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
			}
			if (uiInfo.teamList[i].cinematic >= 0) {
				trap_CIN_RunCinematic(uiInfo.teamList[i].cinematic);
				trap_CIN_SetExtents(uiInfo.teamList[i].cinematic, rect->x, rect->y, rect->w, rect->h);
				trap_CIN_DrawCinematic(uiInfo.teamList[i].cinematic);
			} else {
				trap_R_SetColor( color );
				UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon_Metal);
				trap_R_SetColor(NULL);
				uiInfo.teamList[i].cinematic = -2;
			}
		} else {
			trap_R_SetColor( color );
			UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.teamList[i].teamIcon);
			trap_R_SetColor(NULL);
		}
	}

}

static void UI_DrawPreviewCinematic(rectDef_t *rect, float scale, vec4_t color) {
	if (uiInfo.previewMovie > -2) {
		uiInfo.previewMovie = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.movieList[uiInfo.movieIndex]), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		if (uiInfo.previewMovie >= 0) {
			trap_CIN_RunCinematic(uiInfo.previewMovie);
			trap_CIN_SetExtents(uiInfo.previewMovie, rect->x, rect->y, rect->w, rect->h);
			trap_CIN_DrawCinematic(uiInfo.previewMovie);
		} else {
			uiInfo.previewMovie = -2;
		}
	} 

}


static void UI_DrawGenericNum(rectDef_t *rect, float scale, vec4_t color, int textStyle, int val, int min, int max, int type,int iMenuFont) 
{
	int i;
	char s[256];

	i = val;
	if (i < min || i > max) 
	{
		i = min;
	}

	Com_sprintf(s, sizeof(s), "%i\0", val);
	Text_Paint(rect->x, rect->y, scale, color, s,0, 0, textStyle, iMenuFont);
}


static void UI_DrawSkinColor(rectDef_t *rect, float scale, vec4_t color, int textStyle, int val, int min, int max, int iMenuFont)
{
	char s[256];

	switch(val)
	{
	case TEAM_RED:
		trap_SP_GetStringTextString("MENUS_TEAM_RED", s, sizeof(s));
		//		Com_sprintf(s, sizeof(s), "Red\0");
		break;
	case TEAM_BLUE:
		trap_SP_GetStringTextString("MENUS_TEAM_BLUE", s, sizeof(s));
		//		Com_sprintf(s, sizeof(s), "Blue\0");
		break;
	default:
		trap_SP_GetStringTextString("MENUS_DEFAULT", s, sizeof(s));
		//		Com_sprintf(s, sizeof(s), "Default\0");
		break;
	}

	Text_Paint(rect->x, rect->y, scale, color, s, 0, 0, textStyle, iMenuFont);
}

static void UI_DrawTeamName(rectDef_t *rect, float scale, vec4_t color, qboolean blue, int textStyle, int iMenuFont) {
	int i;
	i = UI_TeamIndexFromName(UI_Cvar_VariableString((blue) ? "ui_blueTeam" : "ui_redTeam"));
	if (i >= 0 && i < uiInfo.teamCount) {
		Text_Paint(rect->x, rect->y, scale, color, va("%s: %s", (blue) ? "Blue" : "Red", uiInfo.teamList[i].teamName),0, 0, textStyle, iMenuFont);
	}
}

static void UI_DrawTeamMember(rectDef_t *rect, float scale, vec4_t color, qboolean blue, int num, int textStyle, int iMenuFont) 
{
	// 0 - None
	// 1 - Human
	// 2..NumCharacters - Bot
	int value = trap_Cvar_VariableValue(va(blue ? "ui_blueteam%i" : "ui_redteam%i", num));
	const char *text;
	int maxcl = trap_Cvar_VariableValue( "sv_maxClients" );
	vec4_t finalColor;
	int numval = num;

	numval *= 2;

	if (blue)
	{
		numval -= 1;
	}

	finalColor[0] = color[0];
	finalColor[1] = color[1];
	finalColor[2] = color[2];
	finalColor[3] = color[3];

	if (numval > maxcl)
	{
		finalColor[0] *= 0.5;
		finalColor[1] *= 0.5;
		finalColor[2] *= 0.5;

		value = -1;
	}

	if (value <= 1) {
		if (value == -1)
		{
			//text = "Closed";
			text = UI_GetStringEdString("MENUS", "CLOSED");
		}
		else
		{
			//text = "Human";
			text = UI_GetStringEdString("MENUS", "HUMAN");
		}
	} else {
		value -= 2;
		if (value >= UI_GetNumBots()) {
			value = 1;
		}
		text = UI_GetBotNameByNumber(value);
	}

	Text_Paint(rect->x, rect->y, scale, finalColor, text, 0, 0, textStyle, iMenuFont);
}

static void UI_DrawEffects(rectDef_t *rect, float scale, vec4_t color) 
{
	UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiSaberColorShaders[uiInfo.effectsColor]);
}

static void UI_DrawMapPreview(rectDef_t *rect, float scale, vec4_t color, qboolean net) {
	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;
	if (map < 0 || map > uiInfo.mapCount) {
		if (net) {
			ui_currentNetMap.integer = 0;
			trap_Cvar_Set("ui_currentNetMap", "0");
		} else {
			ui_currentMap.integer = 0;
			trap_Cvar_Set("ui_currentMap", "0");
		}
		map = 0;
	}

	if (uiInfo.mapList[map].levelShot == -1) {
		uiInfo.mapList[map].levelShot = trap_R_RegisterShaderNoMip(uiInfo.mapList[map].imageName);
	}

	if (uiInfo.mapList[map].levelShot > 0) {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.mapList[map].levelShot);
	} else {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, trap_R_RegisterShaderNoMip("menu/art/unknownmap_mp"));
	}
}						 


static void UI_DrawMapTimeToBeat(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) {
	int minutes, seconds, time;
	if (ui_currentMap.integer < 0 || ui_currentMap.integer > uiInfo.mapCount) {
		ui_currentMap.integer = 0;
		trap_Cvar_Set("ui_currentMap", "0");
	}

	time = uiInfo.mapList[ui_currentMap.integer].timeToBeat[uiInfo.gameTypes[ui_gameType.integer].gtEnum];

	minutes = time / 60;
	seconds = time % 60;

	Text_Paint(rect->x, rect->y, scale, color, va("%02i:%02i", minutes, seconds), 0, 0, textStyle, iMenuFont);
}

// The game type on create server has changed - make the HUMAN/BOTS fields active 
void UpdateBotButtons(void)
{
	menuDef_t *menu;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return;
	}

	Menu_ShowItemByName(menu, "humanbotfield", qtrue);
	Menu_ShowItemByName(menu, "humanbotnonfield", qfalse);
}

static void UI_DrawNetSource(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) 
{
	//[MasterServer]
	if (ui_netSource.integer < 0 || ui_netSource.integer > AS_GLOBAL5) 
	{
		ui_netSource.integer = 0;
	}
	//[/MasterServer]

	trap_SP_GetStringTextString("MENUS_SOURCE", holdSPString, sizeof(holdSPString) );
	Text_Paint(rect->x, rect->y, scale, color, va("%s %s",holdSPString,
		GetNetSourceString(ui_netSource.integer)), 0, 0, textStyle, iMenuFont);
}

static void UI_DrawNetMapPreview(rectDef_t *rect, float scale, vec4_t color) {

	if (uiInfo.serverStatus.currentServerPreview > 0) {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, uiInfo.serverStatus.currentServerPreview);
	} else {
		UI_DrawHandlePic( rect->x, rect->y, rect->w, rect->h, trap_R_RegisterShaderNoMip("menu/art/unknownmap_mp"));
	}
}

static void UI_DrawNetFilter(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) 
{

	if (ui_serverFilterType.integer < 0 || ui_serverFilterType.integer > numServerFilters) 
	{
		ui_serverFilterType.integer = 0;
	}

	trap_SP_GetStringTextString("MENUS_GAME", holdSPString, sizeof(holdSPString));

	//trap_SP_GetStringTextString(serverFilters[ui_serverFilterType.integer].description, holdSPString2, sizeof(holdSPString2));
	strcpy(holdSPString2,serverFilters[ui_serverFilterType.integer].description); 

	Text_Paint(rect->x, rect->y, scale, color, va("%s %s",holdSPString,
		holdSPString2), 0, 0, textStyle, iMenuFont);
}

static const char *UI_AIFromName(const char *name) {
	int j;
	for (j = 0; j < uiInfo.aliasCount; j++) {
		if (Q_stricmp(uiInfo.aliasList[j].name, name) == 0) {
			return uiInfo.aliasList[j].ai;
		}
	}
	return "Kyle";
}

static void UI_DrawAllMapsSelection(rectDef_t *rect, float scale, vec4_t color, int textStyle, qboolean net, int iMenuFont) {
	int map = (net) ? ui_currentNetMap.integer : ui_currentMap.integer;
	if (map >= 0 && map < uiInfo.mapCount) {
		Text_Paint(rect->x, rect->y, scale, color, uiInfo.mapList[map].mapName, 0, 0, textStyle, iMenuFont);
	}
}

static void UI_DrawOpponentName(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) {
	Text_Paint(rect->x, rect->y, scale, color, UI_Cvar_VariableString("ui_opponentName"), 0, 0, textStyle, iMenuFont);
}

static int UI_OwnerDrawWidth(int ownerDraw, float scale) {
	int i, value, iUse = 0;
	const char *text;
	const char *s = NULL;


	switch (ownerDraw) {
	case UI_SKIN_COLOR:
		switch(uiSkinColor)
		{
		case TEAM_RED:
			//			s = "Red";
			s = (char *)UI_GetStringEdString("MENUS", "TEAM_RED");
			break;
		case TEAM_BLUE:
			//			s = "Blue";
			s = (char *)UI_GetStringEdString("MENUS", "TEAM_BLUE");
			break;
		default:
			//			s = "Default";
			s = (char *)UI_GetStringEdString("MENUS", "DEFAULT");
			break;
		}
		break;
	case UI_GAMETYPE:
		s = uiInfo.gameTypes[ui_gameType.integer].gameType;
		break;
	case UI_BLUETEAMNAME:
		i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_blueTeam"));
		if (i >= 0 && i < uiInfo.teamCount) {
			s = va("%s: %s", (char *)UI_GetStringEdString("MENUS", "TEAM_BLUE"), uiInfo.teamList[i].teamName);
		}
		break;
	case UI_REDTEAMNAME:
		i = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_redTeam"));
		if (i >= 0 && i < uiInfo.teamCount) {
			s = va("%s: %s",  (char *)UI_GetStringEdString("MENUS", "TEAM_RED"), uiInfo.teamList[i].teamName);
		}
		break;
	case UI_BLUETEAM1:
	case UI_BLUETEAM2:
	case UI_BLUETEAM3:
	case UI_BLUETEAM4:
	case UI_BLUETEAM5:
	case UI_BLUETEAM6:
	case UI_BLUETEAM7:
	case UI_BLUETEAM8:
		if (ownerDraw <= UI_BLUETEAM5)
		{
			iUse = ownerDraw-UI_BLUETEAM1 + 1;
		}
		else
		{
			iUse = ownerDraw-274; //unpleasent hack because I don't want to move up all the UI_BLAHTEAM# defines
		}

		value = trap_Cvar_VariableValue(va("ui_blueteam%i", iUse));
		if (value <= 1) {
			text = "Human";
		} else {
			value -= 2;
			if (value >= uiInfo.aliasCount) {
				value = 1;
			}
			text = uiInfo.aliasList[value].name;
		}
		s = va("%i. %s", iUse, text);
		break;
	case UI_REDTEAM1:
	case UI_REDTEAM2:
	case UI_REDTEAM3:
	case UI_REDTEAM4:
	case UI_REDTEAM5:
	case UI_REDTEAM6:
	case UI_REDTEAM7:
	case UI_REDTEAM8:
		if (ownerDraw <= UI_REDTEAM5)
		{
			iUse = ownerDraw-UI_REDTEAM1 + 1;
		}
		else
		{
			iUse = ownerDraw-277; //unpleasent hack because I don't want to move up all the UI_BLAHTEAM# defines
		}

		value = trap_Cvar_VariableValue(va("ui_redteam%i", iUse));
		if (value <= 1) {
			text = "Human";
		} else {
			value -= 2;
			if (value >= uiInfo.aliasCount) {
				value = 1;
			}
			text = uiInfo.aliasList[value].name;
		}
		s = va("%i. %s", iUse, text);
		break;
	case UI_NETSOURCE:
		//[MasterServer]
		if (ui_netSource.integer < 0 || ui_netSource.integer > AS_GLOBAL5) {
			ui_netSource.integer = 0;
		}
		//[/MasterServer]
		trap_SP_GetStringTextString("MENUS_SOURCE", holdSPString, sizeof(holdSPString));
		s = va("%s %s", holdSPString, GetNetSourceString(ui_netSource.integer));
		break;
	case UI_NETFILTER:
		if (ui_serverFilterType.integer < 0 || ui_serverFilterType.integer > numServerFilters) {
			ui_serverFilterType.integer = 0;
		}
		trap_SP_GetStringTextString("MENUS_GAME", holdSPString, sizeof(holdSPString));
		//trap_SP_GetStringTextString(serverFilters[ui_serverFilterType.integer].description, holdSPString2, sizeof(holdSPString2));
		strcpy(holdSPString2, serverFilters[ui_serverFilterType.integer].description);

		s = va("%s %s", holdSPString, holdSPString2 );
		break;
	case UI_ALLMAPS_SELECTION:
		break;
	case UI_OPPONENT_NAME:
		break;
	case UI_KEYBINDSTATUS:
		if (Display_KeyBindPending()) {
			s = UI_GetStringEdString("MP_INGAME", "WAITING_FOR_NEW_KEY");
		} else {
			//	s = "Press ENTER or CLICK to change, Press BACKSPACE to clear";
		}
		break;
	case UI_SERVERREFRESHDATE:
		s = UI_Cvar_VariableString(va("ui_lastServerRefresh_%i", ui_netSource.integer));
		break;
	default:
		break;
	}

	if (s) {
		return Text_Width(s, scale, 0);
	}
	return 0;
}

static void UI_DrawBotName(rectDef_t *rect, float scale, vec4_t color, int textStyle,int iMenuFont) 
{
	int value = uiInfo.botIndex;
	const char *text = "";
	if (value >= UI_GetNumBots()) {
		value = 0;
	}
	text = UI_GetBotNameByNumber(value);
	Text_Paint(rect->x, rect->y, scale, color, text, 0, 0, textStyle,iMenuFont);
}

static void UI_DrawBotSkill(rectDef_t *rect, float scale, vec4_t color, int textStyle,int iMenuFont) 
{
	if (uiInfo.skillIndex >= 0 && uiInfo.skillIndex < numSkillLevels) 
	{
		Text_Paint(rect->x, rect->y, scale, color, (char *)UI_GetStringEdString("MP_INGAME", (char *)skillLevels[uiInfo.skillIndex]), 0, 0, textStyle,iMenuFont);
	}
}

static void UI_DrawRedBlue(rectDef_t *rect, float scale, vec4_t color, int textStyle,int iMenuFont) 
{
	Text_Paint(rect->x, rect->y, scale, color, (uiInfo.redBlue == 0) ? UI_GetStringEdString("MP_INGAME","RED") : UI_GetStringEdString("MP_INGAME","BLUE"), 0, 0, textStyle,iMenuFont);
}

static void UI_DrawCrosshair(rectDef_t *rect, float scale, vec4_t color) {
	float size = 32.0f;

	trap_R_SetColor( color );
	if (uiInfo.currentCrosshair < 0 || uiInfo.currentCrosshair >= NUM_CROSSHAIRS) {
		uiInfo.currentCrosshair = 0;
	}

	//Raz: Don't stretch crosshairs
	size = min( rect->w, rect->h );
	UI_DrawHandlePic( rect->x, rect->y, size, size, uiInfo.uiDC.Assets.crosshairShader[uiInfo.currentCrosshair]);
	trap_R_SetColor( NULL );
}

/*
===============
UI_BuildPlayerList
===============
*/
static void UI_BuildPlayerList() {
	uiClientState_t	cs;
	int		n, count, team, team2, playerTeamNumber;
	char	info[MAX_INFO_STRING];

	trap_GetClientState( &cs );
	trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );
	uiInfo.playerNumber = cs.clientNum;
	uiInfo.teamLeader = atoi(Info_ValueForKey(info, "tl"));
	team = atoi(Info_ValueForKey(info, "t"));
	trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) );
	count = atoi( Info_ValueForKey( info, "sv_maxclients" ) );
	uiInfo.playerCount = 0;
	uiInfo.myTeamCount = 0;
	playerTeamNumber = 0;
	for( n = 0; n < count; n++ ) {
		trap_GetConfigString( CS_PLAYERS + n, info, MAX_INFO_STRING );

		if (info[0]) {
			Q_strncpyz( uiInfo.playerNames[uiInfo.playerCount], Info_ValueForKey( info, "n" ), MAX_NAME_LENGTH );
			Q_CleanStr( uiInfo.playerNames[uiInfo.playerCount] );
			uiInfo.playerIndexes[uiInfo.playerCount] = n;
			uiInfo.playerCount++;
			team2 = atoi(Info_ValueForKey(info, "t"));
			if (team2 == team && n != uiInfo.playerNumber) {
				Q_strncpyz( uiInfo.teamNames[uiInfo.myTeamCount], Info_ValueForKey( info, "n" ), MAX_NAME_LENGTH );
				Q_CleanStr( uiInfo.teamNames[uiInfo.myTeamCount] );
				uiInfo.teamClientNums[uiInfo.myTeamCount] = n;
				if (uiInfo.playerNumber == n) {
					playerTeamNumber = uiInfo.myTeamCount;
				}
				uiInfo.myTeamCount++;
			}
		}
	}

	if (!uiInfo.teamLeader) {
		trap_Cvar_Set("cg_selectedPlayer", va("%d", playerTeamNumber));
	}

	n = trap_Cvar_VariableValue("cg_selectedPlayer");
	if (n < 0 || n > uiInfo.myTeamCount) {
		n = 0;
	}


	if (n < uiInfo.myTeamCount) {
		trap_Cvar_Set("cg_selectedPlayerName", uiInfo.teamNames[n]);
	}
	else
	{
		trap_Cvar_Set("cg_selectedPlayerName", "Everyone");
	}

	if (!team || team == TEAM_SPECTATOR || !uiInfo.teamLeader)
	{
		n = uiInfo.myTeamCount;
		trap_Cvar_Set("cg_selectedPlayer", va("%d", n));
		trap_Cvar_Set("cg_selectedPlayerName", "N/A");
	}
}


static void UI_DrawSelectedPlayer(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) {
	if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) {
		uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
		UI_BuildPlayerList();
	}
	Text_Paint(rect->x, rect->y, scale, color, UI_Cvar_VariableString("cg_selectedPlayerName"), 0, 0, textStyle, iMenuFont);
}

static void UI_DrawServerRefreshDate(rectDef_t *rect, float scale, vec4_t color, int textStyle, int iMenuFont) 
{
	if (uiInfo.serverStatus.refreshActive) 
	{
		vec4_t lowLight, newColor;
		lowLight[0] = 0.8 * color[0]; 
		lowLight[1] = 0.8 * color[1]; 
		lowLight[2] = 0.8 * color[2]; 
		lowLight[3] = 0.8 * color[3]; 
		LerpColor(color,lowLight,newColor,0.5+0.5*sin((float)(uiInfo.uiDC.realTime / PULSE_DIVISOR)));

		trap_SP_GetStringTextString("MP_INGAME_GETTINGINFOFORSERVERS", holdSPString, sizeof(holdSPString));
		//[MasterServer]
		Text_Paint(rect->x, rect->y, scale, newColor, va((char *) holdSPString, trap_LAN_GetServerCount(UI_SourceForLAN())), 0, 0, textStyle, iMenuFont);
		//[/MasterServer]
	} 
	else 
	{
		char buff[64];
		Q_strncpyz(buff, UI_Cvar_VariableString(va("ui_lastServerRefresh_%i", ui_netSource.integer)), 64);
		trap_SP_GetStringTextString("MP_INGAME_SERVER_REFRESHTIME", holdSPString, sizeof(holdSPString));

		Text_Paint(rect->x, rect->y, scale, color, va("%s: %s", holdSPString, buff), 0, 0, textStyle, iMenuFont);
	}
}

static void UI_DrawServerMOTD(rectDef_t *rect, float scale, vec4_t color, int iMenuFont) {
	if (uiInfo.serverStatus.motdLen) {
		float maxX;

		if (uiInfo.serverStatus.motdWidth == -1) {
			uiInfo.serverStatus.motdWidth = 0;
			uiInfo.serverStatus.motdPaintX = rect->x + 1;
			uiInfo.serverStatus.motdPaintX2 = -1;
		}

		if (uiInfo.serverStatus.motdOffset > uiInfo.serverStatus.motdLen) {
			uiInfo.serverStatus.motdOffset = 0;
			uiInfo.serverStatus.motdPaintX = rect->x + 1;
			uiInfo.serverStatus.motdPaintX2 = -1;
		}

		if (uiInfo.uiDC.realTime > uiInfo.serverStatus.motdTime) {
			uiInfo.serverStatus.motdTime = uiInfo.uiDC.realTime + 10;
			if (uiInfo.serverStatus.motdPaintX <= rect->x + 2) {
				if (uiInfo.serverStatus.motdOffset < uiInfo.serverStatus.motdLen) {
					uiInfo.serverStatus.motdPaintX += Text_Width(&uiInfo.serverStatus.motd[uiInfo.serverStatus.motdOffset], scale, 1) - 1;
					uiInfo.serverStatus.motdOffset++;
				} else {
					uiInfo.serverStatus.motdOffset = 0;
					if (uiInfo.serverStatus.motdPaintX2 >= 0) {
						uiInfo.serverStatus.motdPaintX = uiInfo.serverStatus.motdPaintX2;
					} else {
						uiInfo.serverStatus.motdPaintX = rect->x + rect->w - 2;
					}
					uiInfo.serverStatus.motdPaintX2 = -1;
				}
			} else {
				//serverStatus.motdPaintX--;
				uiInfo.serverStatus.motdPaintX -= 2;
				if (uiInfo.serverStatus.motdPaintX2 >= 0) {
					//serverStatus.motdPaintX2--;
					uiInfo.serverStatus.motdPaintX2 -= 2;
				}
			}
		}

		maxX = rect->x + rect->w - 2;
		Text_Paint_Limit(&maxX, uiInfo.serverStatus.motdPaintX, rect->y + rect->h - 3, scale, color, &uiInfo.serverStatus.motd[uiInfo.serverStatus.motdOffset], 0, 0, iMenuFont); 
		if (uiInfo.serverStatus.motdPaintX2 >= 0) {
			float maxX2 = rect->x + rect->w - 2;
			Text_Paint_Limit(&maxX2, uiInfo.serverStatus.motdPaintX2, rect->y + rect->h - 3, scale, color, uiInfo.serverStatus.motd, 0, uiInfo.serverStatus.motdOffset, iMenuFont); 
		}
		if (uiInfo.serverStatus.motdOffset && maxX > 0) {
			// if we have an offset ( we are skipping the first part of the string ) and we fit the string
			if (uiInfo.serverStatus.motdPaintX2 == -1) {
				uiInfo.serverStatus.motdPaintX2 = rect->x + rect->w - 2;
			}
		} else {
			uiInfo.serverStatus.motdPaintX2 = -1;
		}

	}
}

static void UI_DrawKeyBindStatus(rectDef_t *rect, float scale, vec4_t color, int textStyle,int iMenuFont) {
//	int ofs = 0; TTimo: unused
	if (Display_KeyBindPending()) 
	{
		Text_Paint(rect->x, rect->y, scale, color, UI_GetStringEdString("MP_INGAME", "WAITING_FOR_NEW_KEY"), 0, 0, textStyle,iMenuFont);
	} else {
//		Text_Paint(rect->x, rect->y, scale, color, "Press ENTER or CLICK to change, Press BACKSPACE to clear", 0, 0, textStyle,iMenuFont);
	}
}

static void UI_DrawGLInfo(rectDef_t *rect, float scale, vec4_t color, int textStyle,int iMenuFont) 
{
	char * eptr;
	char buff[4096];
	const char *lines[128];
	int y, numLines, i;

	Text_Paint(rect->x + 2, rect->y, scale, color, va("GL_VENDOR: %s", uiInfo.uiDC.glconfig.vendor_string), 0, rect->w, textStyle,iMenuFont);
	Text_Paint(rect->x + 2, rect->y + 15, scale, color, va("GL_VERSION: %s: %s", uiInfo.uiDC.glconfig.version_string,uiInfo.uiDC.glconfig.renderer_string), 0, rect->w, textStyle,iMenuFont);
	Text_Paint(rect->x + 2, rect->y + 30, scale, color, va ("GL_PIXELFORMAT: color(%d-bits) Z(%d-bits) stencil(%d-bits)", uiInfo.uiDC.glconfig.colorBits, uiInfo.uiDC.glconfig.depthBits, uiInfo.uiDC.glconfig.stencilBits), 0, rect->w, textStyle,iMenuFont);

	// build null terminated extension strings
	Q_strncpyz(buff, uiInfo.uiDC.glconfig.extensions_string, 4096);
	eptr = buff;
	y = rect->y + 45;
	numLines = 0;
	while ( y < rect->y + rect->h && *eptr )
	{
		while ( *eptr && *eptr == ' ' )
			*eptr++ = '\0';

		// track start of valid string
		if (*eptr && *eptr != ' ') 
		{
			lines[numLines++] = eptr;
		}

		while ( *eptr && *eptr != ' ' )
			eptr++;
	}

	i = 0;
	while (i < numLines) 
	{
		Text_Paint(rect->x + 2, y, scale, color, lines[i++], 0, (rect->w/2), textStyle,iMenuFont);
		if (i < numLines) 
		{
			Text_Paint(rect->x + rect->w / 2, y, scale, color, lines[i++], 0, (rect->w/2), textStyle,iMenuFont);
		}
		y += 10;
		if (y > rect->y + rect->h - 11) 
		{
			break;
		}
	}


}

/*
=================
UI_Version
=================
*/
static void UI_Version(rectDef_t *rect, float scale, vec4_t color, int iMenuFont) 
{
	int width;

	width = uiInfo.uiDC.textWidth(JK_VERSION, scale, iMenuFont);

	uiInfo.uiDC.drawText(rect->x - width, rect->y, scale, color, JK_VERSION, 0, 0, 0, iMenuFont);
}

/*
=================
UI_OwnerDraw
=================
*/
// FIXME: table drive
//

extern void JKG_GangWars_TeamREDText( rectDef_t *rect, float scale, vec4_t color, int iMenuFont );
extern void JKG_GangWars_TeamBLUEText( rectDef_t *rect, float scale, vec4_t color, int iMenuFont );

static void UI_OwnerDraw(itemDef_t *item, float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle,int iMenuFont, int ownerDrawID) 
{
	rectDef_t rect;
	int drawRank = 0, iUse = 0;

	rect.x = x + text_x;
	rect.y = y + text_y;
	rect.w = w;
	rect.h = h;


	switch (ownerDraw)
	{
	case UI_JKG_SLICE_DLGTEXT:
		JKG_Slice_DrawDialog(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_SLICE_INTRUSION:
		JKG_Slice_DrawIntrusion(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_SLICE_WARNING:
		JKG_Slice_DrawWarningLevel(x, y, w, h);
		break;
	case UI_JKG_SLICE_CLEARANCE:
		JKG_Slice_DrawSecurityClearance(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_SLICE_SUMMARY:
		JKG_Slice_DrawGridSummary(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_SLICE_GRID:
		JKG_Slice_DrawGridSlot(ownerDrawID, x, y, w, h);
		break;
	case UI_PARTYMNGT_DLGTEXT:
		JKG_PartyMngt_DrawDialog(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_WAITING:
		JKG_Pazaak_DrawWaiting(x, y, w, h, shader);
		break;
	case UI_JKG_PAZAAK_SELSD:
		JKG_Pazaak_DrawSideDeckSlot(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_SELCARD:
		JKG_Pazaak_DrawSelCardSlot(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_TIMEOUT:
		JKG_Pazaak_DrawTimeout(x, y, w, h);
		break;
	case UI_JKG_PAZAAK_DLGTEXT:
		JKG_Pazaak_DrawDialog(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_NAMES:
		JKG_Pazaak_DrawNames(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_POINTS:
		JKG_Pazaak_DrawPoints(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_BCARD:
		JKG_Pazaak_DrawCardSlot(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_PAZAAK_BHAND:
		JKG_Pazaak_DrawHandSlot(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_CONVO_GOUP:
	case UI_JKG_CONVO_GODOWN:
		Conv_OwnerDraw_ScrollButtons(ownerDraw - UI_JKG_CONVO_GOUP, &rect, color);
		break;
	case UI_JKG_CONVO_TEXT:
		Conv_OwnerDraw_Text(&rect, scale, color, iMenuFont);
		break;
	case UI_JKG_CONVO_LASTTEXT:
		Conv_OwnerDraw_LastText(&rect, scale, color, iMenuFont);
		break;
	case UI_JKG_CONVO_TECAPTION:
		Conv_OwnerDraw_TECaption(&rect, scale, color, iMenuFont);
		break;
	case UI_JKG_CONVO_OPT1:
	case UI_JKG_CONVO_OPT2:
	case UI_JKG_CONVO_OPT3:
	case UI_JKG_CONVO_OPT4:
		Conv_OwnerDraw_Choices(ownerDraw-UI_JKG_CONVO_OPT1,&rect, scale, color, iMenuFont);
		break;
	case UI_JKG_LOOT_ITEM:
		JKG_Loot_DrawItemInstanceSlot(ownerDrawID, x, y, w, h);
		break;
	case UI_JKG_LOOT_NAME:
	case UI_JKG_LOOT_ICON:
	case UI_JKG_LOOT_DESC:
	case UI_JKG_LOOT_WEIG:
	case UI_JKG_LOOT_DLG:
		JKG_Loot_DrawDialog(ownerDrawID, x, y, w, h);
		break; //bugfix

	case UI_JKG_INV_TOOLTIP:
		JKG_Inventory_DrawTooltip();
		break;

	case UI_JKG_TOOLTIP:
		JKG_Draw_Tooltip(item);
		break;

	case UI_JKG_TEAMRED:
		//JKG_GangWars_TeamREDText(iMenuFont, &rect, scale, color);
		item->text[0] = '\0';
		rect.x -= text_x;
		JKG_GangWars_TeamREDText(&rect, scale, color, iMenuFont);
		break;
	case UI_JKG_TEAMBLUE:
		//JKG_GangWars_TeamBLUEText(iMenuFont, &rect, scale, color);
		item->text[0] = '\0';
		rect.x -= text_x;
		JKG_GangWars_TeamBLUEText(&rect, scale, color, iMenuFont);
		break;
	case UI_SKIN_COLOR:
		UI_DrawSkinColor(&rect, scale, color, textStyle, uiSkinColor, TEAM_FREE, TEAM_BLUE, iMenuFont);
		break;
	case UI_EFFECTS:
		UI_DrawEffects(&rect, scale, color);
		break;
	case UI_GAMETYPE:
		UI_DrawGameType(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_NETGAMETYPE:
		UI_DrawNetGameType(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_AUTOSWITCHLIST:
		UI_DrawAutoSwitch(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_JOINGAMETYPE:
		UI_DrawJoinGameType(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_TOTALFORCESTARS:
		//      UI_DrawTotalForceStars(&rect, scale, color, textStyle);
		break;
	case UI_BLUETEAMNAME:
		UI_DrawTeamName(&rect, scale, color, qtrue, textStyle, iMenuFont);
		break;
	case UI_REDTEAMNAME:
		UI_DrawTeamName(&rect, scale, color, qfalse, textStyle, iMenuFont);
		break;
	case UI_BLUETEAM1:
	case UI_BLUETEAM2:
	case UI_BLUETEAM3:
	case UI_BLUETEAM4:
	case UI_BLUETEAM5:
	case UI_BLUETEAM6:
	case UI_BLUETEAM7:
	case UI_BLUETEAM8:
		if (ownerDraw <= UI_BLUETEAM5)
		{
			iUse = ownerDraw-UI_BLUETEAM1 + 1;
		}
		else
		{
			iUse = ownerDraw-274; //unpleasent hack because I don't want to move up all the UI_BLAHTEAM# defines
		}
		UI_DrawTeamMember(&rect, scale, color, qtrue, iUse, textStyle, iMenuFont);
		break;
	case UI_REDTEAM1:
	case UI_REDTEAM2:
	case UI_REDTEAM3:
	case UI_REDTEAM4:
	case UI_REDTEAM5:
	case UI_REDTEAM6:
	case UI_REDTEAM7:
	case UI_REDTEAM8:
		if (ownerDraw <= UI_REDTEAM5)
		{
			iUse = ownerDraw-UI_REDTEAM1 + 1;
		}
		else
		{
			iUse = ownerDraw-277; //unpleasent hack because I don't want to move up all the UI_BLAHTEAM# defines
		}
		UI_DrawTeamMember(&rect, scale, color, qfalse, iUse, textStyle, iMenuFont);
		break;
	case UI_NETSOURCE:
		UI_DrawNetSource(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_NETMAPPREVIEW:
		UI_DrawNetMapPreview(&rect, scale, color);
		break;
	case UI_NETFILTER:
		UI_DrawNetFilter(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_ALLMAPS_SELECTION:
		UI_DrawAllMapsSelection(&rect, scale, color, textStyle, qtrue, iMenuFont);
		break;
	case UI_MAPS_SELECTION:
		UI_DrawAllMapsSelection(&rect, scale, color, textStyle, qfalse, iMenuFont);
		break;
	case UI_OPPONENT_NAME:
		UI_DrawOpponentName(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_BOTNAME:
		UI_DrawBotName(&rect, scale, color, textStyle,iMenuFont);
		break;
	case UI_BOTSKILL:
		UI_DrawBotSkill(&rect, scale, color, textStyle,iMenuFont);
		break;
	case UI_REDBLUE:
		UI_DrawRedBlue(&rect, scale, color, textStyle,iMenuFont);
		break;
	case UI_CROSSHAIR:
		UI_DrawCrosshair(&rect, scale, color);
		break;
	case UI_SELECTEDPLAYER:
		UI_DrawSelectedPlayer(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_SERVERREFRESHDATE:
		UI_DrawServerRefreshDate(&rect, scale, color, textStyle, iMenuFont);
		break;
	case UI_SERVERMOTD:
		UI_DrawServerMOTD(&rect, scale, color, iMenuFont);
		break;
	case UI_GLINFO:
		UI_DrawGLInfo(&rect,scale, color, textStyle, iMenuFont);
		break;
	case UI_KEYBINDSTATUS:
		UI_DrawKeyBindStatus(&rect,scale, color, textStyle,iMenuFont);
		break;
	case UI_VERSION:
		UI_Version(&rect, scale, color, iMenuFont);
		break;
	default:
		break;
	}

}

static qboolean UI_OwnerDrawVisible(int flags) {
	qboolean vis = qtrue;

	while (flags) {

		if (flags & UI_SHOW_FFA) {
			if (trap_Cvar_VariableValue("g_gametype") != GT_FFA && trap_Cvar_VariableValue("g_gametype")) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_FFA;
		}

		if (flags & UI_SHOW_NOTFFA) {
			if (trap_Cvar_VariableValue("g_gametype") == GT_FFA) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NOTFFA;
		}

		if (flags & UI_SHOW_LEADER) {
			// these need to show when this client can give orders to a player or a group
			if (!uiInfo.teamLeader) {
				vis = qfalse;
			} else {
				// if showing yourself
				if (ui_selectedPlayer.integer < uiInfo.myTeamCount && uiInfo.teamClientNums[ui_selectedPlayer.integer] == uiInfo.playerNumber) { 
					vis = qfalse;
				}
			}
			flags &= ~UI_SHOW_LEADER;
		} 
		if (flags & UI_SHOW_NOTLEADER) {
			// these need to show when this client is assigning their own status or they are NOT the leader
			if (uiInfo.teamLeader) {
				// if not showing yourself
				if (!(ui_selectedPlayer.integer < uiInfo.myTeamCount && uiInfo.teamClientNums[ui_selectedPlayer.integer] == uiInfo.playerNumber)) { 
					vis = qfalse;
				}
				// these need to show when this client can give orders to a player or a group
			}
			flags &= ~UI_SHOW_NOTLEADER;
		} 
		if (flags & UI_SHOW_FAVORITESERVERS) {
			// this assumes you only put this type of display flag on something showing in the proper context
			if (ui_netSource.integer != AS_FAVORITES) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_FAVORITESERVERS;
		} 
		if (flags & UI_SHOW_NOTFAVORITESERVERS) {
			// this assumes you only put this type of display flag on something showing in the proper context
			if (ui_netSource.integer == AS_FAVORITES) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NOTFAVORITESERVERS;
		} 
		if (flags & UI_SHOW_ANYTEAMGAME) {
			if (uiInfo.gameTypes[ui_gameType.integer].gtEnum <= GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYTEAMGAME;
		} 
		if (flags & UI_SHOW_ANYNONTEAMGAME) {
			if (uiInfo.gameTypes[ui_gameType.integer].gtEnum > GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_ANYNONTEAMGAME;
		} 
		if (flags & UI_SHOW_NETANYTEAMGAME) {
			if (uiInfo.gameTypes[ui_netGameType.integer].gtEnum <= GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYTEAMGAME;
		} 
		if (flags & UI_SHOW_NETANYNONTEAMGAME) {
			if (uiInfo.gameTypes[ui_netGameType.integer].gtEnum > GT_TEAM ) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NETANYNONTEAMGAME;
		} 
		if (flags & UI_SHOW_NEWHIGHSCORE) {
			if (uiInfo.newHighScoreTime < uiInfo.uiDC.realTime) {
				vis = qfalse;
			} else {
				if (uiInfo.soundHighScore) {
					if (trap_Cvar_VariableValue("sv_killserver") == 0) {
						// wait on server to go down before playing sound
						//trap_S_StartLocalSound(uiInfo.newHighScoreSound, CHAN_ANNOUNCER);
						uiInfo.soundHighScore = qfalse;
					}
				}
			}
			flags &= ~UI_SHOW_NEWHIGHSCORE;
		} 
		if (flags & UI_SHOW_NEWBESTTIME) {
			if (uiInfo.newBestTime < uiInfo.uiDC.realTime) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_NEWBESTTIME;
		} 
		if (flags & UI_SHOW_DEMOAVAILABLE) {
			if (!uiInfo.demoAvailable) {
				vis = qfalse;
			}
			flags &= ~UI_SHOW_DEMOAVAILABLE;
		} else {
			flags = 0;
		}
	}
	return vis;
}

static qboolean UI_Handicap_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int h;
		h = Com_Clamp( 5, 100, trap_Cvar_VariableValue("handicap") );
		if (key == A_MOUSE2) {
			h -= 5;
		} else {
			h += 5;
		}
		if (h > 100) {
			h = 5;
		} else if (h < 0) {
			h = 100;
		}
		trap_Cvar_Set( "handicap", va( "%i", h) );
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_Effects_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int team = (int)(trap_Cvar_VariableValue("ui_myteam"));

		if (team == TEAM_RED || team==TEAM_BLUE)
			return qfalse;

		if (key == A_MOUSE2) {
			uiInfo.effectsColor--;
		} else {
			uiInfo.effectsColor++;
		}

		if( uiInfo.effectsColor > 5 ) {
			uiInfo.effectsColor = 0;
		} else if (uiInfo.effectsColor < 0) {
			uiInfo.effectsColor = 5;
		}

		trap_Cvar_SetValue( "color1", /*uitogamecode[uiInfo.effectsColor]*/uiInfo.effectsColor );
		return qtrue;
	}
	return qfalse;
}

extern void	Item_RunScript(itemDef_t *item, const char *s);		//from ui_shared;

// For hot keys on the chat main menu.
static qboolean UI_Chat_Main_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "attack");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "defend");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "request");
	}
	else if ((key == A_4) || ( key == A_STRING))
	{
		item = Menu_FindItemByName(menu, "reply");
	}
	else if ((key == A_5) || ( key == A_PERCENT))
	{
		item = Menu_FindItemByName(menu, "spot");
	}
	else if ((key == A_6) || ( key == A_CARET))
	{
		item = Menu_FindItemByName(menu, "tactics");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

// For hot keys on the chat main menu.
static qboolean UI_Chat_Attack_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "att_01");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "att_02");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "att_03");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

// For hot keys on the chat main menu.
static qboolean UI_Chat_Defend_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "def_01");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "def_02");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "def_03");
	}
	else if ((key == A_4) || ( key == A_STRING))
	{
		item = Menu_FindItemByName(menu, "def_04");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

// For hot keys on the chat main menu.
static qboolean UI_Chat_Request_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "req_01");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "req_02");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "req_03");
	}
	else if ((key == A_4) || ( key == A_STRING))
	{
		item = Menu_FindItemByName(menu, "req_04");
	}
	else if ((key == A_5) || ( key == A_PERCENT))
	{
		item = Menu_FindItemByName(menu, "req_05");
	}
	else if ((key == A_6) || ( key == A_CARET))
	{
		item = Menu_FindItemByName(menu, "req_06");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

// For hot keys on the chat main menu.
static qboolean UI_Chat_Reply_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "rep_01");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "rep_02");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "rep_03");
	}
	else if ((key == A_4) || ( key == A_STRING))
	{
		item = Menu_FindItemByName(menu, "rep_04");
	}
	else if ((key == A_5) || ( key == A_PERCENT))
	{
		item = Menu_FindItemByName(menu, "rep_05");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

// For hot keys on the chat main menu.
static qboolean UI_Chat_Spot_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "spot_01");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "spot_02");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "spot_03");
	}
	else if ((key == A_4) || ( key == A_STRING))
	{
		item = Menu_FindItemByName(menu, "spot_04");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

// For hot keys on the chat main menu.
static qboolean UI_Chat_Tactical_HandleKey(int key) 
{
	menuDef_t *menu;
	itemDef_t *item;

	menu = Menu_GetFocused();	

	if (!menu)
	{
		return (qfalse);
	}

	if ((key == A_1) || ( key == A_PLING))
	{
		item = Menu_FindItemByName(menu, "tac_01");
	}
	else if ((key == A_2) || ( key == A_AT))
	{
		item = Menu_FindItemByName(menu, "tac_02");
	}
	else if ((key == A_3) || ( key == A_HASH))
	{
		item = Menu_FindItemByName(menu, "tac_03");
	}
	else if ((key == A_4) || ( key == A_STRING))
	{
		item = Menu_FindItemByName(menu, "tac_04");
	}
	else if ((key == A_5) || ( key == A_PERCENT))
	{
		item = Menu_FindItemByName(menu, "tac_05");
	}
	else if ((key == A_6) || ( key == A_CARET))
	{
		item = Menu_FindItemByName(menu, "tac_06");
	}
	else
	{
		return (qfalse);
	}

	if (item)
	{
		Item_RunScript(item, item->action);
	}

	return (qtrue);
}

static qboolean UI_GameType_HandleKey(int flags, float *special, int key, qboolean resetMap) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int oldCount = UI_MapCountByGameType(qtrue);

		// hard coded mess here
		if (key == A_MOUSE2) {
			ui_gameType.integer--;
			if (ui_gameType.integer == 2) {
				ui_gameType.integer = 1;
			} else if (ui_gameType.integer < 2) {
				ui_gameType.integer = uiInfo.numGameTypes - 1;
			}
		} else {
			ui_gameType.integer++;
			if (ui_gameType.integer >= uiInfo.numGameTypes) {
				ui_gameType.integer = 1;
			} else if (ui_gameType.integer == 2) {
				ui_gameType.integer = 3;
			}
		}

		trap_Cvar_Set("ui_gameType", va("%d", ui_gameType.integer));
		UI_SetCapFragLimits(qtrue);
		if (resetMap && oldCount != UI_MapCountByGameType(qtrue)) {
			trap_Cvar_Set( "ui_currentMap", "0");
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, 0, NULL);
		}
		return qtrue;
	}
	return qfalse;
}

// If we're in the solo menu, don't let them see siege maps.
static qboolean UI_InSoloMenu( void )
{
	menuDef_t *menu;
	itemDef_t *item;
	char *name = "solo_gametypefield";

	menu = Menu_GetFocused();	// Get current menu (either video or ingame video, I would assume)

	if (!menu)
	{
		return (qfalse);
	}

	item = Menu_FindItemByName(menu, name);
	if (item)
	{
		return qtrue;
	}

	return (qfalse);
}

static qboolean UI_NetGameType_HandleKey(int flags, float *special, int key) 
{
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) 
	{

		if (key == A_MOUSE2) 
		{
			ui_netGameType.integer--;
		} 
		else 
		{
			ui_netGameType.integer++;
		}

		if (ui_netGameType.integer < 0) 
		{
			ui_netGameType.integer = uiInfo.numGameTypes - 1;
		} 
		else if (ui_netGameType.integer >= uiInfo.numGameTypes) 
		{
			ui_netGameType.integer = 0;
		} 

		trap_Cvar_Set( "ui_netGameType", va("%d", ui_netGameType.integer));
		trap_Cvar_Set( "ui_actualnetGameType", va("%d", uiInfo.gameTypes[ui_netGameType.integer].gtEnum));
		trap_Cvar_Set( "ui_currentNetMap", "0");
		UI_MapCountByGameType(qfalse);
		Menu_SetFeederSelection(NULL, FEEDER_ALLMAPS, 0, NULL);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_AutoSwitch_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int switchVal = trap_Cvar_VariableValue("cg_autoswitch");

		if (key == A_MOUSE2) {
			switchVal--;
		} else {
			switchVal++;
		}

		if (switchVal < 0)
		{
			switchVal = 2;
		}
		else if (switchVal >= 3)
		{
			switchVal = 0;
		} 

		trap_Cvar_Set( "cg_autoswitch", va("%i", switchVal));
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_JoinGameType_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {

		if (key == A_MOUSE2) {
			ui_joinGameType.integer--;
		} else {
			ui_joinGameType.integer++;
		}

		if (ui_joinGameType.integer < 0) {
			ui_joinGameType.integer = uiInfo.numJoinGameTypes - 1;
		} else if (ui_joinGameType.integer >= uiInfo.numJoinGameTypes) {
			ui_joinGameType.integer = 0;
		}

		trap_Cvar_Set( "ui_joinGameType", va("%d", ui_joinGameType.integer));
		UI_BuildServerDisplayList(1);
		return qtrue;
	}
	return qfalse;
}



static qboolean UI_Skill_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int i = trap_Cvar_VariableValue( "g_spSkill" );

		if (key == A_MOUSE2) {
			i--;
		} else {
			i++;
		}

		if (i < 1) {
			i = numSkillLevels;
		} else if (i > numSkillLevels) {
			i = 1;
		}

		trap_Cvar_Set("g_spSkill", va("%i", i));
		return qtrue;
	}
	return qfalse;
}


static qboolean UI_TeamName_HandleKey(int flags, float *special, int key, qboolean blue) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int i;
		i = UI_TeamIndexFromName(UI_Cvar_VariableString((blue) ? "ui_blueTeam" : "ui_redTeam"));

		if (key == A_MOUSE2) {
			i--;
		} else {
			i++;
		}

		if (i >= uiInfo.teamCount) {
			i = 0;
		} else if (i < 0) {
			i = uiInfo.teamCount - 1;
		}

		trap_Cvar_Set( (blue) ? "ui_blueTeam" : "ui_redTeam", uiInfo.teamList[i].teamName);

		return qtrue;
	}
	return qfalse;
}

static qboolean UI_TeamMember_HandleKey(int flags, float *special, int key, qboolean blue, int num) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		// 0 - None
		// 1 - Human
		// 2..NumCharacters - Bot
		char *cvar = va(blue ? "ui_blueteam%i" : "ui_redteam%i", num);
		int value = trap_Cvar_VariableValue(cvar);
		int maxcl = trap_Cvar_VariableValue( "sv_maxClients" );
		int numval = num;

		numval *= 2;

		if (blue)
		{
			numval -= 1;
		}

		if (numval > maxcl)
		{
			return qfalse;
		}

		if (value < 1)
		{
			value = 1;
		}

		if (key == A_MOUSE2) {
			value--;
		} else {
			value++;
		}

		/*if (ui_actualNetGameType.integer >= GT_TEAM) {
		if (value >= uiInfo.characterCount + 2) {
		value = 0;
		} else if (value < 0) {
		value = uiInfo.characterCount + 2 - 1;
		}
		} else {*/
		if (value >= UI_GetNumBots() + 2) {
			value = 1;
		} else if (value < 1) {
			value = UI_GetNumBots() + 2 - 1;
		}
		//}

		trap_Cvar_Set(cvar, va("%i", value));
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetSource_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {

		if (key == A_MOUSE2) {
			ui_netSource.integer--;
		} else {
			ui_netSource.integer++;
		}

		//[MasterServer]
		if(ui_netSource.integer >= AS_GLOBAL2 && ui_netSource.integer <= AS_GLOBAL5)
		{
			char masterstr[2], cvarname[sizeof("sv_master1")];
			while(ui_netSource.integer >= AS_GLOBAL2 && ui_netSource.integer <= AS_GLOBAL5)
			{
				Com_sprintf(cvarname, sizeof(cvarname), "sv_master%d", ui_netSource.integer-AS_FAVORITES+1);
				trap_Cvar_VariableStringBuffer(cvarname, masterstr, sizeof(masterstr));

				if(*masterstr)
					break;

				if (key == A_MOUSE2) {
					ui_netSource.integer--;
				}
				else
				{
					ui_netSource.integer++;
				}
			}
		}
		//[/MasterServer]

		if (ui_netSource.integer >= numNetSources) {
			ui_netSource.integer = 0;
		} else if (ui_netSource.integer < 0) {
			ui_netSource.integer = numNetSources - 1;
		}

		UI_BuildServerDisplayList(1);
		//[MasterServer]
		if (ui_netSource.integer >= AS_LOCAL) {
			//[/MasterServer]
			UI_StartServerRefresh(qtrue);
		}
		trap_Cvar_Set( "ui_netSource", va("%d", ui_netSource.integer));
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_NetFilter_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {

		if (key == A_MOUSE2) {
			ui_serverFilterType.integer--;
		} else {
			ui_serverFilterType.integer++;
		}

		if (ui_serverFilterType.integer >= numServerFilters) {
			ui_serverFilterType.integer = 0;
		} else if (ui_serverFilterType.integer < 0) {
			ui_serverFilterType.integer = numServerFilters - 1;
		}
		UI_BuildServerDisplayList(1);
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_OpponentName_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_BotName_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		//		int game = trap_Cvar_VariableValue("g_gametype");
		int value = uiInfo.botIndex;

		if (key == A_MOUSE2) {
			value--;
		} else {
			value++;
		}

		/*
		if (game >= GT_TEAM) {
			if (value >= uiInfo.characterCount + 2) {
				value = 0;
			} else if (value < 0) {
				value = uiInfo.characterCount + 2 - 1;
			}
		} else {
		*/
			if (value >= UI_GetNumBots()/* + 2*/) {
				value = 0;
			} else if (value < 0) {
				value = UI_GetNumBots()/* + 2*/ - 1;
			}
		//}
		uiInfo.botIndex = value;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_BotSkill_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		if (key == A_MOUSE2) {
			uiInfo.skillIndex--;
		} else {
			uiInfo.skillIndex++;
		}
		if (uiInfo.skillIndex >= numSkillLevels) {
			uiInfo.skillIndex = 0;
		} else if (uiInfo.skillIndex < 0) {
			uiInfo.skillIndex = numSkillLevels-1;
		}
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_RedBlue_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		uiInfo.redBlue ^= 1;
		return qtrue;
	}
	return qfalse;
}

static qboolean UI_Crosshair_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		if (key == A_MOUSE2) {
			uiInfo.currentCrosshair--;
		} else {
			uiInfo.currentCrosshair++;
		}

		if (uiInfo.currentCrosshair >= NUM_CROSSHAIRS) {
			uiInfo.currentCrosshair = 0;
		} else if (uiInfo.currentCrosshair < 0) {
			uiInfo.currentCrosshair = NUM_CROSSHAIRS - 1;
		}
		trap_Cvar_Set("cg_drawCrosshair", va("%d", uiInfo.currentCrosshair)); 
		return qtrue;
	}
	return qfalse;
}



static qboolean UI_SelectedPlayer_HandleKey(int flags, float *special, int key) {
	if (key == A_MOUSE1 || key == A_MOUSE2 || key == A_ENTER || key == A_KP_ENTER) {
		int selected;

		UI_BuildPlayerList();
		if (!uiInfo.teamLeader) {
			return qfalse;
		}
		selected = trap_Cvar_VariableValue("cg_selectedPlayer");

		if (key == A_MOUSE2) {
			selected--;
		} else {
			selected++;
		}

		if (selected > uiInfo.myTeamCount) {
			selected = 0;
		} else if (selected < 0) {
			selected = uiInfo.myTeamCount;
		}

		if (selected == uiInfo.myTeamCount) {
			trap_Cvar_Set( "cg_selectedPlayerName", "Everyone");
		} else {
			trap_Cvar_Set( "cg_selectedPlayerName", uiInfo.teamNames[selected]);
		}
		trap_Cvar_Set( "cg_selectedPlayer", va("%d", selected));
	}
	return qfalse;
}

/*
static qboolean UI_VoiceChat_HandleKey(int flags, float *special, int key)
{

	qboolean ret = qfalse;

	switch(key)
	{
		case A_1:
		case A_KP_1:
			ret = qtrue;
			break;
		case A_2:
		case A_KP_2:
			ret = qtrue;
			break;

	}
  
	return ret;
}
*/


static qboolean UI_OwnerDrawHandleKey(int ownerDraw, int flags, float *special, int key, int ownerDrawID) {
	int iUse = 0;

	switch (ownerDraw) {
	case UI_JKG_SLICE_SUMMARY:
		return JKG_Slice_Summary_HandleKey(ownerDrawID, flags, special, key);
	case UI_JKG_SLICE_GRID:
		return JKG_Slice_Grid_HandleKey(ownerDrawID, flags, special, key);
	case UI_JKG_CONVO_OPT1:
	case UI_JKG_CONVO_OPT2:
	case UI_JKG_CONVO_OPT3:
	case UI_JKG_CONVO_OPT4:
		return Conv_HandleKey_Options(ownerDraw - UI_JKG_CONVO_OPT1, flags, special, key);
		break;
	case UI_JKG_CONVO_KEYHANDLER: 
		return Conv_HandleKey_KeyHandler(flags, special, key);
		break;
	case UI_SKIN_COLOR:
		return UI_SkinColor_HandleKey(flags, special, key, uiSkinColor, TEAM_FREE, TEAM_BLUE, ownerDraw);
		break;	
	case UI_CHAT_MAIN:
		return UI_Chat_Main_HandleKey(key);
		break;
	case UI_CHAT_ATTACK:
		return UI_Chat_Attack_HandleKey(key);
		break;
	case UI_CHAT_DEFEND:
		return UI_Chat_Defend_HandleKey(key);
		break;
	case UI_CHAT_REQUEST:
		return UI_Chat_Request_HandleKey(key);
		break;
	case UI_CHAT_REPLY:
		return UI_Chat_Reply_HandleKey(key);
		break;
	case UI_CHAT_SPOT:
		return UI_Chat_Spot_HandleKey(key);
		break;
	case UI_CHAT_TACTICAL:
		return UI_Chat_Tactical_HandleKey(key);
		break;
	case UI_EFFECTS:
		return UI_Effects_HandleKey(flags, special, key);
		break;
	case UI_GAMETYPE:
		return UI_GameType_HandleKey(flags, special, key, qtrue);
		break;
	case UI_NETGAMETYPE:
		return UI_NetGameType_HandleKey(flags, special, key);
		break;
	case UI_AUTOSWITCHLIST:
		return UI_AutoSwitch_HandleKey(flags, special, key);
		break;
	case UI_JOINGAMETYPE:
		return UI_JoinGameType_HandleKey(flags, special, key);
		break;
	case UI_BLUETEAMNAME:
		return UI_TeamName_HandleKey(flags, special, key, qtrue);
		break;
	case UI_REDTEAMNAME:
		return UI_TeamName_HandleKey(flags, special, key, qfalse);
		break;
	case UI_BLUETEAM1:
	case UI_BLUETEAM2:
	case UI_BLUETEAM3:
	case UI_BLUETEAM4:
	case UI_BLUETEAM5:
	case UI_BLUETEAM6:
	case UI_BLUETEAM7:
	case UI_BLUETEAM8:
		if (ownerDraw <= UI_BLUETEAM5)
		{
			iUse = ownerDraw-UI_BLUETEAM1 + 1;
		}
		else
		{
			iUse = ownerDraw-274; //unpleasent hack because I don't want to move up all the UI_BLAHTEAM# defines
		}

		UI_TeamMember_HandleKey(flags, special, key, qtrue, iUse);
		break;
	case UI_REDTEAM1:
	case UI_REDTEAM2:
	case UI_REDTEAM3:
	case UI_REDTEAM4:
	case UI_REDTEAM5:
	case UI_REDTEAM6:
	case UI_REDTEAM7:
	case UI_REDTEAM8:
		if (ownerDraw <= UI_REDTEAM5)
		{
			iUse = ownerDraw-UI_REDTEAM1 + 1;
		}
		else
		{
			iUse = ownerDraw-277; //unpleasent hack because I don't want to move up all the UI_BLAHTEAM# defines
		}
		UI_TeamMember_HandleKey(flags, special, key, qfalse, iUse);
		break;
	case UI_NETSOURCE:
		UI_NetSource_HandleKey(flags, special, key);
		break;
	case UI_NETFILTER:
		UI_NetFilter_HandleKey(flags, special, key);
		break;
	case UI_OPPONENT_NAME:
		UI_OpponentName_HandleKey(flags, special, key);
		break;
	case UI_BOTNAME:
		return UI_BotName_HandleKey(flags, special, key);
		break;
	case UI_BOTSKILL:
		return UI_BotSkill_HandleKey(flags, special, key);
		break;
	case UI_REDBLUE:
		UI_RedBlue_HandleKey(flags, special, key);
		break;
	case UI_CROSSHAIR:
		UI_Crosshair_HandleKey(flags, special, key);
		break;
	case UI_SELECTEDPLAYER:
		UI_SelectedPlayer_HandleKey(flags, special, key);
		break;
		//	case UI_VOICECHAT:
		//		UI_VoiceChat_HandleKey(flags, special, key);
		//		break;
	default:
		break;
	}

	return qfalse;
}


static float UI_GetValue(int ownerDraw) {
	return 0;
}

/*
=================
UI_ServersQsortCompare
=================
*/
static int QDECL UI_ServersQsortCompare( const void *arg1, const void *arg2 ) {
	//[MasterServer]
	return trap_LAN_CompareServers( UI_SourceForLAN(),
		//[/MasterServer]
		uiInfo.serverStatus.sortKey, uiInfo.serverStatus.sortDir, *(int*)arg1, *(int*)arg2);
}


/*
=================
UI_ServersSort
=================
*/
void UI_ServersSort(int column, qboolean force) {

	if ( !force ) {
		if ( uiInfo.serverStatus.sortKey == column ) {
			return;
		}
	}

	uiInfo.serverStatus.sortKey = column;
	qsort( &uiInfo.serverStatus.displayServers[0], uiInfo.serverStatus.numDisplayServers, sizeof(int), UI_ServersQsortCompare);
}

/*
static void UI_StartSinglePlayer() {
	int i,j, k, skill;
	char buff[1024];
	i = trap_Cvar_VariableValue( "ui_currentTier" );
  if (i < 0 || i >= tierCount) {
    i = 0;
  }
	j = trap_Cvar_VariableValue("ui_currentMap");
	if (j < 0 || j > MAPS_PER_TIER) {
		j = 0;
	}

 	trap_Cvar_SetValue( "singleplayer", 1 );
 	trap_Cvar_SetValue( "g_gametype", Com_Clamp( 0, 7, tierList[i].gameTypes[j] ) );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", tierList[i].maps[j] ) );
	skill = trap_Cvar_VariableValue( "g_spSkill" );

	if (j == MAPS_PER_TIER-1) {
		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
		Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[0]), skill, "", teamList[k].teamMembers[0]);
	} else {
		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));
		for (i = 0; i < PLAYERS_PER_TEAM; i++) {
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[i]), skill, "Blue", teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}

		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
		for (i = 1; i < PLAYERS_PER_TEAM; i++) {
			Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %i %s 250 %s\n", UI_AIFromName(teamList[k].teamMembers[i]), skill, "Red", teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
		}
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; team Red\n" );
	}
	

}
*/

/*
===============
UI_LoadMods
===============
*/
static void UI_LoadMods() {
	int		numdirs;
	char	dirlist[2048];
	char	*dirptr;
	char  *descptr;
	int		i;
	int		dirlen;

	uiInfo.modCount = 0;
	numdirs = trap_FS_GetFileList( "$modlist", "", dirlist, sizeof(dirlist) );
	dirptr  = dirlist;
	for( i = 0; i < numdirs; i++ ) {
		dirlen = strlen( dirptr ) + 1;
		descptr = dirptr + dirlen;
		uiInfo.modList[uiInfo.modCount].modName = String_Alloc(dirptr);
		uiInfo.modList[uiInfo.modCount].modDescr = String_Alloc(descptr);
		dirptr += dirlen + strlen(descptr) + 1;
		uiInfo.modCount++;
		if (uiInfo.modCount >= MAX_MODS) {
			break;
		}
	}

}


/*
===============
UI_LoadMovies
===============
*/
static void UI_LoadMovies() {
	char	movielist[4096];
	char	*moviename;
	int		i, len;

	uiInfo.movieCount = trap_FS_GetFileList( "video", "roq", movielist, 4096 );

	if (uiInfo.movieCount) {
		if (uiInfo.movieCount > MAX_MOVIES) {
			uiInfo.movieCount = MAX_MOVIES;
		}
		moviename = movielist;
		for ( i = 0; i < uiInfo.movieCount; i++ ) {
			len = strlen( moviename );
			if (!Q_stricmp(moviename +  len - 4,".roq")) {
				moviename[len-4] = '\0';
			}
			Q_strupr(moviename);
			uiInfo.movieList[i] = String_Alloc(moviename);
			moviename += len + 1;
		}
	}

}



/*
===============
UI_LoadDemos
===============
*/

#if 1

/*
===============
UI_LoadDemos
===============
*/

static void UI_LoadDemosInDirectory( const char *directory )
{
	char	demolist[MAX_DEMOLIST] = {0}, *demoname = NULL;
	char	fileList[MAX_DEMOLIST] = {0}, *fileName = NULL;
	char	demoExt[32] = {0};
	int		i=0, j=0, len=0, numFiles=0;
	int		protocol = trap_Cvar_VariableValue( "com_protocol" ), protocolLegacy = trap_Cvar_VariableValue( "com_legacyprotocol" );

	if ( !protocol )
		protocol = trap_Cvar_VariableValue( "protocol" );
	if ( protocolLegacy == protocol )
		protocolLegacy = 0;

	Com_sprintf( demoExt, sizeof( demoExt ), ".%s%d", DEMO_EXTENSION, protocol);

	uiInfo.demoCount += trap_FS_GetFileList( directory, demoExt, demolist, sizeof( demolist ) );

	demoname = demolist;

	for ( j=0; j<2; j++ )
	{
		if ( uiInfo.demoCount > MAX_DEMOS )
			uiInfo.demoCount = MAX_DEMOS;

		for( ; uiInfo.loadedDemos<uiInfo.demoCount; uiInfo.loadedDemos++)
		{
			len = strlen( demoname );
			Com_sprintf( uiInfo.demoList[uiInfo.loadedDemos], sizeof( uiInfo.demoList[0] ), "%s/%s", directory + strlen( DEMO_DIRECTORY ), demoname );
			demoname += len + 1;
		}

		if ( !j )
		{
			if ( protocolLegacy > 0 && uiInfo.demoCount < MAX_DEMOS )
			{
				Com_sprintf( demoExt, sizeof( demoExt ), ".%s%d", DEMO_EXTENSION, protocolLegacy );
				uiInfo.demoCount += trap_FS_GetFileList( directory, demoExt, demolist, sizeof( demolist ) );
				demoname = demolist;
			}
			else
				break;
		}
	}

	numFiles = trap_FS_GetFileList( directory, "/", fileList, sizeof( fileList ) );

	fileName = fileList;
	for ( i=0; i<numFiles; i++ )
	{
		len = strlen( fileName );
		fileName[len] = '\0';
		if ( Q_stricmp( fileName, "." ) && Q_stricmp( fileName, ".." ) )
			UI_LoadDemosInDirectory( va( "%s/%s", directory, fileName ) );
		fileName += len+1;
	}

}

static void UI_LoadDemos( void )
{
	uiInfo.demoCount = 0;
	uiInfo.loadedDemos = 0;
	memset( uiInfo.demoList, 0, sizeof( uiInfo.demoList ) );
	UI_LoadDemosInDirectory( DEMO_DIRECTORY );
}

#else

static void UI_LoadDemos( void )
{
	char	demolist[4096] = {0};
	char	demoExt[8] = {0};
	char	*demoname = NULL;
	int		i, len, extLen;

	Com_sprintf( demoExt, sizeof( demoExt ), "dm_%d", (int)trap_Cvar_VariableValue( "protocol" ) );
	uiInfo.demoCount = Com_Clampi( 0, MAX_DEMOS, trap_FS_GetFileList( "demos", demoExt, demolist, sizeof( demolist ) ) );
	Com_sprintf( demoExt, sizeof( demoExt ), ".dm_%d", (int)trap_Cvar_VariableValue( "protocol" ) );
	extLen = strlen( demoExt );

	if ( uiInfo.demoCount )
	{
		demoname = demolist;
		for ( i=0; i<uiInfo.demoCount; i++ )
		{
			len = strlen( demoname );
			if ( !Q_stricmp( demoname + len - extLen, demoExt) )
				demoname[len-extLen] = '\0';
			Q_strupr( demoname );
			uiInfo.demoList[i] = String_Alloc( demoname );
			demoname += len + 1;
		}
	}
}

#endif

static qboolean UI_SetNextMap(int actual, int index) {
	int i;
	for (i = actual + 1; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, index + 1, "skirmish");
			return qtrue;
		}
	}
	return qfalse;
}


static void UI_StartSkirmish(qboolean next) {
	int i, k, g, delay, temp;
	float skill;
	char buff[MAX_STRING_CHARS];

	temp = trap_Cvar_VariableValue( "g_gametype" );
	trap_Cvar_Set("ui_gameType", va("%i", temp));

	if (next) {
		int actual;
		int index = trap_Cvar_VariableValue("ui_mapIndex");
		UI_MapCountByGameType(qtrue);
		UI_SelectedMap(index, &actual);
		if (UI_SetNextMap(actual, index)) {
		} else {
			UI_GameType_HandleKey(0, 0, A_MOUSE1, qfalse);
			UI_MapCountByGameType(qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, 0, "skirmish");
		}
	}

	g = uiInfo.gameTypes[ui_gameType.integer].gtEnum;
	trap_Cvar_SetValue( "g_gametype", g );
	trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", uiInfo.mapList[ui_currentMap.integer].mapLoadName) );
	skill = trap_Cvar_VariableValue( "g_spSkill" );
	trap_Cvar_Set("ui_scoreMap", uiInfo.mapList[ui_currentMap.integer].mapName);

	k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_opponentName"));

	trap_Cvar_Set("ui_singlePlayerActive", "1");

	// set up sp overrides, will be replaced on postgame
	temp = trap_Cvar_VariableValue( "capturelimit" );
	trap_Cvar_Set("ui_saveCaptureLimit", va("%i", temp));
	temp = trap_Cvar_VariableValue( "fraglimit" );
	trap_Cvar_Set("ui_saveFragLimit", va("%i", temp));
	temp = trap_Cvar_VariableValue( "duel_fraglimit" );
	trap_Cvar_Set("ui_saveDuelLimit", va("%i", temp));

	UI_SetCapFragLimits(qfalse);

	temp = trap_Cvar_VariableValue( "cg_drawTimer" );
	trap_Cvar_Set("ui_drawTimer", va("%i", temp));
	temp = trap_Cvar_VariableValue( "g_friendlyFire" );
	trap_Cvar_Set("ui_friendlyFire", va("%i", temp));
	temp = trap_Cvar_VariableValue( "sv_maxClients" );
	trap_Cvar_Set("ui_maxClients", va("%i", temp));
	temp = trap_Cvar_VariableValue( "sv_pure" );
	trap_Cvar_Set("ui_pure", va("%i", temp));

	trap_Cvar_Set("cg_cameraOrbit", "0");
	trap_Cvar_Set("cg_thirdPerson", "0");
	trap_Cvar_Set("cg_drawTimer", "1");
	trap_Cvar_Set("sv_pure", "0");
	trap_Cvar_Set("g_friendlyFire", "0");
//	trap_Cvar_Set("g_redTeam", UI_Cvar_VariableString("ui_teamName"));
//	trap_Cvar_Set("g_blueTeam", UI_Cvar_VariableString("ui_opponentName"));

	if (trap_Cvar_VariableValue("ui_recordSPDemo")) {
		Com_sprintf(buff, MAX_STRING_CHARS, "%s_%i", uiInfo.mapList[ui_currentMap.integer].mapLoadName, g);
		trap_Cvar_Set("ui_recordSPDemoName", buff);
	}

	delay = 500;

	if (g == GT_DUEL || g == GT_POWERDUEL) {
		temp = uiInfo.mapList[ui_currentMap.integer].teamMembers * 2;
		trap_Cvar_Set("sv_maxClients", va("%d", temp));
		Com_sprintf( buff, sizeof(buff), "wait ; addbot %s %f "", %i \n", uiInfo.mapList[ui_currentMap.integer].opponentName, skill, delay);
		trap_Cmd_ExecuteText( EXEC_APPEND, buff );
	} else {
		temp = uiInfo.mapList[ui_currentMap.integer].teamMembers * 2;
		trap_Cvar_Set("sv_maxClients", va("%d", temp));
		for (i =0; i < uiInfo.mapList[ui_currentMap.integer].teamMembers; i++) {
			Com_sprintf( buff, sizeof(buff), "addbot \"%s\" %f %s %i %s\n", UI_AIFromName(uiInfo.teamList[k].teamMembers[i]), skill, (g == GT_FFA) ? "" : "Blue", delay, uiInfo.teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
			delay += 500;
		}
		k = UI_TeamIndexFromName(UI_Cvar_VariableString("ui_teamName"));
		for (i =0; i < uiInfo.mapList[ui_currentMap.integer].teamMembers-1; i++) {
			Com_sprintf( buff, sizeof(buff), "addbot \"%s\" %f %s %i %s\n", UI_AIFromName(uiInfo.teamList[k].teamMembers[i]), skill, (g == GT_FFA) ? "" : "Red", delay, uiInfo.teamList[k].teamMembers[i]);
			trap_Cmd_ExecuteText( EXEC_APPEND, buff );
			delay += 500;
		}
	}
	if (g >= GT_TEAM ) {
		trap_Cmd_ExecuteText( EXEC_APPEND, "wait 5; team Red\n" );
	}
}

static void UI_Update(const char *name) {
	int	val = trap_Cvar_VariableValue(name);

	if (Q_stricmp(name, "s_khz") == 0) 
	{
		trap_Cmd_ExecuteText( EXEC_APPEND, "snd_restart\n" );
		return;
	}

	/* Raz: Truncate the name, try and avoid overflow glitches for long names and menu items
	if (Q_stricmp(name, "ui_SetName") == 0)
		trap_Cvar_Set( "name", UI_Cvar_VariableString("ui_Name"));
		*/
	if ( !Q_stricmp( name, "ui_SetName" ) )
	{
		char buf[MAX_NETNAME] = {0};
		Q_strncpyz( buf, UI_Cvar_VariableString( "ui_Name" ), sizeof( buf ) );
		trap_Cvar_Set( "name", buf );
	}
	else if (Q_stricmp(name, "ui_setRate") == 0) {
		float rate = trap_Cvar_VariableValue("rate");
		if (rate >= 5000) {
			trap_Cvar_Set("cl_maxpackets", "30");
			trap_Cvar_Set("cl_packetdup", "1");
		} else if (rate >= 4000) {
			trap_Cvar_Set("cl_maxpackets", "15");
			trap_Cvar_Set("cl_packetdup", "2");		// favor less prediction errors when there's packet loss
		} else {
			trap_Cvar_Set("cl_maxpackets", "15");
			trap_Cvar_Set("cl_packetdup", "1");		// favor lower bandwidth
		}
	} 
	/* Raz: Truncate the name, try and avoid overflow glitches for long names and menu items
	else if (Q_stricmp(name, "ui_GetName") == 0) 
		trap_Cvar_Set( "ui_Name", UI_Cvar_VariableString("name"));
		*/
	else if ( !Q_stricmp( name, "ui_GetName" ) )
	{
		char buf[MAX_NETNAME] = {0};
		Q_strncpyz( buf, UI_Cvar_VariableString( "name" ), sizeof( buf ) );
		trap_Cvar_Set( "ui_Name", buf );
	}
	else if (Q_stricmp(name, "ui_r_colorbits") == 0) 
	{
		switch (val) 
		{
		case 0:
			trap_Cvar_SetValue( "ui_r_depthbits", 0 );
			break;

		case 16:
			trap_Cvar_SetValue( "ui_r_depthbits", 16 );
			break;

		case 32:
			trap_Cvar_SetValue( "ui_r_depthbits", 24 );
			break;
		}
	} 
	else if (Q_stricmp(name, "ui_r_lodbias") == 0) 
	{
		switch (val) 
		{
		case 0:
			trap_Cvar_SetValue( "ui_r_subdivisions", 4 );
			break;
		case 1:
			trap_Cvar_SetValue( "ui_r_subdivisions", 12 );
			break;

		case 2:
			trap_Cvar_SetValue( "ui_r_subdivisions", 20 );
			break;
		}
	} 
	else if (Q_stricmp(name, "ui_r_glCustom") == 0) 
	{
		switch (val) 
		{
			case 0:	// high quality

				trap_Cvar_SetValue( "ui_r_fullScreen", 1 );
				trap_Cvar_SetValue( "ui_r_subdivisions", 4 );
				trap_Cvar_SetValue( "ui_r_lodbias", 0 );
				trap_Cvar_SetValue( "ui_r_colorbits", 32 );
				trap_Cvar_SetValue( "ui_r_depthbits", 24 );
				trap_Cvar_SetValue( "ui_r_picmip", 0 );
				trap_Cvar_SetValue( "ui_r_mode", 4 );
				trap_Cvar_SetValue( "ui_r_texturebits", 32 );
				trap_Cvar_SetValue( "ui_r_fastSky", 0 );
				trap_Cvar_SetValue( "ui_r_inGameVideo", 1 );
				//trap_Cvar_SetValue( "ui_cg_shadows", 2 );//stencil
				trap_Cvar_Set( "ui_r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
				break;

			case 1: // normal 
				trap_Cvar_SetValue( "ui_r_fullScreen", 1 );
				trap_Cvar_SetValue( "ui_r_subdivisions", 4 );
				trap_Cvar_SetValue( "ui_r_lodbias", 0 );
				trap_Cvar_SetValue( "ui_r_colorbits", 0 );
				trap_Cvar_SetValue( "ui_r_depthbits", 24 );
				trap_Cvar_SetValue( "ui_r_picmip", 1 );
				trap_Cvar_SetValue( "ui_r_mode", 3 );
				trap_Cvar_SetValue( "ui_r_texturebits", 0 );
				trap_Cvar_SetValue( "ui_r_fastSky", 0 );
				trap_Cvar_SetValue( "ui_r_inGameVideo", 1 );
				//trap_Cvar_SetValue( "ui_cg_shadows", 2 );
				trap_Cvar_Set( "ui_r_texturemode", "GL_LINEAR_MIPMAP_LINEAR" );
				break;

			case 2: // fast

				trap_Cvar_SetValue( "ui_r_fullScreen", 1 );
				trap_Cvar_SetValue( "ui_r_subdivisions", 12 );
				trap_Cvar_SetValue( "ui_r_lodbias", 1 );
				trap_Cvar_SetValue( "ui_r_colorbits", 0 );
				trap_Cvar_SetValue( "ui_r_depthbits", 0 );
				trap_Cvar_SetValue( "ui_r_picmip", 2 );
				trap_Cvar_SetValue( "ui_r_mode", 3 );
				trap_Cvar_SetValue( "ui_r_texturebits", 0 );
				trap_Cvar_SetValue( "ui_r_fastSky", 1 );
				trap_Cvar_SetValue( "ui_r_inGameVideo", 0 );
				//trap_Cvar_SetValue( "ui_cg_shadows", 1 );
				trap_Cvar_Set( "ui_r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
				break;

			case 3: // fastest

				trap_Cvar_SetValue( "ui_r_fullScreen", 1 );
				trap_Cvar_SetValue( "ui_r_subdivisions", 20 );
				trap_Cvar_SetValue( "ui_r_lodbias", 2 );
				trap_Cvar_SetValue( "ui_r_colorbits", 16 );
				trap_Cvar_SetValue( "ui_r_depthbits", 16 );
				trap_Cvar_SetValue( "ui_r_mode", 3 );
				trap_Cvar_SetValue( "ui_r_picmip", 3 );
				trap_Cvar_SetValue( "ui_r_texturebits", 16 );
				trap_Cvar_SetValue( "ui_r_fastSky", 1 );
				trap_Cvar_SetValue( "ui_r_inGameVideo", 0 );
				//trap_Cvar_SetValue( "ui_cg_shadows", 0 );
				trap_Cvar_Set( "ui_r_texturemode", "GL_LINEAR_MIPMAP_NEAREST" );
			break;
		}
	} 
	else if (Q_stricmp(name, "ui_mousePitch") == 0) 
	{
		if (val == 0) 
		{
			trap_Cvar_SetValue( "m_pitch", 0.022f );
		} 
		else 
		{
			trap_Cvar_SetValue( "m_pitch", -0.022f );
		}
	}
	else if (Q_stricmp(name, "ui_mousePitchVeh") == 0) 
	{
		if (val == 0) 
		{
			trap_Cvar_SetValue( "m_pitchVeh", 0.022f );
		} 
		else 
		{
			trap_Cvar_SetValue( "m_pitchVeh", -0.022f );
		}
	}
}

int gUISelectedMap = 0;

/*
===============
UI_DeferMenuScript

Return true if the menu script should be deferred for later
===============
*/
static qboolean UI_DeferMenuScript ( char **args )
{
	const char* name;

	// Whats the reason for being deferred?
	if (!String_Parse( (char**)args, &name)) 
	{
		return qfalse;
	}

	// Handle the custom cases
	if ( !Q_stricmp ( name, "VideoSetup" ) )
	{
		const char* warningMenuName;
		qboolean	deferred;

		// No warning menu specified
		if ( !String_Parse( (char**)args, &warningMenuName) )
		{
			return qfalse;
		}

		// Defer if the video options were modified
		deferred = trap_Cvar_VariableValue ( "ui_r_modified" ) ? qtrue : qfalse;

		if ( deferred )
		{
			// Open the warning menu
			Menus_OpenByName(warningMenuName);
		}

		return deferred;
	}
	else if ( !Q_stricmp ( name, "RulesBackout" ) )
	{
		qboolean deferred;

		deferred = trap_Cvar_VariableValue ( "ui_rules_backout" ) ? qtrue : qfalse ;

		trap_Cvar_Set ( "ui_rules_backout", "0" );

		return deferred;
	}

	return qfalse;
}

/*
=================
UI_UpdateVideoSetup

Copies the temporary user interface version of the video cvars into
their real counterparts.  This is to create a interface which allows 
you to discard your changes if you did something you didnt want
=================
*/
void UI_UpdateVideoSetup ( void )
{
	trap_Cvar_Set ( "r_mode", UI_Cvar_VariableString ( "ui_r_mode" ) );
	trap_Cvar_Set ( "r_fullscreen", UI_Cvar_VariableString ( "ui_r_fullscreen" ) );
	trap_Cvar_Set ( "r_colorbits", UI_Cvar_VariableString ( "ui_r_colorbits" ) );
	trap_Cvar_Set ( "r_lodbias", UI_Cvar_VariableString ( "ui_r_lodbias" ) );
	trap_Cvar_Set ( "r_picmip", UI_Cvar_VariableString ( "ui_r_picmip" ) );
	trap_Cvar_Set ( "r_texturebits", UI_Cvar_VariableString ( "ui_r_texturebits" ) );
	trap_Cvar_Set ( "r_texturemode", UI_Cvar_VariableString ( "ui_r_texturemode" ) );
	trap_Cvar_Set ( "r_detailtextures", UI_Cvar_VariableString ( "ui_r_detailtextures" ) );
	trap_Cvar_Set ( "r_ext_compress_textures", UI_Cvar_VariableString ( "ui_r_ext_compress_textures" ) );
	trap_Cvar_Set ( "r_depthbits", UI_Cvar_VariableString ( "ui_r_depthbits" ) );
	trap_Cvar_Set ( "r_subdivisions", UI_Cvar_VariableString ( "ui_r_subdivisions" ) );
	trap_Cvar_Set ( "r_fastSky", UI_Cvar_VariableString ( "ui_r_fastSky" ) );
	trap_Cvar_Set ( "r_inGameVideo", UI_Cvar_VariableString ( "ui_r_inGameVideo" ) );
	trap_Cvar_Set ( "r_allowExtensions", UI_Cvar_VariableString ( "ui_r_allowExtensions" ) );
	trap_Cvar_Set ( "cg_shadows", UI_Cvar_VariableString ( "ui_cg_shadows" ) );
	trap_Cvar_Set ( "ui_r_modified", "0" );

	trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
}

/*
=================
UI_GetVideoSetup

Retrieves the current actual video settings into the temporary user
interface versions of the cvars.
=================
*/
void UI_GetVideoSetup ( void )
{
	// Make sure the cvars are registered as read only.
	trap_Cvar_Register ( NULL, "ui_r_glCustom",				"4", CVAR_ROM|CVAR_INTERNAL|CVAR_ARCHIVE );

	trap_Cvar_Register ( NULL, "ui_r_mode",					"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_fullscreen",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_colorbits",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_lodbias",				"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_picmip",				"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_texturebits",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_texturemode",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_detailtextures",		"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_ext_compress_textures","0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_depthbits",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_subdivisions",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_fastSky",				"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_inGameVideo",			"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_allowExtensions",		"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_cg_shadows",				"0", CVAR_ROM|CVAR_INTERNAL );
	trap_Cvar_Register ( NULL, "ui_r_modified",				"0", CVAR_ROM|CVAR_INTERNAL );

	// Copy over the real video cvars into their temporary counterparts
	trap_Cvar_Set ( "ui_r_mode",		UI_Cvar_VariableString ( "r_mode" ) );
	trap_Cvar_Set ( "ui_r_colorbits",	UI_Cvar_VariableString ( "r_colorbits" ) );
	trap_Cvar_Set ( "ui_r_fullscreen",	UI_Cvar_VariableString ( "r_fullscreen" ) );
	trap_Cvar_Set ( "ui_r_lodbias",		UI_Cvar_VariableString ( "r_lodbias" ) );
	trap_Cvar_Set ( "ui_r_picmip",		UI_Cvar_VariableString ( "r_picmip" ) );
	trap_Cvar_Set ( "ui_r_texturebits", UI_Cvar_VariableString ( "r_texturebits" ) );
	trap_Cvar_Set ( "ui_r_texturemode", UI_Cvar_VariableString ( "r_texturemode" ) );
	trap_Cvar_Set ( "ui_r_detailtextures", UI_Cvar_VariableString ( "r_detailtextures" ) );
	trap_Cvar_Set ( "ui_r_ext_compress_textures", UI_Cvar_VariableString ( "r_ext_compress_textures" ) );
	trap_Cvar_Set ( "ui_r_depthbits", UI_Cvar_VariableString ( "r_depthbits" ) );
	trap_Cvar_Set ( "ui_r_subdivisions", UI_Cvar_VariableString ( "r_subdivisions" ) );
	trap_Cvar_Set ( "ui_r_fastSky", UI_Cvar_VariableString ( "r_fastSky" ) );
	trap_Cvar_Set ( "ui_r_inGameVideo", UI_Cvar_VariableString ( "r_inGameVideo" ) );
	trap_Cvar_Set ( "ui_r_allowExtensions", UI_Cvar_VariableString ( "r_allowExtensions" ) );
	trap_Cvar_Set ( "ui_cg_shadows", UI_Cvar_VariableString ( "cg_shadows" ) );
	trap_Cvar_Set ( "ui_r_modified", "0" );
}

// If the game type is siege, hide the addbot button. I would have done a cvar text on that item,
// but it already had one on it.
static void UI_SetBotButton ( void )
{
	int gameType = trap_Cvar_VariableValue( "g_gametype" );
	int server;
	menuDef_t *menu;
	itemDef_t *item;
	char *name = "addBot";

	server = trap_Cvar_VariableValue( "sv_running" );

	// If a client, don't show add bot button
	if (server==0)
	{
		menu = Menu_GetFocused();	// Get current menu (either video or ingame video, I would assume)

		if (!menu)
		{
			return;
		}

		item = Menu_FindItemByName(menu, name);
		if (item)
		{
			Menu_ShowItemByName(menu, name, qfalse);
		}
	}
}

// Update the model cvar and everything is good.
static void UI_UpdateCharacterCvars ( void )
{
	char skin[MAX_QPATH];
	char model[MAX_QPATH];
	char head[MAX_QPATH];
	char torso[MAX_QPATH];
	char legs[MAX_QPATH];

	trap_Cvar_VariableStringBuffer("ui_char_model", model, sizeof(model));
	trap_Cvar_VariableStringBuffer("ui_char_skin_head", head, sizeof(head));
	trap_Cvar_VariableStringBuffer("ui_char_skin_torso", torso, sizeof(torso));
	trap_Cvar_VariableStringBuffer("ui_char_skin_legs", legs, sizeof(legs));

	Com_sprintf( skin, sizeof( skin ), "%s/%s|%s|%s", 
		model, 
		head, 
		torso, 
		legs 
		);

	trap_Cvar_Set ( "model", skin );

	trap_Cvar_Set ( "char_color_red", UI_Cvar_VariableString ( "ui_char_color_red" ) );
	trap_Cvar_Set ( "char_color_green", UI_Cvar_VariableString ( "ui_char_color_green" ) );
	trap_Cvar_Set ( "char_color_blue", UI_Cvar_VariableString ( "ui_char_color_blue" ) );
	trap_Cvar_Set ( "ui_selectedModelIndex", "-1");

}

static void UI_GetCharacterCvars ( void )
{
	char *model;
	char *skin;
	int i;

	trap_Cvar_Set ( "ui_char_color_red", UI_Cvar_VariableString ( "char_color_red" ) );
	trap_Cvar_Set ( "ui_char_color_green", UI_Cvar_VariableString ( "char_color_green" ) );
	trap_Cvar_Set ( "ui_char_color_blue", UI_Cvar_VariableString ( "char_color_blue" ) );

	model = UI_Cvar_VariableString ( "model" );
	skin = strrchr(model,'/');
	if (skin && strchr(model,'|'))	//we have a multipart custom jedi
	{
		char skinhead[MAX_QPATH];
		char skintorso[MAX_QPATH];
		char skinlower[MAX_QPATH];
		char *p2;

		*skin=0;
		skin++;
		//now get the the individual files

		//advance to second
		p2 = strchr(skin, '|'); 
		assert(p2);
		*p2=0;
		p2++;
		strcpy (skinhead, skin);


		//advance to third
		skin = strchr(p2, '|');
		assert(skin);
		*skin=0;
		skin++;
		strcpy (skintorso,p2);

		strcpy (skinlower,skin);



		trap_Cvar_Set("ui_char_model", model);
		trap_Cvar_Set("ui_char_skin_head", skinhead);
		trap_Cvar_Set("ui_char_skin_torso", skintorso);
		trap_Cvar_Set("ui_char_skin_legs", skinlower);

		for (i = 0; i < uiInfo.playerSpeciesCount; i++)
		{
			if ( !stricmp(model, uiInfo.playerSpecies[i].Name) )
			{
				uiInfo.playerSpeciesIndex = i;
				break;
			}
		}
	}
	else
	{
		model = UI_Cvar_VariableString ( "ui_char_model" );
		for (i = 0; i < uiInfo.playerSpeciesCount; i++)
		{
			if ( !stricmp(model, uiInfo.playerSpecies[i].Name) )
			{
				uiInfo.playerSpeciesIndex = i;
				return;	//FOUND IT, don't fall through
			}
		}
		//nope, didn't find it.
		uiInfo.playerSpeciesIndex = 0;//jic
		trap_Cvar_Set("ui_char_model", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name);
		trap_Cvar_Set("ui_char_skin_head", "head_a1");
		trap_Cvar_Set("ui_char_skin_torso","torso_a1");
		trap_Cvar_Set("ui_char_skin_legs", "lower_a1");
	}
}

void UI_SetSiegeObjectiveGraphicPos(menuDef_t *menu,const char *itemName,const char *cvarName)
{
	itemDef_t	*item;
	char		cvarBuf[1024];
	const char	*holdVal;
	char		*holdBuf;

	item = Menu_FindItemByName(menu, itemName);

	if (item)
	{
		// get cvar data
		trap_Cvar_VariableStringBuffer(cvarName, cvarBuf, sizeof(cvarBuf));

		holdBuf = cvarBuf;
		if (String_Parse(&holdBuf,&holdVal))
		{
			item->window.rectClient.x = atof(holdVal);
			if (String_Parse(&holdBuf,&holdVal))
			{
				item->window.rectClient.y = atof(holdVal);
				if (String_Parse(&holdBuf,&holdVal))
				{
					item->window.rectClient.w = atof(holdVal);
					if (String_Parse(&holdBuf,&holdVal))
					{
						item->window.rectClient.h = atof(holdVal);

						item->window.rect.x = item->window.rectClient.x;
						item->window.rect.y = item->window.rectClient.y;

						item->window.rect.w = item->window.rectClient.w;
						item->window.rect.h = item->window.rectClient.h;
					}
				}
			}
		}
	}
}

void UI_FindCurrentSiegeTeamClass( void )
{
	menuDef_t *menu;
	int myTeam = (int)(trap_Cvar_VariableValue("ui_myteam"));
	char *itemname;
	itemDef_t *item;
	int	baseClass;

	menu = Menu_GetFocused();	// Get current menu

	if (!menu)
	{
		return;
	}

	if (( myTeam != TEAM_RED ) && ( myTeam != TEAM_BLUE ))
	{
		return;
	}

	// If the player is on a team, 
	if ( myTeam == TEAM_RED )
	{			
		itemDef_t *item;
		item = (itemDef_t *) Menu_FindItemByName(menu, "onteam1" );
		if (item)
		{
			Item_RunScript(item, item->action);
		}
	}
	else if ( myTeam == TEAM_BLUE )
	{			
		itemDef_t *item;
		item = (itemDef_t *) Menu_FindItemByName(menu, "onteam2" );
		if (item)
		{
			Item_RunScript(item, item->action);
		}
	}	


	baseClass = (int)trap_Cvar_VariableValue("ui_siege_class");

	// Find correct class button and activate it.
	if (baseClass == SPC_INFANTRY)
	{
		itemname = "class1_button";
	}
	else if (baseClass == SPC_HEAVY_WEAPONS)
	{
		itemname = "class2_button";
	}
	else if (baseClass == SPC_DEMOLITIONIST)
	{
		itemname = "class3_button";
	}
	else if (baseClass == SPC_VANGUARD)
	{
		itemname = "class4_button";
	}
	else if (baseClass == SPC_SUPPORT)
	{
		itemname = "class5_button";
	}
	else if (baseClass == SPC_SUPPORT)
	{
		itemname = "class5_button";
	}
	else if (baseClass == SPC_JEDI)
	{
		itemname = "class6_button";
	}
	else 
	{
		return;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, itemname );
	if (item)
	{
		Item_RunScript(item, item->action);
	}

}

void UI_UpdateSiegeObjectiveGraphics( void )
{
	menuDef_t *menu;
	int	teamI,objI;

	menu = Menu_GetFocused();	// Get current menu

	if (!menu)
	{
		return;
	}

	// Hiding a bunch of fields because the opening section of the siege menu was getting too long
	Menu_ShowGroup(menu,"class_button",qfalse);
	Menu_ShowGroup(menu,"class_count",qfalse);
	Menu_ShowGroup(menu,"feeders",qfalse);
	Menu_ShowGroup(menu,"classdescription",qfalse);
	Menu_ShowGroup(menu,"minidesc",qfalse);
	Menu_ShowGroup(menu,"obj_longdesc",qfalse);
	Menu_ShowGroup(menu,"objective_pic",qfalse);
	Menu_ShowGroup(menu,"stats",qfalse);
	Menu_ShowGroup(menu,"forcepowerlevel",qfalse);

	// Get objective icons for each team
	for (teamI=1;teamI<3;teamI++)
	{
		for (objI=1;objI<8;objI++)
		{
			Menu_SetItemBackground(menu,va("tm%i_icon%i",teamI,objI),va("*team%i_objective%i_mapicon",teamI,objI));
			Menu_SetItemBackground(menu,va("tm%i_l_icon%i",teamI,objI),va("*team%i_objective%i_mapicon",teamI,objI));
		}
	}

	// Now get their placement on the map
	for (teamI=1;teamI<3;teamI++)
	{
		for (objI=1;objI<8;objI++)
		{
			UI_SetSiegeObjectiveGraphicPos(menu,va("tm%i_icon%i",teamI,objI),va("team%i_objective%i_mappos",teamI,objI));
		}
	}

}

saber_colors_t TranslateSaberColor( const char *name );

static void UI_UpdateSaberCvars ( void )
{
	saber_colors_t colorI;

	trap_Cvar_Set ( "saber1", UI_Cvar_VariableString ( "ui_saber" ) );
	trap_Cvar_Set ( "saber2", UI_Cvar_VariableString ( "ui_saber2" ) );

	colorI = TranslateSaberColor( UI_Cvar_VariableString ( "ui_saber_color" ) );
	trap_Cvar_Set ( "color1", va("%d",colorI));
	trap_Cvar_Set ( "g_saber_color", UI_Cvar_VariableString ( "ui_saber_color" ));

	colorI = TranslateSaberColor( UI_Cvar_VariableString ( "ui_saber2_color" ) );
	trap_Cvar_Set ( "color2", va("%d",colorI) );
	trap_Cvar_Set ( "g_saber2_color", UI_Cvar_VariableString ( "ui_saber2_color" ));
}

// More hard coded goodness for the menus.
static void UI_SetSaberBoxesandHilts (void)
{
	menuDef_t *menu;
	itemDef_t *item;
	qboolean	getBig = qfalse;
	char sType[MAX_QPATH];

	menu = Menu_GetFocused();	// Get current menu (either video or ingame video, I would assume)

	if (!menu)
	{
		return;
	}

	trap_Cvar_VariableStringBuffer( "ui_saber_type", sType, sizeof(sType) );

	if ( Q_stricmp( "dual", sType ) != 0 )
	{
//		trap_Cvar_Set("ui_saber", "single_1");
//		trap_Cvar_Set("ui_saber2", "single_1");
		getBig = qtrue;
	}

	else if (Q_stricmp( "staff", sType ) != 0 )
	{
//		trap_Cvar_Set("ui_saber", "dual_1");
//		trap_Cvar_Set("ui_saber2", "none");
		getBig = qtrue;
	}

	if (!getBig)
	{
		return;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, "box2middle" );

	if(item)
	{
		item->window.rect.x = 212;
		item->window.rect.y = 126;
		item->window.rect.w = 219;
		item->window.rect.h = 44;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, "box2bottom" );

	if(item)
	{
		item->window.rect.x = 212;
		item->window.rect.y = 170;
		item->window.rect.w = 219;
		item->window.rect.h = 60;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, "box3middle" );

	if(item)
	{
		item->window.rect.x = 418;
		item->window.rect.y = 126;
		item->window.rect.w = 219;
		item->window.rect.h = 44;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, "box3bottom" );

	if(item)
	{
		item->window.rect.x = 418;
		item->window.rect.y = 170;
		item->window.rect.w = 219;
		item->window.rect.h = 60;
	}
}

//extern qboolean UI_SaberModelForSaber( const char *saberName, char *saberModel );
extern qboolean UI_SaberSkinForSaber( const char *saberName, char *saberSkin );
extern qboolean ItemParse_asset_model_go( itemDef_t *item, const char *name,int *runTimeLength );
extern qboolean ItemParse_model_g2skin_go( itemDef_t *item, const char *skinName );

static void UI_UpdateSaberType( void )
{
	char sType[MAX_QPATH];
	trap_Cvar_VariableStringBuffer( "ui_saber_type", sType, sizeof(sType) );

	if ( Q_stricmp( "single", sType ) == 0 ||
		Q_stricmp( "staff", sType ) == 0 )
	{
		trap_Cvar_Set( "ui_saber2", "" );
	}
}

static void UI_UpdateSaberHilt( qboolean secondSaber )
{
	menuDef_t *menu;
	itemDef_t *item;
	char model[MAX_QPATH];
	char modelPath[MAX_QPATH];
	char skinPath[MAX_QPATH];
	char *itemName;
	char *saberCvarName;
	int	animRunLength;

	menu = Menu_GetFocused();	// Get current menu (either video or ingame video, I would assume)

	if (!menu)
	{
		return;
	}

	if ( secondSaber )
	{
		itemName = "saber2";
		saberCvarName = "ui_saber2";
	}
	else
	{
		itemName = "saber";
		saberCvarName = "ui_saber";
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, itemName );

	if(!item)
	{
		Com_Error( ERR_FATAL, "UI_UpdateSaberHilt: Could not find item (%s) in menu (%s)", itemName, menu->window.name);
	}

	trap_Cvar_VariableStringBuffer( saberCvarName, model, sizeof(model) );

	strcpy(item->text, model);
	//item->text = model;
	//read this from the sabers.cfg
	if ( UI_SaberModelForSaber( model, modelPath ) )
	{//successfully found a model
		ItemParse_asset_model_go( item, modelPath, &animRunLength );//set the model
		//get the customSkin, if any
		//COM_StripExtension( modelPath, skinPath );
		//COM_DefaultExtension( skinPath, sizeof( skinPath ), ".skin" );
		if ( UI_SaberSkinForSaber( model, skinPath ) )
		{
			ItemParse_model_g2skin_go( item, skinPath );//apply the skin
		}
		else
		{
			ItemParse_model_g2skin_go( item, NULL );//apply the skin
		}
	}
}

static void UI_UpdateSaberColor( qboolean secondSaber )
{
}

extern char * SaberColorToString(saber_colors_t color);

static void UI_GetSaberCvars ( void )
{
//	trap_Cvar_Set ( "ui_saber_type", UI_Cvar_VariableString ( "g_saber_type" ) );
	trap_Cvar_Set ( "ui_saber", UI_Cvar_VariableString ( "saber1" ) );
	trap_Cvar_Set ( "ui_saber2", UI_Cvar_VariableString ( "saber2" ));

	trap_Cvar_Set("g_saber_color", SaberColorToString(trap_Cvar_VariableValue("color1")));
	trap_Cvar_Set("g_saber2_color", SaberColorToString(trap_Cvar_VariableValue("color2")));

	trap_Cvar_Set ( "ui_saber_color", UI_Cvar_VariableString ( "g_saber_color" ) );
	trap_Cvar_Set ( "ui_saber2_color", UI_Cvar_VariableString ( "g_saber2_color" ) );


}


//extern qboolean ItemParse_model_g2skin_go( itemDef_t *item, const char *skinName );
extern qboolean ItemParse_model_g2anim_go( itemDef_t *item, const char *animName );
//extern qboolean ItemParse_asset_model_go( itemDef_t *item, const char *name );

void UI_UpdateCharacterSkin( void )
{
	menuDef_t *menu;
	itemDef_t *item;
	char skin[MAX_QPATH];
	char model[MAX_QPATH];
	char head[MAX_QPATH];
	char torso[MAX_QPATH];
	char legs[MAX_QPATH];

	menu = Menu_GetFocused();	// Get current menu

	if (!menu)
	{
		return;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, "character");

	if (!item)
	{
		Com_Error( ERR_FATAL, "UI_UpdateCharacterSkin: Could not find item (character) in menu (%s)", menu->window.name);
	}

	trap_Cvar_VariableStringBuffer("ui_char_model", model, sizeof(model));
	trap_Cvar_VariableStringBuffer("ui_char_skin_head", head, sizeof(head));
	trap_Cvar_VariableStringBuffer("ui_char_skin_torso", torso, sizeof(torso));
	trap_Cvar_VariableStringBuffer("ui_char_skin_legs", legs, sizeof(legs));

	Com_sprintf( skin, sizeof( skin ), "models/players/%s/|%s|%s|%s", 
										model, 
										head, 
										torso, 
										legs 
				);

	ItemParse_model_g2skin_go( item, skin );
}

static void UI_ResetCharacterListBoxes( void )
{

	itemDef_t *item;
	menuDef_t *menu;
	listBoxDef_t *listPtr;

	menu = Menu_GetFocused();

	if (menu)
	{
		item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "headlistbox");
		if (item)
		{
			listBoxDef_t *listPtr = (listBoxDef_t*)item->typeData;
			if( listPtr )
			{
				listPtr->cursorPos = 0;
			}
			item->cursorPos = 0;
		}

		item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "torsolistbox");
		if (item)
		{
			listPtr = (listBoxDef_t*)item->typeData;
			if( listPtr )
			{
				listPtr->cursorPos = 0;
			}
			item->cursorPos = 0;
		}

		item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "lowerlistbox");
		if (item)
		{
			listPtr = (listBoxDef_t*)item->typeData;
			if( listPtr )
			{
				listPtr->cursorPos = 0;
			}
			item->cursorPos = 0;
		}

		item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "colorbox");
		if (item)
		{
			listPtr = (listBoxDef_t*)item->typeData;
			if( listPtr )
			{
				listPtr->cursorPos = 0;
			}
			item->cursorPos = 0;
		}
	}
}

//#define MAX_SABER_HILTS	64

char *saberSingleHiltInfo [MAX_SABER_HILTS];
char *saberStaffHiltInfo [MAX_SABER_HILTS];

qboolean UI_SaberProperNameForSaber( const char *saberName, char *saberProperName );
void UI_SaberGetHiltInfo( const char *singleHilts[MAX_SABER_HILTS], const char *staffHilts[MAX_SABER_HILTS] );


static void UI_UpdateCharacter( qboolean changedModel )
{
	menuDef_t *menu;
	itemDef_t *item;
	char modelPath[MAX_QPATH];
	int	animRunLength;

	menu = Menu_GetFocused();	// Get current menu

	if (!menu)
	{
		return;
	}

	item = (itemDef_t *) Menu_FindItemByName(menu, "character");

	if (!item)
	{
		Com_Error( ERR_FATAL, "UI_UpdateCharacter: Could not find item (character) in menu (%s)", menu->window.name);
	}

	ItemParse_model_g2anim_go( item, ui_char_anim.string );

	Com_sprintf( modelPath, sizeof( modelPath ), "models/players/%s/model.glm", UI_Cvar_VariableString ( "ui_char_model" ) );
	ItemParse_asset_model_go( item, modelPath, &animRunLength );

	if ( changedModel )
	{//set all skins to first skin since we don't know you always have all skins
		//FIXME: could try to keep the same spot in each list as you swtich models
		UI_FeederSelection(FEEDER_PLAYER_SKIN_HEAD, 0, item);	//fixme, this is not really the right item!!
		UI_FeederSelection(FEEDER_PLAYER_SKIN_TORSO, 0, item);
		UI_FeederSelection(FEEDER_PLAYER_SKIN_LEGS, 0, item);
		UI_FeederSelection(FEEDER_COLORCHOICES, 0, item);
	}
	UI_UpdateCharacterSkin();
}

static void UI_RunMenuScript(char **args) 
{
	const char *name, *name2;
	char buff[1024];

	if (String_Parse(args, &name)) 
	{
		if (Q_stricmp(name, "StartServer") == 0) 
		{
			int i, added = 0;
			float skill;
			
			trap_Cvar_Set("cg_thirdPerson", "0");
			trap_Cvar_Set("cg_cameraOrbit", "0");
			// for Solo games I set this to 1 in the menu and don't want it stomped here,
			// this cvar seems to be reset to 0 in all the proper places so... -dmv
		//	trap_Cvar_Set("ui_singlePlayerActive", "0");

			// if a solo game is started, automatically turn dedicated off here (don't want to do it in the menu, might get annoying)
			if( trap_Cvar_VariableValue( "ui_singlePlayerActive" ) )
			{
				trap_Cvar_Set( "dedicated", "0" );
			}
			else
			{
				trap_Cvar_SetValue( "dedicated", Com_Clamp( 0, 2, ui_dedicated.integer ) );
			}
			trap_Cvar_SetValue( "g_gametype", Com_Clamp( 0, 8, uiInfo.gameTypes[ui_netGameType.integer].gtEnum ) );
			//trap_Cvar_Set("g_redTeam", UI_Cvar_VariableString("ui_teamName"));
			//trap_Cvar_Set("g_blueTeam", UI_Cvar_VariableString("ui_opponentName"));
			trap_Cmd_ExecuteText( EXEC_APPEND, va( "wait ; wait ; map %s\n", uiInfo.mapList[ui_currentNetMap.integer].mapLoadName ) );
			skill = trap_Cvar_VariableValue( "g_spSkill" );

			if (trap_Cvar_VariableValue( "g_gametype" ) == GT_DUEL ||
				trap_Cvar_VariableValue( "g_gametype" ) == GT_POWERDUEL)
			{ //always set fraglimit 1 when starting a duel game
				trap_Cvar_Set("fraglimit", "1");
				trap_Cvar_Set("timelimit", "0");
			}

			for (i = 0; i < PLAYERS_PER_TEAM; i++) 
			{
				int bot = trap_Cvar_VariableValue( va("ui_blueteam%i", i+1));
				int maxcl = trap_Cvar_VariableValue( "sv_maxClients" );

				if (bot > 1) 
				{
					int numval = i+1;

					numval *= 2;

					numval -= 1;

					if (numval <= maxcl)
					{
						if (ui_actualNetGameType.integer >= GT_TEAM) {
							Com_sprintf( buff, sizeof(buff), "addbot \"%s\" %f %s\n", UI_GetBotNameByNumber(bot-2), skill, "Blue");
						} else {
							Com_sprintf( buff, sizeof(buff), "addbot \"%s\" %f \n", UI_GetBotNameByNumber(bot-2), skill);
						}
						trap_Cmd_ExecuteText( EXEC_APPEND, buff );
						added++;
					}
				}
				bot = trap_Cvar_VariableValue( va("ui_redteam%i", i+1));
				if (bot > 1) {
					int numval = i+1;

					numval *= 2;

					if (numval <= maxcl)
					{
						if (ui_actualNetGameType.integer >= GT_TEAM) {
							Com_sprintf( buff, sizeof(buff), "addbot \"%s\" %f %s\n", UI_GetBotNameByNumber(bot-2), skill, "Red");
						} else {
							Com_sprintf( buff, sizeof(buff), "addbot \"%s\" %f \n", UI_GetBotNameByNumber(bot-2), skill);
						}
						trap_Cmd_ExecuteText( EXEC_APPEND, buff );
						added++;
					}
				}
				if (added >= maxcl)
				{ //this means the client filled up all their slots in the UI with bots. So stretch out an extra slot for them, and then stop adding bots.
					trap_Cvar_Set("sv_maxClients", va("%i", added+1));
					break;
				}
			}
		} else if (Q_stricmp(name, "updateSPMenu") == 0) {
			UI_SetCapFragLimits(qtrue);
			UI_MapCountByGameType(qtrue);
			ui_mapIndex.integer = UI_GetIndexFromSelection(ui_currentMap.integer);
			trap_Cvar_Set("ui_mapIndex", va("%d", ui_mapIndex.integer));
			Menu_SetFeederSelection(NULL, FEEDER_MAPS, ui_mapIndex.integer, "skirmish");
			UI_GameType_HandleKey(0, 0, A_MOUSE1, qfalse);
			UI_GameType_HandleKey(0, 0, A_MOUSE2, qfalse);
		} else if (Q_stricmp(name, "resetDefaults") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, "cvar_restart\n");
			trap_Cmd_ExecuteText( EXEC_APPEND, "exec mpdefault.cfg\n");
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart\n" );
			trap_Cvar_Set("com_introPlayed", "1" );
		} else if (Q_stricmp(name, "saveControls") == 0) {
			Controls_SetConfig(qtrue);
		} else if (Q_stricmp(name, "loadControls") == 0) {
			Controls_GetConfig();
		} else if (Q_stricmp(name, "clearError") == 0) {
			trap_Cvar_Set("com_errorMessage", "");
		} else if (Q_stricmp(name, "loadGameInfo") == 0) {
			UI_ParseGameInfo("ui/jamp/gameinfo.txt");
		} else if (Q_stricmp(name, "RefreshServers") == 0) {
			UI_StartServerRefresh(qtrue);
			UI_BuildServerDisplayList(1);
		} else if (Q_stricmp(name, "RefreshFilter") == 0) {
			UI_StartServerRefresh(qfalse);
			UI_BuildServerDisplayList(1);
		} else if (Q_stricmp(name, "RunSPDemo") == 0) {
			if (uiInfo.demoAvailable) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("demo %s_%i\n", uiInfo.mapList[ui_currentMap.integer].mapLoadName, uiInfo.gameTypes[ui_gameType.integer].gtEnum));
			}
		} else if (Q_stricmp(name, "LoadDemos") == 0) {
			UI_LoadDemos();
		} else if (Q_stricmp(name, "LoadMovies") == 0) {
			UI_LoadMovies();
		} else if (Q_stricmp(name, "LoadMods") == 0) {
			UI_LoadMods();
		} else if (Q_stricmp(name, "playMovie") == 0) {
			if (uiInfo.previewMovie >= 0) {
				trap_CIN_StopCinematic(uiInfo.previewMovie);
			}
			trap_Cmd_ExecuteText( EXEC_APPEND, va("cinematic %s.roq 2\n", uiInfo.movieList[uiInfo.movieIndex]));
		} else if (Q_stricmp(name, "RunMod") == 0) {
			trap_Cvar_Set( "fs_game", uiInfo.modList[uiInfo.modIndex].modName);
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		} else if (Q_stricmp(name, "RunDemo") == 0) {
			trap_Cmd_ExecuteText( EXEC_APPEND, va("demo \"%s\"\n", uiInfo.demoList[uiInfo.demoIndex]));
		} else if (Q_stricmp(name, "Quake3") == 0) {
			trap_Cvar_Set( "fs_game", "");
			trap_Cmd_ExecuteText( EXEC_APPEND, "vid_restart;" );
		} else if (Q_stricmp(name, "closeJoin") == 0) {
			if (uiInfo.serverStatus.refreshActive) {
				UI_StopServerRefresh();
				uiInfo.serverStatus.nextDisplayRefresh = 0;
				uiInfo.nextServerStatusRefresh = 0;
				uiInfo.nextFindPlayerRefresh = 0;
				UI_BuildServerDisplayList(1);
			} else {
				Menus_CloseByName("joinserver");
				Menus_OpenByName("main");
			}
		} else if (Q_stricmp(name, "StopRefresh") == 0) {
			UI_StopServerRefresh();
			uiInfo.serverStatus.nextDisplayRefresh = 0;
			uiInfo.nextServerStatusRefresh = 0;
			uiInfo.nextFindPlayerRefresh = 0;
		} else if (Q_stricmp(name, "UpdateFilter") == 0) {
			if (ui_netSource.integer == AS_LOCAL) {
				UI_StartServerRefresh(qtrue);
			}
			UI_BuildServerDisplayList(1);
			UI_FeederSelection(FEEDER_SERVERS, 0, NULL );

		} else if (Q_stricmp(name, "ServerStatus") == 0) {
			trap_LAN_GetServerAddressString(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], uiInfo.serverStatusAddress, sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
		} else if (Q_stricmp(name, "FoundPlayerServerStatus") == 0) {
			Q_strncpyz(uiInfo.serverStatusAddress, uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer], sizeof(uiInfo.serverStatusAddress));
			UI_BuildServerStatus(qtrue);
			Menu_SetFeederSelection(NULL, FEEDER_FINDPLAYER, 0, NULL);
		} else if (Q_stricmp(name, "FindPlayer") == 0) {
			UI_BuildFindPlayerList(qtrue);
			// clear the displayed server status info
			uiInfo.serverStatusInfo.numLines = 0;
			Menu_SetFeederSelection(NULL, FEEDER_FINDPLAYER, 0, NULL);
		}
		else if (Q_stricmp(name, "checkservername") == 0)
		{
			UI_CheckServerName();

		}
		else if (Q_stricmp(name, "checkpassword") == 0)
		{
			if( UI_CheckPassword() )
			{
				UI_JoinServer();
			}

		}
		else if (Q_stricmp(name, "JoinServer") == 0)
		{
			UI_JoinServer();
		}
		else if (Q_stricmp(name, "FoundPlayerJoinServer") == 0) {
			trap_Cvar_Set("ui_singlePlayerActive", "0");
			if (uiInfo.currentFoundPlayerServer >= 0 && uiInfo.currentFoundPlayerServer < uiInfo.numFoundPlayerServers) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer] ) );
			}
		} else if (Q_stricmp(name, "Quit") == 0) {
			trap_Cvar_Set("ui_singlePlayerActive", "0");
			trap_Cmd_ExecuteText( EXEC_NOW, "quit");
		} else if (Q_stricmp(name, "Controls") == 0) {
			trap_Cvar_Set( "cl_paused", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			Menus_CloseAll();
			Menus_ActivateByName("setup_menu2");
		} 
		else if (Q_stricmp(name, "Leave") == 0) 
		{
			trap_Cmd_ExecuteText( EXEC_APPEND, "disconnect\n" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			Menus_CloseAll();
			Menus_ActivateByName("main");
		} 
		else if (Q_stricmp(name, "getvideosetup") == 0) 
		{
			UI_GetVideoSetup ( );
		}
		else if (Q_stricmp(name, "getsaberhiltinfo") == 0) 
		{
			UI_SaberGetHiltInfo(const_cast<const char **>(saberSingleHiltInfo), const_cast<const char **>(saberStaffHiltInfo));
		}
		else if (Q_stricmp(name, "updatevideosetup") == 0)
		{
			UI_UpdateVideoSetup ( );
		}
		else if (Q_stricmp(name, "ServerSort") == 0) 
		{
			int sortColumn;
			if (Int_Parse(args, &sortColumn)) {
				// if same column we're already sorting on then flip the direction
				if (sortColumn == uiInfo.serverStatus.sortKey) {
					uiInfo.serverStatus.sortDir = !uiInfo.serverStatus.sortDir;
				}
				// make sure we sort again
				UI_ServersSort(sortColumn, qtrue);
			}
		} else if (Q_stricmp(name, "nextSkirmish") == 0) {
			UI_StartSkirmish(qtrue);
		} else if (Q_stricmp(name, "SkirmishStart") == 0) {
			UI_StartSkirmish(qfalse);
		} else if (Q_stricmp(name, "closeingame") == 0) {
			trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
			trap_Key_ClearStates();
			trap_Cvar_Set( "cl_paused", "0" );
			Menus_CloseAll();
		} else if (Q_stricmp(name, "voteMap") == 0) {
			if (ui_currentNetMap.integer >=0 && ui_currentNetMap.integer < uiInfo.mapCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote map %s\n",uiInfo.mapList[ui_currentNetMap.integer].mapLoadName) );
			}
		} else if (Q_stricmp(name, "voteKick") == 0) {
			if (uiInfo.playerIndex >= 0 && uiInfo.playerIndex < uiInfo.playerCount) {
				//trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote kick \"%s\"\n",uiInfo.playerNames[uiInfo.playerIndex]) );
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote clientkick \"%i\"\n",uiInfo.playerIndexes[uiInfo.playerIndex]) );
			}
		} else if (Q_stricmp(name, "voteGame") == 0) {
			if (ui_netGameType.integer >= 0 && ui_netGameType.integer < uiInfo.numGameTypes) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callvote g_gametype %i\n",uiInfo.gameTypes[ui_netGameType.integer].gtEnum) );
			}
		} else if (Q_stricmp(name, "voteLeader") == 0) {
			if (uiInfo.teamIndex >= 0 && uiInfo.teamIndex < uiInfo.myTeamCount) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("callteamvote leader \"%s\"\n",uiInfo.teamNames[uiInfo.teamIndex]) );
			}
		} else if (Q_stricmp(name, "addBot") == 0) {
			if (trap_Cvar_VariableValue("g_gametype") >= GT_TEAM) {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot \"%s\" %i %s\n", UI_GetBotNameByNumber(uiInfo.botIndex), uiInfo.skillIndex+1, (uiInfo.redBlue == 0) ? "Red" : "Blue") );
			} else {
				trap_Cmd_ExecuteText( EXEC_APPEND, va("addbot \"%s\" %i %s\n", UI_GetBotNameByNumber(uiInfo.botIndex), uiInfo.skillIndex+1, (uiInfo.redBlue == 0) ? "Red" : "Blue") );
			}
		} else if (Q_stricmp(name, "addFavorite") == 0) 
		{
			if (ui_netSource.integer != AS_FAVORITES) 
			{
				char name[MAX_HOSTNAMELENGTH] = {0};
				char addr[MAX_ADDRESSLENGTH] = {0};
				int res;
				//[MasterServer]
				trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				//[/MasterServer]
				name[0] = addr[0] = '\0';
				Q_strncpyz(name, 	Info_ValueForKey(buff, "hostname"), sizeof( name ) );
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), sizeof( addr ) );
				if (strlen(name) > 0 && strlen(addr) > 0) 
				{
					res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
					if (res == 0) 
					{
						// server already in the list
						Com_Printf("Favorite already in list\n");
					}
					else if (res == -1) 
					{
						// list full
						Com_Printf("Favorite list full\n");
					}
					else 
					{
						// successfully added
						Com_Printf("Added favorite server %s\n", addr);


//						trap_SP_GetStringTextString((char *)va("%s_GETTINGINFOFORSERVERS",uiInfo.uiDC.Assets.stringedFile), holdSPString, sizeof(holdSPString));
//						Text_Paint(rect->x, rect->y, scale, newColor, va((char *) holdSPString, trap_LAN_GetServerCount(ui_netSource.integer)), 0, 0, textStyle);

					}
				}
			}
		} 
		else if (Q_stricmp(name, "deleteFavorite") == 0) 
		{
			if (ui_netSource.integer == AS_FAVORITES) 
			{
				char addr[MAX_NAME_LENGTH];
				//[MasterServer]
				trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, MAX_STRING_CHARS);
				//[/MasterServer]
				addr[0] = '\0';
				Q_strncpyz(addr, 	Info_ValueForKey(buff, "addr"), sizeof( addr ) );
				if (strlen(addr) > 0) 
				{
					trap_LAN_RemoveServer(AS_FAVORITES, addr);
				}
			}
		} 
		else if (Q_stricmp(name, "createFavorite") == 0) 
		{
		//	if (ui_netSource.integer == AS_FAVORITES) 
		//rww - don't know why this check was here.. why would you want to only add new favorites when the filter was favorites?
			{
				char name[MAX_HOSTNAMELENGTH] = {0};
				char addr[MAX_ADDRESSLENGTH] = {0};
				int res;

				name[0] = addr[0] = '\0';
				Q_strncpyz(name, 	UI_Cvar_VariableString("ui_favoriteName"), sizeof( name ) );
				Q_strncpyz(addr, 	UI_Cvar_VariableString("ui_favoriteAddress"), sizeof( addr ) );
				if (/*strlen(name) > 0 &&*/ strlen(addr) > 0) {
					res = trap_LAN_AddServer(AS_FAVORITES, name, addr);
					if (res == 0) {
						// server already in the list
						Com_Printf("Favorite already in list\n");
					}
					else if (res == -1) {
						// list full
						Com_Printf("Favorite list full\n");
					}
					else {
						// successfully added
						Com_Printf("Added favorite server %s\n", addr);
					}
				}
			}
		} else if (Q_stricmp(name, "orders") == 0) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer < uiInfo.myTeamCount) {
					strcpy(buff, orders);
					trap_Cmd_ExecuteText( EXEC_APPEND, va(buff, uiInfo.teamClientNums[selectedPlayer]) );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				} else {
					int i;
					for (i = 0; i < uiInfo.myTeamCount; i++) {
						if (Q_stricmp(UI_Cvar_VariableString("name"), uiInfo.teamNames[i]) == 0) {
							continue;
						}
						strcpy(buff, orders);
						trap_Cmd_ExecuteText( EXEC_APPEND, va(buff, uiInfo.teamNames[i]) );
						trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
					}
				}
				trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
				trap_Key_ClearStates();
				trap_Cvar_Set( "cl_paused", "0" );
				Menus_CloseAll();
			}
		} else if (Q_stricmp(name, "voiceOrdersTeam") == 0) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");
				if (selectedPlayer == uiInfo.myTeamCount) {
					trap_Cmd_ExecuteText( EXEC_APPEND, orders );
					trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );
				}
				trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
				trap_Key_ClearStates();
				trap_Cvar_Set( "cl_paused", "0" );
				Menus_CloseAll();
			}
		} else if (Q_stricmp(name, "voiceOrders") == 0) {
			const char *orders;
			if (String_Parse(args, &orders)) {
				int selectedPlayer = trap_Cvar_VariableValue("cg_selectedPlayer");

				if (selectedPlayer == uiInfo.myTeamCount)
				{
					selectedPlayer = -1;
					strcpy(buff, orders);
					trap_Cmd_ExecuteText( EXEC_APPEND, va(buff, selectedPlayer) );
				}
				else
				{
					strcpy(buff, orders);
					trap_Cmd_ExecuteText( EXEC_APPEND, va(buff, uiInfo.teamClientNums[selectedPlayer]) );
				}
				trap_Cmd_ExecuteText( EXEC_APPEND, "\n" );

				trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
				trap_Key_ClearStates();
				trap_Cvar_Set( "cl_paused", "0" );
				Menus_CloseAll();
			}
		}
		else if (Q_stricmp(name, "setBotButton") == 0) 
		{
			UI_SetBotButton();
		}
		else if (Q_stricmp(name, "glCustom") == 0) {
			trap_Cvar_Set("ui_r_glCustom", "4");
		} 
		else if (Q_stricmp(name, "setMovesListDefault") == 0) 
		{
			uiInfo.movesTitleIndex = 2;
		}
		else if (Q_stricmp(name, "resetMovesList") == 0) 
		{
			menuDef_t *menu;
			menu = Menus_FindByName("rulesMenu_moves");
			//update saber models
			if (menu)
			{
				itemDef_t *item  = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "character");
				if (item)
				{
					UI_SaberAttachToChar( item );
				}
			}

			trap_Cvar_Set( "ui_move_desc", " " );
		}
		else if (Q_stricmp(name, "resetcharacterlistboxes") == 0) 
		{
			UI_ResetCharacterListBoxes();
		}
		else if (Q_stricmp(name, "setMoveCharacter") == 0) 
		{
			itemDef_t *item;
			menuDef_t *menu;
			modelDef_t *modelPtr;
			int	animRunLength;

			UI_GetCharacterCvars();

			uiInfo.movesTitleIndex = 0;

			menu = Menus_FindByName("rulesMenu_moves");

			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "character");
				if (item)
				{
					modelPtr = (modelDef_t*)item->typeData;
					if (modelPtr)
					{
						char modelPath[MAX_QPATH];

						uiInfo.movesBaseAnim = datapadMoveTitleBaseAnims[uiInfo.movesTitleIndex];
						ItemParse_model_g2anim_go( item,  uiInfo.movesBaseAnim );
						uiInfo.moveAnimTime = 0 ;

						Com_sprintf( modelPath, sizeof( modelPath ), "models/players/%s/model.glm", UI_Cvar_VariableString ( "ui_char_model" ) );
						ItemParse_asset_model_go( item, modelPath, &animRunLength);

						UI_UpdateCharacterSkin();
						UI_SaberAttachToChar( item );
					}
				}
			} 
		}
		else if (Q_stricmp(name, "character") == 0) 
		{
			UI_UpdateCharacter( qfalse );
		}
		else if (Q_stricmp(name, "characterchanged") == 0) 
		{
			UI_UpdateCharacter( qtrue );
		}
		else if (Q_stricmp(name, "updatecharcvars") == 0
			|| (Q_stricmp(name, "updatecharmodel") == 0) )
		{
			UI_UpdateCharacterCvars();
		}
		else if (Q_stricmp(name, "getcharcvars") == 0) 
		{
			UI_GetCharacterCvars();
		}
		else if (Q_stricmp(name, "char_skin") == 0) 
		{
			UI_UpdateCharacterSkin();
		}
		else if (Q_stricmp(name, "clearmouseover") == 0) 
		{
			itemDef_t *item;
			menuDef_t *menu = Menu_GetFocused();

			if (menu) 
			{
				int count,j;
				const char *itemName;
				String_Parse(args, &itemName);

				count = Menu_ItemsMatchingGroup(menu, itemName);

				for (j = 0; j < count; j++) 
				{
					item = Menu_GetMatchingItemByNumber( menu, j, itemName);
					if (item != NULL) 
					{
						item->window.flags &= ~WINDOW_MOUSEOVER;
					}
				}
			}

		}
		else if (Q_stricmp(name, "update") == 0) 
		{
			if (String_Parse(args, &name2)) 
			{
				UI_Update(name2);
			}
		}
		else if (Q_stricmp(name, "setBotButtons") == 0)
		{
			UpdateBotButtons();
		}
		else if (Q_stricmp(name, "getsabercvars") == 0) 
		{
			UI_GetSaberCvars();
		}
		else if (Q_stricmp(name, "setsaberboxesandhilts") == 0) 
		{
			UI_SetSaberBoxesandHilts();
		}
		else if (Q_stricmp(name, "saber_type") == 0) 
		{
			UI_UpdateSaberType();
		}
		else if (Q_stricmp(name, "saber_hilt") == 0) 
		{
			UI_UpdateSaberHilt( qfalse );
		}
		else if (Q_stricmp(name, "saber_color") == 0) 
		{
			UI_UpdateSaberColor( qfalse );
		}
		else if (Q_stricmp(name, "setscreensaberhilt") == 0) 
		{
			menuDef_t *menu;
			itemDef_t *item;

			menu = Menu_GetFocused();	// Get current menu
			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "hiltbut");
				if (item)
				{
					if (saberSingleHiltInfo[item->cursorPos])
					{
						trap_Cvar_Set( "ui_saber", saberSingleHiltInfo[item->cursorPos] );
					}
				}
			}
		}
		else if (Q_stricmp(name, "setscreensaberhilt1") == 0) 
		{
			menuDef_t *menu;
			itemDef_t *item;

			menu = Menu_GetFocused();	// Get current menu
			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "hiltbut1");
				if (item)
				{
					if (saberSingleHiltInfo[item->cursorPos])
					{
						trap_Cvar_Set( "ui_saber", saberSingleHiltInfo[item->cursorPos] );
					}
				}
			}
		}
		else if (Q_stricmp(name, "setscreensaberhilt2") == 0) 
		{
			menuDef_t *menu;
			itemDef_t *item;

			menu = Menu_GetFocused();	// Get current menu
			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "hiltbut2");
				if (item)
				{
					if (saberSingleHiltInfo[item->cursorPos])
					{
						trap_Cvar_Set( "ui_saber2", saberSingleHiltInfo[item->cursorPos] );
					}
				}
			}
		}
		else if (Q_stricmp(name, "setscreensaberstaff") == 0) 
		{
			menuDef_t *menu;
			itemDef_t *item;

			menu = Menu_GetFocused();	// Get current menu
			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "hiltbut_staves");
				if (item)
				{
					if (saberSingleHiltInfo[item->cursorPos])
					{
						trap_Cvar_Set( "ui_saber", saberStaffHiltInfo[item->cursorPos] );
					}
				}
			}
		}
		else if (Q_stricmp(name, "saber2_hilt") == 0) 
		{
			UI_UpdateSaberHilt( qtrue );
		}
		else if (Q_stricmp(name, "saber2_color") == 0) 
		{
			UI_UpdateSaberColor( qtrue );
		}
		else if (Q_stricmp(name, "updatesabercvars") == 0) 
		{
			UI_UpdateSaberCvars();
		}
		else if (Q_stricmp(name, "setcurrentNetMap") == 0) 
		{
			menuDef_t *menu;
			itemDef_t *item;

			menu = Menu_GetFocused();	// Get current menu
			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "maplist");
				if (item)
				{
					listBoxDef_t *listPtr = (listBoxDef_t*)item->typeData;
					if (listPtr)
					{
						trap_Cvar_Set("ui_currentNetMap", va("%d",listPtr->cursorPos));
					}
				}
			}
		}
		else if (Q_stricmp(name, "resetmaplist") == 0) 
		{
			menuDef_t *menu;
			itemDef_t *item;

			menu = Menu_GetFocused();	// Get current menu
			if (menu)
			{
				item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "maplist");
				if (item)
				{
					uiInfo.uiDC.feederSelection(item->special, item->cursorPos, item);
				}
			}
		}
		else if (Q_stricmp(name, "getmousepitch") == 0)
		{
			trap_Cvar_Set("ui_mousePitch", (trap_Cvar_VariableValue("m_pitch") >= 0) ? "0" : "1");
		}
		else if (Q_stricmp(name, "clampmaxplayers") == 0)
		{
			UI_ClampMaxPlayers();
		}
		else 
		{
			Com_Printf("unknown UI script %s\n", name);
		}
	}
}

static void UI_GetTeamColor(vec4_t *color) {
}

void UI_ClampMaxPlayers(void)
{
	char	buf[32];
	// min checks
	//
	if( uiInfo.gameTypes[ui_netGameType.integer].gtEnum == GT_DUEL ) //DUEL
	{
		if( trap_Cvar_VariableValue("sv_maxClients") < 2 )
		{
			trap_Cvar_Set("sv_maxClients", "2");
		}
	}
	else if( uiInfo.gameTypes[ui_netGameType.integer].gtEnum == GT_POWERDUEL ) // POWER DUEL
	{
		if( trap_Cvar_VariableValue("sv_maxClients") < 3 )
		{
			trap_Cvar_Set("sv_maxClients", "3");
		}
	}


	// max check for all game types
	if( trap_Cvar_VariableValue("sv_maxClients") > MAX_CLIENTS )
	{
		sprintf(buf,"%d",MAX_CLIENTS);
		trap_Cvar_Set("sv_maxClients", buf);
	}

}

void	UI_UpdateSiegeStatusIcons(void)
{
	menuDef_t *menu = Menu_GetFocused();
	int i;

	menu = Menu_GetFocused();	// Get current menu

	if (!menu)
	{
		return;
	}

	for (i=0;i<7;i++)
	{

		Menu_SetItemBackground(menu,va("wpnicon0%d",i), va("*ui_class_weapon%d",i));
	}

	for (i=0;i<7;i++)
	{
		Menu_SetItemBackground(menu,va("itemicon0%d",i), va("*ui_class_item%d",i));
	}

	for (i=0;i<10;i++)
	{
		Menu_SetItemBackground(menu,va("forceicon0%d",i), va("*ui_class_power%d",i));
	}

	for (i=10;i<15;i++)
	{
		Menu_SetItemBackground(menu,va("forceicon%d",i), va("*ui_class_power%d",i));
	}

}

/*
==================
UI_MapCountByGameType
==================
*/
static int UI_MapCountByGameType(qboolean singlePlayer) {
	int i, c, game;
	c = 0;
	game = singlePlayer ? uiInfo.gameTypes[ui_gameType.integer].gtEnum : uiInfo.gameTypes[ui_netGameType.integer].gtEnum;
	if (game == GT_SINGLE_PLAYER) {
		game++;
	} 
	if (game == GT_TEAM) {
		game = GT_FFA;
	}

	for (i = 0; i < uiInfo.mapCount; i++) {
		uiInfo.mapList[i].active = qfalse;
		if ( uiInfo.mapList[i].typeBits & (1 << game)) {
			if (singlePlayer) {
				if (!(uiInfo.mapList[i].typeBits & (1 << GT_SINGLE_PLAYER))) {
					continue;
				}
			}
			c++;
			uiInfo.mapList[i].active = qtrue;
		}
	}
	return c;
}

qboolean UI_hasSkinForBase(const char *base, const char *team) {
	char	test[1024];
	fileHandle_t	f;

	Com_sprintf( test, sizeof( test ), "models/players/%s/%s/lower_default.skin", base, team );
	trap_FS_FOpenFile(test, &f, FS_READ);
	if (f != 0) {
		trap_FS_FCloseFile(f);
		return qtrue;
	}
	Com_sprintf( test, sizeof( test ), "models/players/characters/%s/%s/lower_default.skin", base, team );
	trap_FS_FOpenFile(test, &f, FS_READ);
	if (f != 0) {
		trap_FS_FCloseFile(f);
		return qtrue;
	}
	return qfalse;
}

/*
==================
UI_HeadCountByColor
==================
*/
static int UI_HeadCountByColor() {
	int i, c;
	char *teamname;

	c = 0;

	if(ui_gameType.integer >= GT_TEAM)
	{
		// Let's not be preposterous..
		int gwTeam;
		if(uiSkinColor == TEAM_BLUE || uiSkinColor == TEAM_RED)
		{
			gwTeam = (uiSkinColor == TEAM_RED) ? cgImports->GetRedTeam() : cgImports->GetBlueTeam();
			if(bgGangWarsTeams[gwTeam].useTeamColors)
			{
				if(uiSkinColor == TEAM_BLUE)
				{
					teamname = "/blue";
					goto goAndCollectDollars;
				}
				else
				{
					teamname = "/red";
					goto goAndCollectDollars;
				}
			}
			else
			{
				return bgGangWarsTeams[gwTeam].numModelsInStore;
			}
		}
	}
	switch(uiSkinColor)
	{
		case TEAM_BLUE:
			teamname = "/blue";
			break;
		case TEAM_RED:
			teamname = "/red";
			break;
		default:
			teamname = "/default";
	}

	// Count each head with this color
goAndCollectDollars:
	for (i=0; i<uiInfo.q3HeadCount; i++)
	{
		if (uiInfo.q3HeadNames[i] && strstr(uiInfo.q3HeadNames[i], teamname))
		{
			c++;
		}
	}
	return c;
}

/*
==================
UI_InsertServerIntoDisplayList
==================
*/
static void UI_InsertServerIntoDisplayList(int num, int position) {
	int i;

	if (position < 0 || position > uiInfo.serverStatus.numDisplayServers ) {
		return;
	}
	//
	uiInfo.serverStatus.numDisplayServers++;
	for (i = uiInfo.serverStatus.numDisplayServers; i > position; i--) {
		uiInfo.serverStatus.displayServers[i] = uiInfo.serverStatus.displayServers[i-1];
	}
	uiInfo.serverStatus.displayServers[position] = num;
}

/*
==================
UI_RemoveServerFromDisplayList
==================
*/
static void UI_RemoveServerFromDisplayList(int num) {
	int i, j;

	for (i = 0; i < uiInfo.serverStatus.numDisplayServers; i++) {
		if (uiInfo.serverStatus.displayServers[i] == num) {
			uiInfo.serverStatus.numDisplayServers--;
			for (j = i; j < uiInfo.serverStatus.numDisplayServers; j++) {
				uiInfo.serverStatus.displayServers[j] = uiInfo.serverStatus.displayServers[j+1];
			}
			return;
		}
	}
}

/*
==================
UI_BinaryServerInsertion
==================
*/
static void UI_BinaryServerInsertion(int num) {
	int mid, offset, res, len;

	// use binary search to insert server
	len = uiInfo.serverStatus.numDisplayServers;
	mid = len;
	offset = 0;
	res = 0;
	while(mid > 0) {
		mid = len >> 1;
		//
		//[MasterServer]
		res = trap_LAN_CompareServers( UI_SourceForLAN(), 
			//[/MasterServer]
			uiInfo.serverStatus.sortKey,
			uiInfo.serverStatus.sortDir, num, uiInfo.serverStatus.displayServers[offset+mid]);
		// if equal
		if (res == 0) {
			UI_InsertServerIntoDisplayList(num, offset+mid);
			return;
		}
		// if larger
		else if (res == 1) {
			offset += mid;
			len -= mid;
		}
		// if smaller
		else {
			len -= mid;
		}
	}
	if (res == 1) {
		offset++;
	}
	UI_InsertServerIntoDisplayList(num, offset);
}

/*
==================
UI_BuildServerDisplayList
==================
*/
static void UI_BuildServerDisplayList(int force) {
	int i, count, clients, maxClients, ping, game, len/*, visible*/;
	char info[MAX_STRING_CHARS];
//	qboolean startRefresh = qtrue; TTimo: unused
	static int numinvisible;

	if (!(force || uiInfo.uiDC.realTime > uiInfo.serverStatus.nextDisplayRefresh)) {
		return;
	}
	// if we shouldn't reset
	if ( force == 2 ) {
		force = 0;
	}

	// do motd updates here too
	trap_Cvar_VariableStringBuffer( "cl_motdString", uiInfo.serverStatus.motd, sizeof(uiInfo.serverStatus.motd) );
	len = strlen(uiInfo.serverStatus.motd);
	if (len == 0) {
		strcpy(uiInfo.serverStatus.motd, "Welcome to Jedi Academy MP!");
		len = strlen(uiInfo.serverStatus.motd);
	} 
	if (len != uiInfo.serverStatus.motdLen) {
		uiInfo.serverStatus.motdLen = len;
		uiInfo.serverStatus.motdWidth = -1;
	} 

	if (force) {
		numinvisible = 0;
		// clear number of displayed servers
		uiInfo.serverStatus.numDisplayServers = 0;
		uiInfo.serverStatus.numPlayersOnServers = 0;
		// set list box index to zero
		Menu_SetFeederSelection(NULL, FEEDER_SERVERS, 0, NULL);
		// mark all servers as visible so we store ping updates for them
		trap_LAN_MarkServerVisible(UI_SourceForLAN(), -1, qtrue);
	}

	// get the server count (comes from the master)
	count = trap_LAN_GetServerCount(UI_SourceForLAN());
	if (count == -1 || (ui_netSource.integer == AS_LOCAL && count == 0) ) {
		// still waiting on a response from the master
		uiInfo.serverStatus.numDisplayServers = 0;
		uiInfo.serverStatus.numPlayersOnServers = 0;
		uiInfo.serverStatus.nextDisplayRefresh = uiInfo.uiDC.realTime + 500;
		return;
	}

//	visible = qfalse;
	for (i = 0; i < count; i++) {
		// if we already got info for this server
		if (!trap_LAN_ServerIsVisible(UI_SourceForLAN(), i)) {
			continue;
		}
//		visible = qtrue;
		// get the ping for this server
		ping = trap_LAN_GetServerPing(UI_SourceForLAN(), i);
		if (ping > 0 || ui_netSource.integer == AS_FAVORITES) {

			trap_LAN_GetServerInfo(UI_SourceForLAN(), i, info, MAX_STRING_CHARS);

			clients = atoi(Info_ValueForKey(info, "clients"));
			uiInfo.serverStatus.numPlayersOnServers += clients;

			if (ui_browserShowEmpty.integer == 0) {
				if (clients == 0) {
					trap_LAN_MarkServerVisible(UI_SourceForLAN(), i, qfalse);
					continue;
				}
			}

			if (ui_browserShowFull.integer == 0) {
				maxClients = atoi(Info_ValueForKey(info, "sv_maxclients"));
				if (clients == maxClients) {
					trap_LAN_MarkServerVisible(UI_SourceForLAN(), i, qfalse);
					continue;
				}
			}

			if (uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum != -1) {
				game = atoi(Info_ValueForKey(info, "gametype"));
				if (game != uiInfo.joinGameTypes[ui_joinGameType.integer].gtEnum) {
					trap_LAN_MarkServerVisible(UI_SourceForLAN(), i, qfalse);
					continue;
				}
			}

			if (ui_serverFilterType.integer > 0) {
				if (Q_stricmp(Info_ValueForKey(info, "game"), serverFilters[ui_serverFilterType.integer].basedir) != 0 &&
					serverFilters[ui_serverFilterType.integer].basedir[0]) {
						trap_LAN_MarkServerVisible(UI_SourceForLAN(), i, qfalse);
						continue;
				}
			}
			// make sure we never add a favorite server twice
			if (ui_netSource.integer == AS_FAVORITES) {
				UI_RemoveServerFromDisplayList(i);
			}
			// insert the server into the list
			UI_BinaryServerInsertion(i);
			// done with this server
			if (ping > 0) {
				trap_LAN_MarkServerVisible(UI_SourceForLAN(), i, qfalse);
				numinvisible++;
			}
		}
	}

	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime;

	// if there were no servers visible for ping updates
//	if (!visible) {
//		UI_StopServerRefresh();
//		uiInfo.serverStatus.nextDisplayRefresh = 0;
//	}
}

typedef struct
{
	char *name, *altName;
} serverStatusCvar_t;

serverStatusCvar_t serverStatusCvars[] = {
	{"sv_hostname", "Name"},
	{"Address", ""},
	{"gamename", "Game name"},
	{"g_gametype", "Game type"},
	{"mapname", "Map"},
	{"version", ""},
	{"protocol", ""},
	{"timelimit", ""},
	{"fraglimit", ""},
	{NULL, NULL}
};

/*
==================
UI_SortServerStatusInfo
==================
*/
static void UI_SortServerStatusInfo( serverStatusInfo_t *info ) {
	int i, j, index;
	char *tmp1, *tmp2;

	// FIXME: if "gamename" == "base" or "missionpack" then
	// replace the gametype number by FFA, CTF etc.
	//
	index = 0;
	for (i = 0; serverStatusCvars[i].name; i++) {
		for (j = 0; j < info->numLines; j++) {
			if ( !info->lines[j][1] || info->lines[j][1][0] ) {
				continue;
			}
			if ( !Q_stricmp(serverStatusCvars[i].name, info->lines[j][0]) ) {
				// swap lines
				tmp1 = info->lines[index][0];
				tmp2 = info->lines[index][3];
				info->lines[index][0] = info->lines[j][0];
				info->lines[index][3] = info->lines[j][3];
				info->lines[j][0] = tmp1;
				info->lines[j][3] = tmp2;
				//
				if ( strlen(serverStatusCvars[i].altName) ) {
					info->lines[index][0] = serverStatusCvars[i].altName;
				}
				index++;
			}
		}
	}
}


/*
==================
UI_CheckPassword
==================
*/
static qboolean UI_CheckPassword( void )
{
	static char info[MAX_STRING_CHARS];
	
	int index = uiInfo.serverStatus.currentServer;
	if( (index < 0) || (index >= uiInfo.serverStatus.numDisplayServers) )
	{	// warning?
		return qfalse;
	}
	
	trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);

	if ( atoi(Info_ValueForKey(info, "needpass")) )
	{

		Menus_OpenByName("password_request");
		return qfalse;

	}

	// This isn't going to make it (too late in dev), like James said I should check to see when we receive
	// a packet *if* we do indeed get a 0 ping just make it 1 so then a 0 ping is guaranteed to be bad
	/*
	// also check ping!
	ping = atoi(Info_ValueForKey(info, "ping"));
	// NOTE : PING -- it's very questionable as to whether a ping of < 0 or <= 0 indicates a bad server
	// what I do know, is that getting "ping" from the ServerInfo on a bad server returns 0.
	// So I'm left with no choice but to not allow you to enter a server with a ping of 0
	if( ping <= 0 )
	{
		Menus_OpenByName("bad_server");
		return qfalse;
	}
	*/


	return qtrue;
	

}

/*
==================
UI_JoinServer
==================
*/
static void UI_JoinServer( void )
{
	char buff[1024] = {0};

	trap_Cvar_Set("cg_thirdPerson", "0");
	trap_Cvar_Set("cg_cameraOrbit", "0");
	trap_Cvar_Set("ui_singlePlayerActive", "0");
	if (uiInfo.serverStatus.currentServer >= 0 && uiInfo.serverStatus.currentServer < uiInfo.serverStatus.numDisplayServers)
	{
		trap_LAN_GetServerAddressString(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.serverStatus.currentServer], buff, 1024);
		trap_Cmd_ExecuteText( EXEC_APPEND, va( "connect %s\n", buff ) );
	}

}



/*
==================
UI_CheckServerName
==================
*/
static void UI_CheckServerName( void )
{
	qboolean	changed = qfalse;

	char hostname[MAX_HOSTNAMELENGTH] = {0};
	char *c = hostname;
			
	trap_Cvar_VariableStringBuffer( "sv_hostname", hostname, sizeof( hostname ) );
	
	while( *c )
	{
		if ( (*c == '\\') || (*c == ';') || (*c == '"'))
		{
			*c = '.';
			changed = qtrue;
		}
		c++;
	}
	if( changed )
	{
		trap_Cvar_Set("sv_hostname", hostname );
	}
	
}


/*
==================
UI_GetServerStatusInfo
==================
*/
static int UI_GetServerStatusInfo( const char *serverAddress, serverStatusInfo_t *info ) {
	char *p, *score, *ping, *name;
	int i, len;

	if (!info) {
		trap_LAN_ServerStatus( serverAddress, NULL, 0);
		return qfalse;
	}
	memset(info, 0, sizeof(*info));
	if ( trap_LAN_ServerStatus( serverAddress, info->text, sizeof(info->text)) ) {
		Q_strncpyz(info->address, serverAddress, sizeof(info->address));
		p = info->text;
		info->numLines = 0;
		info->lines[info->numLines][0] = "Address";
		info->lines[info->numLines][1] = "";
		info->lines[info->numLines][2] = "";
		info->lines[info->numLines][3] = info->address;
		info->numLines++;
		// get the cvars
		while (p && *p) {
			p = strchr(p, '\\');
			if (!p) break;
			*p++ = '\0';
			if (*p == '\\')
				break;
			info->lines[info->numLines][0] = p;
			info->lines[info->numLines][1] = "";
			info->lines[info->numLines][2] = "";
			p = strchr(p, '\\');
			if (!p) break;
			*p++ = '\0';
			info->lines[info->numLines][3] = p;

			info->numLines++;
			if (info->numLines >= MAX_SERVERSTATUS_LINES)
				break;
		}
		// get the player list
		if (info->numLines < MAX_SERVERSTATUS_LINES-3) {
			// empty line
			info->lines[info->numLines][0] = "";
			info->lines[info->numLines][1] = "";
			info->lines[info->numLines][2] = "";
			info->lines[info->numLines][3] = "";
			info->numLines++;
			// header
			info->lines[info->numLines][0] = "num";
			info->lines[info->numLines][1] = "score";
			info->lines[info->numLines][2] = "ping";
			info->lines[info->numLines][3] = "name";
			info->numLines++;
			// parse players
			i = 0;
			len = 0;
			while (p && *p) {
				if (*p == '\\')
					*p++ = '\0';
				if (!p)
					break;
				score = p;
				p = strchr(p, ' ');
				if (!p)
					break;
				*p++ = '\0';
				ping = p;
				p = strchr(p, ' ');
				if (!p)
					break;
				*p++ = '\0';
				name = p;
				Com_sprintf(&info->pings[len], sizeof(info->pings)-len, "%d", i);
				info->lines[info->numLines][0] = &info->pings[len];
				len += strlen(&info->pings[len]) + 1;
				info->lines[info->numLines][1] = score;
				info->lines[info->numLines][2] = ping;
				info->lines[info->numLines][3] = name;
				info->numLines++;
				if (info->numLines >= MAX_SERVERSTATUS_LINES)
					break;
				p = strchr(p, '\\');
				if (!p)
					break;
				*p++ = '\0';
				//
				i++;
			}
		}
		UI_SortServerStatusInfo( info );
		return qtrue;
	}
	return qfalse;
}

/*
==================
stristr
==================
*/
static char *stristr(char *str, char *charset) {
	int i;

	while(*str) {
		for (i = 0; charset[i] && str[i]; i++) {
			if (toupper(charset[i]) != toupper(str[i])) break;
		}
		if (!charset[i]) return str;
		str++;
	}
	return NULL;
}

/*
==================
UI_BuildFindPlayerList
==================
*/
static void UI_BuildFindPlayerList(qboolean force) {
	static int numFound, numTimeOuts;
	int i, j, resend;
	serverStatusInfo_t info;
	char name[MAX_NAME_LENGTH+2];
	char infoString[MAX_STRING_CHARS];

	if (!force) {
		if (!uiInfo.nextFindPlayerRefresh || uiInfo.nextFindPlayerRefresh > uiInfo.uiDC.realTime) {
			return;
		}
	}
	else {
		memset(&uiInfo.pendingServerStatus, 0, sizeof(uiInfo.pendingServerStatus));
		uiInfo.numFoundPlayerServers = 0;
		uiInfo.currentFoundPlayerServer = 0;
		trap_Cvar_VariableStringBuffer( "ui_findPlayer", uiInfo.findPlayerName, sizeof(uiInfo.findPlayerName));
		Q_CleanStr(uiInfo.findPlayerName);
		// should have a string of some length
		if (!strlen(uiInfo.findPlayerName)) {
			uiInfo.nextFindPlayerRefresh = 0;
			return;
		}
		// set resend time
		resend = ui_serverStatusTimeOut.integer / 2 - 10;
		if (resend < 50) {
			resend = 50;
		}
		trap_Cvar_Set("cl_serverStatusResendTime", va("%d", resend));
		// reset all server status requests
		trap_LAN_ServerStatus( NULL, NULL, 0);
		//
		uiInfo.numFoundPlayerServers = 1;

		trap_SP_GetStringTextString("MENUS_SEARCHING", holdSPString, sizeof(holdSPString));
		trap_Cvar_Set( "ui_playerServersFound", va(	holdSPString,uiInfo.pendingServerStatus.num, numFound));
		//	Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
		//					sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
		//						"searching %d...", uiInfo.pendingServerStatus.num);
		numFound = 0;
		numTimeOuts++;
	}
	for (i = 0; i < MAX_SERVERSTATUSREQUESTS; i++) {
		// if this pending server is valid
		if (uiInfo.pendingServerStatus.server[i].valid) {
			// try to get the server status for this server
			if (UI_GetServerStatusInfo( uiInfo.pendingServerStatus.server[i].adrstr, &info ) ) {
				//
				numFound++;
				// parse through the server status lines
				for (j = 0; j < info.numLines; j++) {
					// should have ping info
					if ( !info.lines[j][2] || !info.lines[j][2][0] ) {
						continue;
					}
					// clean string first
					Q_strncpyz(name, info.lines[j][3], sizeof(name));
					Q_CleanStr(name);
					// if the player name is a substring
					if (stristr(name, uiInfo.findPlayerName)) {
						// add to found server list if we have space (always leave space for a line with the number found)
						if (uiInfo.numFoundPlayerServers < MAX_FOUNDPLAYER_SERVERS-1) {
							//
							Q_strncpyz(uiInfo.foundPlayerServerAddresses[uiInfo.numFoundPlayerServers-1],
								uiInfo.pendingServerStatus.server[i].adrstr,
								sizeof(uiInfo.foundPlayerServerAddresses[0]));
							Q_strncpyz(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
								uiInfo.pendingServerStatus.server[i].name,
								sizeof(uiInfo.foundPlayerServerNames[0]));
							uiInfo.numFoundPlayerServers++;
						}
						else {
							// can't add any more so we're done
							uiInfo.pendingServerStatus.num = uiInfo.serverStatus.numDisplayServers;
						}
					}
				}

				trap_SP_GetStringTextString("MENUS_SEARCHING", holdSPString, sizeof(holdSPString));
				trap_Cvar_Set( "ui_playerServersFound", va(	holdSPString,uiInfo.pendingServerStatus.num, numFound));
			//	Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
			//					sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
			//						"searching %d/%d...", uiInfo.pendingServerStatus.num, numFound);
				// retrieved the server status so reuse this spot
				uiInfo.pendingServerStatus.server[i].valid = qfalse;
			}
		}
		// if empty pending slot or timed out
		if (!uiInfo.pendingServerStatus.server[i].valid ||
			uiInfo.pendingServerStatus.server[i].startTime < uiInfo.uiDC.realTime - ui_serverStatusTimeOut.integer) {
			if (uiInfo.pendingServerStatus.server[i].valid) {
				numTimeOuts++;
			}
			// reset server status request for this address
			UI_GetServerStatusInfo( uiInfo.pendingServerStatus.server[i].adrstr, NULL );
			// reuse pending slot
			uiInfo.pendingServerStatus.server[i].valid = qfalse;
			// if we didn't try to get the status of all servers in the main browser yet
			if (uiInfo.pendingServerStatus.num < uiInfo.serverStatus.numDisplayServers) {
				uiInfo.pendingServerStatus.server[i].startTime = uiInfo.uiDC.realTime;
					trap_LAN_GetServerAddressString(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.pendingServerStatus.num],
						uiInfo.pendingServerStatus.server[i].adrstr, sizeof(uiInfo.pendingServerStatus.server[i].adrstr));
					trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[uiInfo.pendingServerStatus.num], infoString, sizeof(infoString));
					Q_strncpyz(uiInfo.pendingServerStatus.server[i].name, Info_ValueForKey(infoString, "hostname"), sizeof(uiInfo.pendingServerStatus.server[0].name));
					uiInfo.pendingServerStatus.server[i].valid = qtrue;
					uiInfo.pendingServerStatus.num++;

					trap_SP_GetStringTextString("MENUS_SEARCHING", holdSPString, sizeof(holdSPString));
					trap_Cvar_Set( "ui_playerServersFound", va(	holdSPString,uiInfo.pendingServerStatus.num, numFound));

					//	Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1],
					//					sizeof(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1]),
					//						"searching %d/%d...", uiInfo.pendingServerStatus.num, numFound);
				}
		}
	}
	for (i = 0; i < MAX_SERVERSTATUSREQUESTS; i++) {
		if (uiInfo.pendingServerStatus.server[i].valid) {
			break;
		}
	}
	// if still trying to retrieve server status info
	if (i < MAX_SERVERSTATUSREQUESTS) {
		uiInfo.nextFindPlayerRefresh = uiInfo.uiDC.realTime + 25;
	}
	else {
		// add a line that shows the number of servers found
		if (!uiInfo.numFoundPlayerServers) 
		{
			Com_sprintf(uiInfo.foundPlayerServerNames[uiInfo.numFoundPlayerServers-1], sizeof(uiInfo.foundPlayerServerAddresses[0]), "no servers found");
		}
		else 
		{
			trap_SP_GetStringTextString("MENUS_SERVERS_FOUNDWITH", holdSPString, sizeof(holdSPString));
			trap_Cvar_Set( "ui_playerServersFound", va(	holdSPString,
				uiInfo.numFoundPlayerServers-1,
				uiInfo.numFoundPlayerServers == 2 ? "":"s",
				uiInfo.findPlayerName) );
		}
		uiInfo.nextFindPlayerRefresh = 0;
		// show the server status info for the selected server
		UI_FeederSelection(FEEDER_FINDPLAYER, uiInfo.currentFoundPlayerServer, NULL);
	}
}

/*
==================
UI_BuildServerStatus
==================
*/
static void UI_BuildServerStatus(qboolean force) {

	if (uiInfo.nextFindPlayerRefresh) {
		return;
	}
	if (!force) {
		if (!uiInfo.nextServerStatusRefresh || uiInfo.nextServerStatusRefresh > uiInfo.uiDC.realTime) {
			return;
		}
	}
	else {
		Menu_SetFeederSelection(NULL, FEEDER_SERVERSTATUS, 0, NULL);
		uiInfo.serverStatusInfo.numLines = 0;
		// reset all server status requests
		trap_LAN_ServerStatus( NULL, NULL, 0);
	}
	if (uiInfo.serverStatus.currentServer < 0 || uiInfo.serverStatus.currentServer > uiInfo.serverStatus.numDisplayServers || uiInfo.serverStatus.numDisplayServers == 0) {
		return;
	}
	if (UI_GetServerStatusInfo( uiInfo.serverStatusAddress, &uiInfo.serverStatusInfo ) ) {
		uiInfo.nextServerStatusRefresh = 0;
		UI_GetServerStatusInfo( uiInfo.serverStatusAddress, NULL );
	}
	else {
		uiInfo.nextServerStatusRefresh = uiInfo.uiDC.realTime + 500;
	}
}

/*
==================
UI_FeederCount
==================
*/
static int UI_FeederCount(float feederID) 
{
	int count=0,i; 
	static char info[MAX_STRING_CHARS];

	switch ( (int)feederID )
	{
	case FEEDER_INVENTORY:
		return JKG_Inventory_FeederCount();
	case FEEDER_SLICEPROGRAMS:
		return JKG_Slice_ProgramCount();
	case FEEDER_PARTYMEMBERSINVITES:
	case FEEDER_PARTYSEEKERS:
		return JKG_PartyMngt_FeederCount((int)feederID);

	case FEEDER_SABER_SINGLE_INFO:

		for (i=0;i<MAX_SABER_HILTS;i++)
		{
			if (saberSingleHiltInfo[i])
			{
				count++;
			}
			else
			{//done
				break;
			}
		}
		return count;

	case FEEDER_SABER_STAFF_INFO:

		for (i=0;i<MAX_SABER_HILTS;i++)
		{
			if (saberStaffHiltInfo[i])
			{
				count++;
			}
			else
			{//done
				break;
			}
		}
		return count;

	case FEEDER_Q3HEADS:
		return UI_HeadCountByColor();

	case FEEDER_CINEMATICS:
		return uiInfo.movieCount;

	case FEEDER_MAPS:
	case FEEDER_ALLMAPS:
		return UI_MapCountByGameType(feederID == FEEDER_MAPS ? qtrue : qfalse);

	case FEEDER_SERVERS:
		return uiInfo.serverStatus.numDisplayServers;

	case FEEDER_SERVERSTATUS:
		return uiInfo.serverStatusInfo.numLines;

	case FEEDER_FINDPLAYER:
		return uiInfo.numFoundPlayerServers;

	case FEEDER_PLAYER_LIST:
		if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) 
		{
			uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
			UI_BuildPlayerList();
		}
		return uiInfo.playerCount;

	case FEEDER_TEAM_LIST:
		if (uiInfo.uiDC.realTime > uiInfo.playerRefresh) 
		{
			uiInfo.playerRefresh = uiInfo.uiDC.realTime + 3000;
			UI_BuildPlayerList();
		}
		return uiInfo.myTeamCount;

	case FEEDER_MODS:
		return uiInfo.modCount;

	case FEEDER_DEMOS:
		return uiInfo.demoCount;

	case FEEDER_MOVES :

		for (i=0;i<MAX_MOVES;i++)
		{
			if (datapadMoveData[uiInfo.movesTitleIndex][i].title)
			{
				count++;
			}
		}

		return count;

	case FEEDER_MOVES_TITLES :
		return (MD_MOVE_TITLE_MAX);

	case FEEDER_PLAYER_SPECIES:
		return uiInfo.playerSpeciesCount;

	case FEEDER_PLAYER_SKIN_HEAD:
		return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadCount;

	case FEEDER_PLAYER_SKIN_TORSO:
		return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoCount;

	case FEEDER_PLAYER_SKIN_LEGS:
		return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegCount;

	case FEEDER_COLORCHOICES:
		return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorCount;
	}

	return 0;
}

static const char *UI_SelectedMap(int index, int *actual) {
	int i, c;
	c = 0;
	*actual = 0;

	for (i = 0; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			if (c == index) {
				*actual = i;
				return uiInfo.mapList[i].mapName;
			} else {
				c++;
			}
		}
	}
	return "";
}

/*
==================
UI_HeadCountByColor
==================
*/
static const char *UI_SelectedTeamHead(int index, int *actual) {
	char *teamname;
	int i,c=0;
	int gwTeam;

	if(ui_gameType.integer >= GT_TEAM)
	{
		gwTeam = (uiSkinColor == TEAM_RED) ? cgImports->GetRedTeam() : cgImports->GetBlueTeam();
		*actual = index;
		return uiInfo.q3HeadNames[index];
	}

	switch(uiSkinColor)
	{
		case TEAM_BLUE:
			teamname = "/blue";
			break;
		case TEAM_RED:
			teamname = "/red";
			break;
		default:
			teamname = "/default";
			break;
	}

	// Count each head with this color

	for (i=0; i<uiInfo.q3HeadCount; i++)
	{
		if (uiInfo.q3HeadNames[i] && strstr(uiInfo.q3HeadNames[i], teamname))
		{
			if (c==index)
			{
				*actual = i;
				return uiInfo.q3HeadNames[i];
			}
			else
			{
				c++;
			}
		}
	}
	return "";
}


static int UI_GetIndexFromSelection(int actual) {
	int i, c;
	c = 0;
	for (i = 0; i < uiInfo.mapCount; i++) {
		if (uiInfo.mapList[i].active) {
			if (i == actual) {
				return c;
			}
			c++;
		}
	}
	return 0;
}

static void UI_UpdatePendingPings() { 
	trap_LAN_ResetPings(UI_SourceForLAN());
	uiInfo.serverStatus.refreshActive = qtrue;
	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;



}

static const char *UI_FeederItemText(float feederID, int index, int column, 
	qhandle_t *handle1, qhandle_t *handle2, qhandle_t *handle3) {
		static char info[MAX_STRING_CHARS]; // don't change this size without changing the sizes inside the SaberProperName calls
	static char hostname[MAX_HOSTNAMELENGTH] = {0};
		static char clientBuff[32];
		static char needPass[32];
		static int lastColumn = -1;
		static int lastTime = 0;
		*handle1 = *handle2 = *handle3 = -1;

		if (feederID == FEEDER_SLICEPROGRAMS) {
			return JKG_Slice_ProgramItemText(index, column, handle1, handle2, handle3);
		} else if (feederID == FEEDER_PARTYMEMBERSINVITES || feederID == FEEDER_PARTYSEEKERS)
		{
			return JKG_PartyMngt_FeederItemText((int)feederID, index, column, handle1, handle2, handle3);
		}
		/*else if ( feederID == FEEDER_INVENTORY )
		{
		return JKG_Inventory_FeederItemText (index, column, handle1, handle2, handle3);
		}*/
		else if (feederID == FEEDER_SABER_SINGLE_INFO)
		{
			//char *saberProperName=0;
			UI_SaberProperNameForSaber( saberSingleHiltInfo[index], info );
			return info;
		}
		else if	(feederID == FEEDER_SABER_STAFF_INFO)
		{
			//char *saberProperName=0;
			UI_SaberProperNameForSaber( saberStaffHiltInfo[index], info );
			return info;
		}
		else if (feederID == FEEDER_Q3HEADS) {
			int actual;
			return UI_SelectedTeamHead(index, &actual);
		} 
		else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) {
			int actual;
			return UI_SelectedMap(index, &actual);
		} else if (feederID == FEEDER_SERVERS) {
			if (index >= 0 && index < uiInfo.serverStatus.numDisplayServers) {
				int ping, game;
				if (lastColumn != column || lastTime > uiInfo.uiDC.realTime + 5000) {
					trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);
					lastColumn = column;
					lastTime = uiInfo.uiDC.realTime;
				}
				ping = atoi(Info_ValueForKey(info, "ping"));
				if (ping == -1) {
					// if we ever see a ping that is out of date, do a server refresh
					// UI_UpdatePendingPings();
				}
				switch (column) {
				case SORT_HOST : 
					if (ping <= 0) {
						return Info_ValueForKey(info, "addr");
					} else {
						//check for password
						//if ( atoi(Info_ValueForKey(info, "needpass")) )
						//{
						//	*handle3 = uiInfo.uiDC.Assets.needPass;
						//}
						// Don't do this, instead we will render a text field at some point that claims that they need a password
						if ( ui_netSource.integer == AS_LOCAL ) {
							Com_sprintf( hostname, sizeof(hostname), "%s [%s]",
								Info_ValueForKey(info, "hostname"),
								netnames[atoi(Info_ValueForKey(info, "nettype"))] );
							return hostname;
						}
						else {
							if (atoi(Info_ValueForKey(info, "sv_allowAnonymous")) != 0) {				// anonymous server
								Com_sprintf( hostname, sizeof(hostname), "(A) %s",
									Info_ValueForKey(info, "hostname"));
							} else {
								Com_sprintf( hostname, sizeof(hostname), "%s",
									Info_ValueForKey(info, "hostname"));
							}
							return hostname;
						}
					}
				case SORT_MAP : 
					return Info_ValueForKey(info, "mapname");
				case SORT_CLIENTS : 
					Com_sprintf( clientBuff, sizeof(clientBuff), "%s (%s)", Info_ValueForKey(info, "clients"), Info_ValueForKey(info, "sv_maxclients"));
					return clientBuff;
				case SORT_GAME : 
					game = atoi(Info_ValueForKey(info, "gametype"));
					if (game >= 0 && game < numTeamArenaGameTypes) {
						strcpy(needPass,teamArenaGameTypes[game]);
					} else {
						if (ping <= 0)
						{
							strcpy(needPass,"Inactive");
						}
						strcpy(needPass,"Unknown");
					}

					return needPass;
				case SORT_PING : 
					if (ping <= 0) {
						return "...";
					} else {
						return Info_ValueForKey(info, "ping");
					}
				}
			}
		} else if (feederID == FEEDER_SERVERSTATUS) {
			if ( index >= 0 && index < uiInfo.serverStatusInfo.numLines ) {
				if ( column >= 0 && column < 4 ) {
					return uiInfo.serverStatusInfo.lines[index][column];
				}
			}
		} else if (feederID == FEEDER_FINDPLAYER) {
			if ( index >= 0 && index < uiInfo.numFoundPlayerServers ) {
				//return uiInfo.foundPlayerServerAddresses[index];
				return uiInfo.foundPlayerServerNames[index];
			}
		} else if (feederID == FEEDER_PLAYER_LIST) {
			if (index >= 0 && index < uiInfo.playerCount) {
				return uiInfo.playerNames[index];
			}
		} else if (feederID == FEEDER_TEAM_LIST) {
			if (index >= 0 && index < uiInfo.myTeamCount) {
				return uiInfo.teamNames[index];
			}
		} else if (feederID == FEEDER_MODS) {
			if (index >= 0 && index < uiInfo.modCount) {
				if (uiInfo.modList[index].modDescr && *uiInfo.modList[index].modDescr) {
					return uiInfo.modList[index].modDescr;
				} else {
					return uiInfo.modList[index].modName;
				}
			}
		} else if (feederID == FEEDER_CINEMATICS) {
			if (index >= 0 && index < uiInfo.movieCount) {
				return uiInfo.movieList[index];
			}
		} else if (feederID == FEEDER_DEMOS) {
			if (index >= 0 && index < uiInfo.demoCount) {
				return uiInfo.demoList[index];
			}
		} 
		else if (feederID == FEEDER_MOVES) 
		{
			return datapadMoveData[uiInfo.movesTitleIndex][index].title;
		}
		else if (feederID == FEEDER_MOVES_TITLES) 
		{
			return datapadMoveTitleData[index];
		}
		else if (feederID == FEEDER_PLAYER_SPECIES) 
		{
			return uiInfo.playerSpecies[index].Name;
		} 
		else if (feederID == FEEDER_LANGUAGES) 
		{
			return 0;
		} 
		else if (feederID == FEEDER_COLORCHOICES)
		{
			if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorCount) 
			{
				*handle1 = trap_R_RegisterShaderNoMip( uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorShader[index]);
				return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorShader[index];
			}
		}
		else if (feederID == FEEDER_PLAYER_SKIN_HEAD)
		{
			if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadCount) 
			{
				*handle1 = trap_R_RegisterShaderNoMip(va("models/players/%s/icon_%s", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadNames[index]));
				return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadNames[index];
			}
		}
		else if (feederID == FEEDER_PLAYER_SKIN_TORSO)
		{
			if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoCount) 
			{
				*handle1 = trap_R_RegisterShaderNoMip(va("models/players/%s/icon_%s", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoNames[index]));
				return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoNames[index];
			}
		}
		else if (feederID == FEEDER_PLAYER_SKIN_LEGS)
		{
			if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegCount) 
			{
				*handle1 = trap_R_RegisterShaderNoMip(va("models/players/%s/icon_%s", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegNames[index]));
				return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegNames[index];
			}
		}
		else if (feederID == FEEDER_SIEGE_BASE_CLASS)
		{
			return ""; 
		}
		else if (feederID == FEEDER_SIEGE_CLASS_WEAPONS)
		{
			return ""; 
		}
		return "";
}


static qhandle_t UI_FeederItemImage(float feederID, int index) {
	static char info[MAX_STRING_CHARS];

	if (feederID == FEEDER_SABER_SINGLE_INFO) 
	{
		return 0;
	}
	else if (feederID == FEEDER_SABER_STAFF_INFO)
	{
		return 0;
	}
	else if (feederID == FEEDER_Q3HEADS) 
	{
		int actual;
		UI_SelectedTeamHead(index, &actual);
		index = actual;

		if (index >= 0 && index < uiInfo.q3HeadCount)
		{ //we want it to load them as it draws them, like the TA feeder
			//return uiInfo.q3HeadIcons[index];
			int selModel = trap_Cvar_VariableValue("ui_selectedModelIndex");

			if (selModel != -1)
			{
				if (uiInfo.q3SelectedHead != selModel)
				{
					uiInfo.q3SelectedHead = selModel;
					//UI_FeederSelection(FEEDER_Q3HEADS, uiInfo.q3SelectedHead);
					Menu_SetFeederSelection(NULL, FEEDER_Q3HEADS, selModel, NULL);
				}
			}

			if (!uiInfo.q3HeadIcons[index])
			{ //this isn't the best way of doing this I guess, but I didn't want a whole seperate string array
				//for storing shader names. I can't just replace q3HeadNames with the shader name, because we
				//print what's in q3HeadNames and the icon name would look funny.
				char iconNameFromSkinName[256];
				int i = 0;
				int skinPlace;

				i = strlen(uiInfo.q3HeadNames[index]);

				while (uiInfo.q3HeadNames[index][i] != '/')
				{
					i--;
				}

				i++;
				skinPlace = i; //remember that this is where the skin name begins

				//now, build a full path out of what's in q3HeadNames, into iconNameFromSkinName
				Com_sprintf(iconNameFromSkinName, sizeof(iconNameFromSkinName), "models/players/%s", uiInfo.q3HeadNames[index]);

				i = strlen(iconNameFromSkinName);

				while (iconNameFromSkinName[i] != '/')
				{
					i--;
				}

				i++;
				iconNameFromSkinName[i] = 0; //terminate, and append..
				Q_strcat(iconNameFromSkinName, 256, "icon_");

				//and now, for the final step, append the skin name from q3HeadNames onto the end of iconNameFromSkinName
				i = strlen(iconNameFromSkinName);

				while (uiInfo.q3HeadNames[index][skinPlace])
				{
					iconNameFromSkinName[i] = uiInfo.q3HeadNames[index][skinPlace];
					i++;
					skinPlace++;
				}
				iconNameFromSkinName[i] = 0;

				//and now we are ready to register (thankfully this will only happen once)
				uiInfo.q3HeadIcons[index] = trap_R_RegisterShaderNoMip(iconNameFromSkinName);
			}
			return uiInfo.q3HeadIcons[index];
		}
	}
	else if (feederID == FEEDER_INVENTORY)
	{
		//This is where we put BlasTech's avatar on everything ~eezstreet
		return JKG_GetInventoryIcon(index);
	}
	else if (feederID == FEEDER_ALLMAPS || feederID == FEEDER_MAPS) 
	{
		int actual;
		UI_SelectedMap(index, &actual);
		index = actual;
		if (index >= 0 && index < uiInfo.mapCount) {
			if (uiInfo.mapList[index].levelShot == -1) {
				uiInfo.mapList[index].levelShot = trap_R_RegisterShaderNoMip(uiInfo.mapList[index].imageName);
			}
			return uiInfo.mapList[index].levelShot;
		}
	}
	else if (feederID == FEEDER_PLAYER_SKIN_HEAD) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadCount) 
		{
			//return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadIcons[index];
			return trap_R_RegisterShaderNoMip(va("models/players/%s/icon_%s", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadNames[index]));
		}
	} 
	else if (feederID == FEEDER_PLAYER_SKIN_TORSO) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoCount) 
		{
			//return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoIcons[index];
			return trap_R_RegisterShaderNoMip(va("models/players/%s/icon_%s", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoNames[index]));
		}
	} 
	else if (feederID == FEEDER_PLAYER_SKIN_LEGS) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegCount) 
		{
			//return uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegIcons[index];
			return trap_R_RegisterShaderNoMip(va("models/players/%s/icon_%s", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].Name, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegNames[index]));
		}
	} 
	else if (feederID == FEEDER_COLORCHOICES)
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorCount) 
		{
			return trap_R_RegisterShaderNoMip( uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorShader[index]);
		}
	}

	return 0;
}


qboolean UI_FeederSelection(float feederFloat, int index, itemDef_t *item) 
{
	static char info[MAX_STRING_CHARS];
	const int feederID = feederFloat;

	if (feederID == FEEDER_SLICEPROGRAMS) {
		return JKG_Slice_ProgramSelection(index);
	}
	else if ( feederID == FEEDER_INVENTORY )
	{
		return JKG_Inventory_FeederSelection (index);
	}
	else if (feederID == FEEDER_PARTYMEMBERSINVITES || feederID == FEEDER_PARTYSEEKERS)
	{
		return JKG_PartyMngt_FeederSelection(feederID, index, item);
	}
	else if (feederID == FEEDER_Q3HEADS) 
	{
		int actual;
		UI_SelectedTeamHead(index, &actual);
		uiInfo.q3SelectedHead = index;
		trap_Cvar_Set("ui_selectedModelIndex", va("%i", index));
		index = actual;
		if (index >= 0 && index < uiInfo.q3HeadCount) 
		{
			trap_Cvar_Set( "model", uiInfo.q3HeadNames[index]);	//standard model
			trap_Cvar_Set ( "char_color_red", "255" );			//standard colors
			trap_Cvar_Set ( "char_color_green", "255" );
			trap_Cvar_Set ( "char_color_blue", "255" );
		}
	} 
	else if (feederID == FEEDER_MOVES) 
	{
		itemDef_t *item;
		menuDef_t *menu;
		modelDef_t *modelPtr;

		menu = Menus_FindByName("rulesMenu_moves");

		if (menu)
		{
			item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "character");
			if (item)
			{
				modelPtr = (modelDef_t*)item->typeData;
				if (modelPtr)
				{
					char modelPath[MAX_QPATH];
					int animRunLength;

					ItemParse_model_g2anim_go( item,  datapadMoveData[uiInfo.movesTitleIndex][index].anim );

					Com_sprintf( modelPath, sizeof( modelPath ), "models/players/%s/model.glm", UI_Cvar_VariableString ( "ui_char_model" ) );
					ItemParse_asset_model_go( item, modelPath, &animRunLength );
					UI_UpdateCharacterSkin();

					uiInfo.moveAnimTime = uiInfo.uiDC.realTime + animRunLength;

					if (datapadMoveData[uiInfo.movesTitleIndex][index].anim)
					{

						// Play sound for anim
						if (datapadMoveData[uiInfo.movesTitleIndex][index].sound == MDS_FORCE_JUMP)
						{
							trap_S_StartLocalSound( uiInfo.uiDC.Assets.moveJumpSound, CHAN_LOCAL );
						}
						else if (datapadMoveData[uiInfo.movesTitleIndex][index].sound == MDS_ROLL)
						{
							trap_S_StartLocalSound( uiInfo.uiDC.Assets.moveRollSound, CHAN_LOCAL );
						}
						else if (datapadMoveData[uiInfo.movesTitleIndex][index].sound == MDS_SABER)
						{
							// Randomly choose one sound
							int soundI = Q_irand( 1, 6 );
							sfxHandle_t *soundPtr;
							soundPtr = &uiInfo.uiDC.Assets.datapadmoveSaberSound1;
							if (soundI == 2)
							{
								soundPtr = &uiInfo.uiDC.Assets.datapadmoveSaberSound2;
							}
							else if (soundI == 3)
							{
								soundPtr = &uiInfo.uiDC.Assets.datapadmoveSaberSound3;
							}
							else if (soundI == 4)
							{
								soundPtr = &uiInfo.uiDC.Assets.datapadmoveSaberSound4;
							}
							else if (soundI == 5)
							{
								soundPtr = &uiInfo.uiDC.Assets.datapadmoveSaberSound5;
							}
							else if (soundI == 6)
							{
								soundPtr = &uiInfo.uiDC.Assets.datapadmoveSaberSound6;
							}

							trap_S_StartLocalSound( *soundPtr, CHAN_LOCAL );
						}

						if (datapadMoveData[uiInfo.movesTitleIndex][index].desc)
						{
							trap_Cvar_Set( "ui_move_desc", datapadMoveData[uiInfo.movesTitleIndex][index].desc);
						}
					}
					UI_SaberAttachToChar( item );
				}
			}
		}
	} 
	else if (feederID == FEEDER_MOVES_TITLES) 
	{
		itemDef_t *item;
		menuDef_t *menu;
		modelDef_t *modelPtr;

		uiInfo.movesTitleIndex = index;
		uiInfo.movesBaseAnim = datapadMoveTitleBaseAnims[uiInfo.movesTitleIndex];
		menu = Menus_FindByName("rulesMenu_moves");

		if (menu)
		{
			item = (itemDef_t *) Menu_FindItemByName((menuDef_t *) menu, "character");
			if (item)
			{
				modelPtr = (modelDef_t*)item->typeData;
				if (modelPtr)
				{
					char modelPath[MAX_QPATH];
					int	animRunLength;

					uiInfo.movesBaseAnim = datapadMoveTitleBaseAnims[uiInfo.movesTitleIndex];
					ItemParse_model_g2anim_go( item,  uiInfo.movesBaseAnim );

					Com_sprintf( modelPath, sizeof( modelPath ), "models/players/%s/model.glm", UI_Cvar_VariableString ( "ui_char_model" ) );
					ItemParse_asset_model_go( item, modelPath, &animRunLength );

					UI_UpdateCharacterSkin();

				}
			}
		}
	}
	else if (feederID == FEEDER_MAPS || feederID == FEEDER_ALLMAPS) 
	{
		int actual, map;
		const char *checkValid = NULL;

		map = (feederID == FEEDER_ALLMAPS) ? ui_currentNetMap.integer : ui_currentMap.integer;
		if (uiInfo.mapList[map].cinematic >= 0) {
			trap_CIN_StopCinematic(uiInfo.mapList[map].cinematic);
			uiInfo.mapList[map].cinematic = -1;
		}
		checkValid = UI_SelectedMap(index, &actual);

		if (!checkValid || !checkValid[0])
		{ //this isn't a valid map to select, so reselect the current
			index = ui_mapIndex.integer;
			UI_SelectedMap(index, &actual);
		}

		trap_Cvar_Set("ui_mapIndex", va("%d", index));
		gUISelectedMap = index;
		ui_mapIndex.integer = index;

		if (feederID == FEEDER_MAPS) {
			ui_currentMap.integer = actual;
			trap_Cvar_Set("ui_currentMap", va("%d", actual));
			uiInfo.mapList[ui_currentMap.integer].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[ui_currentMap.integer].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
			//trap_Cvar_Set("ui_opponentModel", uiInfo.mapList[ui_currentMap.integer].opponentName);
			//updateOpponentModel = qtrue;
		} else {
			ui_currentNetMap.integer = actual;
			trap_Cvar_Set("ui_currentNetMap", va("%d", actual));
			uiInfo.mapList[ui_currentNetMap.integer].cinematic = trap_CIN_PlayCinematic(va("%s.roq", uiInfo.mapList[ui_currentNetMap.integer].mapLoadName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}

	} else if (feederID == FEEDER_SERVERS) {
		const char *mapName = NULL;
		uiInfo.serverStatus.currentServer = index;
		trap_LAN_GetServerInfo(UI_SourceForLAN(), uiInfo.serverStatus.displayServers[index], info, MAX_STRING_CHARS);
		uiInfo.serverStatus.currentServerPreview = trap_R_RegisterShaderNoMip(va("levelshots/%s", Info_ValueForKey(info, "mapname")));
		if (uiInfo.serverStatus.currentServerCinematic >= 0) {
			trap_CIN_StopCinematic(uiInfo.serverStatus.currentServerCinematic);
			uiInfo.serverStatus.currentServerCinematic = -1;
		}
		mapName = Info_ValueForKey(info, "mapname");
		if (mapName && *mapName) {
			uiInfo.serverStatus.currentServerCinematic = trap_CIN_PlayCinematic(va("%s.roq", mapName), 0, 0, 0, 0, (CIN_loop | CIN_silent) );
		}
	} else if (feederID == FEEDER_SERVERSTATUS) {
		//
	} else if (feederID == FEEDER_FINDPLAYER) {
		uiInfo.currentFoundPlayerServer = index;
		//
		if ( index < uiInfo.numFoundPlayerServers-1) {
			// build a new server status for this server
			Q_strncpyz(uiInfo.serverStatusAddress, uiInfo.foundPlayerServerAddresses[uiInfo.currentFoundPlayerServer], sizeof(uiInfo.serverStatusAddress));
			Menu_SetFeederSelection(NULL, FEEDER_SERVERSTATUS, 0, NULL);
			UI_BuildServerStatus(qtrue);
		}
	} else if (feederID == FEEDER_PLAYER_LIST) {
		uiInfo.playerIndex = index;
	} else if (feederID == FEEDER_TEAM_LIST) {
		uiInfo.teamIndex = index;
	} else if (feederID == FEEDER_MODS) {
		uiInfo.modIndex = index;
	} else if (feederID == FEEDER_CINEMATICS) {
		uiInfo.movieIndex = index;
		if (uiInfo.previewMovie >= 0) {
			trap_CIN_StopCinematic(uiInfo.previewMovie);
		}
		uiInfo.previewMovie = -1;
	} else if (feederID == FEEDER_DEMOS) {
		uiInfo.demoIndex = index;
	}
	else if (feederID == FEEDER_COLORCHOICES) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorCount)
		{
			Item_RunScript(item, uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].ColorActionText[index]);
		}
	}
	else if (feederID == FEEDER_PLAYER_SKIN_HEAD) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadCount)
		{
			trap_Cvar_Set("ui_char_skin_head", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinHeadNames[index]);
		}
	} 
	else if (feederID == FEEDER_PLAYER_SKIN_TORSO) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoCount)
		{
			trap_Cvar_Set("ui_char_skin_torso", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinTorsoNames[index]);
		}
	} 
	else if (feederID == FEEDER_PLAYER_SKIN_LEGS) 
	{
		if (index >= 0 && index < uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegCount)
		{
			trap_Cvar_Set("ui_char_skin_legs", uiInfo.playerSpecies[uiInfo.playerSpeciesIndex].SkinLegNames[index]);
		}
	} 
	else if (feederID == FEEDER_PLAYER_SPECIES) 
	{
		uiInfo.playerSpeciesIndex = index;
	} 
	else if (feederID == FEEDER_LANGUAGES) 
	{
		uiInfo.languageCountIndex = index;
	} 

	return qtrue;
}


static qboolean GameType_Parse(char **p, qboolean join) {
	char *token;

	token = COM_ParseExt((const char **)p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	if (join) {
		uiInfo.numJoinGameTypes = 0;
	} else {
		uiInfo.numGameTypes = 0;
	}

	while ( 1 ) {
		token = COM_ParseExt((const char **)p, qtrue);

		if (Q_stricmp(token, "}") == 0) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			// two tokens per line, character name and sex
			if (join) {
				if (!String_Parse(p, &uiInfo.joinGameTypes[uiInfo.numJoinGameTypes].gameType) || !Int_Parse(p, &uiInfo.joinGameTypes[uiInfo.numJoinGameTypes].gtEnum)) {
					return qfalse;
				}
			} else {
				if (!String_Parse(p, &uiInfo.gameTypes[uiInfo.numGameTypes].gameType) || !Int_Parse(p, &uiInfo.gameTypes[uiInfo.numGameTypes].gtEnum)) {
					return qfalse;
				}
			}

			if (join) {
				if (uiInfo.numJoinGameTypes < MAX_GAMETYPES) {
					uiInfo.numJoinGameTypes++;
				} else {
					Com_Printf("Too many net game types, last one replace!\n");
				}		
			} else {
				if (uiInfo.numGameTypes < MAX_GAMETYPES) {
					uiInfo.numGameTypes++;
				} else {
					Com_Printf("Too many game types, last one replace!\n");
				}		
			}

			token = COM_ParseExt((const char **)p, qtrue);
			if (token[0] != '}') {
				return qfalse;
			}
		}
	}
	return qfalse;
}

static qboolean MapList_Parse(char **p) {
	char *token;

	token = COM_ParseExt((const char **)p, qtrue);

	if (token[0] != '{') {
		return qfalse;
	}

	uiInfo.mapCount = 0;

	while ( 1 ) {
		token = COM_ParseExt((const char **)p, qtrue);

		if (Q_stricmp(token, "}") == 0) {
			return qtrue;
		}

		if ( !token || token[0] == 0 ) {
			return qfalse;
		}

		if (token[0] == '{') {
			if (!String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].mapName) || !String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].mapLoadName) 
				||!Int_Parse(p, &uiInfo.mapList[uiInfo.mapCount].teamMembers) ) {
				return qfalse;
			}

			if (!String_Parse(p, &uiInfo.mapList[uiInfo.mapCount].opponentName)) {
				return qfalse;
			}

			uiInfo.mapList[uiInfo.mapCount].typeBits = 0;

			while (1) {
				token = COM_ParseExt((const char **)p, qtrue);
				if (token[0] >= '0' && token[0] <= '9') {
					uiInfo.mapList[uiInfo.mapCount].typeBits |= (1 << (token[0] - 0x030));
					if (!Int_Parse(p, &uiInfo.mapList[uiInfo.mapCount].timeToBeat[token[0] - 0x30])) {
						return qfalse;
					}
				} else {
					break;
				} 
			}

			//mapList[mapCount].imageName = String_Alloc(va("levelshots/%s", mapList[mapCount].mapLoadName));
			//if (uiInfo.mapCount == 0) {
			// only load the first cinematic, selection loads the others
			//  uiInfo.mapList[uiInfo.mapCount].cinematic = trap_CIN_PlayCinematic(va("%s.roq",uiInfo.mapList[uiInfo.mapCount].mapLoadName), qfalse, qfalse, qtrue, 0, 0, 0, 0);
			//}
			uiInfo.mapList[uiInfo.mapCount].cinematic = -1;
			uiInfo.mapList[uiInfo.mapCount].levelShot = trap_R_RegisterShaderNoMip(va("levelshots/%s_small", uiInfo.mapList[uiInfo.mapCount].mapLoadName));

			if (uiInfo.mapCount < MAX_MAPS) {
				uiInfo.mapCount++;
			} else {
				Com_Printf("Too many maps, last one replaced!\n");
			}
		}
	}
	return qfalse;
}

static void UI_ParseGameInfo(const char *teamFile) {
	char	*token;
	char *p;
	char *buff = NULL;
	//int mode = 0; TTimo: unused

	buff = GetMenuBuffer(teamFile);
	if (!buff) {
		return;
	}

	p = buff;

	while ( 1 ) {
		token = COM_ParseExt( (const char **)(&p), qtrue );
		if( !token || token[0] == 0 || token[0] == '}') {
			break;
		}

		if ( Q_stricmp( token, "}" ) == 0 ) {
			break;
		}

		if (Q_stricmp(token, "gametypes") == 0) {

			if (GameType_Parse(&p, qfalse)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token, "joingametypes") == 0) {

			if (GameType_Parse(&p, qtrue)) {
				continue;
			} else {
				break;
			}
		}

		if (Q_stricmp(token, "maps") == 0) {
			// start a new menu
			MapList_Parse(&p);
		}

	}
}

static void UI_Pause(qboolean b) {
	if (b) {
		// pause the game and set the ui keycatcher
		trap_Cvar_Set( "cl_paused", "1" );
		trap_Key_SetCatcher( KEYCATCH_UI );
	} else {
		// unpause the game and clear the ui keycatcher
		trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
		trap_Key_ClearStates();
		trap_Cvar_Set( "cl_paused", "0" );
	}
}

static int UI_PlayCinematic(const char *name, float x, float y, float w, float h) {
	return trap_CIN_PlayCinematic(name, x, y, w, h, (CIN_loop | CIN_silent));
}

static void UI_StopCinematic(int handle) {
	if (handle >= 0) {
		trap_CIN_StopCinematic(handle);
	}
}

static void UI_DrawCinematic(int handle, float x, float y, float w, float h) {
	trap_CIN_SetExtents(handle, x, y, w, h);
	trap_CIN_DrawCinematic(handle);
}

static void UI_RunCinematicFrame(int handle) {
	trap_CIN_RunCinematic(handle);
}

/*
=================
bIsImageFile
builds path and scans for valid image extentions
=================
*/
static qboolean bIsImageFile(const char* dirptr, const char* skinname)
{
	char fpath[MAX_QPATH];
	int f;

	Com_sprintf(fpath, MAX_QPATH, "models/players/%s/icon_%s.jpg", dirptr, skinname);
	trap_FS_FOpenFile(fpath, &f, FS_READ);
	if (!f)
	{ //not there, try png
		Com_sprintf(fpath, MAX_QPATH, "models/players/%s/icon_%s.png", dirptr, skinname);
		trap_FS_FOpenFile(fpath, &f, FS_READ);
	}
	if (!f)
	{ //not there, try tga
		Com_sprintf(fpath, MAX_QPATH, "models/players/%s/icon_%s.tga", dirptr, skinname);
		trap_FS_FOpenFile(fpath, &f, FS_READ);
	}
	if (f) 
	{
		trap_FS_FCloseFile(f);
		return qtrue;
	}

	return qfalse;
}


/*
=================
PlayerModel_BuildList
=================
*/
void UI_BuildQ3Model_List( void )
{
	int		numdirs;
	int		numfiles;
	char	dirlist[2048];
	char	filelist[2048];
	char	skinname[64];
	char*	dirptr;
	char*	fileptr;
	char*	check;
	int		i;
	int		j, k, p, s;
	int		dirlen;
	int		filelen;

	uiInfo.q3HeadCount = 0;

	if(ui_gameType.integer >= GT_TEAM)
	{
		// Gang Wars - UI stuff for model store
		int gwTeam;
		int myTeam = (int)(trap_Cvar_VariableValue("ui_myteam"));

		if(myTeam == TEAM_BLUE || myTeam == TEAM_RED)
		{
			gwTeam = (myTeam == TEAM_RED) ? cgImports->GetRedTeam() : cgImports->GetBlueTeam();
			if(Q_stricmp(bgGangWarsTeams[gwTeam].modelStore[0], "NULL"))
			{
				// Okay. Loop through all the model store entries..
				for(i = 0; i < bgGangWarsTeams[gwTeam].numModelsInStore; i++)
				{				
					Com_sprintf( uiInfo.q3HeadNames[uiInfo.q3HeadCount], sizeof(uiInfo.q3HeadNames[uiInfo.q3HeadCount]), va("%s", bgGangWarsTeams[gwTeam].modelStore[i]));
					uiInfo.q3HeadIcons[uiInfo.q3HeadCount++] = 0;//trap_R_RegisterShaderNoMip(fpath);
					//rww - we are now registering them as they are drawn like the TA feeder, so as to decrease UI load time.
				}
				return;
			}
		}
	}

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players", "/", dirlist, 2048 );
	dirptr  = dirlist;
	for (i=0; i<numdirs && uiInfo.q3HeadCount < MAX_Q3PLAYERMODELS; i++,dirptr+=dirlen+1)
	{
		dirlen = strlen(dirptr);

		if (dirlen && dirptr[dirlen-1]=='/') dirptr[dirlen-1]='\0';

		if (!strcmp(dirptr,".") || !strcmp(dirptr,".."))
			continue;


		numfiles = trap_FS_GetFileList( va("models/players/%s",dirptr), "skin", filelist, 2048 );
		fileptr  = filelist;
		for (j=0; j<numfiles && uiInfo.q3HeadCount < MAX_Q3PLAYERMODELS;j++,fileptr+=filelen+1)
		{
			int skinLen = 0;

			filelen = strlen(fileptr);

			COM_StripExtension(fileptr,skinname, sizeof( skinname ) );

			skinLen = strlen(skinname);
			k = 0;
			while (k < skinLen && skinname[k] && skinname[k] != '_')
			{
				k++;
			}
			if (skinname[k] == '_')
			{
				p = 0;

				while (skinname[k])
				{
					skinname[p] = skinname[k];
					k++;
					p++;
				}
				skinname[p] = '\0';
			}

			/*
			Com_sprintf(fpath, 2048, "models/players/%s/icon%s.jpg", dirptr, skinname);

			trap_FS_FOpenFile(fpath, &f, FS_READ);

			if (f)
			*/
			check = &skinname[1];
			if (bIsImageFile(dirptr, check))
			{ //if it exists
				qboolean iconExists = qfalse;

				//trap_FS_FCloseFile(f);

				if (skinname[0] == '_')
				{ //change character to append properly
					skinname[0] = '/';
				}

				s = 0;

				while (s < uiInfo.q3HeadCount)
				{ //check for dupes
					if (!Q_stricmp(va("%s%s", dirptr, skinname), uiInfo.q3HeadNames[s]))
					{
						iconExists = qtrue;
						break;
					}
					s++;
				}

				if (iconExists)
				{
					continue;
				}

				Com_sprintf( uiInfo.q3HeadNames[uiInfo.q3HeadCount], sizeof(uiInfo.q3HeadNames[uiInfo.q3HeadCount]), va("%s%s", dirptr, skinname));
				uiInfo.q3HeadIcons[uiInfo.q3HeadCount++] = 0;//trap_R_RegisterShaderNoMip(fpath);
				//rww - we are now registering them as they are drawn like the TA feeder, so as to decrease UI load time.
			}

			if (uiInfo.q3HeadCount >= MAX_Q3PLAYERMODELS)
			{
				return;
			}
		}
	}	

}

// Gang Wars stuff
void UI_InitGangWars(void)
{
	JKG_BG_GangWarsInit();
}

/*
=================
UI_ParseColorData
=================
*/
//static qboolean UI_ParseColorData(char* buf, playerSpeciesInfo_t &species)
static qboolean UI_ParseColorData(char* buf, playerSpeciesInfo_t *species,char*	file)
{
	const char	*token;
	const char	*p;

	p = buf;
	COM_BeginParseSession(file);
	species->ColorCount = 0;

	while ( p )
	{
		token = COM_ParseExt( &p, qtrue );	//looking for the shader
		if ( token[0] == 0 )
		{
			return species->ColorCount;
		}
		Q_strncpyz( species->ColorShader[species->ColorCount], token, sizeof(species->ColorShader[0]) );

		token = COM_ParseExt( &p, qtrue );	//looking for action block {
		if ( token[0] != '{' )
		{
			return qfalse;
		}

		assert(!species->ColorActionText[species->ColorCount][0]);
		token = COM_ParseExt( &p, qtrue );	//looking for action commands
		while (token[0] != '}')
		{
			if ( token[0] == 0)
			{	//EOF
				return qfalse;
			}
			assert(species->ColorCount < sizeof(species->ColorActionText)/sizeof(species->ColorActionText[0]) );
			Q_strcat(species->ColorActionText[species->ColorCount], sizeof(species->ColorActionText[0]), token);
			Q_strcat(species->ColorActionText[species->ColorCount], sizeof(species->ColorActionText[0]), " ");
			token = COM_ParseExt( &p, qtrue );	//looking for action commands or final }
		}
		species->ColorCount++;	//next color please
	}
	return qtrue;//never get here
}

/*
=================
UI_BuildPlayerModel_List
=================
*/
static void UI_BuildPlayerModel_List( qboolean inGameLoad )
{
	int		numdirs;
	char	dirlist[2048];
	char*	dirptr;
	int		dirlen;
	int		i;
	int		j;


	uiInfo.playerSpeciesCount = 0;
	uiInfo.playerSpeciesIndex = 0;
	memset(uiInfo.playerSpecies, 0, sizeof (uiInfo.playerSpecies) );

	// iterate directory of all player models
	numdirs = trap_FS_GetFileList("models/players", "/", dirlist, 2048 );
	dirptr  = dirlist;
	for (i=0; i<numdirs; i++,dirptr+=dirlen+1)
	{
		char	filelist[2048];
		char*	fileptr;
		int		filelen;
		int f = 0;
		char fpath[2048];

		dirlen = strlen(dirptr);

		if (dirlen)
		{
			if (dirptr[dirlen-1]=='/')
				dirptr[dirlen-1]='\0';
		}
		else
		{
			continue;
		}

		if (!strcmp(dirptr,".") || !strcmp(dirptr,".."))
			continue;

		Com_sprintf(fpath, 2048, "models/players/%s/PlayerChoice.txt", dirptr);
		filelen = trap_FS_FOpenFile(fpath, &f, FS_READ);

		if (f)
		{ 
			char buffer[2048];
			char	skinname[64];
			int		numfiles;
			int		iSkinParts=0;

			trap_FS_Read(&buffer, filelen, f);
			trap_FS_FCloseFile(f);
			buffer[filelen] = 0;	//ensure trailing NULL

			//record this species
			Q_strncpyz( uiInfo.playerSpecies[uiInfo.playerSpeciesCount].Name, dirptr, sizeof(uiInfo.playerSpecies[0].Name) );

			if (!UI_ParseColorData(buffer,&uiInfo.playerSpecies[uiInfo.playerSpeciesCount],fpath))
			{
				Com_Printf(S_COLOR_RED"UI_BuildPlayerModel_List: Errors parsing '%s'\n", fpath);
			}

			numfiles = trap_FS_GetFileList( va("models/players/%s",dirptr), ".skin", filelist, 2048 );
			fileptr  = filelist;
			for (j=0; j<numfiles; j++,fileptr+=filelen+1)
			{
				if (trap_Cvar_VariableValue("fs_copyfiles") > 0 )
				{
					trap_FS_FOpenFile(va("models/players/%s/%s",dirptr,fileptr), &f, FS_READ);
					if (f) trap_FS_FCloseFile(f);
				}

				filelen = strlen(fileptr);
				COM_StripExtension(fileptr,skinname, sizeof(skinname));

				if (bIsImageFile(dirptr, skinname))
				{ //if it exists
					if (strnicmp(skinname,"head_",5) == 0)
					{
						if (uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinHeadCount < MAX_PLAYERMODELS) 
						{
							Q_strncpyz(
								uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinHeadNames[uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinHeadCount++], 
								skinname, 
								sizeof(uiInfo.playerSpecies[0].SkinHeadNames[0])
								);
							iSkinParts |= 1<<0;
						}
					} else
						if (strnicmp(skinname,"torso_",6) == 0)
						{
							if (uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinTorsoCount < MAX_PLAYERMODELS) 
							{
								Q_strncpyz(uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinTorsoNames[uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinTorsoCount++], 
									skinname, 
									sizeof(uiInfo.playerSpecies[0].SkinTorsoNames[0])
									);
								iSkinParts |= 1<<1;
							}
						} else
							if (strnicmp(skinname,"lower_",6) == 0)
							{
								if (uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinLegCount < MAX_PLAYERMODELS) 
								{
									Q_strncpyz(uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinLegNames[uiInfo.playerSpecies[uiInfo.playerSpeciesCount].SkinLegCount++], 
										skinname, 
										sizeof(uiInfo.playerSpecies[0].SkinLegNames[0]) 
										);
									iSkinParts |= 1<<2;
								}
							}

				}
			}
			if (iSkinParts != 7)
			{	//didn't get a skin for each, then skip this model.
				memset(&uiInfo.playerSpecies[uiInfo.playerSpeciesCount], 0, sizeof(uiInfo.playerSpecies[uiInfo.playerSpeciesCount]));//undo the colors
				continue;
			}
			uiInfo.playerSpeciesCount++;
			if (!inGameLoad && ui_PrecacheModels.integer)
			{
				int g2Model;
				void *ghoul2 = 0;
				Com_sprintf( fpath, sizeof( fpath ), "models/players/%s/model.glm", dirptr );
				g2Model = trap_G2API_InitGhoul2Model(&ghoul2, fpath, 0, 0, 0, 0, 0);
				if (g2Model >= 0)
				{
					//					trap_G2API_RemoveGhoul2Model( &ghoul2, 0 );
					trap_G2API_CleanGhoul2Models (&ghoul2);
				}
			}
			if (uiInfo.playerSpeciesCount >= MAX_PLAYERMODELS)
			{
				return;
			}
		}
	}	

}

/*
=================
UI_Init
=================
*/
//#define JKG_CLIENTSIDE_VERSION "0.3.25a"



void _UI_Init( qboolean inGameLoad ) {
	const char *menuSet;
	int start;

	// Some knucklehead decided to update client version once we got to the client aux lib,
	// which is kinda stupid since we can't get there unless we actually connect to a server.
	// DERP. --eez
	//trap_Cvar_Set("clver", JKG_VERSION); // this is handled in engine now --eez

	// Get the list of possible languages
	uiInfo.languageCount = trap_SP_GetNumLanguages();	// this does a dir scan, so use carefully

	uiInfo.inGameLoad = inGameLoad;

	// set some cvar crap in the engine
	trap_Cvar_Set("sv_master1", "master.jkhub.org");
	trap_Cvar_Set("sv_master2", "masterjk3.ravensoft.com");
	trap_Cvar_Set("sv_master3", "master.qtracker.com");

	UI_InitGangWars();

	UI_RegisterCvars();
	UI_InitMemory();

	// cache redundant calulations
	trap_GetGlconfig( &uiInfo.uiDC.glconfig );

	// for 640x480 virtualized screen
	uiInfo.uiDC.yscale = uiInfo.uiDC.glconfig.vidHeight * (1.0/480.0);
	uiInfo.uiDC.xscale = uiInfo.uiDC.glconfig.vidWidth * (1.0/640.0);
	if ( uiInfo.uiDC.glconfig.vidWidth * 480 > uiInfo.uiDC.glconfig.vidHeight * 640 ) {
		// wide screen
		uiInfo.uiDC.bias = 0.5 * ( uiInfo.uiDC.glconfig.vidWidth - ( uiInfo.uiDC.glconfig.vidHeight * (640.0/480.0) ) );
	}
	else {
		// no wide screen
		uiInfo.uiDC.bias = 0;
	}


	//UI_Load();
	uiInfo.uiDC.registerShaderNoMip = &trap_R_RegisterShaderNoMip;
	uiInfo.uiDC.setColor = &UI_SetColor;
	uiInfo.uiDC.drawHandlePic = &UI_DrawHandlePic;
	uiInfo.uiDC.drawStretchPic = &trap_R_DrawStretchPic;
	uiInfo.uiDC.drawText = &Text_Paint;
	uiInfo.uiDC.textWidth = &Text_Width;
	uiInfo.uiDC.textHeight = &Text_Height;
	uiInfo.uiDC.registerModel = &trap_R_RegisterModel;
	uiInfo.uiDC.modelBounds = &trap_R_ModelBounds;
	uiInfo.uiDC.fillRect = &UI_FillRect;
	uiInfo.uiDC.drawRect = &_UI_DrawRect;
	uiInfo.uiDC.drawSides = &_UI_DrawSides;
	uiInfo.uiDC.drawTopBottom = &_UI_DrawTopBottom;
	uiInfo.uiDC.clearScene = &trap_R_ClearScene;
	uiInfo.uiDC.drawSides = &_UI_DrawSides;
	uiInfo.uiDC.addRefEntityToScene = &trap_R_AddRefEntityToScene;
	uiInfo.uiDC.renderScene = &trap_R_RenderScene;
	uiInfo.uiDC.RegisterFont = &trap_R_RegisterFont;
	uiInfo.uiDC.Font_StrLenPixels = trap_R_Font_StrLenPixels;
	uiInfo.uiDC.Font_StrLenChars = trap_R_Font_StrLenChars;
	uiInfo.uiDC.Font_HeightPixels = trap_R_Font_HeightPixels;
	uiInfo.uiDC.Font_DrawString = trap_R_Font_DrawString;
	uiInfo.uiDC.Language_IsAsian = trap_Language_IsAsian;
	uiInfo.uiDC.Language_UsesSpaces = trap_Language_UsesSpaces;
	uiInfo.uiDC.AnyLanguage_ReadCharFromString = trap_AnyLanguage_ReadCharFromString;
	uiInfo.uiDC.ownerDrawItem = &UI_OwnerDraw;
	uiInfo.uiDC.getValue = &UI_GetValue;
	uiInfo.uiDC.ownerDrawVisible = &UI_OwnerDrawVisible;
	uiInfo.uiDC.runScript = &UI_RunMenuScript;
	uiInfo.uiDC.deferScript = &UI_DeferMenuScript;
	uiInfo.uiDC.getTeamColor = &UI_GetTeamColor;
	uiInfo.uiDC.setCVar = trap_Cvar_Set;
	uiInfo.uiDC.getCVarString = trap_Cvar_VariableStringBuffer;
	uiInfo.uiDC.getCVarValue = trap_Cvar_VariableValue;
	uiInfo.uiDC.drawTextWithCursor = &Text_PaintWithCursor;
	uiInfo.uiDC.setOverstrikeMode = &trap_Key_SetOverstrikeMode;
	uiInfo.uiDC.getOverstrikeMode = &trap_Key_GetOverstrikeMode;
	uiInfo.uiDC.startLocalSound = &trap_S_StartLocalSound;
	uiInfo.uiDC.ownerDrawHandleKey = &UI_OwnerDrawHandleKey;
	uiInfo.uiDC.feederCount = &UI_FeederCount;
	uiInfo.uiDC.feederItemImage = &UI_FeederItemImage;
	uiInfo.uiDC.feederItemText = &UI_FeederItemText;
	uiInfo.uiDC.feederSelection = &UI_FeederSelection;
	uiInfo.uiDC.setBinding = &trap_Key_SetBinding;
	uiInfo.uiDC.getBindingBuf = &trap_Key_GetBindingBuf;
	uiInfo.uiDC.keynumToStringBuf = &trap_Key_KeynumToStringBuf;
	uiInfo.uiDC.executeText = &trap_Cmd_ExecuteText;
	uiInfo.uiDC.Error = &Com_Error; 
	uiInfo.uiDC.Print = &Com_Printf; 
	uiInfo.uiDC.Pause = &UI_Pause;
	uiInfo.uiDC.ownerDrawWidth = &UI_OwnerDrawWidth;
	uiInfo.uiDC.registerSound = &trap_S_RegisterSound;
	uiInfo.uiDC.startBackgroundTrack = &trap_S_StartBackgroundTrack;
	uiInfo.uiDC.stopBackgroundTrack = &trap_S_StopBackgroundTrack;
	uiInfo.uiDC.playCinematic = &UI_PlayCinematic;
	uiInfo.uiDC.stopCinematic = &UI_StopCinematic;
	uiInfo.uiDC.drawCinematic = &UI_DrawCinematic;
	uiInfo.uiDC.runCinematicFrame = &UI_RunCinematicFrame;

	Init_Display(&uiInfo.uiDC);

	UI_BuildPlayerModel_List(inGameLoad);

	String_Init();

	uiInfo.uiDC.cursor	= trap_R_RegisterShaderNoMip ( "menu/art/3_cursor2" );
	uiInfo.uiDC.whiteShader = trap_R_RegisterShaderNoMip( "white" );

	AssetCache();

	start = trap_Milliseconds();

	uiInfo.teamCount = 0;
	uiInfo.characterCount = 0;
	uiInfo.aliasCount = 0;

	UI_ParseGameInfo("ui/jamp/gameinfo.txt");

	menuSet = UI_Cvar_VariableString("ui_menuFilesMP");
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/jampmenus.txt";
	}

#if 1
	if (inGameLoad)
	{
		UI_LoadMenus("ui/jampingame.txt", qtrue);
		// Jedi Knight Galaxies
		UI_LoadMenus("ui/jkgmenus.txt", qfalse);
	}
	else if (!ui_bypassMainMenuLoad.integer)
	{
		UI_LoadMenus(menuSet, qtrue);
		UI_LoadMenus("ui/jkgmenus.txt", qfalse);
	}
#else //this was adding quite a giant amount of time to the load time
	UI_LoadMenus(menuSet, qtrue);
	UI_LoadMenus("ui/jampingame.txt", qtrue);
#endif
	
	/* Raz: Truncate the name, try and avoid overflow glitches for long names and menu items
	trap_Cvar_Register(NULL, "ui_name", UI_Cvar_VariableString("name"), CVAR_INTERNAL );	//get this now, jic the menus change again trying to setName before getName
	*/
	{
		char buf[MAX_NETNAME] = {0};
		Q_strncpyz( buf, UI_Cvar_VariableString( "name" ), sizeof( buf ) );
		trap_Cvar_Register( NULL, "ui_Name", buf, CVAR_INTERNAL );
	}


	Menus_CloseAll();

	trap_LAN_LoadCachedServers();

	UI_BuildQ3Model_List();
	UI_LoadBots();

	// sets defaults for ui temp cvars
	uiInfo.effectsColor = /*gamecodetoui[*/(int)trap_Cvar_VariableValue("color1");//-1];
	uiInfo.currentCrosshair = (int)trap_Cvar_VariableValue("cg_drawCrosshair");
	trap_Cvar_Set("ui_mousePitch", (trap_Cvar_VariableValue("m_pitch") >= 0) ? "0" : "1");
	trap_Cvar_Set("ui_mousePitchVeh", (trap_Cvar_VariableValue("m_pitchVeh") >= 0) ? "0" : "1");

	uiInfo.serverStatus.currentServerCinematic = -1;
	uiInfo.previewMovie = -1;

	trap_Cvar_Register(NULL, "debug_protocol", "", 0 );

	trap_Cvar_Set("ui_actualNetGameType", va("%d", ui_netGameType.integer));
}

/*
=================
UI_KeyEvent
=================
*/
void _UI_KeyEvent( int key, qboolean down ) {

	if (Menu_Count() > 0) {
		menuDef_t *menu = Menu_GetFocused();
		if (menu) {
			if (key == A_ESCAPE && down && !Menus_AnyFullScreenVisible()) {
				// Jedi Knight Galaxies: properly handle onESC
				if (!menu->handleEsc) Menus_CloseAll();
				// onESC is executed AFTER CloseAll so that it can open windows (without having em close right away :P)
				// That is, if the window doesnt have handleEsc defined, in which case its up to the window to close it
				if (menu->onESC) {
					itemDef_t it;
					it.parent = menu;
					Item_RunScript(&it, menu->onESC);
				}
			} else {
				Menu_HandleKey(menu, key, down );
			}
		} else {
			trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
			trap_Key_ClearStates();
			trap_Cvar_Set( "cl_paused", "0" );
		}
	}

	//if ((s > 0) && (s != menu_null_sound)) {
	//  trap_S_StartLocalSound( s, CHAN_LOCAL_SOUND );
	//}
}


/*
=================
UI_MouseEvent
=================
*/
void _UI_MouseEvent( int dx, int dy )
{
	// update mouse screen position
	uiInfo.uiDC.cursorx += dx;
	if (uiInfo.uiDC.cursorx < 0)
		uiInfo.uiDC.cursorx = 0;
	else if (uiInfo.uiDC.cursorx > SCREEN_WIDTH)
		uiInfo.uiDC.cursorx = SCREEN_WIDTH;

	uiInfo.uiDC.cursory += dy;
	if (uiInfo.uiDC.cursory < 0)
		uiInfo.uiDC.cursory = 0;
	else if (uiInfo.uiDC.cursory > SCREEN_HEIGHT)
		uiInfo.uiDC.cursory = SCREEN_HEIGHT;

	if (Menu_Count() > 0) {
		//menuDef_t *menu = Menu_GetFocused();
		//Menu_HandleMouseMove(menu, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory);
		Display_MouseMove(NULL, uiInfo.uiDC.cursorx, uiInfo.uiDC.cursory);
	}

}

void UI_LoadNonIngame() {
	const char *menuSet = UI_Cvar_VariableString("ui_menuFilesMP");
	if (menuSet == NULL || menuSet[0] == '\0') {
		menuSet = "ui/jampmenus.txt";
	}
	UI_LoadMenus(menuSet, qfalse);
	uiInfo.inGameLoad = qfalse;
}

void _UI_SetActiveMenu( uiMenuCommand_t menu ) {
	char buf[256];

	// this should be the ONLY way the menu system is brought up
	// enusure minumum menu data is cached
	if (Menu_Count() > 0) {
		vec3_t v;
		v[0] = v[1] = v[2] = 0;
		switch ( menu ) {
		case UIMENU_NONE:
			trap_Key_SetCatcher( trap_Key_GetCatcher() & ~KEYCATCH_UI );
			trap_Key_ClearStates();
			trap_Cvar_Set( "cl_paused", "0" );
			Menus_CloseAll();

			return;
		case UIMENU_MAIN:
			{
				qboolean active = qfalse;
				//trap_Cvar_Set( "sv_killserver", "1" );
				trap_Key_SetCatcher( KEYCATCH_UI );
				//trap_S_StartLocalSound( trap_S_RegisterSound("sound/misc/menu_background.wav", qfalse) , CHAN_LOCAL_SOUND );
				//trap_S_StartBackgroundTrack("sound/misc/menu_background.wav", NULL);
				if (uiInfo.inGameLoad) 
				{
					//				UI_LoadNonIngame();
				}

				Menus_CloseAll();
				Menus_ActivateByName("main");
				trap_Cvar_VariableStringBuffer("com_errorMessage", buf, sizeof(buf));

				if (buf[0]) 
				{
					if (!ui_singlePlayerActive.integer) 
					{
					Menus_ActivateByName("error_popmenu");
					active = qtrue;
				} 
				else 
				{
					trap_Cvar_Set("com_errorMessage", "");
				}
			}
			return;
		}

		case UIMENU_TEAM:
			trap_Key_SetCatcher( KEYCATCH_UI );
			Menus_ActivateByName("team");
			return;
		case UIMENU_POSTGAME:
			//trap_Cvar_Set( "sv_killserver", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			if (uiInfo.inGameLoad) {
				//Raz: Not loading menus was causing issues when connecting to invalid hostnames.
				UI_LoadNonIngame();
			}
			Menus_CloseAll();
			Menus_ActivateByName("endofgame");
			return;
		case UIMENU_INGAME:
			if (cgImports->EscapeTrapped()) 
				return;
			trap_Cvar_Set( "cl_paused", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			UI_BuildPlayerList();
			Menus_CloseAll();
			Menus_ActivateByName("ingame");
			return;
		case UIMENU_PLAYERCONFIG:
			// trap_Cvar_Set( "cl_paused", "1" );
			trap_Key_SetCatcher( KEYCATCH_UI );
			UI_BuildPlayerList();
			Menus_CloseAll();
			Menus_ActivateByName("ingame_player");
			return;
		case UIMENU_VOICECHAT:
			// trap_Cvar_Set( "cl_paused", "1" );
			// No chatin non-siege games.

			if (trap_Cvar_VariableValue( "g_gametype" ) < GT_TEAM)
			{
				return;
			}

			trap_Key_SetCatcher( KEYCATCH_UI );
			Menus_CloseAll();
			Menus_ActivateByName("ingame_voicechat");
			return;
		case UIMENU_CLOSEALL:
			Menus_CloseAll();
			return;
		case UIMENU_CLASSSEL:
			trap_Key_SetCatcher( KEYCATCH_UI );
			Menus_CloseAll();
			Menus_ActivateByName("ingame_siegeclass");
			return;
		}
	}
}

qboolean _UI_IsFullscreen( void ) {
	return Menus_AnyFullScreenVisible();
}



static connstate_t	lastConnState;
static char			lastLoadingText[MAX_INFO_VALUE];

static void UI_ReadableSize ( char *buf, int bufsize, int value )
{
	if (value > 1024*1024*1024 ) { // gigs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d GB", 
			(value % (1024*1024*1024))*100 / (1024*1024*1024) );
	} else if (value > 1024*1024 ) { // megs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d MB", 
			(value % (1024*1024))*100 / (1024*1024) );
	} else if (value > 1024 ) { // kilos
		Com_sprintf( buf, bufsize, "%d KB", value / 1024 );
	} else { // bytes
		Com_sprintf( buf, bufsize, "%d bytes", value );
	}
}

// Assumes time is in msec
static void UI_PrintTime ( char *buf, int bufsize, int time ) {
	time /= 1000;  // change to seconds

	if (time > 3600) { // in the hours range
		Com_sprintf( buf, bufsize, "%d hr %2d min", time / 3600, (time % 3600) / 60 );
	} else if (time > 60) { // mins
		Com_sprintf( buf, bufsize, "%2d min %2d sec", time / 60, time % 60 );
	} else  { // secs
		Com_sprintf( buf, bufsize, "%2d sec", time );
	}
}

void Text_PaintCenter(float x, float y, float scale, vec4_t color, const char *text, float adjust, int iMenuFont) {
	int len = Text_Width(text, scale, iMenuFont);
	Text_Paint(x - len / 2, y, scale, color, text, 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE, iMenuFont);
}


static void UI_DisplayDownloadInfo( const char *downloadName, float centerPoint, float yStart, float scale, int iMenuFont) {
	char sDownLoading[256];
	char sEstimatedTimeLeft[256];
	char sTransferRate[256];
	char sOf[20];
	char sCopied[256];
	char sSec[20];
	//
	int downloadSize, downloadCount, downloadTime;
	char dlSizeBuf[64], totalSizeBuf[64], xferRateBuf[64], dlTimeBuf[64];
	int xferRate;
	int leftWidth;
	const char *s;

	vec4_t colorLtGreyAlpha = {0, 0, 0, .5};

	UI_FillRect( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, colorLtGreyAlpha );

	s = GetCRDelineatedString("MENUS","DOWNLOAD_STUFF", 0);	// "Downloading:"
	strcpy(sDownLoading,s?s:"");
	s = GetCRDelineatedString("MENUS","DOWNLOAD_STUFF", 1);	// "Estimated time left:"
	strcpy(sEstimatedTimeLeft,s?s:"");
	s = GetCRDelineatedString("MENUS","DOWNLOAD_STUFF", 2);	// "Transfer rate:"
	strcpy(sTransferRate,s?s:"");
	s = GetCRDelineatedString("MENUS","DOWNLOAD_STUFF", 3);	// "of"
	strcpy(sOf,s?s:"");
	s = GetCRDelineatedString("MENUS","DOWNLOAD_STUFF", 4);	// "copied"
	strcpy(sCopied,s?s:"");
	s = GetCRDelineatedString("MENUS","DOWNLOAD_STUFF", 5);	// "sec."
	strcpy(sSec,s?s:"");

	downloadSize = trap_Cvar_VariableValue( "cl_downloadSize" );
	downloadCount = trap_Cvar_VariableValue( "cl_downloadCount" );
	downloadTime = trap_Cvar_VariableValue( "cl_downloadTime" );

	leftWidth = 320;

	UI_SetColor(colorWhite);

	Text_PaintCenter(centerPoint, yStart + 112, scale, colorWhite, sDownLoading, 0, iMenuFont);
	Text_PaintCenter(centerPoint, yStart + 192, scale, colorWhite, sEstimatedTimeLeft, 0, iMenuFont);
	Text_PaintCenter(centerPoint, yStart + 248, scale, colorWhite, sTransferRate, 0, iMenuFont);

	if (downloadSize > 0) {
		s = va( "%s (%d%%)", downloadName, (int)( (float)downloadCount * 100.0f / downloadSize ) );
	} else {
		s = downloadName;
	}

	Text_PaintCenter(centerPoint, yStart+136, scale, colorWhite, s, 0, iMenuFont);

	UI_ReadableSize( dlSizeBuf,		sizeof dlSizeBuf,		downloadCount );
	UI_ReadableSize( totalSizeBuf,	sizeof totalSizeBuf,	downloadSize );

	if (downloadCount < 4096 || !downloadTime) {
		Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, "estimating", 0, iMenuFont);
		Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s %s %s %s)", dlSizeBuf, sOf, totalSizeBuf, sCopied), 0, iMenuFont);
	} else {
		if ((uiInfo.uiDC.realTime - downloadTime) / 1000) {
			xferRate = downloadCount / ((uiInfo.uiDC.realTime - downloadTime) / 1000);
		} else {
			xferRate = 0;
		}
		UI_ReadableSize( xferRateBuf, sizeof xferRateBuf, xferRate );

		// Extrapolate estimated completion time
		if (downloadSize && xferRate) {
			int n = downloadSize / xferRate; // estimated time for entire d/l in secs

			// We do it in K (/1024) because we'd overflow around 4MB
			UI_PrintTime ( dlTimeBuf, sizeof dlTimeBuf, 
				(n - (((downloadCount/1024) * n) / (downloadSize/1024))) * 1000);

			Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, dlTimeBuf, 0, iMenuFont);
			Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s %s %s %s)", dlSizeBuf, sOf, totalSizeBuf, sCopied), 0, iMenuFont);
		} else {
			Text_PaintCenter(leftWidth, yStart+216, scale, colorWhite, "estimating", 0, iMenuFont);
			if (downloadSize) {
				Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s %s %s %s)", dlSizeBuf, sOf, totalSizeBuf, sCopied), 0, iMenuFont);
			} else {
				Text_PaintCenter(leftWidth, yStart+160, scale, colorWhite, va("(%s %s)", dlSizeBuf, sCopied), 0, iMenuFont);
			}
		}

		if (xferRate) {
			Text_PaintCenter(leftWidth, yStart+272, scale, colorWhite, va("%s/%s", xferRateBuf,sSec), 0, iMenuFont);
		}
	}
}

void StripIP( char *in, char *out )
{
	char	*last;
	qboolean found = qfalse;
	
	last = in;

	while (*in)
	{
		if (*in==':')
		{
			found = qtrue;
			in++;
		}

		if (found)
		{
			*out++ = *in++;
			continue;
		}

		in++;
	}
}

void StripPort( const char *in, char *out ) 
{
	while ( *in && *in != ':' ) {
		*out++ = *in++;
	}
	*out = 0;
}


/*
========================
UI_DrawConnectScreen

This will also be overlaid on the cgame info screen during loading
to prevent it from blinking away too rapidly on local or lan games.
========================
*/
extern vmCvar_t connmsg;

void UI_DrawConnectScreen( qboolean overlay ) {
	const char *s;
	uiClientState_t	cstate;
	char			info[MAX_INFO_VALUE];
	char text[256];
	float centerPoint, yStart, scale;

	char sStringEdTemp[256];

	menuDef_t *menu = Menus_FindByName("Connect");


	if ( !overlay && menu ) {
		Menu_Paint(menu, qtrue);
	}

	if (!overlay) {
		centerPoint = 320;
		yStart = 130;
		scale = 1.0f;	// -ste
	} else {
		centerPoint = 320;
		yStart = 32;
		scale = 1.0f;	// -ste
		return;
	}

	// see what information we should display
	trap_GetClientState( &cstate );


	info[0] = '\0';
	if( trap_GetConfigString( CS_SERVERINFO, info, sizeof(info) ) ) {
		trap_SP_GetStringTextString("MENUS_LOADING_MAPNAME", sStringEdTemp, sizeof(sStringEdTemp));
		Text_PaintCenter(centerPoint, yStart, scale, colorWhite, va( /*"Loading %s"*/sStringEdTemp, Info_ValueForKey( info, "mapname" )), 0, FONT_MEDIUM);
	}

	if (connmsg.string[0]) {
		Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite, connmsg.string, ITEM_TEXTSTYLE_SHADOWEDMORE, FONT_MEDIUM);
	} else {
		char IP[256] = "";
		char PORT[256] = "";

		if (!Q_stricmp(cstate.servername,"localhost")) {
			trap_SP_GetStringTextString("MENUS_STARTING_UP", sStringEdTemp, sizeof(sStringEdTemp));
			Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite, sStringEdTemp, ITEM_TEXTSTYLE_SHADOWEDMORE, FONT_MEDIUM);
		} else {
			trap_SP_GetStringTextString("MENUS_CONNECTING_TO", sStringEdTemp, sizeof(sStringEdTemp));
			strcpy(text, va(/*"Connecting to %s"*/sStringEdTemp, cstate.servername));
			Text_PaintCenter(centerPoint, yStart + 48, scale, colorWhite,text , ITEM_TEXTSTYLE_SHADOWEDMORE, FONT_MEDIUM);
		}
	}
	//UI_DrawProportionalString( 320, 96, "Press Esc to abort", UI_CENTER|UI_SMALLFONT|UI_DROPSHADOW, menu_text_color );

	// display global MOTD at bottom (removed -.-, fed up with the Welcome player #812318231 shit -.-)
	//Text_PaintCenter(centerPoint, 425, scale, colorWhite, Info_ValueForKey( cstate.updateInfoString, "motd" ), 0, FONT_MEDIUM);
	// print any server info (server full, bad version, etc)
	if ( cstate.connState < CA_CONNECTED ) {
		// UI Server Commands
		if ( cstate.messageString[0] == '@' ) { // Server commands start with @ to distinguish them from regular messages
			if ( UI_ServerCommand( &cstate.messageString[1] ) ) { // If this returns true, the command was handled
				return;
			}
		}
		// Unhandled server commands just print out
		Text_PaintCenter(centerPoint, yStart + 176, scale, colorWhite, cstate.messageString, 0, FONT_MEDIUM);
	}

	if ( lastConnState > cstate.connState ) {
		lastLoadingText[0] = '\0';
	}
	lastConnState = cstate.connState;

	switch ( cstate.connState ) {
	case CA_CONNECTING:
		{
			trap_SP_GetStringTextString("MENUS_AWAITING_CONNECTION", sStringEdTemp, sizeof(sStringEdTemp));
			s = va(/*"Awaiting connection...%i"*/sStringEdTemp, cstate.connectPacketCount);
		}
		break;
	case CA_CHALLENGING:
		{
			trap_SP_GetStringTextString("MENUS_AWAITING_CHALLENGE", sStringEdTemp, sizeof(sStringEdTemp));
			s = va(/*"Awaiting challenge...%i"*/sStringEdTemp, cstate.connectPacketCount);
		}
		break;
	case CA_CONNECTED:
		{
			char downloadName[MAX_INFO_VALUE];
			trap_Cvar_VariableStringBuffer( "cl_downloadName", downloadName, sizeof(downloadName) );
			if (*downloadName) {
				UI_DisplayDownloadInfo( downloadName, centerPoint, yStart, scale, FONT_MEDIUM );
				return;
			}
			trap_SP_GetStringTextString("MENUS_AWAITING_GAMESTATE", sStringEdTemp, sizeof(sStringEdTemp));
			s = /*"Awaiting gamestate..."*/sStringEdTemp;
			break;
		}
	case CA_LOADING:
		return;
	case CA_PRIMED:
		return;
	default:
		return;
	}

	if (Q_stricmp(cstate.servername,"localhost")) {
		Text_PaintCenter(centerPoint, yStart + 80, scale, colorWhite, s, 0, FONT_MEDIUM);
	}
	// password required / connection rejected information goes here
}


/*
================
cvars
================
*/

typedef struct {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
} cvarTable_t;

vmCvar_t	ui_ffa_fraglimit;
vmCvar_t	ui_ffa_timelimit;

vmCvar_t	ui_selectedModelIndex;
vmCvar_t	ui_char_model;
vmCvar_t	ui_char_skin_head;
vmCvar_t	ui_char_skin_torso;
vmCvar_t	ui_char_skin_legs;

vmCvar_t	ui_saber_type;
vmCvar_t	ui_saber;
vmCvar_t	ui_saber2;
vmCvar_t	ui_saber_color;
vmCvar_t	ui_saber2_color;

vmCvar_t	ui_team_fraglimit;
vmCvar_t	ui_team_timelimit;
vmCvar_t	ui_team_friendly;

vmCvar_t	ui_ctf_capturelimit;
vmCvar_t	ui_ctf_timelimit;
vmCvar_t	ui_ctf_friendly;

vmCvar_t	ui_arenasFile;
vmCvar_t	ui_botsFile;
vmCvar_t	ui_spSkill;

vmCvar_t	ui_browserMaster;
vmCvar_t	ui_browserGameType;
vmCvar_t	ui_browserSortKey;
vmCvar_t	ui_browserShowFull;
vmCvar_t	ui_browserShowEmpty;

vmCvar_t	ui_drawCrosshair;
vmCvar_t	ui_drawCrosshairNames;
vmCvar_t	ui_marks;

vmCvar_t	ui_redteam;
vmCvar_t	ui_redteam1;
vmCvar_t	ui_redteam2;
vmCvar_t	ui_redteam3;
vmCvar_t	ui_redteam4;
vmCvar_t	ui_redteam5;
vmCvar_t	ui_redteam6;
vmCvar_t	ui_redteam7;
vmCvar_t	ui_redteam8;
vmCvar_t	ui_blueteam;
vmCvar_t	ui_blueteam1;
vmCvar_t	ui_blueteam2;
vmCvar_t	ui_blueteam3;
vmCvar_t	ui_blueteam4;
vmCvar_t	ui_blueteam5;
vmCvar_t	ui_blueteam6;
vmCvar_t	ui_blueteam7;
vmCvar_t	ui_blueteam8;
vmCvar_t	ui_teamName;
vmCvar_t	ui_dedicated;
vmCvar_t	ui_gameType;
vmCvar_t	ui_netGameType;
vmCvar_t	ui_actualNetGameType;
vmCvar_t	ui_joinGameType;
vmCvar_t	ui_netSource;
vmCvar_t	ui_serverFilterType;
vmCvar_t	ui_opponentName;
vmCvar_t	ui_menuFiles;
vmCvar_t	ui_currentMap;
vmCvar_t	ui_currentNetMap;
vmCvar_t	ui_mapIndex;
vmCvar_t	ui_currentOpponent;
vmCvar_t	ui_selectedPlayer;
vmCvar_t	ui_selectedPlayerName;
vmCvar_t	ui_lastServerRefresh_0;
vmCvar_t	ui_lastServerRefresh_1;
vmCvar_t	ui_lastServerRefresh_2;
vmCvar_t	ui_lastServerRefresh_3;
vmCvar_t	ui_singlePlayerActive;
vmCvar_t	ui_scoreAccuracy;
vmCvar_t	ui_scoreImpressives;
vmCvar_t	ui_scoreExcellents;
vmCvar_t	ui_scoreCaptures;
vmCvar_t	ui_scoreDefends;
vmCvar_t	ui_scoreAssists;
vmCvar_t	ui_scoreGauntlets;
vmCvar_t	ui_scoreScore;
vmCvar_t	ui_scorePerfect;
vmCvar_t	ui_scoreTeam;
vmCvar_t	ui_scoreBase;
vmCvar_t	ui_scoreTimeBonus;
vmCvar_t	ui_scoreSkillBonus;
vmCvar_t	ui_scoreShutoutBonus;
vmCvar_t	ui_scoreTime;
vmCvar_t	ui_captureLimit;
vmCvar_t	ui_fragLimit;
vmCvar_t	ui_findPlayer;
vmCvar_t	ui_hudFiles;
vmCvar_t	ui_recordSPDemo;
vmCvar_t	ui_realCaptureLimit;
vmCvar_t	ui_realWarmUp;
vmCvar_t	ui_serverStatusTimeOut;
vmCvar_t	se_language;

vmCvar_t	ui_bypassMainMenuLoad;

// Jedi Knight Galaxies - Engine hook
vmCvar_t	jkg_ssquality;
vmCvar_t	ui_inventoryFilter;
vmCvar_t	ui_jkguser;
vmCvar_t	ui_jkgpass;
vmCvar_t	ui_jkgslot;
vmCvar_t	jkguser;
vmCvar_t	jkgpass;
vmCvar_t	jkgslot;
vmCvar_t	conflag;
vmCvar_t	connmsg;

// bk001129 - made static to avoid aliasing
static cvarTable_t		cvarTable[] = {
	{ &ui_ffa_fraglimit, "ui_ffa_fraglimit", "20", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_ffa_timelimit, "ui_ffa_timelimit", "0", CVAR_ARCHIVE|CVAR_INTERNAL },

	{ &ui_selectedModelIndex, "ui_selectedModelIndex", "16", CVAR_ARCHIVE|CVAR_INTERNAL },

	{ &ui_char_model,			"ui_char_model",		"jedi_tf",CVAR_ROM|CVAR_INTERNAL},
	{ &ui_char_skin_head,		"ui_char_skin_head",	"head_a1",CVAR_ROM|CVAR_INTERNAL},
	{ &ui_char_skin_torso,		"ui_char_skin_torso",	"torso_a1",CVAR_ROM|CVAR_INTERNAL}, 
	{ &ui_char_skin_legs,		"ui_char_skin_legs",	"lower_a1",CVAR_ROM|CVAR_INTERNAL}, 

	{ &ui_char_anim,			"ui_char_anim",			"BOTH_WALK1",CVAR_ROM|CVAR_INTERNAL}, 

	{ &ui_saber_type,			"ui_saber_type",		"single",CVAR_ROM|CVAR_INTERNAL},
	{ &ui_saber,				"ui_saber",				"single_1",CVAR_ROM|CVAR_INTERNAL}, 
	{ &ui_saber2,				"ui_saber2",			"none",CVAR_ROM|CVAR_INTERNAL}, 
	{ &ui_saber_color,			"ui_saber_color",		"yellow",CVAR_ROM|CVAR_INTERNAL},
	{ &ui_saber2_color,			"ui_saber2_color",		"yellow",CVAR_ROM|CVAR_INTERNAL},

	{ &ui_char_color_red,		"ui_char_color_red",	"255", CVAR_ROM|CVAR_INTERNAL}, 
	{ &ui_char_color_green,		"ui_char_color_green",	"255", CVAR_ROM|CVAR_INTERNAL}, 
	{ &ui_char_color_blue,		"ui_char_color_blue",	"255", CVAR_ROM|CVAR_INTERNAL}, 

	{ &ui_PrecacheModels,		"ui_PrecacheModels",	"0", CVAR_ARCHIVE}, 

	{ &ui_team_fraglimit, "ui_team_fraglimit", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_team_timelimit, "ui_team_timelimit", "20", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_team_friendly, "ui_team_friendly",  "1", CVAR_ARCHIVE|CVAR_INTERNAL },

	{ &ui_ctf_capturelimit, "ui_ctf_capturelimit", "8", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_ctf_timelimit, "ui_ctf_timelimit", "30", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_ctf_friendly, "ui_ctf_friendly",  "0", CVAR_ARCHIVE|CVAR_INTERNAL },

	{ &ui_botsFile, "g_botsFile", "", CVAR_INIT|CVAR_ROM },
	{ &ui_spSkill, "g_spSkill", "2", CVAR_ARCHIVE|CVAR_INTERNAL },

	{ &ui_browserMaster, "ui_browserMaster", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_browserGameType, "ui_browserGameType", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_browserSortKey, "ui_browserSortKey", "4", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_browserShowFull, "ui_browserShowFull", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_browserShowEmpty, "ui_browserShowEmpty", "1", CVAR_ARCHIVE|CVAR_INTERNAL },

	{ &ui_drawCrosshair, "cg_drawCrosshair", "1", CVAR_ARCHIVE },
	{ &ui_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
	{ &ui_marks, "cg_marks", "1", CVAR_ARCHIVE },

	{ &ui_debug, "ui_debug", "0", CVAR_TEMP|CVAR_INTERNAL },
	{ &ui_initialized, "ui_initialized", "0", CVAR_TEMP|CVAR_INTERNAL },
	//{ &ui_teamName, "ui_teamName", "Empire", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_opponentName, "ui_opponentName", "Rebellion", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam, "ui_redteam", "Empire", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam, "ui_blueteam", "Rebellion", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_dedicated, "ui_dedicated", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_gameType, "ui_gametype", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_joinGameType, "ui_joinGametype", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_netGameType, "ui_netGametype", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_actualNetGameType, "ui_actualNetGametype", "3", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam1, "ui_redteam1", "1", CVAR_ARCHIVE|CVAR_INTERNAL }, //rww - these used to all default to 0 (closed).. I changed them to 1 (human)
	{ &ui_redteam2, "ui_redteam2", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam3, "ui_redteam3", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam4, "ui_redteam4", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam5, "ui_redteam5", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam6, "ui_redteam6", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam7, "ui_redteam7", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_redteam8, "ui_redteam8", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam1, "ui_blueteam1", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam2, "ui_blueteam2", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam3, "ui_blueteam3", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam4, "ui_blueteam4", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam5, "ui_blueteam5", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam6, "ui_blueteam6", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam7, "ui_blueteam7", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_blueteam8, "ui_blueteam8", "1", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_netSource, "ui_netSource", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_menuFiles, "ui_menuFilesMP", "ui/jampmenus.txt", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_currentMap, "ui_currentMap", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_currentNetMap, "ui_currentNetMap", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_mapIndex, "ui_mapIndex", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_currentOpponent, "ui_currentOpponent", "0", CVAR_ARCHIVE|CVAR_INTERNAL },
	{ &ui_selectedPlayer, "cg_selectedPlayer", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_selectedPlayerName, "cg_selectedPlayerName", "", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_lastServerRefresh_0, "ui_lastServerRefresh_0", "", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_lastServerRefresh_1, "ui_lastServerRefresh_1", "", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_lastServerRefresh_2, "ui_lastServerRefresh_2", "", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_lastServerRefresh_3, "ui_lastServerRefresh_3", "", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_singlePlayerActive, "ui_singlePlayerActive", "0", CVAR_INTERNAL},
	{ &ui_scoreAccuracy, "ui_scoreAccuracy", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreImpressives, "ui_scoreImpressives", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreExcellents, "ui_scoreExcellents", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreCaptures, "ui_scoreCaptures", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreDefends, "ui_scoreDefends", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreAssists, "ui_scoreAssists", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreGauntlets, "ui_scoreGauntlets", "0",CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreScore, "ui_scoreScore", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scorePerfect, "ui_scorePerfect", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreTeam, "ui_scoreTeam", "0 to 0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreBase, "ui_scoreBase", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreTime, "ui_scoreTime", "00:00", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreTimeBonus, "ui_scoreTimeBonus", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreSkillBonus, "ui_scoreSkillBonus", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_scoreShutoutBonus, "ui_scoreShutoutBonus", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_fragLimit, "ui_fragLimit", "10", CVAR_INTERNAL},
	{ &ui_captureLimit, "ui_captureLimit", "5", CVAR_INTERNAL},
	{ &ui_findPlayer, "ui_findPlayer", "Kyle", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_recordSPDemo, "ui_recordSPDemo", "0", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &ui_realCaptureLimit, "capturelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE| CVAR_NORESTART},
	{ &ui_serverStatusTimeOut, "ui_serverStatusTimeOut", "7000", CVAR_ARCHIVE|CVAR_INTERNAL},
	{ &se_language, "se_language","english", CVAR_ARCHIVE | CVAR_NORESTART},	//text (string ed)

	{ &ui_bypassMainMenuLoad, "ui_bypassMainMenuLoad", "0", CVAR_INTERNAL },
	//END JLFCALLOUT
	// JKG
	{ &jkg_ssquality,		"jkg_ssquality", "95", CVAR_ARCHIVE },
	{ &ui_inventoryFilter,	"ui_inventoryFilter", "0", CVAR_ARCHIVE },
	{ &ui_hidehud,			"ui_hidehud",	"0", CVAR_INTERNAL},
	{ &ui_jkguser,			"ui_jkguser", "", CVAR_INTERNAL },
	{ &ui_jkgpass,			"ui_jkgpass", "", CVAR_INTERNAL },
	{ &ui_jkgslot,			"ui_jkgslot", "0", CVAR_INTERNAL },
	{ &jkguser,				"user", "sample", CVAR_USERINFO },
	{ &jkgpass,				"pass", "password", CVAR_USERINFO },
	{ &jkgslot,				"slot", "0", CVAR_USERINFO },
	{ &conflag,				"cflag", "", CVAR_USERINFO | CVAR_ROM },
	{ &connmsg,				"connmsg", "", CVAR_ROM },
};

// bk001129 - made static to avoid aliasing
static int		cvarTableSize = sizeof(cvarTable) / sizeof(cvarTable[0]);


/*
=================
UI_RegisterCvars
=================
*/
void UI_RegisterCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
	}
}

/*
=================
UI_UpdateCvars
=================
*/
void UI_UpdateCvars( void ) {
	int			i;
	cvarTable_t	*cv;

	for ( i = 0, cv = cvarTable ; i < cvarTableSize ; i++, cv++ ) {
		trap_Cvar_Update( cv->vmCvar );
	}
}


/*
=================
ArenaServers_StopRefresh
=================
*/
static void UI_StopServerRefresh( void )
{
	int count;

	if (!uiInfo.serverStatus.refreshActive) {
		// not currently refreshing
		return;
	}
	uiInfo.serverStatus.refreshActive = qfalse;
	Com_Printf("%d servers listed in browser with %d players.\n",
		uiInfo.serverStatus.numDisplayServers,
		uiInfo.serverStatus.numPlayersOnServers);
	count = trap_LAN_GetServerCount(UI_SourceForLAN());
	if (count - uiInfo.serverStatus.numDisplayServers > 0) {
		Com_Printf("%d servers not listed due to filters, packet loss, or pings higher than %d\n",
			count - uiInfo.serverStatus.numDisplayServers,
			(int) trap_Cvar_VariableValue("cl_maxPing"));
	}

}


/*
=================
UI_DoServerRefresh
=================
*/
static void UI_DoServerRefresh( void )
{
	qboolean wait = qfalse;

	if (!uiInfo.serverStatus.refreshActive) {
		return;
	}
	if (ui_netSource.integer != AS_FAVORITES) {
		if (ui_netSource.integer == AS_LOCAL) {
			if (!trap_LAN_GetServerCount(UI_SourceForLAN())) {
				wait = qtrue;
			}
		} else {
			if (trap_LAN_GetServerCount(UI_SourceForLAN()) < 0) {
				wait = qtrue;
			}
		}
	}

	if (uiInfo.uiDC.realTime < uiInfo.serverStatus.refreshtime) {
		if (wait) {
			return;
		}
	}

	// if still trying to retrieve pings
	if (trap_LAN_UpdateVisiblePings(UI_SourceForLAN())) {
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;
	} else if (!wait) {
		// get the last servers in the list
		UI_BuildServerDisplayList(2);
		// stop the refresh
		UI_StopServerRefresh();
	}
	//
	UI_BuildServerDisplayList(0);
}

/*
=================
UI_StartServerRefresh
=================
*/
static void UI_StartServerRefresh(qboolean full)
{
	int		i;
	char	*ptr;

	qtime_t q;
	trap_RealTime(&q);
	trap_Cvar_Set( va("ui_lastServerRefresh_%i", ui_netSource.integer), va("%s-%i, %i @ %i:%2i", GetMonthAbbrevString(q.tm_mon),q.tm_mday, 1900+q.tm_year,q.tm_hour,q.tm_min));

	if (!full) {
		UI_UpdatePendingPings();
		return;
	}

	uiInfo.serverStatus.refreshActive = qtrue;
	uiInfo.serverStatus.nextDisplayRefresh = uiInfo.uiDC.realTime + 1000;
	// clear number of displayed servers
	uiInfo.serverStatus.numDisplayServers = 0;
	uiInfo.serverStatus.numPlayersOnServers = 0;
	// mark all servers as visible so we store ping updates for them
	trap_LAN_MarkServerVisible(UI_SourceForLAN(), -1, qtrue);
	// reset all the pings
	trap_LAN_ResetPings(UI_SourceForLAN());
	//
	if( ui_netSource.integer == AS_LOCAL ) {
		trap_Cmd_ExecuteText( EXEC_NOW, "localservers\n" );
		uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 1000;
		return;
	}

	uiInfo.serverStatus.refreshtime = uiInfo.uiDC.realTime + 5000;

	if( UI_SourceForLAN() != AS_LOCAL && UI_SourceForLAN() != AS_FAVORITES) {
		if( ui_netSource.integer == AS_GLOBAL ) {
			i = 1;
		}
		else {
			i = ui_netSource.integer - AS_FAVORITES + 1;
		}

		ptr = UI_Cvar_VariableString("debug_protocol");
		if (strlen(ptr)) {
			trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers %d %s\n", i, ptr));
		}
		else {
			// cleaned this up slightly --eez
			if(!Q_stricmp(serverFilters[ui_serverFilterType.integer].description, "JKG"))
			{
#ifdef			__PTR
				trap_JKG_ChangeProtocol( 28 );
				trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers %d 28\n", i ) );			// PTR uses protocol 28
#else
				trap_JKG_ChangeProtocol( 27 );
				trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers %d 27\n", i ) );			// hax. JKG servers use protocol 27
#endif
			}
			else
			{
				trap_JKG_ChangeProtocol( atoi(UI_Cvar_VariableString("protocol")) );
				trap_Cmd_ExecuteText( EXEC_NOW, va( "globalservers %d %s\n", i, UI_Cvar_VariableString("protocol") ) );
			}
		}
	}
}

// TODO: move me into ui_shared.cpp somewhere plz
extern int Menu_Count();
extern menuDef_t Menus[MAX_MENUS];      // defined menus
extern displayContextDef_t *DC;
bool IsCursorTouching(rectDef_t *rect)
{
	bool xValid = false;
	bool yValid = false;
	if(DC->cursorx > rect->x && DC->cursorx < rect->x+rect->w)
		xValid = true;
	if(DC->cursory > rect->y && DC->cursory < rect->y+rect->h)
		yValid = true;
	return xValid && yValid;
}

#include <vector>
#include <string>
using namespace std;
#define NUMBER_CHARS_TOOLTIP_LINE	80
void RunTooltip(itemDef_t *item, itemDef_t *tooltip) {
	if(!item) return;
	if(!item->tooltip) return;

	// Some preliminary stuff needs to be set
	vec4_t defaultColor = {1.0f, 1.0f, 1.0f, 1.0f};
	tooltip->window.rect.x = DC->cursorx + 10;
	tooltip->window.rect.y = DC->cursory + 10;
	DC->setColor(defaultColor);

	// Render lines
	vector<string> lines;
	string stdstringVer = item->tooltip;

	size_t previous = 0;
	while( 1 )
	{
		size_t nextLine = stdstringVer.find("\\n", previous);
		if(nextLine == string::npos) 
		{ // last line
			lines.push_back(stdstringVer.substr(previous));
			break;
		}
		string brokenString = stdstringVer.substr(previous, nextLine-1);
		previous = nextLine+1;
		if(brokenString.length() <= NUMBER_CHARS_TOOLTIP_LINE)
		{
			lines.push_back(brokenString);
			continue;
		}
		// have to break this into multiple lines
		size_t chunk = 0;
		while( 1 )
		{
			if(chunk + NUMBER_CHARS_TOOLTIP_LINE >= brokenString.length())
			{
				// last chunk
				lines.push_back(brokenString.substr(chunk));
				break;
			}
			string chunkStr = brokenString.substr(chunk, chunk + NUMBER_CHARS_TOOLTIP_LINE);
			chunk += NUMBER_CHARS_TOOLTIP_LINE;
			lines.push_back(chunkStr);
		}
	}

	if(lines.begin() == lines.end())
		return; // no lines detected

	// Render back the lines in a sensible way.
	float lineHeight = DC->textHeight(lines[0].c_str(), 1.0f, tooltip->iMenuFont) * tooltip->textscale;
	float textHeight = lines.size() * lineHeight;

	// Find the biggest text element so we can get the size of the tooltip
	auto biggestElement = lines.begin();
	for(auto it = lines.begin(); it != lines.end(); ++it)
	{
		if(it->length() > biggestElement->length())
			biggestElement = it;
	}
	tooltip->window.rect.w = DC->textWidth(biggestElement->c_str(), 1.0f, tooltip->iMenuFont) * tooltip->textscale;
	tooltip->window.rect.w += 70.0f; // 35px padding
	tooltip->window.rect.h = textHeight + 40.0f; // 20px padding

	// Now then...ACTUALLY RENDER the text...
	for(auto it = lines.begin(); it != lines.end(); ++it)
	{
		float lineWidth = DC->textWidth(it->c_str(), 1.0f, tooltip->iMenuFont) * tooltip->textscale;
		float textX = 0.0f;
		if(tooltip->textalignment == ITEM_ALIGN_CENTER)
		{
			textX = tooltip->window.rect.x + 35.0f + (lineWidth/2);
		}
		else if(tooltip->textalignment == ITEM_ALIGN_RIGHT)
		{
			textX = tooltip->window.rect.x + tooltip->window.rect.w - 35.0f - lineWidth;
		}
		else
		{
			textX = tooltip->window.rect.x + 35.0f;
		}
		DC->drawText(	textX,
						lineHeight*(it-lines.begin()),
						tooltip->textscale,
						defaultColor,
						it->c_str(),
						0, 0, 0,
						tooltip->iMenuFont);
	}

	DC->drawHandlePic(tooltip->window.rect.x, tooltip->window.rect.y, tooltip->window.rect.w, tooltip->window.rect.h,
						tooltip->window.background);
}

void JKG_Draw_Tooltip(itemDef_t *tooltip) {
	bool ranAny = false;
	if(!tooltip) return;
	menuDef_t *menu = (menuDef_t *)(tooltip->parent);
	for(int i = 0; i < menu->itemCount; i++)
	{
		itemDef_t *item = menu->items[i];
		if(!item)									break;
		if(!(item->window.flags & WINDOW_VISIBLE))	continue;
		if(!item->tooltip)							continue;
		if(!IsCursorTouching(&item->window.rect))	continue;
		
		// Now actually render said tooltip
		RunTooltip(item, tooltip);
		ranAny = true;
	}

	if(!ranAny)
		tooltip->window.flags |= WINDOW_VISIBLE;
	else
		tooltip->window.flags &= ~WINDOW_VISIBLE;
}

