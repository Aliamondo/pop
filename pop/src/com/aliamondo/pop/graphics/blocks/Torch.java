package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class Torch extends GeneralBlock{
	public enum Type {
		DEFAULT, TORCH_WITH_DEBRIS
	}

	Animation animation;
	float elapsedTime = 0;
	Type type = Type.DEFAULT;
	TextureRegion debris = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/debris.png")), 120, 155);
	TextureAtlas spriteSheet;
	
	public Torch(Type type, int x, int y) {
		super(x, y, Block.TORCH, FLOOR_OFFSET_LEFT, FLOOR_WIDTH, FLOOR_HEIGHT);

		this.type = type;
		spriteSheet = new TextureAtlas(Gdx.files.internal("sprites/dungeon/torch.txt"));

		animation = new Animation(1/15f, spriteSheet.getRegions());
	}
	
	@Override
	public void render(SpriteBatch batch, Modifier modifier) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		batch.draw(animation.getKeyFrame(elapsedTime, true), getPos().x, getPos().y);
		if (type == Type.TORCH_WITH_DEBRIS) {
			batch.draw(debris, getPos().x, getPos().y);
		}
	}
	
	@Override
	public void render(SpriteBatch batch, Modifier modifier, float x, float y) {
		elapsedTime += Gdx.graphics.getDeltaTime();
		batch.draw(animation.getKeyFrame(elapsedTime, true), x, y);
		if (type == Type.TORCH_WITH_DEBRIS) {
			batch.draw(debris, x, y);
		}
	}
	
	@Override
	public void dispose() {
		spriteSheet.dispose();
		debris.getTexture().dispose();
	}
}