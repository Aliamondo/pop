package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;
import com.badlogic.gdx.graphics.g2d.TextureAtlas.AtlasRegion;

public class Sword extends GeneralBlock {
	AtlasRegion frame0, frame1;
	float elapsedTime = 0;
	TextureAtlas spriteSheet;
	
	public Sword(int x, int y) {
		super(x, y, Block.SWORD, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);

		spriteSheet = new TextureAtlas(Gdx.files.internal("sprites/dungeon/sword.txt"));
		frame0 = spriteSheet.findRegion("0");
		frame1 = spriteSheet.findRegion("1");
	}
	
	@Override
	public void render(SpriteBatch batch, Modifier modifier) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		if (elapsedTime <= 2) {
			batch.draw(frame0, getPos().x, getPos().y);
		}
		else if (elapsedTime <= 2.25) {
			batch.draw(frame1, getPos().x, getPos().y);	
		}
		else {
			batch.draw(frame0, getPos().x, getPos().y);
			elapsedTime = 0;
		}
	}
	
	@Override
	public void render(SpriteBatch batch, Modifier modifier, float x, float y) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		if (elapsedTime <= 2) {
			batch.draw(frame0, getPos().x, getPos().y);
		}
		else if (elapsedTime <= 2.25) {
			batch.draw(frame1, getPos().x, getPos().y);	
		}
		else {
			batch.draw(frame0, getPos().x, getPos().y);
			elapsedTime = 0;
		}
	}
	
	@Override
	public void dispose() {
		spriteSheet.dispose();
	}
}
