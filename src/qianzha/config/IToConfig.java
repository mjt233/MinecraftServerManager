package qianzha.config;


import qianzha.frame.ConsolePanel;
import qianzha.frame.MainFrame;

public interface IToConfig {
	ConsolePanel getConsolePanel();
	MainFrame getFrame();
	void setServer(ServerConfig s);
}
