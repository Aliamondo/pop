package com.aliamondo.pop.levels;

/* OpenPrince
 * Copyright (C) 2012, Robert Biro (DarthJDG)
 * Modified by Aliamondo, 2014
 * 
 * Many algorithms and reverse engineered data are based on the Princed Project.
 * <http://www.princed.org/>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import java.util.ArrayList;
import java.util.List;

public class Level {
	public List<LevelScreen> screens;
	public List<Event> events;
	
	public int startScreenId;
	public int startBlock;
	public boolean startLookingLeft = true;

	public Level() {
		screens = new ArrayList<LevelScreen>();
		events = new ArrayList<Event>();
	}

	public LevelScreen addScreen() {
		int id = screens.size() + 1;
		LevelScreen s = new LevelScreen(id);
		screens.add(s);
		return s;
	}
	
	public Event addEvent(byte door1, byte door2) {
		Event e = new Event(door1, door2);
		events.add(e);
		return e;
	}
}
