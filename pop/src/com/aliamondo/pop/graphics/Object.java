package com.aliamondo.pop.graphics;

import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.math.Rectangle;
import com.badlogic.gdx.math.Vector2;

public abstract class Object {
	Vector2 pos;
	Rectangle rect;

	public Object(int x, int y, int width, int height) {
		this.pos = new Vector2(x * 64, (2 - y) * 128);
		this.rect = new Rectangle(this.pos.x, this.pos.y, width, height);
	}
	
	public Object(int x, int y, int offsetX, int width, int height) {
		this.pos = new Vector2(x * 64, (2 - y) * 128);
		this.rect = new Rectangle(this.pos.x + offsetX, this.pos.y, width, height);
	}

	/**
	 * Currently empty as Object itself doesn't need to render anything
	 * Override it
	 */
	public void render(SpriteBatch batch) {
		//TO DO
	}

	/**
	 * @return <b>pos</b> - position of the object
	 */
	public Vector2 getPos() {
		return pos;
	}

	/**
	 * @return <b>rect</b> - rectangle of the object
	 */
	public Rectangle getRect() {
		return rect;
	}

	/**
	 * Set object's rectangle dimensions
	 */
	public void setRectDimensions(int width, int height) {
		this.rect.setWidth(width);
		this.rect.setHeight(height);
	}
	
	/*
	 * Set object's rectangle position
	 */
	public void setRectPos(float x, float y) {
		this.rect.x = x;
		this.rect.y = y;
	}

	/**
	 * Set position of the object
	 * @param
	 */
	public void setPos(Vector2 pos) {
		this.pos = pos;
		this.rect.x = pos.x;
		this.rect.y = pos.y;
	}

	/**
	 * Set position of the object using given coordinates
	 * @param
	 */
	public void setPos(float x, float y) {
		this.pos = new Vector2(x, y);
		this.rect.x = x;
		this.rect.y = y;
	}

	/**
	 * Currently empty as Object itself isn't necessarily having any textures.
	 * Override it
	 */
	public void dispose() {
		//TO-DO
	}
}
