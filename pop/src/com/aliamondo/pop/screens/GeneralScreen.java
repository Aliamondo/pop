package com.aliamondo.pop.screens;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Screen;

public abstract class GeneralScreen implements Screen{
	Game game;

	public GeneralScreen(Game game, GeneralScreen screen) {
		this.game = game;
		if (screen != null) screen.dispose();
		screen = null;
	}

	@Override
	public void resize (int width, int height) {
	}

	@Override
	public void show () {
	}

	@Override
	public void hide () {
	}

	@Override
	public void pause () {
	}

	@Override
	public void resume () {
	}

	@Override
	public void render (float time) {
		
	}
	
	@Override
	public void dispose () {
	}
}
