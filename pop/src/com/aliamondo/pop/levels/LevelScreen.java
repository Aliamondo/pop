package com.aliamondo.pop.levels;

/* OpenPrince
 * Copyright (C) 2012, Robert Biro (DarthJDG)
 * Modified by Aliamondo, 2014.
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

import com.aliamondo.pop.graphics.blocks.GeneralBlock;

public class LevelScreen {
	public int id;
	public int width;
	public int height;
	
	public LevelScreen linkLeft;
	public LevelScreen linkRight;
	public LevelScreen linkTop;
	public LevelScreen linkBottom;
	
	public List<GeneralBlock> blocks;
	public List<Modifier> modifiers;

	public LevelScreen(int id) {
		this(id, 10, 3);
	}

	public LevelScreen(int id, int width, int height) {
		this.id = id;
		this.width = width;
		this.height = height;

		blocks = new ArrayList<GeneralBlock>();
		modifiers = new ArrayList<Modifier>();
	}
}
