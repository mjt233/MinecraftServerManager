package qianzha.frame;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.prefs.Preferences;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.EmptyBorder;

import mjt233.msmc.InfoShower;
import qianzha.config.IToConfig;
import qianzha.config.QzConfig;
import qianzha.frame.config.ConfigPanel;
import qianzha.frame.icon.IconQz;

@SuppressWarnings("serial")
public class MainFrame extends JFrame implements IToConfig{
	public static final String PATH_ROOT;
	public static final String PATH_CONFIG;
	static {
		PATH_ROOT = System.getProperty("user.dir").replace('\\', '/');
		PATH_CONFIG = PATH_ROOT + "/config/windows.json";
	}
	private JPanel contentPane;
	private ConsolePanel cp;
	private QzConfig config;
	private ConfigPanel setPanel;
	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainFrame frame = new MainFrame();
					ConsolePanel console = frame.getConsolePanel();
					frame.setVisible(true);
					InfoShower is = new InfoShower("www.xiaotao233.top", 6636, 1, 12345, InfoShower.CONTROLLER, console);
					console.addSenders(new IConsoleSender() {
						@Override
						public void send(ActionEvent e, ConsolePanel cp, String text) {
							String str = text+"\n";
							try {
								is.ac.write(0x0, str.getBytes());
							} catch (IOException e1) {
								cp.appendMsg("AccessClientError", "-------信息发送失败-------");
							}
						}
					});
					new Thread(is).start();;
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the frame.
	 */
	public MainFrame() {
		setIconImage(IconQz.Logo.getImage());
		setTitle("MSMClient qz.test");
		initConfig();
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		
		addWindowListener(new WindowListener() {
			static final String LOCA_X = "locaX";
			static final String LOCA_Y = "locaY";
			static final String WIDTH = "width";
			static final String HEIGHT = "height";
			static final String E_State = "eState";
			Preferences pref = Preferences.userNodeForPackage(this.getClass());
			
			@Override
			public void windowOpened(WindowEvent e) {
				config.doConfig(MainFrame.this);
				int x = pref.getInt(LOCA_X, getX());
				int y = pref.getInt(LOCA_Y, getY());
				int width = pref.getInt(WIDTH, getWidth());
				int height = pref.getInt(HEIGHT, getHeight());
				setBounds(x, y, width, height);
				setExtendedState(pref.getInt(E_State, JFrame.NORMAL));
			}
			@Override
			public void windowIconified(WindowEvent e) {
			}
			
			@Override
			public void windowDeiconified(WindowEvent e) {
			}
			
			@Override
			public void windowDeactivated(WindowEvent e) {
			}
			
			@Override
			public void windowClosing(WindowEvent e) {
				pref.putInt(E_State, getExtendedState() & ~JFrame.ICONIFIED);
				setExtendedState(JFrame.NORMAL);
			}
			
			@Override
			public void windowClosed(WindowEvent e) {
				pref.putInt(LOCA_X, getX());
				pref.putInt(LOCA_Y, getY());
				pref.putInt(WIDTH, getWidth());
				pref.putInt(HEIGHT, getHeight());
				saveConfig();
				System.exit(0);
			}
			
			@Override
			public void windowActivated(WindowEvent e) {
			}
		});
		setBounds(100, 100, 450, 300);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		contentPane.setLayout(new BorderLayout(0, 0));
		setContentPane(contentPane);
		
		JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.LEFT);
		tabbedPane.setBackground(new Color(0, 0, 0, 0));
		contentPane.add(tabbedPane, BorderLayout.CENTER);
		
		{
			cp = new ConsolePanel();
			cp.setName("控制台面板");
			tabbedPane.addTab(cp.getName(), null, cp, cp.getName());
//			cp.addSenders(new IConsoleSender() {
//				@Override
//				public void send(ActionEvent e, ConsolePanel cp, String text) {
//					cp.appendMsg("Sender Event", new Date(e.getWhen()), text);
//					switch(text) {
//					case "getSize": 
//						cp.appendMsg("Test Command", "Size:"+getSize());
//						break;
//					case "config":
//						cp.appendMsg("Test Command", "Config:");
//						config.setConfig(MainFrame.this);
//						cp.appendMsgLine(config.toJSONString());
//						break;
//					}
//				}
//			});
		}
		
		{
			setPanel = new ConfigPanel(config, this);
			setPanel.setName("设置");
			tabbedPane.addTab(setPanel.getName(), null, setPanel, setPanel.getName());
		}
		
	}
	
	public void initConfig() {
		File fConfig = new File(PATH_CONFIG);
		if(fConfig.exists()) {
			try {
				FileInputStream inConfig = new FileInputStream(fConfig);
				config = QzConfig.read(inConfig);
				inConfig.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		else {
			config = QzConfig.getDefaultConfig();
			saveConfig();
		}
		String laf = config.getLookAndFeel();
		try {
			UIManager.setLookAndFeel(laf);
		} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
				| UnsupportedLookAndFeelException e) {
			e.printStackTrace();
		}
	}
	
	public void saveConfig() {
		FileOutputStream os;
		try {
			File cFile = new File(PATH_CONFIG);
			if(!cFile.exists()) {
				File cFolder = cFile.getParentFile();
				if(!cFolder.exists()) cFolder.mkdirs(); 
				cFile.createNewFile();
			}
			os = new FileOutputStream(PATH_CONFIG);
			config.save(os);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	@Override
	public ConsolePanel getConsolePanel() {
		return this.cp;
	}

	@Override
	public void setSize(int width, int height) {
		super.setSize(width, height);
	}

	@Override
	public MainFrame getFrame() {
		return this;
	}

	public QzConfig getConfig() {
		return config;
	}
	
}
