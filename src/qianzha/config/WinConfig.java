package qianzha.config;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.swing.UIManager;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.annotation.JSONField;
import com.alibaba.fastjson.serializer.SerializerFeature;

import qianzha.frame.ConsolePanel;
import qianzha.frame.MainFrame;

public class WinConfig implements IQzSavableConfig {
	public static final int RS_NONEED = 0;
	public static final int RS_LAF = 1;
	@JSONField(serialize = false)
	private int restart;
	
	/**
	 * bit mask
	 * <ul>
	 * <li><code>RS_NONEED</code>
	 * <li><code>RS_LAF</code><br>
	 * Look&Feel have Changed.
	 * </ul>
	 */
	public int getRestart() {
		return restart;
	}
	/**
	 * @see #getRestart()
	 */
	public void setRestart(int restart) {
		this.restart = restart;
	}
	public WinConfig() {
		setDefaultConfig();
	}
	
	@JSONField(name="Look&Feel")
	private String lookAndFeel;
	@JSONField(name="Console")
	private ConsoleConfig console;
	
	public String getLookAndFeel() {
		return lookAndFeel;
	}
	public static String getDefaultLAF() {
		return UIManager.getSystemLookAndFeelClassName();
	}
	public void setLookAndFeel(String lookAndFeel) {
		this.lookAndFeel = lookAndFeel;
	}
	public ConsoleConfig getConsole() {
		return console;
	}
	public void setConsole(ConsoleConfig console) {
		this.console = console;
	}
	
	
	public void setDefaultConfig() {
		lookAndFeel = getDefaultLAF();
		console = new ConsoleConfig();
	}
	
	public static WinConfig getConfig(IToConfig itc) {
		WinConfig ret = new WinConfig();
		ret.lookAndFeel = UIManager.getLookAndFeel().getClass().getName();
		ret.setConfig(itc);
		return ret;
	}
	
	public void setConfig(IToConfig itc) {
		ConsolePanel cp = itc.getConsolePanel();
		MainFrame f = itc.getFrame();
		console = ConsoleConfig.getConfig(itc);
		cp.appendTestMsg(""+f.getExtendedState());
		cp.appendTestMsg(""+f.getSize());
		cp.appendTestMsg(""+f.getPreferredSize());
	}
	
	public void doConfig(IToConfig toConfig){
		console.doConfig(toConfig);
	}
	
	
	public void save(OutputStream os) throws IOException {
		JSON.writeJSONString(os, IQzSavableConfig.DEFAULT_CHARSET, this, SerializerFeature.PrettyFormat);
	}
	
	public String toJSONString() {
		return JSON.toJSONString(this, SerializerFeature.PrettyFormat);
	}
	
	public static WinConfig read(InputStream is) throws IOException {
		WinConfig ret = JSON.parseObject(is, IQzSavableConfig.DEFAULT_CHARSET, WinConfig.class);
		return ret;
	}
	
	public static WinConfig read(String json) {
		WinConfig ret = new WinConfig();
		ret = JSON.parseObject(json, WinConfig.class);
		return ret;
	}
	
	public boolean needToRestart() {
		return this.lookAndFeel != UIManager.getLookAndFeel().getClass().getName();
	}
	
}
