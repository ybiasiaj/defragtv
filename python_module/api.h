#pragma once

#define PYTHON_MODULE_VERSION 0.07
#define PYTHON_MODULE_NAME "ioq3_py"

#if PY_CLIENT == 1
void python_module_initialize();
void python_module_shutdown();
void python_module_reload();
void python_module_invoke();
void python_module_invoke_cmd();
void python_module_eval();
void python_module_eval_cmd();

#endif

#if PY_MODULE == 1
#include "q3def.h"
extern "C" {
	void CL_AddReliableCommand(const char* cmd, qboolean isDisconnectCmd);
	void Cbuf_ExecuteText(int exec_when, const char* text);
	void	Cmd_AddCommand(const char* cmd_name, xcommand_t function);
	char* Cmd_Args(void);
	int		Cmd_Argc(void);
	char* Cmd_Argv(int arg);
	void /*QDECL*/ Com_Printf(const char* fmt, ...);
	void /*QDECL*/ Com_Error(int code, const char* fmt, ...);
	void 	Cvar_Set(const char* var_name, const char* value);
	float	Cvar_VariableValue(const char* var_name);
	char* Cvar_VariableString(const char* var_name);
	char* Info_ValueForKey(const char* s, const char* key);
	void	SCR_AdjustFrom640(float* x, float* y, float* w, float* h);
	void	SCR_FillRect(float x, float y, float width, float height,
						 const float* color);
	void	SCR_DrawNamedPic(float x, float y, float width, float height, const char* picname);
	void	SCR_DrawBigString(int x, int y, const char* s, float alpha, qboolean noColorEscape);
	void	SCR_DrawBigStringColor(int x, int y, const char* s, vec4_t color, qboolean noColorEscape);
	void	SCR_DrawSmallStringExt(int x, int y, const char* string, float* setColor, qboolean forceColor, qboolean noColorEscape);
	void	SCR_DrawSmallChar(int x, int y, int ch);

	// non-standard functions, added for defragtv use
	const char* CL_GetPlayerNameForClient(int clientNum);
	const char* CL_GetServerMapName();
	qboolean CL_DoesMapExist(const char* mapname);
}
#endif
