from dataclasses import dataclass
import sys
import time

import q3a
import log

from utility import *

# TODO scores time are in seconds, parse for display
# TODO prune scores table of old entries, so it's not full of duplicates / dc'd players

@dataclass
class Score:
	client: int = -1
	time: int = 0
	timefmt: str = ""
	ping: int = 0
	follow: int = 0
	name: str = ""
	updated: int = -1
scores = []
following = -1

def update_scores():
	i = int(0)
	#log.writeln("script", "updating player list")

def timer_format(ms):
	m, ms = divmod(ms, 60000)
	s, ms = divmod(ms, 1000)

	#fmt = ""
	#fmt += str(m) + ":"
	#fmt += '{:02}'.format(s) + "."
	#fmt += '{:03}'.format(ms)

	return '{}:{:02}.{:03}'.format(m,s,ms)

# by time descending, then active players without a time, then spectators without a time
def score_order(s):
	if(s.follow == -1 and s.time > 0): return s.time
	if(s.follow != -1 and s.time > 0): return s.time
	if(s.follow == -1 and s.time ==0): return int(10000000000)
	if(s.follow != -1 and s.time ==0): return int(9223372036854775806)
	

def parse_scores():
	base = 5
	len = 4
	tm = time.time()
	num = int(q3a.Cmd_Argv(1))
	if(q3a.Cmd_Argc() < base + (len * num)): return

	for s in scores:
		s.updated = 0

	for i in range(0, num):
		client = int(q3a.Cmd_Argv(base + (i * len) + 0))
		timer  = int(q3a.Cmd_Argv(base + (i * len) + 1))
		ping   = int(q3a.Cmd_Argv(base + (i * len) + 2))
		follow = int(q3a.Cmd_Argv(base + (i * len) + 3))
		scores.append(Score(client, timer, timer_format(timer), ping, follow, q3a.CL_GetPlayerNameForClient(client), 1))
		#log.writeln("script", str(i) + "	" + str(client) + " " + str(timer) + " " + str(ping) + " " + str(follow));
		#log.writeln("script", q3a.CL_GetPlayerNameForClient(client))

	scores.sort(key = score_order)
