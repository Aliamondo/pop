package com.aliamondo.pop.graphics;

import java.util.ArrayList;
import java.util.List;

import com.aliamondo.pop.graphics.blocks.BrokenBoard;
import com.aliamondo.pop.graphics.blocks.GeneralBlock;
import com.aliamondo.pop.graphics.characters.Kid.State;
import com.aliamondo.pop.levels.Block;
import com.aliamondo.pop.levels.Level;
import com.aliamondo.pop.levels.LevelScreen;
import com.aliamondo.pop.levels.Modifier;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.graphics.Texture;
import com.badlogic.gdx.graphics.g2d.SpriteBatch;
import com.badlogic.gdx.graphics.g2d.TextureRegion;
import com.badlogic.gdx.math.Intersector;
import com.badlogic.gdx.math.Rectangle;

public class LevelRenderer {
	Level level;
	WorldRenderer worldRenderer;
	TextureRegion pillarFrontTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/pillar_front.png")), 120, 155);
	TextureRegion wallFrontTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/wall.png")), 64, 155);
	TextureRegion wallTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/wall.png")), 120, 155);
	TextureRegion floorTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/floor.png")), 120, 155);
	TextureRegion blockFrontTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/other/over.png")), 120, 155);
	TextureRegion topBigPillarFrontTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/top_big_pillar_front.png")), 120, 155);
	TextureRegion bottomBigPillarFrontTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/bottom_big_pillar_front.png")), 120, 155);
	TextureRegion exitRightStairsTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/exit_right_stairs.png")), 120, 155);
	TextureRegion exitLeftStairsTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/dungeon/exit_left_stairs.png")), 120, 155);
	//TextureRegion gemsModifierTexture = new TextureRegion(new Texture(Gdx.files.internal("sprites/other/gems_modifier.png")), 120, 155);

	public List<BrokenBoard> brokenBoards = new ArrayList<BrokenBoard>();
	
	int screenId = 0;
	
	public boolean showCollisionOverlays = false;

	public LevelRenderer(Level level, WorldRenderer worldRenderer) {
		this.worldRenderer = worldRenderer;
		this.level = level;
		this.screenId = level.startScreenId;
		worldRenderer.kid.setPos(level.startBlock % 10 * GeneralBlock.BLOCK_WIDTH + GeneralBlock.FLOOR_OFFSET_LEFT + 1, (2 - level.startBlock / 10) * GeneralBlock.BLOCK_HEIGHT + GeneralBlock.FLOOR_HEIGHT + 1);
		worldRenderer.kid.setLookingLeft(level.startLookingLeft);
		
//		for (int i = 0; i < 3; i++) {
//			for (int j = 0; j <= 9; j++) {
//				System.out.print(this.level.screens.get(screenId).blocks.get(i*10 + j).getType() + " ");
//			}
//			System.out.println();
//		}
	}

	public void render(SpriteBatch batch) {
		renderBottomScreenLink(batch);
		renderLeftScreenLink(batch);
		renderLevelScreen(batch, screenId);
		renderBrokenBoards(batch);
	}

	public void renderAfterKid(SpriteBatch batch) {
		List<GeneralBlock> blocks = level.screens.get(screenId).blocks;
		List<Modifier> modifiers = level.screens.get(screenId).modifiers;
		
		for (int i = 2; i >= 0; i--) {
			for (int j = 0; j <= 9; j++) {
				GeneralBlock block = blocks.get(i * 10 + j);
				Modifier modifier = modifiers.get(i * 10 + j);

				Rectangle blockRectangle = new Rectangle(block.getPos().x, block.getPos().y, GeneralBlock.FLOOR_WIDTH + GeneralBlock.FLOOR_OFFSET_LEFT, GeneralBlock.FLOOR_HEIGHT);
				
				if (block.getType() == Block.PILLAR) {
					//block.render(batch);
					batch.draw(pillarFrontTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.TOP_BIG_PILLAR) {
					batch.draw(topBigPillarFrontTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.BOTTOM_BIG_PILLAR) {
					batch.draw(bottomBigPillarFrontTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.POTION) {
					block.render(batch, modifier);
				}
				else if (block.getType() == Block.CHOPPER) {
					if (worldRenderer.kid.getPos().x < block.getPos().x + 25) {
						modifier = block.renderAndReturnModifier(batch, modifier);
						level.screens.get(getScreenId()).modifiers.set(i * 10 + j, modifier);
					}
				}
//				else if (block.getType() == Block.SPIKES) {
//					if (worldRenderer.kid.getPos().x < block.getPos().x + 30 && worldRenderer.kid.getPos().x > block.getPos().x) {
//						modifier = block.renderAndReturnModifier(batch, modifier);
//						level.screens.get(getScreenId()).modifiers.set(i * 10 + j, modifier);
//					}
//				}
				else if (block.getType() == Block.WALL) {
					batch.draw(wallFrontTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.GATE && block.getRect().overlaps(worldRenderer.kid.getRect())) {
					block.render(batch, level.screens.get(getScreenId()).modifiers.get(i * 10 + j));
				}
				else if (worldRenderer.kid.state == State.FALLING && (Intersector.intersectRectangles(worldRenderer.kid.getRect(), blockRectangle) || Intersector.overlapRectangles(worldRenderer.kid.getRect(), blockRectangle))) {
					block.render(batch);
				}
				
				//XXX Turn this on or off for collision texture overlays
				if (showCollisionOverlays) {
					if (Intersector.intersectRectangles(worldRenderer.kid.getRect(), block.getRect())) {
						batch.draw(blockFrontTexture, block.getPos().x, block.getPos().y);
					}
				}
			}
		}
		
	}

	public void renderLevelScreen(SpriteBatch batch, int id) {
		this.screenId = id;

		List<GeneralBlock> blocks = level.screens.get(screenId).blocks;
		List<Modifier> modifiers = level.screens.get(screenId).modifiers;

		for (int i = 2; i >= 0; i--) {
			for (int j = 0; j <= 9; j++) {
				GeneralBlock block = blocks.get(i * 10 + j);
				Modifier modifier = modifiers.get(i * 10 + j);

				if (block.getType() == Block.LOOSE_BOARD && worldRenderer.kid.getPos().y - GeneralBlock.FLOOR_HEIGHT == block.getPos().y && worldRenderer.kid.standingTime < 0.5 && worldRenderer.kid.standingTime > 0) {
					block.renderAnimOnce(batch, modifier);
				}
//				else if (block.getType() == Block.EMPTY || block.getType() == Block.UNKNOWN) {
//					batch.draw(gemsModifierTexture, block.getPos().x, block.getPos().y);
//				}
				else if (block.getType() == Block.POTION || block.getType() == Block.FLOOR) {
					batch.draw(floorTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.CHOPPER) {
					batch.draw(floorTexture, block.getPos().x, block.getPos().y);
					if (worldRenderer.kid.getPos().x >= block.getPos().x + 25) {
						modifier = block.renderAndReturnModifier(batch, modifier);
						modifiers.set(i * 10 + j, modifier);
					}
				}
				else if (block.getType() == Block.SPIKES) {
					if (worldRenderer.kid.getPos().x >= block.getPos().x && worldRenderer.kid.getPos().x <= block.getRect().x + block.getRect().width) {
						modifiers.set(i * 10 + j, Modifier.SPIKE_FULLY_OUT_2);
					}
					else {
						modifiers.set(i * 10 + j, Modifier.SPIKE_NORMAL);
					}
					
//					if (worldRenderer.kid.getPos().x >= block.getPos().x + 30 || worldRenderer.kid.getPos().x <= block.getPos().x) {
					modifier = block.renderAndReturnModifier(batch, modifier);
//					}
				}
				else if (block.getType() == Block.GATE) {
					modifier = block.renderAndReturnModifier(batch, modifier);
					modifiers.set(i * 10 + j, modifier);
				}
				else if (block.getType() == Block.EXIT_RIGHT && modifier == Modifier.GATE_OPEN) {
					batch.draw(exitRightStairsTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.EXIT_LEFT && modifier == Modifier.GATE_OPEN) {
					batch.draw(exitLeftStairsTexture, block.getPos().x, block.getPos().y);
				}
				else if (block.getType() == Block.RAISE_BUTTON || block.getType() == Block.DROP_BUTTON) {
					if (modifier == Modifier.BUTTON_PRESSED) {
						batch.draw(floorTexture, block.getPos().x, block.getPos().y);
					}
					else {
						block.render(batch, modifier);					
					}
				}
				else {
					block.render(batch, modifier);
				}
			}
		}
	}

	public void renderLeftScreenLink(SpriteBatch batch) {
		List<GeneralBlock> blocksLeft = null;
		List<Modifier> modifiersLeft = null;
		boolean renderLeft = false;
		
		if (level.screens.get(screenId).linkLeft != null) {
			blocksLeft = level.screens.get(screenId).linkLeft.blocks;
			modifiersLeft = level.screens.get(screenId).linkLeft.modifiers;
			renderLeft = true;
		}
		
		//render left screen first
		if (renderLeft) {
			for (int i = 2; i >= 0; i--) {
				GeneralBlock blockLeft = blocksLeft.get(i * 10 + 9);
				Modifier modifierLeft = modifiersLeft.get(i * 10 + 9);
				
				if (blockLeft.getType() == Block.FLOOR || blockLeft.getType() == Block.POTION || blockLeft.getType() == Block.CHOPPER) {
					batch.draw(floorTexture, -GeneralBlock.BLOCK_WIDTH, blockLeft.getPos().y);
				}
				else if (blockLeft.getType() == Block.GATE || blockLeft.getType() == Block.SPIKES) {
					modifierLeft = blockLeft.renderAndReturnModifier(batch, modifierLeft, -GeneralBlock.BLOCK_WIDTH, blockLeft.getPos().y);
					modifiersLeft.set(i * 10 + 9, modifierLeft);
				}
				else {
					blockLeft.render(batch, modifierLeft, -GeneralBlock.BLOCK_WIDTH, blockLeft.getPos().y);
				}
			}
		}
		else {//No room on the left
			for (int i = 0; i <= 2; i++) {
				batch.draw(wallTexture, -GeneralBlock.BLOCK_WIDTH, GeneralBlock.BLOCK_HEIGHT * i);
			}
		}
	}
	
	public void renderBottomScreenLink(SpriteBatch batch) {
		List<GeneralBlock> blocksBottom = null;
		List<Modifier> modifiersBottom = null;
		boolean renderBottom = false;
		
		if (level.screens.get(screenId).linkBottom != null) {
			blocksBottom = level.screens.get(screenId).linkBottom.blocks;
			modifiersBottom = level.screens.get(screenId).linkBottom.modifiers;
			renderBottom = true;
		}
		
		//render bottom screen next
		if (renderBottom) {
			for (int j = 0; j <= 9; j++) {
				GeneralBlock blockBottom = blocksBottom.get(j);
				Modifier modifierBottom = modifiersBottom.get(j);
				
				blockBottom.render(batch, modifierBottom, blockBottom.getPos().x, -GeneralBlock.BLOCK_HEIGHT);
			}
		}
	}
	
	public void renderBrokenBoards(SpriteBatch batch) {
		for (BrokenBoard brokenBoard : brokenBoards) {
			if (brokenBoard.getScreenId() == this.screenId) {
				brokenBoard.render(batch, null, brokenBoard.getPos().x - GeneralBlock.BROKEN_BOARD_OFFSET_LEFT / 2, brokenBoard.getPos().y);
			}
			else if (level.screens.get(screenId).linkLeft != null) {
				if (brokenBoard.getScreenId() == level.screens.get(screenId).linkLeft.id - 1) {
					brokenBoard.render(batch, null, -GeneralBlock.NEGATIVE_OFFSET_LEFT - GeneralBlock.BROKEN_BOARD_OFFSET_LEFT / 2, brokenBoard.getPos().y);
				}
			}
		}
	}
	
	public void dropBrokenBoard(SpriteBatch batch, int x, int y, int screenId) {
		BrokenBoard brokenBoard = new BrokenBoard(x, y);
		brokenBoard.setScreenId(screenId);
		brokenBoards.add(brokenBoard);
	}
	
	public int getScreenId() {
		return screenId;
	}

	public void setScreenId(int screenId) {
		this.screenId = screenId;
	}
	
	public void dispose() {
		pillarFrontTexture.getTexture().dispose();
		wallFrontTexture.getTexture().dispose();
		wallTexture.getTexture().dispose();
		floorTexture.getTexture().dispose();
		blockFrontTexture.getTexture().dispose();
		topBigPillarFrontTexture.getTexture().dispose();
		bottomBigPillarFrontTexture.getTexture().dispose();
		exitRightStairsTexture.getTexture().dispose();
		exitLeftStairsTexture.getTexture().dispose();
		
		//Experimental
		for (LevelScreen screen : level.screens) {
			for (GeneralBlock block : screen.blocks) {
				if (block.getTexture() != null && block.getTexture().getTexture() != null) {
					block.dispose();
				}
			}
		}
	}
}
