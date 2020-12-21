from dataclasses import dataclass
import sys
import time
import re
import socket

import log
import q3a
from utility import *

# TODO don't draw empty or full servers
@dataclass
class Server:
	address: tuple = ('')
	hostname: str = ""
	mapname: str = ""
	gametype: str = ""
	physics: str = ""
	players: int = 0

servers = [
	Server(('91.121.153.32',27960)),
	Server(('91.121.153.32',27966)),
	Server(('91.121.153.32',27967)),
	Server(('91.121.153.32',27968)),
	Server(('91.121.153.32',27969)),
	Server(('91.121.153.32',27970)),
	Server(('91.121.153.32',27971)),
	Server(('91.121.153.32',27972)),
	Server(('91.121.153.32',27991)),

	Server(('83.243.73.220',27960)),
	Server(('83.243.73.220',27961)),
	Server(('83.243.73.220',27965)),

	Server(('140.82.4.154',27960)),
	Server(('140.82.4.154',27961)),
	Server(('140.82.4.154',27962)),

	Server(('139.180.168.2',27960)),
	Server(('139.180.168.2',27961)),
]

def defrag_gametype_str(v):
	if v == 0: return 'df'
	if v == 1: return 'df'
	if v == 2: return 'fs'
	if v == 3: return 'ctf'
	if v == 4: return '?'
	if v == 5: return 'mdf'
	if v == 6: return 'fs'
	if v == 7: return 'ctf'
	return '?'

def defrag_physics_str(v):
	if v == 0: return 'vq3'
	if v == 1: return 'cpm'
	return '?'

def parse_server_status_response(sv, str):
	#ioq3_py.log_script(str)
	tokens = re.split('\\\\', str)
	for i, t in enumerate(tokens):
		#ioq3_py.log_script(t)
		if t == 0:
			tp = None
		else:
			tp = tokens[i - 1]
		#
		if tp == "sv_hostname":
			sv.hostname = info = t[:32] + (t[32:] and '..') # truncate so drawing is consistent
			#log.writeln("script", t)
		if tp == "mapname":
			sv.mapname = t
		if tp == "df_promode":
			sv.physics = defrag_physics_str(int(t))
		if tp == "defrag_gametype":
			sv.gametype = defrag_gametype_str(int(t))

def update_server_timeout(sv, timeout):
	#log.writeln("script", f'updating server {sv.address}')
	conn = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	conn.settimeout(timeout)

	msg = b'\xFF\xFF\xFF\xFFgetstatus xxx'
	start = time.time()
	conn.sendto(msg, sv.address)
	try:
		data, server = conn.recvfrom(1024)
		end = time.time()
		elapsed = end - start
		parse_server_status_response(sv, data.decode('utf-8', 'ignore'))
	except socket.timeout:
		log.writeln("script", "timeout")

timelast = time.time()

def update_serverlist():
	global timelast
	timecur = time.time()
	timedelta = timecur - timelast

	if(timedelta > 3):
		#log.writeln("script", "updating server list")
		for i,sv in enumerate(servers):
			j = int(0)
			update_server_timeout(sv, 0.3)
		servers.sort(key = lambda sv : sv.players)

	timelast = timecur

