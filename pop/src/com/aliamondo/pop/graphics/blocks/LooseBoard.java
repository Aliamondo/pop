package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class LooseBoard extends GeneralBlock{
	Animation animation;
	float elapsedTime = 0;
	TextureAtlas spriteSheet;
	
	public LooseBoard(int x, int y) {
		//super(x, y, Block.LOOSE_BOARD, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
		super(x, y, new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/floor.png")), 120, 155),
				Block.LOOSE_BOARD, GeneralBlock.FLOOR_OFFSET_LEFT, GeneralBlock.FLOOR_WIDTH, GeneralBlock.FLOOR_HEIGHT);
		
		spriteSheet = new TextureAtlas(Gdx.files.internal("sprites/dungeon/loose_board.txt"));

		animation = new Animation(0.20f, spriteSheet.getRegions());
		animation.setPlayMode(Animation.LOOP_PINGPONG);
	}
	
	@Override
	public void renderAnim(SpriteBatch batch, Modifier modifier) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		batch.draw(animation.getKeyFrame(elapsedTime, true), getPos().x, getPos().y);
	}
	
	@Override
	public void renderAnimOnce(SpriteBatch batch, Modifier modifier) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		batch.draw(animation.getKeyFrame(elapsedTime, false), getPos().x, getPos().y);
	}
	
	@Override
	public void renderAnim(SpriteBatch batch, Modifier modifier, float x, float y) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		batch.draw(animation.getKeyFrame(elapsedTime, true), x, y);		
	}
	
	@Override
	public void clearAnimationTime() {
		elapsedTime = 0;
	}
	
	@Override
	public void dispose() {
		super.dispose();
		spriteSheet.dispose();
	}
}