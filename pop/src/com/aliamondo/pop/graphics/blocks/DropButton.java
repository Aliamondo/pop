package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class DropButton extends GeneralBlock {
	
	public DropButton(int x, int y, int eventNum) {
		super(x, y, new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/drop_button.png")), 120, 155),
				Block.DROP_BUTTON, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
		
		super.eventNum = eventNum;
	}
}