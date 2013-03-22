# Copyright (C) 2010 Intel Corporation
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

all: build

build: buildSIM buildEFH buildEVH buildQTGUI
	@(rm -rf ./bin)
	@(mkdir ./bin)
	@(cp src/gestureSim/gestureSim ./bin)
	@(cp src/effectHunter/effectHunter ./bin)
	@(cp src/eventHunter/eventHunter ./bin)
	@(cp src/qtGUI/qtGUI ./bin)

buildSIM:
	@(cd src/gestureSim && $(MAKE) && cd ../../)

buildEFH:
	@(cd src/effectHunter && $(MAKE) && cd ../../)

buildEVH:
	@(cd src/eventHunter && $(MAKE) && cd ../../)

buildQTGUI:
	@(cd src/qtGUI && qmake && $(MAKE) && cd ../../)

buildMEEGOGUI:
	@(cd src/meegotouchGUI && qmake && $(MAKE) && cd ../../)
	@(mkdir -p ./bin)
	@(cp src/meegotouchGUI/meegotouchGUI ./bin)

clean: 
	@(rm -rf ./bin)
	@(cd src/gestureSim && $(MAKE) clean && cd ../../)
	@(cd src/effectHunter && $(MAKE) clean && cd ../../)
	@(cd src/eventHunter && $(MAKE) clean && cd ../../)
	@(cd src/qtGUI && $(MAKE) clean && cd ../../)
	@(cd src/meegotouchGUI && $(MAKE) clean && cd ../../)
