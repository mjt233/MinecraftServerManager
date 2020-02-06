package qianzha.config;

import java.awt.Color;
import java.awt.Font;

import com.alibaba.fastjson.annotation.JSONField;

import qianzha.frame.ConsolePanel;

public class ConsoleConfig {
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
	private ConsoleConfig() {
	}
	
	static ConsoleConfig getDefaultConfig() {
		ConsoleConfig console = new ConsoleConfig();
		console.bgColor = ConsolePanel.DEFAULT_BG_COLOR;
		console.fontColor = ConsolePanel.DEFAULT_FONT_COLOR;
		console.font = ConsolePanel.DEFAULT_FONT;
		return console;
	}
	
	static public ConsoleConfig getConsoleConfig(ConsolePanel cp) {
		ConsoleConfig console = new ConsoleConfig();
		console.setConfig(cp);
		return console;
	}
	
	public void setConfig(ConsolePanel cp) {
		this.bgColor = cp.getBackground();
		this.fontColor = cp.getFontColor();
		this.font = cp.getFont();
	}
	
	public void doConfig(ConsolePanel cp) {
		cp.setBackground(bgColor);
		cp.setFontColor(fontColor);
		cp.setFont(font);
	}
	
}
