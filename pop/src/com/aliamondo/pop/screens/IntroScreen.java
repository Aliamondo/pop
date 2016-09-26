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

public class IntroScreen extends GeneralScreen {

	SpriteBatch batch = null;
	TextureAtlas spriteSheet = null;
	AtlasRegion introBackground = null;
	AtlasRegion gameCopyright = null;
	AtlasRegion textBackground = null;

	TextureRegion gameName = null;

	Music introMusic = null;

	float runningTime = 0;
	boolean backKeyPressed = false;
	boolean enterPressed = false;

	final float FADE_IN_TIME = 1.25f;

	public IntroScreen (Game game, GeneralScreen screen) {
		super(game, screen);
	}

	@Override
	public void show () {
		Gdx.input.setCatchBackKey(true);

		spriteSheet = new TextureAtlas(Gdx.files.internal("intro/intro.txt"));
		introBackground = spriteSheet.findRegion("main background");
		gameCopyright = spriteSheet.findRegion("copyright");
		textBackground = spriteSheet.findRegion("text background");

		gameName = new TextureRegion(new Texture(Gdx.files.internal("intro/game name edited.png")), 267, 65);

		batch = new SpriteBatch();
		batch.getProjectionMatrix().setToOrtho2D(0, 0, 320, 200);

		introMusic = Gdx.audio.newMusic(Gdx.files.internal("music/intro.mp3"));
	}

	@Override
	public void render (float delta) {
		Gdx.gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
		Gdx.gl.glClearColor(255, 255, 255, 1);

		runningTime += delta;

		batch.begin();

		if (runningTime < FADE_IN_TIME) {
			batch.setColor(runningTime/FADE_IN_TIME, runningTime/FADE_IN_TIME, runningTime/FADE_IN_TIME, 1f);
		} else {
			if (!introMusic.isPlaying()) introMusic.play();
		}

		batch.draw(introBackground, 0, 0);

		if (runningTime > 3.65) {
			batch.draw(gameName, (320 - 267)/2, 30);
			batch.draw(gameCopyright, (320 - 224)/2, 4);
		}

		batch.end();

		if (Gdx.input.justTouched()) {
			//			float x = Gdx.input.getX();
			//			float y = Gdx.input.getY();
			Gdx.input.vibrate(100);
			game.setScreen(new LoadingScreen(game, this, 1));
		}

		if (Gdx.input.isKeyPressed(Keys.ENTER)) {
			enterPressed = true;
		}
		if (Gdx.input.isKeyPressed(Keys.BACK) || Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			backKeyPressed = true;
		}

		if (enterPressed && !Gdx.input.isKeyPressed(Keys.ENTER)) {
			game.setScreen(new LoadingScreen(game, this, 1));
		}
		if (backKeyPressed && !Gdx.input.isKeyPressed(Keys.BACK) && !Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			Gdx.app.exit();
		}
	}

	public void dispose() {
		spriteSheet.dispose();
		introBackground.getTexture().dispose();
		gameCopyright.getTexture().dispose();
		textBackground.getTexture().dispose();

		gameName.getTexture().dispose();

		introMusic.stop();
		introMusic.dispose();
		batch.dispose();
	}
}
