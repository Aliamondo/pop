package com.aliamondo.pop.levels;

public class Event {
	public boolean triggersNextEvent = false;
	public int screenNumber = 0;
	public int blockNumber = 0;
	
	public Event(byte door1, byte door2) {
		
		// Move 7 bits to the right
		triggersNextEvent = ((door1 >> 7) & 1) == 0;
		
		// 0x1F = 0b11111. We have 5 bits, so we need to check for 5 ones
		blockNumber = door1 & 0x1F;
		
		// Do similar calculations, but use "& 0x03" to only get 2 bits from the right
		screenNumber = ((door2 >> 3) | ((door1 >>> 5) & 0x03)) & 0x1F;
		// Subtract 1 to make it usable as an index of an ArrayList
		screenNumber -= 1;
	}
}
