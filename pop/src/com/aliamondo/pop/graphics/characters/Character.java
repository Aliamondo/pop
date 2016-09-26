package com.aliamondo.pop.graphics.characters;

import com.aliamondo.pop.graphics.Object;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;

public abstract class Character extends Object{
	boolean lookingLeft = false;
	
	public Character(int x, int y, int width, int height) {
		super(x, y, width, height);
	}

	public void render(SpriteBatch batch) {
		// No need to do anything
		
	}
}
