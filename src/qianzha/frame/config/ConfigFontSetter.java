package qianzha.frame.config;

import java.awt.Font;
import java.awt.GraphicsEnvironment;

import javax.swing.JComponent;
import javax.swing.JLabel;


public class ConfigFontSetter implements IConfigSetter<Font>{
	public static final String[] FONT_NAMES = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();
	public static final int[] FONT_STYLE = new int[] {Font.PLAIN, Font.ITALIC, Font.BOLD, Font.BOLD|Font.ITALIC};
	public static String[] FONT_STYLE_NAME = new String[] {"常规", "斜体", "粗体", "粗斜体"};
	
	public ConfigFontSetter() {
		
	}
	
	@Override
	public JComponent getComponent() {
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public JLabel getLabel() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public Font getOldValue() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public Font getValue() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void saveOldValue() {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void saveValue() {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void setDefault() {
		// TODO Auto-generated method stub
		
	}
}
