package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class Potion extends GeneralBlock{
	TextureRegion emptyPotionTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/potion_empty.png")), 120, 155);
	TextureRegion healthPotionTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/potion_health.png")), 120, 155);
	TextureRegion lifePotionTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/potion_life.png")), 120, 155);
	TextureRegion featherFallPotionTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/potion_feather_fall.png")), 120, 155);
	TextureRegion poisonPotionTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/potion_poison.png")), 120, 155);
	
	public Potion(int x, int y) {
		super(x, y, Block.POTION, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);
	}
	
	@Override
	public void render(SpriteBatch batch, Modifier modifier) {
		if (modifier == Modifier.POTION_EMPTY) {
			batch.draw(emptyPotionTexture, getPos().x, getPos().y);
		} else if (modifier == Modifier.POTION_HEALTH) {
			batch.draw(healthPotionTexture, getPos().x, getPos().y);
		} else if (modifier == Modifier.POTION_LIFE) {
			batch.draw(lifePotionTexture, getPos().x, getPos().y);
		} else if (modifier == Modifier.POTION_FEATHER_FALL) {
			batch.draw(featherFallPotionTexture, getPos().x, getPos().y);
		} else if (modifier == Modifier.POTION_POISON) {
			batch.draw(poisonPotionTexture, getPos().x, getPos().y);
		} else if (modifier == Modifier.POTION_INVERT) {
			batch.draw(featherFallPotionTexture, getPos().x, getPos().y);
		} else if (modifier == Modifier.POTION_OPEN) {
			batch.draw(poisonPotionTexture, getPos().x, getPos().y);
		}
	}
	
	@Override
	public void render(SpriteBatch batch, Modifier modifier, float x, float y) {
		if (modifier == Modifier.POTION_EMPTY) {
			batch.draw(emptyPotionTexture, x, y);
		} else if (modifier == Modifier.POTION_HEALTH) {
			batch.draw(healthPotionTexture, x, y);
		} else if (modifier == Modifier.POTION_LIFE) {
			batch.draw(lifePotionTexture, x, y);
		} else if (modifier == Modifier.POTION_FEATHER_FALL) {
			batch.draw(featherFallPotionTexture, x, y);
		} else if (modifier == Modifier.POTION_POISON) {
			batch.draw(poisonPotionTexture, x, y);
		} else if (modifier == Modifier.POTION_INVERT) {
			batch.draw(featherFallPotionTexture, x, y);
		} else if (modifier == Modifier.POTION_OPEN) {
			batch.draw(poisonPotionTexture, x, y);
		}
	}
	
	@Override
	public void dispose() {
		emptyPotionTexture.getTexture().dispose();
		healthPotionTexture.getTexture().dispose();
		lifePotionTexture.getTexture().dispose();
		featherFallPotionTexture.getTexture().dispose();
		poisonPotionTexture.getTexture().dispose();
	}
}