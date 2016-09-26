package com.aliamondo.pop.graphics.characters;

import java.util.Arrays;

import com.aliamondo.pop.graphics.WorldRenderer;
import com.aliamondo.pop.graphics.blocks.GeneralBlock;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.Animation;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureAtlas;
import com.badlogic.gdx.graphics.g2d.TextureRegion;

public class Kid extends Character{
	// CURRENTLY JUMPING = RUN_JUMPING
	public enum State {
		RUNNING, STANDING, JUMPING, RUN_JUMPING, FALLING, SCALING, WALKING, CLIPPING_UP, CLIPPING_DOWN, COUCHING, PICKING_ITEM
	}
	
	public enum DeathType {
		DEFAULT, SPIKED, CHOPPED
	}
	
	public final int WIDTH = 24;
	public final int HEIGHT = 82;
	
	WorldRenderer worldRenderer;
	
	public Animation runningAnimation, scalingAnimation, walkingAnimation, jumpingAnimation, clippingAnimation, couchingAnimation;
	public Animation couchingUpAnimation, couchingDownAnimation, pickingPotionAnimation;
	public Animation gotSwordAnimation, puttingDownSwordAnimation;
	TextureRegion standing, falling;
	public TextureRegion deadSpiked, deadChopped, deadDefault;
	
	private Modifier potionModifier = Modifier.NONE_NONE;

	public float elapsedTime = 0;
	public float fallingTime = 0;
	public float standingTime = 0;
	public float walkingTime = 0;
	public float jumpingTime = 0;
	public float scalingTime = 0;
	public float clippingUpTime = 0;
	public float clippingDownTime = 0;
	public float couchingTime = 0;
	public float couchingUpTime = 0;
	public float couchingDownTime = 0;
	public float pickingSwordTime = 0;
	public float pickingPotionTime = 0;
	
	public final float RUNNING_SPEED = 220;
	public float GRAVITY = 350; //350
	public final float SCALING_SPEED = 70;
	public final float WALKING_SPEED = 50;
	public final float JUMPING_SPEED = 250;
	public final float CLIPPING_SPEED = 125;

	public State state;
	public DeathType deathType;
	
	//private boolean firstProgramRun = true;
	//private boolean firstFall = true;
	//private boolean firstLanding = true;
	
	private boolean canClipOnBlock = false;
	private GeneralBlock blockToClipUpOn = null;
	private boolean canClipDownTheBlock = false;
	private GeneralBlock blockToClipDownFrom = null;
	
	private boolean canPickSword = false;
	private int swordCoordinatesI = Integer.MIN_VALUE;
	private int swordCoordinatesJ = Integer.MIN_VALUE;
	
	private boolean canPickPotion = false;
	private int potionCoordinatesI = Integer.MIN_VALUE;
	private int potionCoordinatesJ = Integer.MIN_VALUE;
	
	//TODO private boolean hasSword = false;
	
	public int lives = 3;
	public int maxLives = 3;
	
	TextureRegion[] runningFrames = new TextureRegion[9];
	TextureRegion[] scalingFrames = new TextureRegion[19];
	TextureRegion[] walkingFrames = new TextureRegion[12];
	TextureRegion[] jumpingFrames = new TextureRegion[11];
	TextureRegion[] clippingFrames = new TextureRegion[15];
	TextureRegion[] couchingFrames = new TextureRegion[13];
	TextureRegion[] gotSwordFrames = new TextureRegion[15];
	TextureRegion[] puttingDownSwordFrames = new TextureRegion[13];
	TextureRegion[] pickingPotionFrames = new TextureRegion[15];
	TextureRegion currentFrame;
	
	TextureAtlas runningSpriteSheet, scalingSpriteSheet, walkingSpriteSheet;
	TextureAtlas jumpingSpriteSheet, clippingSpriteSheet, couchingSpriteSheet;
	TextureAtlas gotSwordSpriteSheet, puttingDownSwordSpriteSheet, pickingPotionSpriteSheet;
	
	public Kid(int screen, int x, int y, WorldRenderer worldRenderer) {
		super(x, y, 24, 82);

		setPos(x * 64, (2 - y) * 128);
		this.worldRenderer = worldRenderer;

		runningSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/running.txt"));
		scalingSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/scaling.txt"));
		walkingSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/walking.txt"));
		jumpingSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/run_jumping.txt"));
		clippingSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/clipping.txt"));
		couchingSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/couching.txt"));
		gotSwordSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/got_sword.txt"));
		puttingDownSwordSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/putting_down_sword.txt"));
		pickingPotionSpriteSheet = new TextureAtlas(Gdx.files.internal("sprites/kid/drinking_potion.txt"));
		
		standing = new TextureRegion(new Texture(Gdx.files.internal("sprites/kid/standing.png")), 24, 82);
		falling = new TextureRegion(new Texture(Gdx.files.internal("sprites/kid/falling.png")), 44, 70);
		deadChopped = new TextureRegion(new Texture(Gdx.files.internal("sprites/kid/dead_chopped.png")), 90, 24);
		deadSpiked = new TextureRegion(new Texture(Gdx.files.internal("sprites/kid/dead_spiked.png")), 82, 54);
		deadDefault = new TextureRegion(new Texture(Gdx.files.internal("sprites/kid/dead_default.png")), 68, 32);
		
		for (int i = 6; i <= 14; i++) {
			runningFrames[i - 6] = runningSpriteSheet.findRegion("frame" + i);
		}
		runningAnimation = new Animation(0.09f, runningFrames);
		
		for (int i = 1; i <= 19; i++) {
			scalingFrames[i - 1] = scalingSpriteSheet.findRegion("frame" + i);
		}
		scalingAnimation = new Animation(0.08f, scalingFrames);
		
		for (int i = 1; i <= 12; i++) {
			walkingFrames[i - 1] = walkingSpriteSheet.findRegion("frame" + i);
		}
		walkingAnimation = new Animation(0.08f, walkingFrames);
		
		for (int i = 1; i <= 11; i++) {
			jumpingFrames[i - 1] = jumpingSpriteSheet.findRegion("frame" + i);
		}
		jumpingAnimation = new Animation(0.075f, jumpingFrames);
		
		for (int i = 1; i <= 15; i++) {
			clippingFrames[i - 1] = clippingSpriteSheet.findRegion("frame" + i);
		}
		clippingAnimation = new Animation(0.075f, clippingFrames);
		
		for (int i = 1; i <= 13; i++) {
			couchingFrames[i - 1] = couchingSpriteSheet.findRegion("frame" + i);
		}
		couchingAnimation = new Animation(0.075f, couchingFrames);
		couchingDownAnimation = new Animation(0.075f, Arrays.copyOfRange(couchingFrames, 0, 3));
		couchingUpAnimation = new Animation(0.075f, Arrays.copyOfRange(couchingFrames, 3, couchingFrames.length));
		
		for (int i = 1; i <= gotSwordFrames.length; i++) {
			gotSwordFrames[i - 1] = gotSwordSpriteSheet.findRegion("frame" + i);
		}
		gotSwordAnimation = new Animation(0.075f, gotSwordFrames);
		
		for (int i = 1; i <= puttingDownSwordFrames.length; i++) {
			puttingDownSwordFrames[i - 1] = puttingDownSwordSpriteSheet.findRegion("frame" + i);
		}
		puttingDownSwordAnimation = new Animation(0.075f, puttingDownSwordFrames);
		
		for (int i = 1; i <= pickingPotionFrames.length; i++) {
			pickingPotionFrames[i - 1] = pickingPotionSpriteSheet.findRegion("frame" + i);
		}
		pickingPotionAnimation = new Animation(0.075f, pickingPotionFrames);
		
		this.state = State.STANDING;
		this.deathType = DeathType.DEFAULT;
	}
	
	public void clearAnimationTime() {
		elapsedTime = 0;
		fallingTime = 0;
	}

	public void setLookingLeft(boolean lookingleft) {
		super.lookingLeft = lookingleft;
	}
	
	public boolean isLookingLeft() {
		return lookingLeft;
	}
	
	public boolean getCanClipOnBlock() {
		return canClipOnBlock;
	}

	public void setCanClipOnBlock(GeneralBlock blockToClipOn) {
		this.blockToClipUpOn = blockToClipOn;
		if (blockToClipOn == null) canClipOnBlock = false;
		else canClipOnBlock = true;
	}

	public boolean getCanClipDownTheBlock() {
		return canClipDownTheBlock;
	}

	public void setCanClipDownTheBlock(GeneralBlock blockToClipDownOn) {
		this.blockToClipDownFrom = blockToClipDownOn;
		if (blockToClipDownOn == null) canClipDownTheBlock = false;
		else canClipDownTheBlock = true;
	}

	public boolean getCanPickSword() {
		return canPickSword;
	}

	public void setCanPickSword(int i, int j) {
		this.swordCoordinatesI = i;
		this.swordCoordinatesJ = j;
		if (swordCoordinatesI == Integer.MIN_VALUE && swordCoordinatesJ == Integer.MIN_VALUE) {
			canPickSword = false;
		}
		else {
			canPickSword = true;
		}
	}
	
	public boolean getCanPickPotion() {
		return canPickPotion;
	}

	public void setCanPickPotion(int i, int j, Modifier modifier) {
		this.potionCoordinatesI = i;
		this.potionCoordinatesJ = j;
		if (potionCoordinatesI == Integer.MIN_VALUE && potionCoordinatesJ == Integer.MIN_VALUE) {
			this.potionModifier = modifier;
			canPickPotion = false;
		}
		else {
			this.potionModifier = modifier;
			canPickPotion = true;
		}
	}
	
	public void pickSword() {
		if (getCanPickSword()) {
			//hasSword = true;
			worldRenderer.setSwordBlockToFloor(swordCoordinatesI, swordCoordinatesJ);
			swordCoordinatesI = swordCoordinatesJ = 0;
			canPickSword = false;
			
			worldRenderer.playPickingSwordMusic();
		}
	}
	
	public void pickPotion() {
		if (getCanPickPotion()) {
			worldRenderer.setPotionBlockToFloor(potionCoordinatesI, potionCoordinatesJ);
			potionCoordinatesI = potionCoordinatesJ = 0;
			canPickPotion = false;
			
			if (potionModifier == Modifier.POTION_EMPTY) {
				// Do nothing
			}
			else if (potionModifier == Modifier.POTION_HEALTH) {
				if (lives < maxLives) lives++;
			}
			else if (potionModifier == Modifier.POTION_LIFE) {
				if (maxLives < 10) {
					maxLives++;
					lives = maxLives;
				}
			}
			else if (potionModifier == Modifier.POTION_FEATHER_FALL) {
				GRAVITY = 100;
			}
			else if (potionModifier == Modifier.POTION_INVERT) {
				GRAVITY = 350;
			}
			else if (potionModifier == Modifier.POTION_POISON) {
				lives--;
				worldRenderer.renderHittingFlash();
			}
		}
	}

	public void getHitByFalling() {
		if (state == State.FALLING) return;
		if (fallingTime >= 0.65f && fallingTime < 1.0f && GRAVITY == 350) {
			lives--;
			worldRenderer.renderHittingFlash();
		}
		else if (fallingTime < 0.65f) {
			return;
		}
		else if (GRAVITY == 350) {
			worldRenderer.renderHittingFlash();
			lives = 0;
		}
		fallingTime = 0;
	}

	@Override
	public void render(SpriteBatch batch) {
		//elapsedTime += Gdx.graphics.getDeltaTime();
		//System.out.println(state);
		//getHitByFalling();
		/*
		 * STATES TO FINISH THE ANIMATIONS
		 */
		
		if (state != State.PICKING_ITEM && (pickingSwordTime > 0 || pickingPotionTime > 0)) {
			if (pickingSwordTime > 0) {
				currentFrame = couchingAnimation.getKeyFrame(pickingSwordTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!couchingAnimation.isAnimationFinished(pickingSwordTime)) {
					if (couchingAnimation.getKeyFrameIndex(pickingSwordTime) >= 4) {
						pickSword();
					}
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					}
					pickingSwordTime += Gdx.graphics.getDeltaTime();
				}
				else {
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.STANDING;
					pickingSwordTime = 0;
				}
			}
			else if (pickingPotionTime > 0) {
				currentFrame = pickingPotionAnimation.getKeyFrame(pickingPotionTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!pickingPotionAnimation.isAnimationFinished(pickingPotionTime)) {
					pickPotion();
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					}
					pickingPotionTime += Gdx.graphics.getDeltaTime();
				}
				else {
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.STANDING;
					pickingPotionTime = 0;
				}
			}
		}
		else if (state != State.COUCHING && couchingDownTime > 0) {
			currentFrame = couchingUpAnimation.getKeyFrame(couchingUpTime, false);
			setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
			if (!couchingUpAnimation.isAnimationFinished(couchingUpTime)) {
				couchingUpTime += Gdx.graphics.getDeltaTime();
			}
			else {
				couchingDownTime = 0;
				couchingUpTime = 0;
				state = State.STANDING;
			}
			if (lookingLeft) {
				batch.draw(currentFrame, getPos().x, getPos().y);
			} else {
				batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
			}
		}
		else if (state != State.WALKING && walkingTime > 0 && walkingTime < 0.08 * 12) {
			currentFrame = walkingAnimation.getKeyFrame(walkingTime, false);
			setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
			if (!walkingAnimation.isAnimationFinished(walkingTime)) {
				if (lookingLeft) {
					batch.draw(currentFrame, getPos().x, getPos().y);
					setPos(getPos().x - WALKING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
				} else {
					batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					setPos(getPos().x + WALKING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
				}
				walkingTime += Gdx.graphics.getDeltaTime();
			}
			else {
				walkingTime = 0;
			}
		}
		else if (state != State.SCALING && state != State.CLIPPING_UP && scalingTime > 0 && scalingTime < 0.08 * 19) {
			//System.out.println("Finishing scaling: " + scalingTime);
			currentFrame = scalingAnimation.getKeyFrame(scalingTime, false);
			setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
			if (!scalingAnimation.isAnimationFinished(scalingTime)) {
				if (scalingAnimation.getKeyFrameIndex(scalingTime) < 11) {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					}
				}
				else if (scalingAnimation.getKeyFrameIndex(scalingTime) < 15) {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						setPos(getPos().x, getPos().y + SCALING_SPEED * Gdx.graphics.getDeltaTime());
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						setPos(getPos().x, getPos().y + SCALING_SPEED * Gdx.graphics.getDeltaTime());
					}
				}
				else {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						setPos(getPos().x, getPos().y - GRAVITY * Gdx.graphics.getDeltaTime());
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						setPos(getPos().x, getPos().y - GRAVITY * Gdx.graphics.getDeltaTime());
					}
				}
				scalingTime += Gdx.graphics.getDeltaTime();
				state = State.SCALING;
			}
			else {
				scalingTime = 0;
			}
		}
		
		
		/*
		 * PROPER STATES DONE MANUALLY BY USER
		 */
		else if (state == State.STANDING) {
			setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
			if (lookingLeft) {
				batch.draw(standing, getPos().x, getPos().y);
			} else {
				batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
			}
			
			//if (!firstFall) firstLanding = false;
			//if (!firstProgramRun) firstFall = false;
			//firstProgramRun = false;
			
			standingTime += Gdx.graphics.getDeltaTime();
			
			//Clear animation times
			clearAnimationTime();
			setCanClipOnBlock(null);
			setCanClipDownTheBlock(null);
			walkingTime = 0;
			jumpingTime = 0;
			fallingTime = 0;
			scalingTime = 0;
			clippingUpTime = 0;
			clippingDownTime = 0;
			couchingTime = 0;
			couchingUpTime = 0;
			couchingDownTime = 0;
		}
		else if (state == State.PICKING_ITEM) {
			if (canPickSword) {
				currentFrame = couchingAnimation.getKeyFrame(pickingSwordTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!couchingAnimation.isAnimationFinished(pickingSwordTime)) {
					if (couchingAnimation.getKeyFrameIndex(pickingSwordTime) >= 4) {
						pickSword();
					}
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					}
					pickingSwordTime += Gdx.graphics.getDeltaTime();
				}
				else {
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.STANDING;
					pickingSwordTime = 0;
				}
			}
			else if (canPickPotion) {
				currentFrame = pickingPotionAnimation.getKeyFrame(pickingPotionTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!pickingPotionAnimation.isAnimationFinished(pickingPotionTime)) {
					if (pickingPotionAnimation.getKeyFrameIndex(pickingPotionTime) >= 4) {
						pickPotion();
					}
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					}
					pickingPotionTime += Gdx.graphics.getDeltaTime();
				}
				else {
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.STANDING;
					pickingPotionTime = 0;
				}
			}
		}
		else if (state == State.COUCHING) {
			if (worldRenderer.checkIfInAir()) {
				state = State.FALLING;
			}
			else {
				currentFrame = couchingDownAnimation.getKeyFrame(couchingDownTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!couchingDownAnimation.isAnimationFinished(couchingDownTime)) {
					couchingDownTime += Gdx.graphics.getDeltaTime();
				}
				couchingUpTime = 0;
				if (lookingLeft) {
					batch.draw(currentFrame, getPos().x, getPos().y);
				} else {
					batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				}
				fallingTime = 0;
			}
		}
		else if (state == State.RUNNING) {
			if (worldRenderer.checkIfInAir()) {
				state = State.FALLING;
			}
			else {
				currentFrame = runningAnimation.getKeyFrame(elapsedTime, true);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (lookingLeft) {
					batch.draw(currentFrame, getPos().x, getPos().y);
					setPos(getPos().x - RUNNING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
				} else {
					batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
					setPos(getPos().x + RUNNING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
				}
				fallingTime = 0;
			}
			standingTime = Float.MIN_VALUE;
			//walkingTime = 0;
			jumpingTime = 0;
			elapsedTime += Gdx.graphics.getDeltaTime();
		}
		else if (state == State.SCALING) {
			if (worldRenderer.checkIfInAir() && scalingTime == 0) {
				state = State.FALLING;
			}
			else {
				//System.out.println("Manual scaling: " + scalingTime);
				currentFrame = scalingAnimation.getKeyFrame(scalingTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!scalingAnimation.isAnimationFinished(scalingTime)) {
					if (scalingAnimation.getKeyFrameIndex(scalingTime) < 11) {
						if (lookingLeft) {
							batch.draw(currentFrame, getPos().x, getPos().y);
						} else {
							batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						}
					}
					else if (scalingAnimation.getKeyFrameIndex(scalingTime) < 15) {
						if (lookingLeft) {
							batch.draw(currentFrame, getPos().x, getPos().y);
							setPos(getPos().x, getPos().y + SCALING_SPEED * Gdx.graphics.getDeltaTime());
						} else {
							batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
							setPos(getPos().x, getPos().y + SCALING_SPEED * Gdx.graphics.getDeltaTime());
						}
					}
					else {
						if (lookingLeft) {
							batch.draw(currentFrame, getPos().x, getPos().y);
							setPos(getPos().x, getPos().y - GRAVITY * Gdx.graphics.getDeltaTime());
						} else {
							batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
							setPos(getPos().x, getPos().y - GRAVITY * Gdx.graphics.getDeltaTime());
						}
					}
					//walkingTime = 0;
					scalingTime += Gdx.graphics.getDeltaTime();
				}
				else { //Kid has landed on the ground
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.STANDING;
					scalingTime = 0;
				}
				standingTime = 0;
			}
		}
		else if (state == State.CLIPPING_UP) {
			clippingAnimation.setPlayMode(Animation.NORMAL);
			currentFrame = clippingAnimation.getKeyFrame(clippingUpTime, false);
			setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
			if (!clippingAnimation.isAnimationFinished(clippingUpTime)) {
				if (clippingAnimation.getKeyFrameIndex(clippingUpTime) < 11) {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						setPos(getPos().x, getPos().y + CLIPPING_SPEED * Gdx.graphics.getDeltaTime());
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						setPos(getPos().x, getPos().y + CLIPPING_SPEED * Gdx.graphics.getDeltaTime());
					}
				}
				else {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						if (getPos().x > blockToClipUpOn.getRect().x + blockToClipUpOn.getRect().width) {
							setPos(blockToClipUpOn.getRect().x + blockToClipUpOn.getRect().width - 5, blockToClipUpOn.getRect().y + blockToClipUpOn.getRect().height);
						}
						else {
							setPos(getPos().x, blockToClipUpOn.getRect().y + blockToClipUpOn.getRect().height);
						}
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						if (getPos().x < blockToClipUpOn.getRect().x - 25) {
							setPos(blockToClipUpOn.getRect().x - 25 + 1, blockToClipUpOn.getRect().y + blockToClipUpOn.getRect().height);
						}
						else {
							setPos(getPos().x, blockToClipUpOn.getRect().y + blockToClipUpOn.getRect().height);
						}
					}
				}
				//walkingTime = 0;
				clippingUpTime += Gdx.graphics.getDeltaTime();
			}
			else { //Kid has landed on the ground
				setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
				if (lookingLeft) {
					batch.draw(standing, getPos().x, getPos().y);
				} else {
					batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
				}
				setCanClipOnBlock(null);
				state = State.STANDING;
				clippingUpTime = 0;
				scalingTime = 0;
			}
			setPos((float)Math.ceil(getPos().x), getPos().y); //so that there are less or no animation issues
			standingTime = 0;
		}
		else if (state == State.CLIPPING_DOWN) {
			clippingAnimation.setPlayMode(Animation.REVERSED);
			currentFrame = clippingAnimation.getKeyFrame(clippingDownTime, false);
			setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
			if (!clippingAnimation.isAnimationFinished(clippingDownTime)) {
				//System.out.println(clippingAnimation.getKeyFrameIndex(clippingDownTime));
				if (clippingAnimation.getKeyFrameIndex(clippingDownTime) < 11) {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						setPos(getPos().x, getPos().y - CLIPPING_SPEED * Gdx.graphics.getDeltaTime());
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						setPos(getPos().x, getPos().y - CLIPPING_SPEED * Gdx.graphics.getDeltaTime());
					}
				}
				else {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						if (getPos().x > blockToClipDownFrom.getRect().x - 10) {
							setPos(blockToClipDownFrom.getRect().x - 10, getPos().y);
						}
					}
				}
				
				//walkingTime = 0;
				clippingDownTime += Gdx.graphics.getDeltaTime();
			}
			else { //Kid has landed on the ground
				setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
				if (lookingLeft) {
					batch.draw(standing, getPos().x, getPos().y);
				} else {
					batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
				}
				setCanClipDownTheBlock(null);
				state = State.STANDING;
				clippingDownTime = 0;
			}
			standingTime = 0;
		}
		else if (state == State.JUMPING || state == State.RUN_JUMPING) {
			//System.out.println("Jumping manually: " + jumpingTime);
			if (worldRenderer.checkIfInAir() && (jumpingTime == 0 || jumpingTime >= 0.075 * 11)) {
				state = State.FALLING;
			}
			else {
				currentFrame = jumpingAnimation.getKeyFrame(jumpingTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!jumpingAnimation.isAnimationFinished(jumpingTime)) {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						setPos(getPos().x - JUMPING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						setPos(getPos().x + JUMPING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
					}
					jumpingTime += Gdx.graphics.getDeltaTime();
				}
				else {
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.RUNNING;
				}
				fallingTime = 0;
			}
		}
		else if (state == State.WALKING) {
			if (worldRenderer.checkIfInAir()) {
				state = State.FALLING;
			}
			else {
				currentFrame = walkingAnimation.getKeyFrame(walkingTime, false);
				setRectDimensions(currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
				if (!walkingAnimation.isAnimationFinished(walkingTime)) {
					if (lookingLeft) {
						batch.draw(currentFrame, getPos().x, getPos().y);
						setPos(getPos().x - WALKING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
					} else {
						batch.draw(currentFrame, getPos().x + currentFrame.getRegionWidth(), getPos().y, -currentFrame.getRegionWidth(), currentFrame.getRegionHeight());
						setPos(getPos().x + WALKING_SPEED * Gdx.graphics.getDeltaTime(), getPos().y);
					}
					walkingTime += Gdx.graphics.getDeltaTime();
					standingTime = Float.MIN_VALUE;
				} else {
					setRectDimensions(standing.getRegionWidth(), standing.getRegionHeight());
					if (lookingLeft) {
						batch.draw(standing, getPos().x, getPos().y);
					} else {
						batch.draw(standing, getPos().x + standing.getRegionWidth(), getPos().y, -standing.getRegionWidth(), standing.getRegionHeight());
					}
					state = State.STANDING;
					standingTime = 0;
				}
				fallingTime = 0;
			}
		}
		
		//In case we were running/walking/etc and now falling
		if (state == State.FALLING) {
			if (lookingLeft) {
				batch.draw(falling, getPos().x, getPos().y);
				setPos(getPos().x, getPos().y - GRAVITY * Gdx.graphics.getDeltaTime());
			} else {
				batch.draw(falling, getPos().x + falling.getRegionWidth(), getPos().y, -falling.getRegionWidth(), falling.getRegionHeight());
				setPos(getPos().x, getPos().y - GRAVITY * Gdx.graphics.getDeltaTime());
				setRectDimensions(falling.getRegionWidth(), falling.getRegionHeight());
			}
			standingTime = 0;
			jumpingTime = 0;
			walkingTime = 1;
			fallingTime += Gdx.graphics.getDeltaTime();
//			System.out.println(fallingTime);
		}

		setCanPickSword(Integer.MIN_VALUE, Integer.MIN_VALUE);
		setCanPickPotion(Integer.MIN_VALUE, Integer.MIN_VALUE, Modifier.NONE_NONE);
	}
	
	@Override
	public void dispose() {
		standing.getTexture().dispose();
		falling.getTexture().dispose();
		deadChopped.getTexture().dispose();
		deadSpiked.getTexture().dispose();
		deadDefault.getTexture().dispose();
		
		if (currentFrame != null) currentFrame.getTexture().dispose();
		
		runningFrames = null;
		scalingFrames = null;
		walkingFrames = null;
		jumpingFrames = null;
		clippingFrames = null;
		couchingFrames = null;
		gotSwordFrames = null;
		puttingDownSwordFrames = null;
		pickingPotionFrames = null;
		
		runningSpriteSheet.dispose();
		scalingSpriteSheet.dispose();
		walkingSpriteSheet.dispose();
		jumpingSpriteSheet.dispose();
		clippingSpriteSheet.dispose();
		couchingSpriteSheet.dispose();
		gotSwordSpriteSheet.dispose();
		puttingDownSwordSpriteSheet.dispose();
		pickingPotionSpriteSheet.dispose();
	}
}