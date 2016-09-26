package com.aliamondo.pop;

import com.aliamondo.pop.screens.IntroScreen;
import com.badlogic.gdx.Game;

public class PoP extends Game {
	//Icon by CaseyD2K <http://caseyd2k.deviantart.com/>
	@Override
	public void create() {		
		setScreen(new IntroScreen(this, null));
	}
}
