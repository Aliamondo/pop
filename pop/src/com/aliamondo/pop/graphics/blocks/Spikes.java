package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;

public class Spikes extends GeneralBlock{
	private enum OriginalState {
		OPEN, CLOSED
	}
	
	Animation animation;
	float elapsedTime = 0;
	TextureAtlas spriteSheet;
	OriginalState originalState = OriginalState.CLOSED;
	
	public Spikes(int x, int y) {
		super(x, y, Block.SPIKES, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
		
		spriteSheet = new TextureAtlas(Gdx.files.internal("sprites/dungeon/spikes.txt"));

		animation = new Animation(0.10f, spriteSheet.getRegions());
	}
	
	@Override
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier) {
		modifier = renderAndReturnModifier(batch, modifier, getPos().x, getPos().y);
		return modifier;
	}
	
	@Override
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier, float x, float y) {

		if (modifier == Modifier.SPIKE_DISABLED) {
			originalState = OriginalState.CLOSED;
			batch.draw(animation.getKeyFrame(0), x, y);
		}
		else if (modifier == Modifier.SPIKE_FULLY_OUT_2) {
			elapsedTime += Gdx.graphics.getDeltaTime();
			animation.setPlayMode(Animation.NORMAL);
			
			if (!animation.isAnimationFinished(elapsedTime) && originalState != OriginalState.CLOSED) {
				batch.draw(animation.getKeyFrame(elapsedTime, false), x, y);
			}
			else {
				if (originalState == OriginalState.CLOSED) {
					elapsedTime = 0;
					batch.draw(animation.getKeyFrame(0), x, y);
				}
				else {
					batch.draw(animation.getKeyFrame(animation.animationDuration), x, y);
				}
			}
			originalState = OriginalState.OPEN;
		}
		else if (modifier == Modifier.SPIKE_NORMAL) {
			elapsedTime += Gdx.graphics.getDeltaTime();
			animation.setPlayMode(Animation.REVERSED);
			
			if (!animation.isAnimationFinished(elapsedTime) && originalState != OriginalState.OPEN) {
				batch.draw(animation.getKeyFrame((elapsedTime), false), x, y);
			}
			else {
				if (originalState == OriginalState.OPEN) {
					elapsedTime = 0;
					batch.draw(animation.getKeyFrame(0), x, y);
				}
				else {
					batch.draw(animation.getKeyFrame(animation.animationDuration), x, y);
				}
			}
			originalState = OriginalState.CLOSED;
		}
		return modifier;
	}
	
	@Override
	public void dispose() {
		super.dispose();
		spriteSheet.dispose();
	}
}