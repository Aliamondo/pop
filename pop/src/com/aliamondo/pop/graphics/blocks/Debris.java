package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class Debris extends GeneralBlock {
	public Debris(int x, int y) {
		super(x, y, new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/debris.png")), 120, 155),
				Block.DEBRIS, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
	}
}
