package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;

public class Chopper extends GeneralBlock{
	Animation animation;
	float elapsedTime = 0;
	TextureAtlas spriteSheet;
	
	public Chopper(int x, int y) {
		super(x, y, Block.CHOPPER, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
		
		spriteSheet = new TextureAtlas(Gdx.files.internal("sprites/dungeon/chopper.txt"));

		animation = new Animation(0.10f, spriteSheet.getRegions());
	}
	
	@Override
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier) {
		modifier = renderAndReturnModifier(batch, modifier, getPos().x, getPos().y);
		return modifier;
	}
	
	@Override
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier, float x, float y) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		batch.draw(animation.getKeyFrame(elapsedTime, false), x, y);
		if (animation.getKeyFrameIndex(elapsedTime) < 3) {
			modifier = Modifier.CHOMP_PART_OPEN;
		}
		else {
			modifier = Modifier.CHOMP_NORMAL;
		}
		if (elapsedTime > animation.animationDuration + 0.75) {
			elapsedTime = 0;
		}
		return modifier;
	}
	
	@Override
	public void dispose() {
		super.dispose();
		spriteSheet.dispose();
	}
}