package com.aliamondo.pop.screens;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.GL10;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.utils.Timer;
import com.badlogic.gdx.utils.Timer.Task;

public class LoadingScreen extends GeneralScreen {

	SpriteBatch batch = null;
	TextureRegion background;
	
	GameScreen gameScreen;
	
	float time = 0;
	int levelNum = 1;
	
	public LoadingScreen (Game game, GeneralScreen screen, int levelNum) {
		super(game, screen);
		this.levelNum = levelNum;
		background = new TextureRegion(new Texture(Gdx.files.internal("sprites/other/loading.png")), 860, 480);
	}

	@Override
	public void show () {
		Gdx.input.setCatchBackKey(true);

		batch = new SpriteBatch();
		batch.getProjectionMatrix().setToOrtho2D(0, 0, 860, 480);
		
		float delay = 0.25f; // seconds

		Timer.schedule(new Task(){
		    @Override
		    public void run() {
		    	gameScreen = new GameScreen(game, null, levelNum);
		    }
		}, delay);
	}

	@Override
	public void render(float delta) {
		
		Gdx.gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
		
		time += Gdx.graphics.getDeltaTime();
		
		batch.begin();
		
		batch.enableBlending();
		
		batch.draw(background, 0, 0);

		batch.end();
		
		if (gameScreen != null) {
			game.setScreen(gameScreen);
			this.dispose();
		}
	}

	public void dispose() {
		background.getTexture().dispose();
		batch.dispose();
	}
}
