import q3a
import log
from utility import *

from ioq3_py.players import *
from ioq3_py.scores import *
from ioq3_py.commands import *
from ioq3_py.servers import *

@dataclass
class ServerRankScore:
	index: int
	time: int

# or top3?
fastesttimes = []
fastesttime = 9223372036854775807
fastestclient = -1

def draw_scores():
	global fastesttime
	global fasttimes
	global fastestclient


	x = 8
	y = 300
	h = 9
	w = 180
	bg = 0x00000016
	tc = 0xFFFFFFFF
	ol1 = 0xFFAA0099
	ol2 = 0xFFFFFF99
	ol3 = 0xBB770099

	#w = 40
	#hud.quad(x + 300, y, w, h, 0, 0xFF0000FF)
	#hud.quad(x + 300 + w, y, w, h, 0, 0x00FF00FF)
	#hud.quad(x + 300 + w + w, y, w, h, 0, 0x0000FFFF)

	hud.quad(x, y, w, h, 0, bg)
	hud.string(x, y, 0, "time", tc)		
	hud.string(x + 40, y, 0, "ping", tc)
	hud.string(x + 60, y, 0, "#", tc)		
	hud.string(x + 80, y, 0, "name", tc)

	y += h

	for i,s in enumerate(scores):
		if s.updated:
			hud.quad(x, y, w, h, 0, bg)

			if(i == 0):# broken? disappears after a while
				hud.quad(x, y, w, h - 1, 1, ol1)

			if(s.follow != -1):
				if(s.time == 0):
					hud.image(x, y, h, h, "icons/noammo.tga")
				else:
					hud.string(x, y, 0, s.timefmt, tc)	

			if(s.follow == -1):
				if(s.time == 0):
					hud.string(x, y, 0, "dnf", tc)
				else:
					hud.string(x, y, 0, s.timefmt, tc)	

			hud.string(x + 40, y, 0, str(s.ping) if s.follow == -1 else 'S', tc)

			hud.string(x + 60, y, 0, str(s.client), tc)		
			hud.string(x + 80, y, 0, s.name, tc)

			y += h


def draw_serverlist():
	x = 400
	y = 300
	w = 220
	h = 9
	bg = 0x00000016
	tc = 0xFFFFFFFF

	hud.quad(x, y, w, h, 0, bg)
	hud.string(x, y, 0, "name", tc)
	hud.string(x + 110, y, 0, "mode", tc)
	hud.string(x + 150, y, 0, "map", tc)

	y += h
	
	for i, sv in enumerate(servers):
		hud.quad(x, y, w, h, 0, bg)
		hud.string(x, y, 0, str(i), tc)
		hud.string(x + 10, y, 0, sv.hostname, tc)
		hud.string(x + 110, y, 0, sv.gametype, tc)
		hud.string(x + 130, y, 0, sv.physics, tc)
		hud.string(x + 150, y, 0, sv.mapname, tc)
		y += h
