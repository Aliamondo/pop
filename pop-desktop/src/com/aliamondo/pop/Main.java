package com.aliamondo.pop;

import com.badlogic.gdx.Files.FileType;
import com.badlogic.gdx.backends.lwjgl.LwjglApplication;
import com.badlogic.gdx.backends.lwjgl.LwjglApplicationConfiguration;

public class Main {
	public static void main(String[] args) {
		LwjglApplicationConfiguration cfg = new LwjglApplicationConfiguration();
		cfg.title = "Prince of Persia REMAKE";
		cfg.useGL20 = false;
		cfg.width = 800;
		cfg.height = 480;
		cfg.addIcon("icon.png", FileType.Internal);
		//System.setProperty("org.lwjgl.opengl.Display.allowSoftwareOpenGL", "true");
		
		new LwjglApplication(new PoP(), cfg);
	}
}
