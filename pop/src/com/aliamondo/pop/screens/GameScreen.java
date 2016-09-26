package com.aliamondo.pop.screens;

import com.aliamondo.pop.graphics.WorldRenderer;
import com.aliamondo.pop.graphics.characters.Kid.State;
import com.aliamondo.pop.levels.Level;
import com.aliamondo.pop.levels.LevelReader;
import com.badlogic.gdx.Game;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.Input.Keys;
import com.badlogic.gdx.graphics.GL10;

public class GameScreen extends GeneralScreen {
	private WorldRenderer renderer;
	private LevelReader levelReader;
	private Level level;

	private int levelNum = 1;
	boolean backKeyPressed = false;
	boolean levelComplete = false;
	boolean triggerCollisionOverlayKeysPressed = false;
	
	float restartWaitingTime = 0;
	
	public GameScreen (Game game, GeneralScreen screen, int levelNum) {
		super(game, null);
		this.levelNum = levelNum;
		levelReader = new LevelReader();
		level = levelReader.readLevel(levelNum + "");
		renderer = new WorldRenderer(level, this);
	}

	@Override
	public void show() {
	}
	
	public void nextLevel() {
		levelNum++;
		levelComplete = true;
		if (levelNum <= 14) {
			game.setScreen(new LoadingScreen(game, this, levelNum));
		}
		else {
			game.setScreen(new EndScreen(game, this));
		}
	}
	
	public boolean isLevelComplete() {
		return levelComplete;
	}
	
	@Override
	public void render(float delta) {
		Gdx.gl.glClearColor(0f, 0f, 0f, 1);
		Gdx.gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
		renderer.render();
		
		if (levelComplete) return;
		
		/*
		 * CHEATS
		 */
		
		//S + L = Skip Level
		if (Gdx.input.isKeyPressed(Keys.L) && Gdx.input.isKeyPressed(Keys.S) && (Gdx.input.isKeyPressed(Keys.CONTROL_LEFT) || Gdx.input.isKeyPressed(Keys.CONTROL_RIGHT))) {
			nextLevel();
			return;
		}
		
		//E + G = End Game
		if (Gdx.input.isKeyPressed(Keys.E) && Gdx.input.isKeyPressed(Keys.G) && (Gdx.input.isKeyPressed(Keys.CONTROL_LEFT) || Gdx.input.isKeyPressed(Keys.CONTROL_RIGHT))) {
			levelNum = 14;
			nextLevel();
			return;
		}
		
		//C + O = Collision Overlay
		if (Gdx.input.isKeyPressed(Keys.C) && Gdx.input.isKeyPressed(Keys.O) && (Gdx.input.isKeyPressed(Keys.CONTROL_LEFT) || Gdx.input.isKeyPressed(Keys.CONTROL_RIGHT))) {
			triggerCollisionOverlayKeysPressed = true;
			return;
		}
		if (triggerCollisionOverlayKeysPressed && (!Gdx.input.isKeyPressed(Keys.C) || !Gdx.input.isKeyPressed(Keys.O))) {
			renderer.triggerCollisionOverlay();
			triggerCollisionOverlayKeysPressed = false;
		}
		
		/*
		 * CONTROLS
		 */
		
		if (Gdx.input.isKeyPressed(Keys.BACK) || Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			backKeyPressed = true;
		}
		if (backKeyPressed && !Gdx.input.isKeyPressed(Keys.BACK) && !Gdx.input.isKeyPressed(Keys.ESCAPE)) {
			renderer.stopPlayingMusic();
			game.setScreen(new IntroScreen(game, this));
			return;
		}

		if (Gdx.input.isKeyPressed(Keys.R) || Gdx.input.isKeyPressed(Keys.MENU)) {
			renderer.stopPlayingMusic();
			game.setScreen(new LoadingScreen(game, this, levelNum));
			return;
		}
		
		//Controls for kid
		if (renderer.kid.lives > 0) {
			if (renderer.kid.jumpingTime > 0 && renderer.kid.jumpingTime < renderer.kid.jumpingAnimation.animationDuration) {
				renderer.kid.state = State.JUMPING;
			} else if (renderer.kid.clippingUpTime > 0) {
				renderer.kid.state = State.CLIPPING_UP;
			} else if (renderer.kid.clippingDownTime > 0) {
				renderer.kid.state = State.CLIPPING_DOWN;
			} else if (Gdx.input.isKeyPressed(Keys.DOWN)) {
				if (!renderer.kid.getCanClipDownTheBlock()) {
					renderer.kid.state = State.COUCHING;
				}
				else {
					renderer.kid.state = State.CLIPPING_DOWN;
				}
			} else if (Gdx.input.isKeyPressed(Keys.LEFT) && !Gdx.input.isKeyPressed(Keys.SHIFT_LEFT) && !Gdx.input.isKeyPressed(Keys.SHIFT_RIGHT) && !Gdx.input.isKeyPressed(Keys.UP)) {
				renderer.kid.state = State.RUNNING;
				renderer.kid.setLookingLeft(true);
			} else if (Gdx.input.isKeyPressed(Keys.RIGHT) && !Gdx.input.isKeyPressed(Keys.SHIFT_LEFT) && !Gdx.input.isKeyPressed(Keys.SHIFT_RIGHT) && !Gdx.input.isKeyPressed(Keys.UP)) {
				renderer.kid.state = State.RUNNING;
				renderer.kid.setLookingLeft(false);
			} else if (Gdx.input.isKeyPressed(Keys.UP) && !Gdx.input.isKeyPressed(Keys.LEFT) && !Gdx.input.isKeyPressed(Keys.RIGHT)) {
				if (!renderer.kid.getCanClipOnBlock()) {
					renderer.kid.state = State.SCALING;
				}
				else {
					renderer.kid.state = State.CLIPPING_UP;
				}
			} else if (Gdx.input.isKeyPressed(Keys.UP) && Gdx.input.isKeyPressed(Keys.LEFT)) {
				renderer.kid.state = State.RUN_JUMPING;
				renderer.kid.setLookingLeft(true);
			} else if (Gdx.input.isKeyPressed(Keys.UP) && Gdx.input.isKeyPressed(Keys.RIGHT)) {
				renderer.kid.state = State.RUN_JUMPING;
				renderer.kid.setLookingLeft(false);
			} else if (Gdx.input.isKeyPressed(Keys.LEFT) && (Gdx.input.isKeyPressed(Keys.SHIFT_LEFT) || Gdx.input.isKeyPressed(Keys.SHIFT_RIGHT))) {
				renderer.kid.state = State.WALKING;
				renderer.kid.setLookingLeft(true);
			} else if (Gdx.input.isKeyPressed(Keys.RIGHT) && (Gdx.input.isKeyPressed(Keys.SHIFT_LEFT) || Gdx.input.isKeyPressed(Keys.SHIFT_RIGHT))) {
				renderer.kid.state = State.WALKING;
				renderer.kid.setLookingLeft(false);
				//renderer.kid.checkAnimationComplete(renderer.kid.runningAnimation);
			} else if (Gdx.input.isKeyPressed(Keys.SHIFT_LEFT) || Gdx.input.isKeyPressed(Keys.SHIFT_RIGHT)) {
				if (renderer.kid.getCanPickSword() || renderer.kid.getCanPickPotion()) {
					renderer.kid.state = State.PICKING_ITEM;
				}
			} else if (renderer.kid.state != State.FALLING) {
				renderer.kid.state = State.STANDING;
			}
		}
		else { //If kid is dead and a key is pressed, we need to restart
			restartWaitingTime += Gdx.graphics.getDeltaTime();
			if (Gdx.input.isKeyPressed(Keys.ANY_KEY) && restartWaitingTime > 2f) {
				renderer.stopPlayingMusic();
				restartWaitingTime = 0;
				game.setScreen(new LoadingScreen(game, this, levelNum));
				return;
			}
		}
	}

	public Game getGame() {
		return game;
	}
	
	@Override
	public void dispose() {
		renderer.dispose();
		levelReader = null;
		level = null;
	}
}