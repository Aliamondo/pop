package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;

public class Gate extends GeneralBlock{
	private enum OriginalState {
		OPEN, CLOSED, CLOSING
	}
	
	Animation animation;
	float elapsedTime = 0;
	TextureAtlas spriteSheet;
	OriginalState originalState = OriginalState.CLOSED;
	
	public Gate(int x, int y) {
		super(x, y, Block.GATE, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
		
		spriteSheet = new TextureAtlas(Gdx.files.internal("sprites/dungeon/gate.txt"));

		animation = new Animation(0.80f, spriteSheet.getRegions());
	}
	
	@Override
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier) {
		modifier = renderAndReturnModifier(batch, modifier, getPos().x, getPos().y);
		return modifier;
	}
	
	@Override
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier, float x, float y) {

		if (modifier == Modifier.GATE_ALWAYS_OPEN) {
			animation.setPlayMode(Animation.NORMAL);
			originalState = OriginalState.OPEN;
			batch.draw(animation.getKeyFrame(0), x, y);
		}
		else if (modifier == Modifier.GATE_CLOSED) {
			elapsedTime = animation.animationDuration;
			animation.setPlayMode(Animation.NORMAL);
			originalState = OriginalState.CLOSED;
			batch.draw(animation.getKeyFrame(animation.animationDuration), x, y);
		}
		else if (modifier == Modifier.GATE_OPEN) {
			//System.out.println("OPEN");
			elapsedTime += Gdx.graphics.getDeltaTime();
			animation.setPlayMode(Animation.REVERSED);
			
			if (!animation.isAnimationFinished(elapsedTime) && originalState != OriginalState.CLOSED && originalState != OriginalState.CLOSING) {
				batch.draw(animation.getKeyFrame(elapsedTime, false), x, y);
			}
			else {
				if (!animation.isAnimationFinished(elapsedTime) && (originalState == OriginalState.CLOSED || originalState == OriginalState.CLOSING)) {
					elapsedTime = animation.animationDuration - elapsedTime;
					batch.draw(animation.getKeyFrame((elapsedTime), false), x, y);
				}
				else if (animation.isAnimationFinished(elapsedTime) && (originalState == OriginalState.CLOSED || originalState == OriginalState.CLOSING)) {
					elapsedTime = 0;
					batch.draw(animation.getKeyFrame(0), x, y);
				}
				else {
					batch.draw(animation.getKeyFrame(animation.animationDuration), x, y);
					if (elapsedTime > animation.animationDuration + 5) { //Hold gate open for 5 seconds
						elapsedTime = 0;
						originalState = OriginalState.CLOSING;
						modifier = Modifier.GATE_CLOSING;
						return modifier;
					}
				}
			}
			originalState = OriginalState.OPEN;
			modifier = Modifier.GATE_OPEN;
		}
		else if (modifier == Modifier.GATE_CLOSING) {
			elapsedTime += Gdx.graphics.getDeltaTime();
			//System.out.println("CLOSING");
			animation.setPlayMode(Animation.NORMAL);
			
			if (!animation.isAnimationFinished(elapsedTime) && originalState != OriginalState.OPEN) {
				batch.draw(animation.getKeyFrame((elapsedTime), false), x, y);
			}
			else {
				if (!animation.isAnimationFinished(elapsedTime) && originalState == OriginalState.OPEN) {
					elapsedTime = animation.animationDuration - elapsedTime;
					batch.draw(animation.getKeyFrame((elapsedTime), false), x, y);
				}
				else if (animation.isAnimationFinished(elapsedTime) && originalState == OriginalState.OPEN) {
					elapsedTime = 0;
					batch.draw(animation.getKeyFrame(0), x, y);
				}
				else {
					batch.draw(animation.getKeyFrame(animation.animationDuration), x, y);
					originalState = OriginalState.CLOSED;
					modifier = Modifier.GATE_CLOSED;
					return modifier;
				}
			}
			originalState = OriginalState.CLOSING;
			modifier = Modifier.GATE_CLOSING;
		}
		return modifier;
	}
	
	@Override
	public void dispose() {
		super.dispose();
		spriteSheet.dispose();
	}
}