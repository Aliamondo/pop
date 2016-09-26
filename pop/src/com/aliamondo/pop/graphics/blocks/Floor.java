package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;

public class Floor extends GeneralBlock {
	public Floor(int x, int y) {
		super(x, y, Block.FLOOR, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
	}
}
