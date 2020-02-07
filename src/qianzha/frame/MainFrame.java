package qianzha.frame;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.prefs.Preferences;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.EmptyBorder;

import mjt233.msmc.AccessRefuseException;
import mjt233.msmc.ILinkListener;
import mjt233.msmc.InfoShower;
import qianzha.config.IToConfig;
import qianzha.config.ServerConfig;
import qianzha.config.WinConfig;
import qianzha.frame.config.ConfigPanel;
import qianzha.frame.icon.IconQz;
import javax.swing.SwingConstants;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.Insets;
import javax.swing.JButton;

@SuppressWarnings("serial")
public class MainFrame extends JFrame implements IToConfig{
	public static final File PATH_ROOT;
	public static final File PATH_CONFIG;
	public static final File PATH_CRASH_REPORT;
	public static final File FILE_CONFIG_WIN;
	public static final File FILE_SERVER_CONFIG;
	static {
		PATH_ROOT = new File(System.getProperty("user.dir").replace('\\', '/'));
		PATH_CONFIG = new File(PATH_ROOT, "config");
		PATH_CRASH_REPORT = new File(PATH_ROOT, "carsh-report");
		
		FILE_CONFIG_WIN = new File(PATH_CONFIG, "windows.json");
		FILE_SERVER_CONFIG = new File(PATH_CONFIG, "server.json");
	}
	private JPanel contentPane;
	private ConsolePanel cp;
	private WinConfig config;
	private ServerConfig server;
	private ConfigPanel setPanel;
	private Thread linking;
	private JPanel setLinking;
	private JPanel statesPanel;
	private JLabel linkingStates;
	private JLabel serverInfo;
	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainFrame frame = new MainFrame();
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
					crashReport(e);
				}
			}
		});
	}

	protected static void crashReport(Exception e) {
		File crFolder = PATH_CRASH_REPORT;
		if(!crFolder.exists()) crFolder.mkdirs();
		String date = new SimpleDateFormat("yyyy-MM-dd_HH.mm.ss").format(new Date());
		File crFile = new File(crFolder, "crash-"+date+".txt");
		try {
			if(!crFile.exists()) crFile.createNewFile();
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
		try {
			PrintStream ps = new PrintStream(crFile);
			e.printStackTrace(ps);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
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
			setLinking = new JPanel();
			tabbedPane.addTab("连接配置", null, setLinking, null);
			GridBagLayout gbl_setLinking = new GridBagLayout();
			gbl_setLinking.columnWidths = new int[]{69, 0};
			gbl_setLinking.rowHeights = new int[]{15, 0, 0, 0};
			gbl_setLinking.columnWeights = new double[]{1.0, Double.MIN_VALUE};
			gbl_setLinking.rowWeights = new double[]{1.0, 1.0, 0.0, Double.MIN_VALUE};
			setLinking.setLayout(gbl_setLinking);
			{
				lblNewLabel = new JLabel("别看了这啥都没有，去配置文件改啦！");
				lblNewLabel.setHorizontalAlignment(SwingConstants.CENTER);
				GridBagConstraints gbc_lblNewLabel = new GridBagConstraints();
				gbc_lblNewLabel.anchor = GridBagConstraints.SOUTH;
				gbc_lblNewLabel.insets = new Insets(0, 0, 5, 0);
				gbc_lblNewLabel.gridx = 0;
				gbc_lblNewLabel.gridy = 0;
				setLinking.add(lblNewLabel, gbc_lblNewLabel);
			}
			{
				lblNewLabel_1 = new JLabel("\"\\config\\server.json\" (暂时)");
				GridBagConstraints gbc_lblNewLabel_1 = new GridBagConstraints();
				gbc_lblNewLabel_1.insets = new Insets(0, 0, 5, 0);
				gbc_lblNewLabel_1.anchor = GridBagConstraints.NORTH;
				gbc_lblNewLabel_1.gridx = 0;
				gbc_lblNewLabel_1.gridy = 1;
				setLinking.add(lblNewLabel_1, gbc_lblNewLabel_1);
			}
			{
				panel = new JPanel();
				GridBagConstraints gbc_panel = new GridBagConstraints();
				gbc_panel.fill = GridBagConstraints.BOTH;
				gbc_panel.gridx = 0;
				gbc_panel.gridy = 2;
				setLinking.add(panel, gbc_panel);
				{
					btnNewButton = new JButton("（重新）连接");
					btnNewButton.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent e) {
							linkStart();
						}
					});
					panel.add(btnNewButton);
				}
				{
					btnNewButton_1 = new JButton("断开");
					btnNewButton_1.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent e) {
							new Thread(new Runnable() {
								public void run() {
									linkStop();
								}
							}).start();
						}
					});
					panel.add(btnNewButton_1);
				}
			}
		}
		
		{
			setPanel = new ConfigPanel(config, this);
			setPanel.setName("设置");
			tabbedPane.addTab(setPanel.getName(), null, setPanel, setPanel.getName());
		}
		{
			statesPanel = new JPanel();
			FlowLayout fl_statesPanel = (FlowLayout) statesPanel.getLayout();
			fl_statesPanel.setVgap(0);
			fl_statesPanel.setAlignment(FlowLayout.LEFT);
			contentPane.add(statesPanel, BorderLayout.SOUTH);
			{
				linkingStates = new JLabel("[未连接]");
				statesPanel.add(linkingStates);
			}
			{
				serverInfo = new JLabel("IP: [127.0.0.1:6636], SID:1, UID:12345");
				statesPanel.add(serverInfo);
			}
		}
		initLater();
	}
	
	private void initConfig() {
		File[] cfs = new File[] {FILE_CONFIG_WIN, FILE_SERVER_CONFIG};
		for(int i=0; i<cfs.length; i++) {
			if(cfs[i].exists()) {
				try {
					FileInputStream cis = new FileInputStream(cfs[i]);
					switch(i) {
					case 0: 
						config = WinConfig.read(cis);
						if(config == null) config = new WinConfig();
						break;
					case 1: 
						server = ServerConfig.read(cis);
						if(server == null) server = new ServerConfig();
						break;
					}
					cis.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			else {
				switch (i) {
				case 0: config = new WinConfig(); break;
				case 1: server = new ServerConfig(); break;
				}
			}
			String laf = config.getLookAndFeel();
			try {
				UIManager.setLookAndFeel(laf);
			} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
					| UnsupportedLookAndFeelException e) {
				e.printStackTrace();
			}
		}
		saveConfig();
	}
	private void initLater() {
		setServer(server);
	}
	
	public void saveConfig() {
		File cFolder = PATH_CONFIG;
		if(!cFolder.exists()) cFolder.mkdirs(); 
		File[] cfs = new File[] {FILE_CONFIG_WIN, FILE_SERVER_CONFIG};
		for(int i=0; i<cfs.length; i++) {
			try {
				if(!cfs[i].exists()) {
					cfs[i].createNewFile();
				}
				FileOutputStream os = new FileOutputStream(cfs[i]);
				switch(i) {
				case 0: config.save(os); break;
				case 1: server.save(os); break;
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
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

	public WinConfig getConfig() {
		return config;
	}
	
	public void setServer(ServerConfig s) {
		this.server = s;
		serverInfo.setText("IP: [" + server.getAddress() + ":" + server.getPort()
		+ "], SID:" + server.getServerID() + ", UID:" + server.getUserID());
		linkStop();
		if(s.auto) linkStart();
	}
	
	private ILinkListener linkListener = new ILinkListener() {
		public void linkSucceeded() {
			cp.appendMsgLine("连接成功\n");
			linkingStates.setText("[已连接]");
		}
		public void linkFailed() {
			cp.appendMsgLine("连接失败\n");
			linkingStates.setText("[连接失败]");
		}
		public void linkRefused(String msg) {
			cp.appendMsgLine("接入失败,原因:"+msg+"\n");
			linkingStates.setText("[接入失败]");
		}
		public void readSucceeded(String readed) {
			cp.appendMsgLine(readed);
		}
		public void linkBreak() {
			cp.appendMsgLine("连接已断开\n");
			linkingStates.setText("[未连接]");
		}
		
	};
	public void linkStop() {
		if(isLinked()) {
			linkingStates.setText("[正在断开]");
			Thread tmp = linking;
			if(!tmp.isInterrupted()) {
				tmp.interrupt();
			}
			while(tmp.isAlive()) {};
		}
	}
	
	private JLabel lblNewLabel;
	private JLabel lblNewLabel_1;
	private JPanel panel;
	private JButton btnNewButton;
	private JButton btnNewButton_1;
	public void linkStart() {
		new Thread(new Runnable() {
			public void run() {
				linkStop();
				InfoShower is;
				try {
					is = new InfoShower(server.getAddress(), server.getPort(), server.getServerID(), server.getUserID(),
							InfoShower.CONTROLLER, linkListener);
				
					cp.addSenders(new IConsoleSender() {
						@Override
						public void send(ActionEvent e, ConsolePanel cp, String text) {
							String str = text+"\n";
							try {
								is.ac.write(0x0, str.getBytes());
							} catch (IOException e1) {
								cp.appendMsg("SenderError", "-------信息发送失败-------");
							}
						}
					});
					linking = new Thread(is) {
						@Override
						public void interrupt() {
							super.interrupt();
							try {
								is.ac.stop();
							} catch (IOException e) {
								e.printStackTrace();
							}
						}
					};
					linking.start();
				} catch (IOException | AccessRefuseException e2) {
					// TODO Auto-generated catch block
					e2.printStackTrace();
				}
			}
		}).start();
	}
	
	public boolean isLinked() {
		if(linking == null) return false;
		if(linking.isAlive()) return true;
		else return false;
	}
	
	
}
