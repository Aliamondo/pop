package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;

public class Unknown extends GeneralBlock{
	public Unknown(int x, int y) {
		super(x, y, Block.UNKNOWN, WALL_OR_EMPTY_OFFSET_LEFT, WALL_OR_EMPTY_WIDTH, WALL_OR_EMPTY_HEIGHT);
	}
}
