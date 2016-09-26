package com.aliamondo.pop.levels;

/* OpenPrince
 * Copyright (C) 2012, Robert Biro (DarthJDG)
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

public enum BlockGroup {
	NONE (0x0000),
	FREE (0x0100),
	SPIKE (0x0200),
	GATE (0x0300),
	TAPEST (0x0400),
	POTION (0x0500),
	TTOP (0x0600),
	CHOMP (0x0700),
	WALL (0x0800),
	EVENT (0x0900);

	public final int modifierOffset;

	private BlockGroup(int offset) {
		modifierOffset = offset;
	}
}
