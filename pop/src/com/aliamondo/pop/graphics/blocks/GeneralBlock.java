package com.aliamondo.pop.graphics.blocks;

import com.aliamondo.pop.graphics.Object;
import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public abstract class GeneralBlock extends Object {
	TextureRegion texture;
	Block type;
	
	public final static int FLOOR_OFFSET_LEFT = 40;
	public final static int FLOOR_WIDTH = 90 - FLOOR_OFFSET_LEFT; //115 or 90
	public final static int FLOOR_HEIGHT = 20; //32
	
	public final static int WALL_OR_EMPTY_OFFSET_LEFT = 40;
	public final static int WALL_OR_EMPTY_WIDTH = 90 - FLOOR_OFFSET_LEFT;
	public final static int WALL_OR_EMPTY_HEIGHT = 128;
	
	public final static int BROKEN_BOARD_OFFSET_LEFT = 40;
	public final static int BROKEN_BOARD_WIDTH = 90 - BROKEN_BOARD_OFFSET_LEFT;
	public final static int BROKEN_BOARD_HEIGHT = 27;

	public final static int BLOCK_WIDTH = 64;
	public final static int BLOCK_HEIGHT = 128;
	
	public final static int NEGATIVE_OFFSET_LEFT = -10;
	public final static int NEGATIVE_OFFSET_TOP = 3 * BLOCK_HEIGHT;
	
	public boolean startBreaking = false;
	public boolean canBreak = false;
	public float timeUntilChange = 0;
	
	public int eventNum = -1;
	
	public GeneralBlock(int x, int y, TextureRegion texture, Block type) {
		super(x, y, BLOCK_WIDTH, BLOCK_HEIGHT);
		this.texture = texture;
		this.type = type;
	}
	
	public GeneralBlock(int x, int y, TextureRegion texture, Block type, int offsetX, int width, int height) {
		super(x, y, offsetX, width, height);
		this.texture = texture;
		this.type = type;
	}
	
	public GeneralBlock(int x, int y, Block type) {
		super(x, y, BLOCK_WIDTH, BLOCK_HEIGHT);
		this.type = type;
		this.texture = null;
	}
	
	public GeneralBlock(int x, int y, Block type, int offsetX, int width, int height) {
		super(x, y, offsetX, width, height);
		this.type = type;
		this.texture = null;
	}
	
	public void render(SpriteBatch batch, Modifier modifier) {
		//batch.draw(texture, pos.x, pos.y);
		if (texture != null) {
			//batch.draw(texture, getPos().x * 64, (2 - getPos().y) * 128);
			batch.draw(texture, getPos().x, getPos().y);
		}
	}
	
	public void render(SpriteBatch batch, Modifier modifier, float x, float y) {
		//batch.draw(texture, pos.x, pos.y);
		if (texture != null) {
			//batch.draw(texture, getPos().x * 64, (2 - getPos().y) * 128);
			batch.draw(texture, x, y);
		}
	}
	
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier) {
		if (texture != null) {
			batch.draw(texture, getPos().x, getPos().y);
		}
		return modifier;
	}
	
	public Modifier renderAndReturnModifier(SpriteBatch batch, Modifier modifier, float x, float y) {
		if (texture != null) {
			batch.draw(texture, x, y);
		}
		return modifier;
	}
	
	@Override
	public void render(SpriteBatch batch) {
		//batch.draw(texture, pos.x, pos.y);
		if (texture != null) {
			//batch.draw(texture, getPos().x * 64, (2 - getPos().y) * 128);
			batch.draw(texture, getPos().x, getPos().y);
		}
	}
	
	public void renderAnim(SpriteBatch batch) {
		//Will be overridden
	}

	public void renderAnim(SpriteBatch batch, Modifier modifier) {
		//Will be overridden
	}
	
	public void renderAnim(SpriteBatch batch, Modifier modifier, float x, float y) {
		//Will be overridden
	}
	
	public void renderAnimOnce(SpriteBatch batch, Modifier modifier) {
		//Will be overridden
	}
	
	public void clearAnimationTime() {
		//Will be overridden
	}
	
	public TextureRegion getTexture() {
		return texture;
	}
	
	public Block getType() {
		return type;
	}
	
	public void setBrokenBlockPos(float y) {
		float oldX = getRect().x;
		setPos(oldX, y);
	}
	
	public void startBreaking() {
		if (!startBreaking) {
			this.startBreaking = true;
			this.canBreak = false;
			this.timeUntilChange = 0;
		}
	}
	
	public void update() {
		if (startBreaking) {
			timeUntilChange += Gdx.graphics.getDeltaTime();
		
			if (timeUntilChange >= 1.5f) {
				canBreak = true;
				startBreaking = false;
				timeUntilChange = 0;
			}
		}
	}
	
	@Override
	public void dispose() {
		if (texture != null && texture.getTexture() != null) {
			texture.getTexture().dispose();
		}
	}
}
