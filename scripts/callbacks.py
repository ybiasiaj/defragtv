import time

import q3a
import log
from utility import *

from ioq3_py.players import *
from ioq3_py.scores import *
from ioq3_py.commands import *
from ioq3_py.servers import *
from ioq3_py.draw import *

# there is no main loop for python to run, so
timelastscoresupdate = time.time()
def cl_frame():
	global timelastscoresupdate
	timedelta = time.time() - timelastscoresupdate
	if timedelta > 5:
		q3a.CL_AddReliableCommand("score", q3a.qboolean.qfalse)
		timelastscoresupdate = time.time()


register_callback("cl_frame", cl_frame)

# hud rendering must be done here, called during SCR_DrawScreenField
def draw2d():
	draw_scores()
	draw_serverlist()

register_callback("draw2d", draw2d)

def servercommand():
	#log.writeln("script", "servercommand " + q3a.Cmd_Argv(0) + " " + str(q3a.Cmd_Argc()));
	if(q3a.Cmd_Argv(0) == "scores"):
		parse_scores()
		update_serverlist()
	
	if(q3a.Cmd_Argv(0) == "chat"):
		parse_chat()

	if(q3a.Cmd_Argv(0) == "disconnect"):
		log_command()

	if(q3a.Cmd_Argv(0) == "print"):
		parse_print()

	if(q3a.Cmd_Argv(0) == "map"):
		log_command()

register_callback("servercommand", servercommand)