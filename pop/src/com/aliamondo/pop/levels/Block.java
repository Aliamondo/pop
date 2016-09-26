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

import java.util.HashMap;
import java.util.Map;

public enum Block {
	EMPTY (0x00, BlockGroup.FREE),
	FLOOR (0x01, BlockGroup.FREE),
	SPIKES (0x02, BlockGroup.SPIKE),
	PILLAR (0x03, BlockGroup.NONE),
	GATE (0x04, BlockGroup.GATE),
	STUCK_BUTTON (0x05, BlockGroup.NONE),
	DROP_BUTTON (0x06, BlockGroup.EVENT),
	TAPESTRY (0x07, BlockGroup.TAPEST),
	BOTTOM_BIG_PILLAR (0x08, BlockGroup.NONE),
	TOP_BIG_PILLAR (0x09, BlockGroup.NONE),
	POTION (0x0a, BlockGroup.POTION),
	LOOSE_BOARD (0x0b, BlockGroup.NONE),
	TAPESTRY_TOP (0x0c, BlockGroup.TTOP),
	MIRROR (0x0d, BlockGroup.NONE),
	DEBRIS (0x0e, BlockGroup.NONE),
	RAISE_BUTTON (0x0f, BlockGroup.EVENT),
	EXIT_LEFT (0x10, BlockGroup.NONE),
	EXIT_RIGHT (0x11, BlockGroup.NONE),
	CHOPPER (0x12, BlockGroup.CHOMP),
	TORCH (0x13, BlockGroup.NONE),
	WALL (0x14, BlockGroup.WALL),
	SKELETON (0x15, BlockGroup.NONE),
	SWORD (0x16, BlockGroup.NONE),
	BALCONY_LEFT (0x17, BlockGroup.NONE),
	BALCONY_RIGHT (0x18, BlockGroup.NONE),
	LATTICE_PILLAR (0x19, BlockGroup.NONE),
	LATTICE_SUPPORT (0x1a, BlockGroup.NONE),
	SMALL_LATTICE (0x1b, BlockGroup.NONE),
	LATTICE_LEFT (0x1c, BlockGroup.NONE),
	LATTICE_RIGHT (0x1d, BlockGroup.NONE),
	TORCH_WITH_DEBRIS (0x1e, BlockGroup.NONE),
	NULL (0x1f, BlockGroup.NONE),
	UNKNOWN (0x100, BlockGroup.NONE),
	
	BROKEN_BOARD (0x90, BlockGroup.NONE);

	public final int value;
	public final BlockGroup group;

	private Block(int value, BlockGroup group) {
		this.value = value;
		this.group = group;
	}

	private static final Map<Integer, Block> sMap = new HashMap<Integer, Block>();

	static {
		for (Block block : Block.values())
			sMap.put(block.value, block);
	}

	public static Block get(int value) {
		Block b = sMap.get(value);
		if (b == null) {
			b = Block.UNKNOWN;
		}
		return b;
	}
}
