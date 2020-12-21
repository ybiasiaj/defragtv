import time
import re

import q3a
import log
from utility import *

def log_command():
	log.writeln("script", "command " + q3a.Cmd_Argv(0) + " " + str(q3a.Cmd_Argc()));
	for i in range(0, q3a.Cmd_Argc()):
		log.writeln("script", str(i) + "	" + q3a.Cmd_Argv(i));

# if you parse chat or print wrong,
# you could allow executing arbitrary code. beware!
def parse_chat():
	log_command()
	tokens = re.split(' ', q3a.Cmd_Argv(1))
	for t in tokens:
		log.writeln("script", t)
	if tokens[1] == "^2hello" or tokens[1] == "^2hi" or tokens[1] == "^2privet":
		q3a.Cbuf_ExecuteText(1, "say HELLO")
		q3a.Cbuf_ExecuteText(1, "play sound/player/bones/taunt.wav") # can play sound/feedback/intro_01.wav and time s_volume 0 after 'welcome'
	if tokens[1] == "^2?info":
		q3a.Cbuf_ExecuteText(1, "say no info")

def parse_print():
	log_command()
	tokens = re.split(' ', q3a.Cmd_Argv(1))
	#TODO move to events, rename events to callbacks