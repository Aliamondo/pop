package com.aliamondo.pop.screens;

import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.audio.Music;
import com.badlogic.gdx.graphics.GL10;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;
import com.badlogic.gdx.graphics.g2d.TextureAtlas.AtlasRegion;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class EndScreen extends GeneralScreen {

	SpriteBatch batch = null;
	TextureAtlas spriteSheet = null;
	AtlasRegion textBackground = null;

	TextureRegion gameEnd = null;

	Music endMusic = null;

	float runningTime = 0;
	boolean backKeyPressed = false;
	boolean enterPressed = false;

	public EndScreen (Game game, GeneralScreen screen) {
		super(game, screen);
	}

	@Override
	public void show () {
		Gdx.input.setCatchBackKey(true);

		spriteSheet = new TextureAtlas(Gdx.files.internal("intro/intro.txt"));
		textBackground = spriteSheet.findRegion("text background");

		gameEnd = new TextureRegion(new Texture(Gdx.files.internal("intro/end.png")), 264, 134);

		batch = new SpriteBatch();
		batch.getProjectionMatrix().setToOrtho2D(0, 0, 320, 200);
		
		endMusic = Gdx.audio.newMusic(Gdx.files.internal("music/end.mp3"));
		endMusic.setVolume(0.3f);
	}

	@Override
	public void render (float delta) {
		Gdx.gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
		Gdx.gl.glClearColor(100 / 255f, 0, 0, 1);

		runningTime += delta;

		batch.begin();

		batch.draw(textBackground, 0, 0);
		batch.draw(gameEnd, (320 - 264)/2, (200 - 134) / 2);
		
		if (!endMusic.isPlaying()) endMusic.play();
		
		batch.end();

		if (Gdx.input.justTouched()) {
			//			float x = Gdx.input.getX();
			//			float y = Gdx.input.getY();
			Gdx.input.vibrate(100);
			Gdx.app.exit();
		}

		if (Gdx.input.isKeyPressed(Keys.ENTER)) {
			enterPressed = true;
		}
		if (Gdx.input.isKeyPressed(Keys.BACK) || Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			backKeyPressed = true;
		}

		if (enterPressed && !Gdx.input.isKeyPressed(Keys.ENTER)) {
			Gdx.app.exit();
		}
		if (backKeyPressed && !Gdx.input.isKeyPressed(Keys.BACK) && !Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			Gdx.app.exit();
		}
	}

	public void dispose() {
		spriteSheet.dispose();
		textBackground.getTexture().dispose();

		gameEnd.getTexture().dispose();
		
		endMusic.stop();
		endMusic.dispose();

		batch.dispose();
	}
}
