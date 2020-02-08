package qianzha.frame.icon;

import javax.swing.ImageIcon;


public class IconQz {
	public static final String PATH_ICON_ROOT = "/qianzha/frame/icon";
	public static final ImageIcon Logo = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/logo.png"));
	public static final ImageIcon Logo_OK = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/logo_ok.png"));
	public static final ImageIcon Logo_WARN = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/logo_warn.png"));
	public static final ImageIcon Logo_ERROR = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/logo_error.png"));
	
	public static final ImageIcon Warning = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/warning16.png"));
	
	public static final ImageIcon Start = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/start16.png"));
	public static final ImageIcon Restart = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/restart16.png"));
	public static final ImageIcon Stop = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/stop16.png"));
	public static final ImageIcon Clear = new ImageIcon(IconQz.class.getResource(PATH_ICON_ROOT + "/clear16.png"));
}
