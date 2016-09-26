package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class BrokenBoard extends GeneralBlock {
	
	int screenId = 0;
	boolean hitSomeone = false;
	
	public BrokenBoard(int x, int y) {
		super(x, y, new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/broken_board.png")), 120, 155),
				Block.BROKEN_BOARD, GeneralBlock.BROKEN_BOARD_OFFSET_LEFT, GeneralBlock.BROKEN_BOARD_WIDTH, GeneralBlock.BROKEN_BOARD_HEIGHT);
	}
	
	public void setScreenId(int screenId) {
		this.screenId = screenId;
	}
	
	public int getScreenId() {
		return screenId;
	}
	
	public boolean hitSomeone() {
		return hitSomeone;
	}
	
	public void setHitSomeone(boolean hitSomeone) {
		this.hitSomeone = hitSomeone;
	}
}
