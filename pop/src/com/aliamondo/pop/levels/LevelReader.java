package com.aliamondo.pop.levels;

import com.aliamondo.pop.graphics.blocks.*;
import com.aliamondo.pop.graphics.blocks.Torch.Type;
import com.badlogic.gdx.Gdx;
import com.badlogic.gdx.files.FileHandle;

public class LevelReader {
	Level level;

	byte content[];
	final int OFFSET = 19;
	final int ROOM_LINKS_OFFSET = 1952;
	final int START_LOCATION_OFFSET = 2112;
	final int DOORS_1_LOCATION_OFFSET = 1440;
	final int DOORS_2_LOCATION_OFFSET = 1696;

	public Level readLevel(String levelNum) {

		try {
			FileHandle file = Gdx.files.internal("levels/level"+ levelNum +".plv");
			content = file.readBytes();
		}
		catch(Exception e) {
			System.out.println(e.getMessage());
		}

		level = new Level();

		int i, j;
		
		//LENGTH	OFFSET (+19)	DEFINITION
		//	24		2119			guardPosition (1E or 30, if none)
		//	24		2143			guardDirection (00: right; FF: left)

		//Read the blocks and modifiers
		for (i = 0; i < 24; i++) {
			LevelScreen s = level.addScreen();

			for (j = 0; j < 30; j++) {
				Block block = (Block.get((content[i * 30 + j + OFFSET]) % 0x20 & 0xff));
				
				if (block.group == BlockGroup.EVENT) {
					// Otherwise, the event is not saved
					s.blocks.add(getBlockType(block, j, content[i * 30 + 720 + j + OFFSET] & 0xff));
				}
				else {
					s.blocks.add(getBlockType(block, j));
				}
				
				s.modifiers.add(Modifier.get(block.group, content[i * 30 + 720 + j + OFFSET] & 0xff));
			}
		}
		
		//Read the room links, there are 24 rooms
		for (i = 0; i < 24; i++) {
			if (content[i * 4 + 0 + OFFSET + ROOM_LINKS_OFFSET] != 0) {
				level.screens.get(i).linkLeft = level.screens.get(content[i * 4 + 0 + OFFSET + ROOM_LINKS_OFFSET] - 1);
			} else {
				level.screens.get(i).linkLeft = null;
			}
			
			if (content[i * 4 + 1 + OFFSET + ROOM_LINKS_OFFSET] != 0) {
				level.screens.get(i).linkRight = level.screens.get(content[i * 4 + 1 + OFFSET + ROOM_LINKS_OFFSET] - 1);
			} else {
				level.screens.get(i).linkRight = null;
			}
			
			if (content[i * 4 + 2 + OFFSET + ROOM_LINKS_OFFSET] != 0) {
				level.screens.get(i).linkTop = level.screens.get(content[i * 4 + 2 + OFFSET + ROOM_LINKS_OFFSET] - 1);
			} else {
				level.screens.get(i).linkTop = null;
			}
			
			if (content[i * 4 + 3 + OFFSET + ROOM_LINKS_OFFSET] != 0) {
				level.screens.get(i).linkBottom = level.screens.get(content[i * 4 + 3 + OFFSET + ROOM_LINKS_OFFSET] - 1);
			} else {
				level.screens.get(i).linkBottom = null;
			}
		}
		
		//Read the start location data
		level.startScreenId = content[OFFSET + START_LOCATION_OFFSET] - 1;
		level.startBlock = content[OFFSET + START_LOCATION_OFFSET + 1];
		level.startLookingLeft = (content[OFFSET + START_LOCATION_OFFSET + 2] & 0xff) == 0xff;
		
		for (i = 0; i < 256; i++) {
			level.addEvent(content[i + OFFSET + DOORS_1_LOCATION_OFFSET], content[i + OFFSET + DOORS_2_LOCATION_OFFSET]);
		}
		
		return level;

		//File file = fileHandle.file();
		/*InputStream fist = fileHandle.read();
			int f = (int) fileHandle.length();
			byte cont[] = new byte[f];

			fist.read(cont);
			fist.close();

			ByteBuffer buffer = ByteBuffer.wrap(cont);
			buffer.order(ByteOrder.LITTLE_ENDIAN);
		 */
		/*FileInputStream fin = new FileInputStream(file);
			int fileSize = (int) file.length();
			byte content[] = new byte[fileSize];

			fin.read(content);
			fin.close();

			ByteBuffer buffer = ByteBuffer.wrap(content);
			buffer.order(ByteOrder.LITTLE_ENDIAN);
		 */
		/*int i=1, j=1, k=0;
			for (byte c : cont) {
				System.out.print(String.format("0x%02X ", c));
				i++;

				if (i == 10) {
					i = 0;
					j++;
					System.out.println();
					if (j%3== 0 && j <= 24 * 3) {
						System.out.println(j/3);
					} else if(j == 24 * 3 + 3) {
						k++;
						//if (k==2) break;

						j = 3;
						System.out.println(j/3);
					}
				}
			}*/
	}
	
	private GeneralBlock getBlockType(Block block, int pos, int eventNum) {
		int x = pos % 10, y = pos / 10;
		
		switch(block) {
		case DROP_BUTTON:
			return new DropButton(x, y, eventNum);
		case RAISE_BUTTON:
			return new RaiseButton(x, y, eventNum);
		default:
			return new Unknown(x, y);
		}
	}

	private GeneralBlock getBlockType(Block block, int pos) {
		int x = pos % 10, y = pos / 10;

		switch(block) {
		case EMPTY:
			return new Empty(x, y);
		case BALCONY_LEFT:
			//TODO fix this
			return new Floor(x, y);
		case BALCONY_RIGHT:
			//TODO fix this
			return new Floor(x, y);
		case BOTTOM_BIG_PILLAR:
			return new BottomBigPillar(x, y);
		case CHOPPER:
			return new Chopper(x, y);
		case DEBRIS:
			return new Debris(x, y);
		//case DROP_BUTTON:
		//	break;
		case EXIT_LEFT:
			return new ExitLeft(x, y);
		case EXIT_RIGHT:
			return new ExitRight(x, y);
		case FLOOR:
			return new Floor(x, y);
		case GATE:
			return new Gate(x, y);
		case LATTICE_LEFT:
			break;
		case LATTICE_PILLAR:
			//TODO fix this
			return new BottomBigPillar(x, y);
		case LATTICE_RIGHT:
			break;
		case LATTICE_SUPPORT:
			//TODO fix this
			return new TopBigPillar(x, y);
		case LOOSE_BOARD:
			return new LooseBoard(x, y);
		case MIRROR:
			break;
		case PILLAR:
			return new Pillar(x, y);
		case POTION:
			return new Potion(x, y);
		//case RAISE_BUTTON:
		//	break;
		case SKELETON:
			return new Skeleton(x, y);
		case SMALL_LATTICE:
			break;
		case SPIKES:
			return new Spikes(x, y);
		case STUCK_BUTTON:
			//TODO fix this
			return new Floor(x, y);
		case SWORD:
			return new Sword(x, y);
		case TAPESTRY:
			//TODO fix this
			return new Wall(x, y);
		case TAPESTRY_TOP:
			//TODO fix this
			return new Wall(x, y);
		case TOP_BIG_PILLAR:
			return new TopBigPillar(x, y);
		case TORCH:
			return new Torch(Type.DEFAULT, x, y);
		case TORCH_WITH_DEBRIS:
			return new Torch(Type.TORCH_WITH_DEBRIS, x, y);
		case UNKNOWN:
			return new Unknown(x, y);
		case WALL:
			return new Wall(x, y);
		case NULL:
			break;
		default:
			break;
		}

		return new Unknown(x, y);
	}
}
