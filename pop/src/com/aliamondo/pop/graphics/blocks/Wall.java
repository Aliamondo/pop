package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class Wall extends GeneralBlock{
	public Wall(int x, int y) {
		super(x, y, new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/wall.png")), 120, 155),
				Block.WALL, WALL_OR_EMPTY_OFFSET_LEFT, WALL_OR_EMPTY_WIDTH, WALL_OR_EMPTY_HEIGHT);
	}
}
