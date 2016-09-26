package com.aliamondo.pop.graphics;

import java.util.Iterator;
import java.util.List;

import com.aliamondo.pop.graphics.blocks.BrokenBoard;
import com.aliamondo.pop.graphics.blocks.Debris;
import com.aliamondo.pop.graphics.blocks.Empty;
import com.aliamondo.pop.graphics.blocks.Floor;
import com.aliamondo.pop.graphics.blocks.GeneralBlock;
import com.aliamondo.pop.graphics.blocks.Torch;
import com.aliamondo.pop.graphics.blocks.Torch.Type;
import com.aliamondo.pop.graphics.characters.Kid;
import com.aliamondo.pop.graphics.characters.Kid.DeathType;
import com.aliamondo.pop.graphics.characters.Kid.State;
import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Event;
import com.aliamondo.pop.levels.Level;
import com.aliamondo.pop.levels.LevelScreen;
import com.aliamondo.pop.levels.Modifier;
import com.aliamondo.pop.screens.GameScreen;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.audio.Music;
import com.badlogic.gdx.graphics.GL10;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.math.Intersector;
import com.badlogic.gdx.math.Rectangle;

public class WorldRenderer {
	private SpriteBatch batch;
	//private OrthographicCamera cam;
	private LevelRenderer levelRenderer;
	private GameScreen gameScreen;

	public Kid kid;
	private boolean firstRun = true;
	private boolean gameOver = false;
	private boolean gameOverSoundPlayed = false;
	private boolean taricTauntSoundPlayed = false;
	
	TextureRegion gameOverTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/other/game_over.png")), 0, 76, 512, 104);
	TextureRegion liveFullTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/other/live_full.png")), 12, 10);
	TextureRegion liveEmptyTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/other/live_empty.png")), 12, 10);
	
	Music pickingSwordMusic = Gdx.audio.newMusic(Gdx.files.internal("music/darude.mp3"));
	Music backgroundMusic = Gdx.audio.newMusic(Gdx.files.internal("music/background.mp3"));
	Music deathMusic = Gdx.audio.newMusic(Gdx.files.internal("music/death.mp3"));
	Music taricTauntMusic = Gdx.audio.newMusic(Gdx.files.internal("music/taric_taunt.mp3"));
	
	float backgroundMusicVolume = 0.5f;
	
	public WorldRenderer(Level level, GameScreen screen) {
		this.kid = new Kid(0, 0, 0, this);
		this.gameScreen = screen;
		
		levelRenderer = new LevelRenderer(level, this);
		
		closeAllGates();
		
		this.batch = new SpriteBatch();
		batch.getProjectionMatrix().setToOrtho2D(0, 0, 640, 384);

		backgroundMusic.setLooping(true);
		backgroundMusic.setVolume(backgroundMusicVolume);
		backgroundMusic.play();
		//this.cam = new OrthographicCamera(640, 384);
		//this.cam.position.set(800/2, 480/2, 0);

	}
	
	public void playPickingSwordMusic() {
		backgroundMusicVolume = 0f;
		backgroundMusic.setVolume(backgroundMusicVolume);
		pickingSwordMusic.setVolume(0.25f);
		pickingSwordMusic.play();
	}
	
	public void stopPlayingMusic() {
		pickingSwordMusic.stop();
		backgroundMusic.stop();
		deathMusic.stop();
		taricTauntMusic.stop();
	}
	
	public boolean checkIfInAir() {
		//if (kid.state == State.RUN_JUMPING || kid.state == State.JUMPING) return false;
		List<GeneralBlock> blocks = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks;
		boolean noCollisions = true;
		
		List<GeneralBlock> blocksLeft = null;
		
		if (levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft != null) {
			blocksLeft = levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft.blocks;
		}
		
		for (int i = 0; i <= 2; i++) {
			
			if (blocksLeft != null) {
				GeneralBlock blockLeft = blocksLeft.get(i * 10 + 9);
				Rectangle blockLeftRectangle = new Rectangle(GeneralBlock.NEGATIVE_OFFSET_LEFT, blockLeft.getPos().y, GeneralBlock.FLOOR_WIDTH, GeneralBlock.FLOOR_HEIGHT);
				
				if (Intersector.intersectRectangles(kid.getRect(), blockLeftRectangle) || Intersector.overlapRectangles(kid.getRect(), blockLeftRectangle)) {
					if (blockLeft.getType() != Block.EMPTY && blockLeft.getType() != Block.UNKNOWN) {
						noCollisions = false;
					}
				}
			}
			
			for (int j = 0; j <= 9; j++) {
				GeneralBlock block = blocks.get(i * 10 + j);

				if (Intersector.intersectRectangles(kid.getRect(), block.getRect()) || Intersector.overlapRectangles(kid.getRect(), block.getRect())) {
					if (block.getType() != Block.EMPTY && block.getType() != Block.UNKNOWN && block.getType() != Block.WALL) {
						noCollisions = false;
					}
				}
			}
		}
		return noCollisions;
	}
	
	public void checkGravity() {
		if (kid.state == State.CLIPPING_DOWN) return;
		List<GeneralBlock> blocks = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks;
		List<Modifier> modifiers = levelRenderer.level.screens.get(levelRenderer.getScreenId()).modifiers;
		boolean noCollisions = true;
		//System.out.println("kid: " + kid.getPos().toString());
		
		List<GeneralBlock> blocksLeft = null;
		List<Modifier> modifiersLeft = null;
		
		if (levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft != null) {
			blocksLeft = levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft.blocks;
			modifiersLeft = levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft.modifiers;
		}
		
		for (int i = 0; i <= 2; i++) {
			
			//Left room check
			if (blocksLeft != null) {
				GeneralBlock blockLeft = blocksLeft.get(i * 10 + 9);
				Modifier modifierLeft = modifiersLeft.get(i * 10 + 9);
				Rectangle blockLeftRectangle = new Rectangle(GeneralBlock.NEGATIVE_OFFSET_LEFT, blockLeft.getPos().y, GeneralBlock.FLOOR_WIDTH, GeneralBlock.FLOOR_HEIGHT);
				
				// Update block breaking
				blockLeft.update();
				if (blockLeft.canBreak) {
					blockLeft.dispose();
					blocksLeft.set(i * 10 + 9, new Empty(9, i));
					levelRenderer.dropBrokenBoard(batch, 9, i, levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft.id - 1);
				}
				
				if (Intersector.intersectRectangles(kid.getRect(), blockLeftRectangle) || Intersector.overlapRectangles(kid.getRect(), blockLeftRectangle)) {
					if (blockLeft.getType() != Block.EMPTY && blockLeft.getType() != Block.UNKNOWN && blockLeft.getType() != Block.WALL) {
						if (blockLeft.getPos().y <= kid.getPos().y) {
							kid.setPos(kid.getPos().x, blockLeft.getPos().y + GeneralBlock.FLOOR_HEIGHT);
							
							if (blockLeft.getType() == Block.POTION && (kid.state == State.STANDING || kid.state == State.COUCHING)) {
								kid.setCanPickPotion(i, -1, modifierLeft);
							}
							else if (blockLeft.getType() == Block.SPIKES) {
								if (modifierLeft != Modifier.SPIKE_DISABLED && modifierLeft != Modifier.SPIKE_NORMAL && kid.state != State.WALKING && kid.state != State.STANDING && kid.state != State.JUMPING) {
									kid.lives = 0;
									kid.deathType = DeathType.SPIKED;
									renderHittingFlash();
								}
							}
							else if (blockLeft.getType() == Block.RAISE_BUTTON && kid.state != State.JUMPING && kid.state != State.RUN_JUMPING && (kid.jumpingTime == 0 || kid.jumpingTime > kid.jumpingAnimation.animationDuration)) {
								if (blockLeft.eventNum >= 0) {
									int currentEventNum = blockLeft.eventNum;
									Event event;
									do {
										event = levelRenderer.level.events.get(currentEventNum);
										
										if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.GATE) {
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_OPEN);
										}
										else if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.EXIT_LEFT) {
											// Set both exits to be open
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_OPEN);
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber + 1, Modifier.GATE_OPEN);
										}
										currentEventNum++;
									}
									while(event.triggersNextEvent);
								}
								modifiersLeft.set(i * 10 + 9, Modifier.BUTTON_PRESSED);
							}
							else if (blockLeft.getType() == Block.DROP_BUTTON && kid.state != State.JUMPING && kid.state != State.RUN_JUMPING && (kid.jumpingTime == 0 || kid.jumpingTime > kid.jumpingAnimation.animationDuration)) {
								if (blockLeft.eventNum >= 0) {
									int currentEventNum = blockLeft.eventNum;
									Event event;
									do {
										event = levelRenderer.level.events.get(currentEventNum);
										
										if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.GATE && (levelRenderer.level.screens.get(event.screenNumber).modifiers.get(event.blockNumber) == Modifier.GATE_OPEN || levelRenderer.level.screens.get(event.screenNumber).modifiers.get(event.blockNumber) == Modifier.GATE_ALWAYS_OPEN)) {
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_CLOSING);
										}
										currentEventNum++;
									}
									while(event.triggersNextEvent);
								}
								modifiersLeft.set(i * 10 + 9, Modifier.BUTTON_PRESSED);
							}
							
							if (blockLeft.getType() == Block.LOOSE_BOARD && kid.state != State.JUMPING && kid.state != State.RUN_JUMPING && (kid.jumpingTime == 0 || kid.jumpingTime > kid.jumpingAnimation.animationDuration)) {
								blockLeft.startBreaking();
							}
							
							kid.state = State.STANDING;
							kid.getHitByFalling();
							noCollisions = false;
							//System.out.println("Left Screen gravity: " + blockLeft.getType());
						}
					}
				}
				else {
					/*
					 * Release the buttons that kid isn't standing on
					 */
					if (blockLeft.getType() == Block.RAISE_BUTTON || blockLeft.getType() == Block.DROP_BUTTON){
						modifiersLeft.set(i * 10 + 9, Modifier.BUTTON_NOT_PRESSED);
					}
				}
			}
			
			for (int j = 0; j <= 9; j++) {
				GeneralBlock block = blocks.get(i * 10 + j);
				Modifier modifier = modifiers.get(i * 10 + j);
				
				// Update block breaking
				block.update();
				if (block.canBreak) {
					block.dispose();
					blocks.set(i * 10 + j, new Empty(j, i));
					levelRenderer.dropBrokenBoard(batch, j, i, levelRenderer.getScreenId());
				}

				if (Intersector.intersectRectangles(kid.getRect(), block.getRect()) || Intersector.overlapRectangles(kid.getRect(), block.getRect())) {
					if (block.getType() != Block.EMPTY && block.getType() != Block.UNKNOWN && block.getType() != Block.WALL) {
						if (block.getPos().y <= kid.getPos().y) {
							kid.setPos(kid.getPos().x, block.getPos().y + GeneralBlock.FLOOR_HEIGHT);
							
							if (block.getType() == Block.SWORD && (kid.state == State.STANDING || kid.state == State.COUCHING)) {
								kid.setCanPickSword(i, j);
							}
							else if (block.getType() == Block.POTION && (kid.state == State.STANDING || kid.state == State.COUCHING)) {
								kid.setCanPickPotion(i, j, modifier);
							}
							else if (block.getType() == Block.SPIKES) {
								if (modifier != Modifier.SPIKE_DISABLED && modifier != Modifier.SPIKE_NORMAL && kid.state != State.WALKING && kid.state != State.STANDING && kid.state != State.JUMPING) {
									kid.lives = 0;
									kid.deathType = DeathType.SPIKED;
									renderHittingFlash();
								}
							}
							else if (block.getType() == Block.CHOPPER) {
								if (modifier != Modifier.CHOMP_STUCK_OPEN && modifier != Modifier.CHOMP_NORMAL && kid.getPos().x < block.getRect().x - 10) {
									kid.lives = 0;
									kid.deathType = DeathType.CHOPPED;
									renderHittingFlash();
								}
							}
							else if ((block.getType() == Block.EXIT_LEFT || block.getType() == Block.EXIT_RIGHT) && modifier == Modifier.GATE_OPEN) {
								if (kid.state == State.SCALING) {
									gameScreen.nextLevel();
									return;
								}
							}
							else if (block.getType() == Block.RAISE_BUTTON && kid.state != State.JUMPING && kid.state != State.RUN_JUMPING && (kid.jumpingTime == 0 || kid.jumpingTime > kid.jumpingAnimation.animationDuration)) {
								if (block.eventNum >= 0) {
									int currentEventNum = block.eventNum;
									Event event;
									do {
										event = levelRenderer.level.events.get(currentEventNum);
										
										if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.GATE) {
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_OPEN);
										}
										else if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.EXIT_LEFT) {
											// Set both exits to be open
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_OPEN);
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber + 1, Modifier.GATE_OPEN);
										}
										currentEventNum++;
									}
									while(event.triggersNextEvent);
								}
								modifiers.set(i * 10 + j, Modifier.BUTTON_PRESSED);
							}
							else if (block.getType() == Block.DROP_BUTTON && kid.state != State.JUMPING && kid.state != State.RUN_JUMPING && (kid.jumpingTime == 0 || kid.jumpingTime > kid.jumpingAnimation.animationDuration)) {
								if (block.eventNum >= 0) {
									int currentEventNum = block.eventNum;
									Event event;
									do {
										event = levelRenderer.level.events.get(currentEventNum);
										
										if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.GATE && (levelRenderer.level.screens.get(event.screenNumber).modifiers.get(event.blockNumber) == Modifier.GATE_OPEN || levelRenderer.level.screens.get(event.screenNumber).modifiers.get(event.blockNumber) == Modifier.GATE_ALWAYS_OPEN)) {
											levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_CLOSING);
										}
										currentEventNum++;
									}
									while(event.triggersNextEvent);
								}
								modifiers.set(i * 10 + j, Modifier.BUTTON_PRESSED);
							}
							
							if (block.getType() == Block.LOOSE_BOARD && kid.state != State.JUMPING && kid.state != State.RUN_JUMPING && (kid.jumpingTime == 0 || kid.jumpingTime > kid.jumpingAnimation.animationDuration)) {
								block.startBreaking();
							}
							
							kid.state = State.STANDING;
							kid.getHitByFalling();
							noCollisions = false;
							
							//System.out.println("Standing on " + block.getType());
						}
					}
//					else if (block.getType() == Block.WALL) {
//						if (block.getPos().y <= kid.getPos().y) {
//							if (block.getPos().y + GeneralBlock.FLOOR_HEIGHT != kid.getPos().y) {
//								kid.setPos(kid.getPos().x, block.getPos().y + GeneralBlock.WALL_OR_EMPTY_HEIGHT);
//								kid.state = State.STANDING;
//								noCollisions = false;
//							}
//						}
//					}
				}
				else {
					/*
					 * Release the buttons if they are not colliding with kid
					 */
					if (block.getType() == Block.RAISE_BUTTON || block.getType() == Block.DROP_BUTTON) {
						modifiers.set(i * 10 + j, Modifier.BUTTON_NOT_PRESSED);
					}
				}
			}
		}
		if (noCollisions) {
			//System.out.println("NO COLLISIONS");
			kid.state = State.FALLING;
		}
	}
	
	public void checkCollisions() {
		//System.out.println("Kid pos: " + kid.getPos().toString());
		if (kid.getPos().x < GeneralBlock.NEGATIVE_OFFSET_LEFT) {
			moveToLeftLinkRoom();
		}
		if (kid.getPos().x > levelRenderer.level.screens.get(levelRenderer.screenId).width * 64 - 1) {
			moveToRightLinkRoom();
		}
		if (kid.getPos().y < -60) {
			moveToBottomLinkRoom();
		}
		if (kid.getPos().y > 3 * 128 - 1) {
			moveToTopLinkRoom();
		}
		
		List<GeneralBlock> blocks = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks;
		//boolean noCollisions = true;
		for (int i = 0; i <= 2; i++) {
			for (int j = 0; j <= 9; j++) {
				GeneralBlock block = blocks.get(i * 10 + j);
				
				// Update block breaking
				block.update();
				if (block.canBreak) {
					block.dispose();
					blocks.set(i * 10 + j, new Empty(j, i));
					levelRenderer.dropBrokenBoard(batch, j, i, levelRenderer.getScreenId());
				}
				
				/*
				 * FINDING A BLOCK TO CLIP ON
				 */
				if (kid.state == State.SCALING && block.getType() != Block.EMPTY && block.getType() != Block.UNKNOWN && block.getType() != Block.WALL) {
					if (kid.isLookingLeft()) {
						if (kid.getPos().x - (block.getRect().x + block.getRect().width) < 10 && kid.getPos().x - block.getRect().x > 10 && Math.abs(kid.getPos().y + kid.getRect().height - (block.getRect().y + block.getRect().height)) < 1) {
							GeneralBlock neighborBlock = null;
							if (j < 9) {//Can't touch rightmost block while looking left
								neighborBlock = blocks.get(i * 10 + j + 1);
							}
							if (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN) {
								kid.setCanClipOnBlock(block);
							}
						}
					}
					else {
						if (block.getRect().x - kid.getPos().x < 45 && block.getRect().x - kid.getPos().x > 5 && Math.abs(kid.getPos().y + kid.getRect().height - (block.getRect().y + block.getRect().height)) < 1) {
							GeneralBlock neighborBlock = null;
							if (j > 0) {//Can't touch leftmost block while looking right
								neighborBlock = blocks.get(i * 10 + j - 1);
							}
							else if (j == 0) {//Check room on the left
								neighborBlock = levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft.blocks.get(i * 10 + 9);
							}
							
							//System.out.println("---------------------------------\n" + block.getType() + "\n" + "LEFT: " + neighborBlock + "\n---------------------------------");
							
							if (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN) {
								kid.setCanClipOnBlock(block);
							}
						}
					}
					//System.out.println("kid: [" + kid.getPos().x + ", "+ (kid.getPos().y + kid.getRect().height) + "], block: [" + (block.getRect().x + block.getRect().width) + ", " + (block.getRect().y + block.getRect().height) + "]");
				}
				
				/*
				 * COLLISION CHECKS
				 */
				if (Intersector.intersectRectangles(kid.getRect(), block.getRect()) || Intersector.overlapRectangles(kid.getRect(), block.getRect())) {
					
					/*
					 * COLLISION CHECK WHEN SCALING
					 */
					
					if (kid.state == State.SCALING && block.getType() != Block.EMPTY && block.getType() != Block.UNKNOWN && block.getType() != Block.WALL && block.getType() != Block.LOOSE_BOARD) {
						if (block.getRect().y > kid.getPos().y) {
							kid.setPos(kid.getPos().x, block.getRect().y + block.getRect().height / 2 - kid.getRect().height);
						}
					}
					else if (kid.state == State.SCALING && block.getType() == Block.WALL) {
						if (block.getRect().y > kid.getPos().y) {
							kid.setPos(kid.getPos().x, block.getRect().y + block.getRect().height / 12 - kid.getRect().height);
						}
					}
					else if (kid.state == State.SCALING && block.getType() == Block.LOOSE_BOARD) {
						if (block.getRect().y > kid.getPos().y) {
							kid.setPos(kid.getPos().x, block.getRect().y + block.getRect().height / 2 - kid.getRect().height);
							block.startBreaking();
						}
					}
					
					/*
					 * COLLISION CHECK ON SAME HEIGHT
					 */
					Modifier modifier = levelRenderer.level.screens.get(levelRenderer.getScreenId()).modifiers.get(i * 10 + j);
					if (block.getType() == Block.WALL || (block.getType() == Block.GATE && (modifier == Modifier.GATE_CLOSED || modifier == Modifier.UNKNOWN))) {
						if (!checkIfInAir() && block.getPos().y <= kid.getPos().y) {
							if (kid.isLookingLeft() && block.getRect().x < kid.getPos().x) {
								//kid.setPos(block.getPos().x + 1 + block.getRect().getWidth() + GeneralBlock.WALL_OR_EMPTY_OFFSET_LEFT, kid.getPos().y);
								kid.setPos(block.getRect().x + 1 + block.getRect().getWidth(), kid.getPos().y);
							}
							else if (!kid.isLookingLeft() && block.getRect().x >= kid.getPos().x) {
								kid.setPos(block.getPos().x - 1, kid.getPos().y);
							}
						}
						else if (kid.state == State.JUMPING || kid.state == State.RUN_JUMPING) {
							if (kid.isLookingLeft() && block.getRect().x < kid.getPos().x) {
								//kid.setPos(block.getPos().x + 1 + block.getRect().getWidth() + GeneralBlock.WALL_OR_EMPTY_OFFSET_LEFT, kid.getPos().y);
								kid.setPos(block.getRect().x + 1 + block.getRect().getWidth(), kid.getPos().y);
							}
							else if (!kid.isLookingLeft() && block.getRect().x >= kid.getPos().x) {
								kid.setPos(block.getPos().x - 1, kid.getPos().y);
							}
						}
						else if (checkIfInAir()) {
							kid.state = State.FALLING;
						}
					}
					
					/*
					 * FINDING A BLOCK TO CLIP DOWN FROM
					 */
					if (kid.state == State.STANDING && block.getType() != Block.EMPTY && block.getType() != Block.UNKNOWN && block.getType() != Block.WALL) {
						if (kid.isLookingLeft()) {
							if (kid.getPos().x - block.getRect().x > 10 && kid.getPos().y == block.getRect().y + block.getRect().height) {
								GeneralBlock neighborBlock = null;
								if (j < 9) {//Can't touch rightmost block while looking left
									neighborBlock = blocks.get(i * 10 + j + 1);
								}
								if (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN) {
									kid.setCanClipDownTheBlock(block);
								}
							}
						}
						else {
							if (kid.getPos().x - block.getRect().x < 5 && kid.getPos().y == block.getRect().y + block.getRect().height) {
								GeneralBlock neighborBlock = null;
								if (j > 0) {//Can't touch leftmost block while looking right
									neighborBlock = blocks.get(i * 10 + j - 1);
								}
								else if (j == 0) {//Check room on the left
									if (levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft != null) {
										neighborBlock = levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft.blocks.get(i * 10 + 9);
									}
								}
								
								//System.out.println("---------------------------------\n" + block.getType() + "\n" + "LEFT: " + neighborBlock + "\n---------------------------------");
								
								if (neighborBlock != null && (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN)) {
									kid.setCanClipDownTheBlock(block);
								}
							}
						}
						//System.out.println("kid: [" + kid.getPos().x + ", "+ (kid.getPos().y + kid.getRect().height) + "], block: [" + (block.getRect().x + block.getRect().width) + ", " + (block.getRect().y + block.getRect().height) + "]");
					}
					//noCollisions = false;
				}
			}
		}
	}
	
	public void checkCollisionsWithLeftRoom() {
		List<GeneralBlock> blocksLeft = null;
		
		if (levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft != null) {
			blocksLeft = levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft.blocks;
		}
		
		if (blocksLeft == null) {
			if (kid.getPos().x < GeneralBlock.NEGATIVE_OFFSET_LEFT + GeneralBlock.WALL_OR_EMPTY_OFFSET_LEFT) {
				//kid.setPos(GeneralBlock.FLOOR_OFFSET_LEFT + 1, kid.getPos().y);
				kid.setPos(GeneralBlock.NEGATIVE_OFFSET_LEFT + GeneralBlock.WALL_OR_EMPTY_OFFSET_LEFT, kid.getPos().y);
			}
			return;
		}
		
		//check left screen first
		for (int i = 2; i >= 0; i--) {
			GeneralBlock blockLeft = blocksLeft.get(i * 10 + 9);
			Rectangle blockLeftRectangle = new Rectangle(GeneralBlock.NEGATIVE_OFFSET_LEFT, blockLeft.getPos().y, GeneralBlock.WALL_OR_EMPTY_WIDTH, GeneralBlock.WALL_OR_EMPTY_HEIGHT);
			/*
			 * FINDING A BLOCK TO CLIP ON
			 */
			if (kid.state == State.SCALING && blockLeft.getType() != Block.EMPTY && blockLeft.getType() != Block.UNKNOWN && blockLeft.getType() != Block.WALL) {
				//System.out.println("kid: [" + kid.getPos().x + ", "+ (kid.getPos().y + kid.getRect().height) + "], block: [" + (blockLeftRectangle.x + blockLeft.getRect().width) + ", " + (blockLeft.getPos().y + blockLeft.getRect().height) + "]");
				Modifier modifier = levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft.modifiers.get(i * 10 + 9);
				if (blockLeft.getType() == Block.GATE && (modifier == Modifier.GATE_CLOSED || modifier == Modifier.UNKNOWN)) {
					//Gate is closed and we can't clip on it
				}
				else {
					if (kid.isLookingLeft()) {
						if (kid.getPos().x - (blockLeftRectangle.x + blockLeftRectangle.width) < 10 && kid.getPos().x - blockLeftRectangle.x >= 10 && Math.abs(kid.getPos().y + kid.getRect().height - (blockLeft.getPos().y + blockLeft.getRect().height)) < 1) {
							GeneralBlock neighborBlock = null;
							neighborBlock = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks.get(i * 10);
						
							if (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN) {
								GeneralBlock tempBlock = new Floor(-1, 2 - ((int) blockLeft.getPos().y) / 128);
								kid.setCanClipOnBlock(tempBlock);
							}
						}
					}
				}
			}

			if (Intersector.overlapRectangles(kid.getRect(), blockLeftRectangle)) {
				/*
				 * COLLISION CHECK WHEN SCALING
				 */
				
				if (kid.state == State.SCALING && blockLeft.getType() != Block.EMPTY && blockLeft.getType() != Block.UNKNOWN && blockLeft.getType() != Block.WALL && blockLeft.getType() != Block.LOOSE_BOARD) {
					if (blockLeftRectangle.y > kid.getPos().y) {
						kid.setPos(kid.getPos().x, blockLeft.getPos().y + blockLeft.getRect().height / 2 - kid.getRect().height);
					}
				}
				else if (kid.state == State.SCALING && blockLeft.getType() == Block.WALL) {
					if (blockLeftRectangle.y > kid.getPos().y) {
						kid.setPos(kid.getPos().x, blockLeft.getPos().y + blockLeft.getRect().height / 12 - kid.getRect().height);
					}
				}
				else if (kid.state == State.SCALING && blockLeft.getType() == Block.LOOSE_BOARD) {
					if (blockLeftRectangle.y > kid.getPos().y) {
						kid.setPos(kid.getPos().x, blockLeft.getRect().y + blockLeft.getRect().height / 2 - kid.getRect().height);
						blockLeft.startBreaking();
					}
				}
				/*
				 * COLLISION CHECK ON SAME HEIGHT
				 */
				Modifier modifier = levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft.modifiers.get(i * 10 + 9);
				if (blockLeft.getType() == Block.WALL || (blockLeft.getType() == Block.GATE && (modifier == Modifier.GATE_CLOSED || modifier == Modifier.UNKNOWN))) {
					if (!checkIfInAir() && blockLeft.getPos().y <= kid.getPos().y) {
						if (kid.isLookingLeft() && blockLeftRectangle.x < kid.getPos().x) {
							//kid.setPos(block.getPos().x + 1 + block.getRect().getWidth() + GeneralBlock.WALL_OR_EMPTY_OFFSET_LEFT, kid.getPos().y);
							kid.setPos(blockLeftRectangle.x + GeneralBlock.WALL_OR_EMPTY_OFFSET_LEFT, kid.getPos().y);
						}
						else if (!kid.isLookingLeft() && blockLeftRectangle.x >= kid.getPos().x) {
							kid.setPos(blockLeftRectangle.x - 1, kid.getPos().y);
						}
//						else {
//							kid.setPos(blockLeftRectangle.x + 1 + blockLeftRectangle.getWidth(), kid.getPos().y);
//						}
					}
//					else {
//						kid.state = State.FALLING;
//					}
				}
				
				/*
				 * FINDING A BLOCK TO CLIP DOWN FROM
				 */
				if (kid.state == State.STANDING && blockLeft.getType() != Block.EMPTY && blockLeft.getType() != Block.UNKNOWN && blockLeft.getType() != Block.WALL) {
					if (kid.isLookingLeft()) {
						if (kid.getPos().x - blockLeftRectangle.x > 5 && kid.getPos().y == blockLeftRectangle.y + blockLeft.getRect().height) {
							GeneralBlock neighborBlock = null;
							neighborBlock = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks.get(i * 10);

							if (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN) {
								GeneralBlock tempBlock = new Floor(-1, 2 - ((int) blockLeft.getPos().y) / 128);
								kid.setCanClipDownTheBlock(tempBlock);
							}
						}
					}
				}
			}
		}
	}
	
	public void checkCollisionsWithTopRoom() {
		List<GeneralBlock> blocksTop = null;
		
		if (levelRenderer.level.screens.get(levelRenderer.screenId).linkTop != null) {
			blocksTop = levelRenderer.level.screens.get(levelRenderer.screenId).linkTop.blocks;
		}
		
		if (blocksTop == null) {
			if (kid.getPos().y > GeneralBlock.NEGATIVE_OFFSET_TOP + GeneralBlock.WALL_OR_EMPTY_HEIGHT / 12 - kid.getRect().height) {
				kid.setPos(kid.getPos().x, GeneralBlock.NEGATIVE_OFFSET_TOP + GeneralBlock.WALL_OR_EMPTY_HEIGHT / 12 - kid.getRect().height);
			}
			return;
		}
		
		//check left screen first
		for (int j = 0; j <= 9; j++) {
			GeneralBlock blockTop = blocksTop.get(2 * 10 + j);
			Rectangle blockTopRectangle = new Rectangle(blockTop.getRect().x, GeneralBlock.NEGATIVE_OFFSET_TOP, GeneralBlock.FLOOR_WIDTH, GeneralBlock.FLOOR_HEIGHT);
			
			/*
			 * FINDING A BLOCK TO CLIP ON
			 */
			if (kid.state == State.SCALING && blockTop.getType() != Block.EMPTY && blockTop.getType() != Block.UNKNOWN && blockTop.getType() != Block.WALL) {
				if (kid.isLookingLeft()) {
					if (kid.getPos().x - (blockTop.getRect().x + blockTop.getRect().width) < 10 && kid.getPos().x - blockTop.getRect().x > 10 && Math.abs(kid.getPos().y + kid.getRect().height - (blockTopRectangle.y + blockTop.getRect().height)) < 1) {
						GeneralBlock neighborBlock = null;
						if (j < 9) {//Can't touch rightmost block while looking left
							neighborBlock = blocksTop.get(2 * 10 + j + 1);
						}
						if (neighborBlock != null && (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN)) {
							kid.setCanClipOnBlock(blockTop);
						}
					}
				}
				else {
					if (blockTop.getRect().x - kid.getPos().x < 45 && blockTop.getRect().x - kid.getPos().x > 5 && Math.abs(kid.getPos().y + kid.getRect().height - (blockTopRectangle.y + blockTop.getRect().height)) < 1) {
						GeneralBlock neighborBlock = null;
						if (j > 0) {//Can't touch leftmost block while looking right
							neighborBlock = blocksTop.get(2 * 10 + j - 1);
						}
						else if (j == 0) {//Check room on the left
							neighborBlock = levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkTop.linkLeft.blocks.get(2 * 10 + 9);
						}
						
						//System.out.println("---------------------------------\n" + blockTop.getType() + "\n" + "LEFT: " + neighborBlock + "\n---------------------------------");
						
						if (neighborBlock.getType() == Block.EMPTY || neighborBlock.getType() == Block.UNKNOWN) {
							kid.setCanClipOnBlock(blockTop);
						}
					}
				}
				//System.out.println("kid: [" + kid.getPos().x + ", "+ (kid.getPos().y + kid.getRect().height) + "], block: [" + (block.getRect().x + block.getRect().width) + ", " + (block.getRect().y + block.getRect().height) + "]");
			}
			
			
			/*
			 * COLLISION CHECKS
			 */
			blockTop.update();
			if (blockTop.canBreak) {
				blockTop.dispose();
				blocksTop.set(2 * 10 + j, new Empty(j, 2));
				levelRenderer.dropBrokenBoard(batch, j, -1, levelRenderer.getScreenId());
			}
			
			if (Intersector.intersectRectangles(kid.getRect(), blockTopRectangle) || Intersector.overlapRectangles(kid.getRect(), blockTopRectangle)) {
				
				/*
				 * COLLISION CHECK WHEN SCALING
				 */
				//System.out.println(blockTop.getType() + ", " + kid.state);
				
				if (kid.state == State.SCALING && blockTop.getType() != Block.EMPTY && blockTop.getType() != Block.UNKNOWN && blockTop.getType() != Block.WALL && blockTop.getType() != Block.LOOSE_BOARD) {
					if (blockTopRectangle.y > kid.getPos().y) {
						kid.setPos(kid.getPos().x, blockTopRectangle.y + blockTop.getRect().height / 2 - kid.getRect().height);
					}
				}
				else if (kid.state == State.SCALING && blockTop.getType() == Block.WALL) {
					if (blockTopRectangle.y > kid.getPos().y) {
						kid.setPos(kid.getPos().x, blockTopRectangle.y + blockTop.getRect().height / 12 - kid.getRect().height);
					}
				}
				else if (kid.state == State.SCALING && blockTop.getType() == Block.LOOSE_BOARD) {
					if (blockTopRectangle.y > kid.getPos().y) {
						kid.setPos(kid.getPos().x, blockTopRectangle.y + blockTop.getRect().height / 2 - kid.getRect().height);
						blockTop.startBreaking();
					}
				}
			}
		}
	}
	
	public void moveToLeftLinkRoom() {
		LevelScreen linkedRoom = levelRenderer.level.screens.get(levelRenderer.screenId).linkLeft;
		if (linkedRoom == null) {
			kid.getPos().x = GeneralBlock.NEGATIVE_OFFSET_LEFT;
			return;
		}
		levelRenderer.renderLevelScreen(batch, linkedRoom.id - 1);
		
		kid.setPos(GeneralBlock.BLOCK_WIDTH * 9 + GeneralBlock.FLOOR_OFFSET_LEFT, kid.getPos().y);
	}
	
	public void moveToRightLinkRoom() {
		LevelScreen linkedRoom = levelRenderer.level.screens.get(levelRenderer.screenId).linkRight;
		if (linkedRoom == null) {
			kid.getPos().x = levelRenderer.level.screens.get(levelRenderer.screenId).width * 64 - 1;
			return;
		}
		levelRenderer.renderLevelScreen(batch, linkedRoom.id - 1);
		
		kid.setPos(GeneralBlock.NEGATIVE_OFFSET_LEFT, kid.getPos().y);
	}
	
	public void moveToTopLinkRoom() {
		LevelScreen linkedRoom = levelRenderer.level.screens.get(levelRenderer.screenId).linkTop;
		if (linkedRoom == null) {
			//kid.getPos().y = 3 * 128 - 1;
			return;
		}
		
		levelRenderer.renderLevelScreen(batch, linkedRoom.id - 1);
		
		kid.setPos(kid.getPos().x, GeneralBlock.FLOOR_HEIGHT);
	}
	
	public void moveToBottomLinkRoom() {
		LevelScreen linkedRoom = levelRenderer.level.screens.get(levelRenderer.screenId).linkBottom;
		if (linkedRoom == null) {
			//kid.getPos().y = -60;
			kid.lives = 0;
			return;
		}
		
		for (BrokenBoard brokenBoard : levelRenderer.brokenBoards) {
			if (brokenBoard.getPos().y < 0 && brokenBoard.getPos().y > -GeneralBlock.BLOCK_HEIGHT * 3 && linkedRoom.id - 1 == brokenBoard.getScreenId()) {
				brokenBoard.setBrokenBlockPos(GeneralBlock.BLOCK_HEIGHT * 3 + brokenBoard.getPos().y);
			}
		}
		
		levelRenderer.renderLevelScreen(batch, linkedRoom.id - 1);
		
		kid.setPos(kid.getPos().x, GeneralBlock.BLOCK_HEIGHT * 3 - kid.getRect().height);
	}
	
	public void setSwordBlockToFloor(int i, int j) {
		List<GeneralBlock> blocks = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks;
		if (blocks.get(i * 10 + j).getType() == Block.SWORD) {
			blocks.set(i * 10 + j, new Floor(j, i));
		}
	}
	
	public void setPotionBlockToFloor(int i, int j) {
		if (j != -1) { //Current room
			List<GeneralBlock> blocks = levelRenderer.level.screens.get(levelRenderer.getScreenId()).blocks;
			if (blocks.get(i * 10 + j).getType() == Block.POTION) {
				blocks.set(i * 10 + j, new Floor(j, i));
			}
		}
		else { //Left side room
			List<GeneralBlock> blocks = levelRenderer.level.screens.get(levelRenderer.getScreenId()).linkLeft.blocks;
			if (blocks.get(i * 10 + 9).getType() == Block.POTION) {
				blocks.set(i * 10 + 9, new Floor(9, i));
			}
		}
	}
	
	public void checkBrokenBoardsGravity() {
		Iterator<BrokenBoard> iterator = levelRenderer.brokenBoards.iterator();
		
		while (iterator.hasNext()) {
			BrokenBoard brokenBoard = iterator.next();
			
			brokenBoard.setBrokenBlockPos(brokenBoard.getPos().y - kid.GRAVITY * Gdx.graphics.getDeltaTime());
			
			if (!brokenBoard.hitSomeone()) {
				if (brokenBoard.getRect().y + brokenBoard.getRect().height > kid.getRect().y + kid.getRect().height && (Intersector.intersectRectangles(kid.getRect(), brokenBoard.getRect()) || Intersector.overlapRectangles(kid.getRect(), brokenBoard.getRect()))) {
					kid.lives--;
					brokenBoard.setHitSomeone(true);
					renderHittingFlash();
				}
			}
			
			if (brokenBoard.getPos().y < -10) {
				if (levelRenderer.level.screens.get(brokenBoard.getScreenId()).linkBottom != null) {
					brokenBoard.setScreenId(levelRenderer.level.screens.get(brokenBoard.getScreenId()).linkBottom.id - 1);
					brokenBoard.setBrokenBlockPos(3 * GeneralBlock.BLOCK_HEIGHT);
				}
			}
			List<GeneralBlock> blocks = levelRenderer.level.screens.get(brokenBoard.getScreenId()).blocks;

			for (int i = 0; i <= 2; i++) {
				for (int j = 0; j <= 9; j++) {
					GeneralBlock tempBlock = blocks.get(i * 10 + j);

					if (Intersector.overlapRectangles(brokenBoard.getRect(), tempBlock.getRect())) {

						if (tempBlock.getType() == Block.FLOOR) {
							brokenBoard.dispose();
							iterator.remove();
							blocks.set(i * 10 + j, new Debris(j, i));
						} else if (tempBlock.getType() == Block.TORCH) {
							brokenBoard.dispose();
							iterator.remove();
							blocks.set(i * 10 + j, new Torch(Type.TORCH_WITH_DEBRIS, j, i));
						} else if (tempBlock.getType() == Block.RAISE_BUTTON) {
							brokenBoard.dispose();
							iterator.remove();
							if (tempBlock.eventNum >= 0) {
								int currentEventNum = tempBlock.eventNum;
								Event event;
								do {
									event = levelRenderer.level.events.get(currentEventNum);
									
									if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.GATE) {
										levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_ALWAYS_OPEN);
									}
									else if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.EXIT_LEFT) {
										// Set both exits to be open
										levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_OPEN);
										levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber + 1, Modifier.GATE_OPEN);
									}
									currentEventNum++;
								}
								while(event.triggersNextEvent);
							}
							blocks.set(i * 10 + j, new Debris(j, i));
							
						} else if (tempBlock.getType() == Block.DROP_BUTTON) {
							brokenBoard.dispose();
							iterator.remove();
							if (tempBlock.eventNum >= 0) {
								int currentEventNum = tempBlock.eventNum;
								Event event;
								do {
									event = levelRenderer.level.events.get(currentEventNum);
									
									if (levelRenderer.level.screens.get(event.screenNumber).blocks.get(event.blockNumber).getType() == Block.GATE) {
										levelRenderer.level.screens.get(event.screenNumber).modifiers.set(event.blockNumber, Modifier.GATE_CLOSING);
									}
									currentEventNum++;
								}
								while(event.triggersNextEvent);
							}
							blocks.set(i * 10 + j, new Debris(j, i));
							
						} else if (tempBlock.getType() != Block.EMPTY && tempBlock.getType() != Block.UNKNOWN){
							brokenBoard.dispose();
							iterator.remove();
						}
					}
				}
			}
		}
	}
	
	public void closeAllGates() {
		for (LevelScreen screen : levelRenderer.level.screens) {
			for (int i = 0; i < screen.blocks.size(); i++) {
				GeneralBlock block = screen.blocks.get(i);

				if (block.getType() == Block.GATE && screen.modifiers.get(i) != Modifier.GATE_OPEN) {
					screen.modifiers.set(i, Modifier.GATE_CLOSED);
				}
				else if (block.getType() == Block.GATE && screen.modifiers.get(i) == Modifier.GATE_OPEN) {
					screen.modifiers.set(i, Modifier.GATE_ALWAYS_OPEN);
				}
			}
		}
	}
	
	public void renderHittingFlash() {
		Gdx.gl.glClearColor(0.5412f, 0.02745f, 0.02745f, 1);
		Gdx.gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
	}
	
	public void renderDeadKid() {
		if (kid.deathType == DeathType.SPIKED) {
			kid.setRectDimensions(kid.deadSpiked.getRegionWidth(), kid.deadSpiked.getRegionHeight());
			kid.setRectPos(kid.getPos().x - (kid.deadSpiked.getRegionWidth() / 2 - kid.WIDTH / 2), kid.getPos().y);
			
			if (kid.isLookingLeft()) {
				batch.draw(kid.deadSpiked, kid.getRect().x, kid.getRect().y);
			}
			else {
				kid.setRectPos(kid.getRect().x + kid.deadSpiked.getRegionWidth(), kid.getPos().y);
				batch.draw(kid.deadSpiked, kid.getRect().x, kid.getPos().y, -kid.deadSpiked.getRegionWidth(), kid.deadSpiked.getRegionHeight());
			}
		}
		else if (kid.deathType == DeathType.CHOPPED) {
			kid.setRectDimensions(kid.deadChopped.getRegionWidth(), kid.deadChopped.getRegionHeight());
			//kid.setRectPos(kid.getPos().x - (kid.deadChopped.getRegionWidth() / 2 - kid.WIDTH / 2), kid.getPos().y);
			if (kid.getPos().x % 64 < 32) {
				kid.setPos(kid.getPos().x - kid.getPos().x % 64, kid.getPos().y);
			}
			else {
				kid.setPos(kid.getPos().x + 64 - kid.getPos().x % 64, kid.getPos().y);
			}
			
			if (kid.isLookingLeft()) {
				batch.draw(kid.deadChopped, kid.getPos().x, kid.getRect().y);
			}
			else {
				batch.draw(kid.deadChopped, kid.getPos().x + kid.deadChopped.getRegionWidth(), kid.getRect().y, -kid.deadChopped.getRegionWidth(), kid.deadChopped.getRegionHeight());
			}
		}
		else {
			kid.setRectDimensions(kid.deadDefault.getRegionWidth(), kid.deadDefault.getRegionHeight());
			kid.setRectPos(kid.getPos().x - (kid.deadDefault.getRegionWidth() / 2 - kid.WIDTH / 2), kid.getPos().y);
			
			if (kid.isLookingLeft()) {
				batch.draw(kid.deadDefault, kid.getRect().x, kid.getRect().y);
			}
			else {
				kid.setRectPos(kid.getRect().x + kid.deadDefault.getRegionWidth(), kid.getPos().y);
				batch.draw(kid.deadDefault, kid.getRect().x, kid.getPos().y, -kid.deadDefault.getRegionWidth(), kid.deadDefault.getRegionHeight());
			}
		}
	}
	
	public void triggerCollisionOverlay() {
		levelRenderer.showCollisionOverlays = !levelRenderer.showCollisionOverlays;
	}
	
	public void render() {
		if (!pickingSwordMusic.isPlaying() && !gameOver) {
			if (backgroundMusicVolume < 0.5f) {
				backgroundMusic.setVolume(backgroundMusicVolume += 0.01f);
			}
		}
		batch.begin();
		
		if (kid.lives <= 0) {
			gameOver = true;
		}
		
		levelRenderer.render(batch);
		
		if (!gameOver) {
			kid.render(batch);
			
			if (!firstRun) {
				checkCollisions();
				checkCollisionsWithLeftRoom();
				checkCollisionsWithTopRoom();
				checkGravity();
			}
		}
		else {
			renderDeadKid();
		}
		
		// Stop rendering if level is complete
		if (gameScreen.isLevelComplete()) {
			return;
		}
		
		checkBrokenBoardsGravity();
		
		levelRenderer.renderAfterKid(batch);
		
		for (int i = 0; i < kid.lives; i++) {
			batch.draw(liveFullTexture, 15 * i + 20, 15);
		}
		if (kid.lives < kid.maxLives) {
			for (int i = (kid.lives >= 0 ? kid.lives : 0); i < kid.maxLives; i++) {
				batch.draw(liveEmptyTexture, 15 * i + 20, 15);
			}
		}
		
		if (gameOver) {
			batch.draw(gameOverTexture, 640 / 2 - gameOverTexture.getRegionWidth() / 2, 384 / 2 - gameOverTexture.getRegionHeight() / 2);
			if (backgroundMusicVolume > 0) {
				backgroundMusic.setVolume(backgroundMusicVolume);
				backgroundMusicVolume -= 0.02;
			}
			else {
				backgroundMusic.stop();
			}
			if (!gameOverSoundPlayed) {
				gameOverSoundPlayed = true;
				deathMusic.setVolume(0.5f);
				deathMusic.play();
			}
			else if (!deathMusic.isPlaying() && !taricTauntSoundPlayed) {
				if (kid.deathType == DeathType.DEFAULT) {
					taricTauntMusic.setVolume(0.75f);
					taricTauntMusic.play();
					taricTauntSoundPlayed = true;
				}
			}
		}
		
		batch.end();
		
		if (firstRun) {
			firstRun = false;
		}
	}
	
	public void dispose() {
		gameOverTexture.getTexture().dispose();
		liveEmptyTexture.getTexture().dispose();
		liveFullTexture.getTexture().dispose();
		
		backgroundMusic.dispose();
		pickingSwordMusic.dispose();
		deathMusic.dispose();
		taricTauntMusic.dispose();
		
		levelRenderer.dispose();
		kid.dispose();
		kid = null;
		levelRenderer = null;
		
		batch.dispose();
	}
}
