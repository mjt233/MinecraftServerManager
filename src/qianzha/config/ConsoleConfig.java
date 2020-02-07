package qianzha.config;

import java.awt.Color;
import java.awt.Font;

import com.alibaba.fastjson.annotation.JSONField;

import qianzha.frame.ConsolePanel;

public class ConsoleConfig implements IQzConfig {
	public static final String KEY_BG_COLOR = "BGColor";
	public static final String KEY_FONT_COLOR = "FontColor";
	public static final String KEY_FONT = "Font";
	
	@JSONField(name = KEY_BG_COLOR)
	private Color bgColor;
	@JSONField(name = KEY_FONT_COLOR)
	private Color fontColor;
	@JSONField(name = KEY_FONT)
	private Font font;
	
	
	
	public Color getBgColor() {
		return bgColor;
	}
	public void setBgColor(Color bgColor) {
		this.bgColor = bgColor;
	}
	public Color getFontColor() {
		return fontColor;
	}
	public void setFontColor(Color fontColor) {
		this.fontColor = fontColor;
	}
	public void setFont(Font font) {
		this.font = font;
	}
	public Font getFont() {
		return font;
	}
	
	public ConsoleConfig() {
		setDefaultConfig();
	}
	
//	static ConsoleConfig getDefaultConfig() {
//		ConsoleConfig console = new ConsoleConfig();
//		console.setDefaultConfig();
//		return console;
//	}

	public void setDefaultConfig() {
		this.bgColor = ConsolePanel.DEFAULT_BG_COLOR;
		this.fontColor = ConsolePanel.DEFAULT_FONT_COLOR;
		this.font = ConsolePanel.DEFAULT_FONT;
	}
	
	static public ConsoleConfig getConfig(IToConfig toGet) {
		ConsoleConfig console = new ConsoleConfig();
		console.setConfig(toGet);
		return console;
	}
	
	public void setConfig(IToConfig toGet) {
		ConsolePanel cp = toGet.getConsolePanel();
		this.bgColor = cp.getBackground();
		this.fontColor = cp.getFontColor();
		this.font = cp.getFont();
	}

	public void doConfig(IToConfig itc) {
		ConsolePanel cp = itc.getConsolePanel();
		cp.setBackground(bgColor);
		cp.setFontColor(fontColor);
		cp.setFont(font);
	}
	
}
