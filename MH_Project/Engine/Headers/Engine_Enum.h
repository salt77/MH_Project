#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum DIR { DIR_LOOK, DIR_RIGHT, DIR_UP, DIR_END };
	enum PL_ACTION { PL_IDLE, PL_MOVE, PL_ATK, PL_DASH, PL_SMASH, PL_SKILL, PL_TIRED, PL_DAMAGED, PL_DEAD, PL_END };
	enum PL_STATE 
	{ 
		STATE_SPRINT_STOP, 
		STATE_REVIVE, STATE_DEAD,
		STATE_DAMAGE_RESIST, STATE_DOWNTOIDLE_FRONT, STATE_DOWNTOIDLE_BACK, STATE_DOWNIDLE_FRONT, STATE_DOWNIDLE_BACK, STATE_DAMAGEFROM_FRONT, STATE_DAMAGEFROM_BACK, 
		STATE_WINDMILL, STATE_DOUBLE_CRECSENT, 
		STATE_DASHATK, 
		STATE_FURY2, STATE_FURY, 
		STATE_DASH_W, STATE_DASH_S, STATE_DASH_N, STATE_DASH_E, 
		STATE_SMASH3, STATE_SMASH4_B, STATE_SMASH4, STATE_SMASH2_B, STATE_SMASH2, STATE_SMASH1, 
		STATE_ATK4, STATE_ATK3, STATE_ATK2, STATE_ATK1,
		STATE_SPRINT, STATE_RUN, 
		STATE_IDLE, 
		STATE_SP_FEVER,
		STATE_THROW_END, STATE_THROW_DURING, STATE_THROW_BEGIN,
		STATE_TIRED_END, STATE_TIRED_DURING, STATE_TIRED_BEGIN,
		STATE_END
	};
	enum OBJECT
	{
		OBJ_PLAYER, OBJ_AHGLAN,
		OBJ_END
	};
	enum WEAPON_MODE
	{
		WEAPON_DUALSWORD, /*WEAPON_SPEAR, */WEAPON_SECONDARY,
		WEAPON_END
	};
	enum SCENE_ID
	{
		SCENE_LOGO, SCENE_LOADING, SCENE_STAGE,
		SCENE_END
	};
	enum NAMINGSCENE_ID
	{
		NAMING_BOSSNAME, NAMING_STAGENAME, NAMING_BACKGROUND,
		NAMING_END
	};
	enum BOSS_ID
	{
		BOSS_AHGLAN, BOSS_CLOYAN,
		BOSS_END
	};
	enum BUFF_ID
	{
		BUFF_CRITICAL, 
		BUFF_END
	};
	enum DAMAGEFONT_ID
	{
		DAMAGEFONT_NORMAL, DAMAGEFONT_SKILL, DAMAGEFONT_MONSTER, 
		DAMAGEFONT_END
	};

	// ********************************************* //
	enum WINMODE { MODE_FULL, MODE_WIN };
	
	// DYNAMIC의 경우 매 프레임마다 갱신이 필요한 컴포넌트
	enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };
	
	enum BUFFERID{ BUFFER_TRICOL, BUFFER_RCTEX, BUFFER_TERRAINTEX, BUFFER_CUBETEX, BUFFER_END	};
	
	enum INFO { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };
	enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };
		
	enum TEXTURETYPE { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END};

	enum MOUSEKEYSTATE	{ DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	enum MESHTYPE { MESH_STATIC, MESH_DYNAMIC, MESH_NAVI, MESH_END };

	enum COLTYPE { COL_FALSE, COL_TRUE, COL_END };
	
	enum POINTT { POINT_START, POINT_FINISH, POINT_END };
	enum COMPARE { COMPARE_LEFT, COMPARE_RIGHT }; // 플레이어 위치를 기준으로 왼쪽에 선분이 있으면 이동한 것

	enum COLLIDERTYPE { COLTYPE_BOX_DAMAGED, COLTYPE_BOX_HIT, COLTYPE_BOX_OTHER, COLTYPE_SPHERE_DAMAGED, COLTYPE_SPHERE_HIT, COLTYPE_END };
}

#endif // Engine_Enum_h__
