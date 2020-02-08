package qianzha.frame.config;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import qianzha.config.ServerConfig;
import qianzha.frame.MainFrame;

@SuppressWarnings("serial")
public class ServerSetterPanel extends JPanel {

	private ServerConfig config;
	
	private JPanel panel;
	private JButton btnSet;
	private JCheckBox showAuto;

	private GridBagConstraints gbc_showIp;
	private GridBagConstraints gbc_showPort;
	private GridBagConstraints gbc_showSerId;
	private GridBagConstraints gbc_showUsrId;

	private JLabel showIp;
	private JLabel showPort;
	private JLabel showSerId;
	private JLabel showUsrId;
	
	private JTextField inIp;
	private JTextField inPort;
	private JTextField inSerId;
	private JTextField inUsrId;
	
	private ServerSetterPanel(ServerConfig config) {
		this.config = config;
		GridBagLayout gridBagLayout = new GridBagLayout();
		gridBagLayout.columnWidths = new int[]{0, 0, 0, 0};
		gridBagLayout.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0};
		gridBagLayout.columnWeights = new double[]{1.0, 1.0, 0.0, Double.MIN_VALUE};
		gridBagLayout.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, Double.MIN_VALUE};
		setLayout(gridBagLayout);
		
		JLabel IPLabel = new JLabel("ServerIP: ");
		GridBagConstraints gbc_IPLabel = new GridBagConstraints();
		gbc_IPLabel.anchor = GridBagConstraints.EAST;
		gbc_IPLabel.insets = new Insets(0, 0, 5, 5);
		gbc_IPLabel.gridx = 0;
		gbc_IPLabel.gridy = 1;
		add(IPLabel, gbc_IPLabel);
		
		JLabel portLabel = new JLabel("Port: ");
		GridBagConstraints gbc_portLabel = new GridBagConstraints();
		gbc_portLabel.anchor = GridBagConstraints.EAST;
		gbc_portLabel.insets = new Insets(0, 0, 5, 5);
		gbc_portLabel.gridx = 0;
		gbc_portLabel.gridy = 2;
		add(portLabel, gbc_portLabel);
		
		JLabel serIDLabel = new JLabel("SerID: ");
		GridBagConstraints gbc_serIDLabel = new GridBagConstraints();
		gbc_serIDLabel.anchor = GridBagConstraints.EAST;
		gbc_serIDLabel.insets = new Insets(0, 0, 5, 5);
		gbc_serIDLabel.gridx = 0;
		gbc_serIDLabel.gridy = 3;
		add(serIDLabel, gbc_serIDLabel);
		
		JLabel UsrIDLabel = new JLabel("UsrID: ");
		GridBagConstraints gbc_UsrIDLabel = new GridBagConstraints();
		gbc_UsrIDLabel.anchor = GridBagConstraints.EAST;
		gbc_UsrIDLabel.insets = new Insets(0, 0, 5, 5);
		gbc_UsrIDLabel.gridx = 0;
		gbc_UsrIDLabel.gridy = 4;
		add(UsrIDLabel, gbc_UsrIDLabel);
		
		panel = new JPanel();
		GridBagConstraints gbc_panel = new GridBagConstraints();
		gbc_panel.anchor = GridBagConstraints.NORTHEAST;
		gbc_panel.gridwidth = 3;
		gbc_panel.insets = new Insets(0, 0, 0, 5);
		gbc_panel.gridx = 0;
		gbc_panel.gridy = 5;
		add(panel, gbc_panel);
		
		gbc_showIp = new GridBagConstraints();
		gbc_showIp.fill = GridBagConstraints.HORIZONTAL;
		gbc_showIp.anchor = GridBagConstraints.WEST;
		gbc_showIp.insets = new Insets(0, 0, 5, 5);
		gbc_showIp.gridx = 1;
		gbc_showIp.gridy = 1;
		
		gbc_showPort = new GridBagConstraints();
		gbc_showPort.fill = GridBagConstraints.HORIZONTAL;
		gbc_showPort.anchor = GridBagConstraints.WEST;
		gbc_showPort.insets = new Insets(0, 0, 5, 5);
		gbc_showPort.gridx = 1;
		gbc_showPort.gridy = 2;
		
		gbc_showSerId = new GridBagConstraints();
		gbc_showSerId.fill = GridBagConstraints.HORIZONTAL;
		gbc_showSerId.anchor = GridBagConstraints.WEST;
		gbc_showSerId.insets = new Insets(0, 0, 5, 5);
		gbc_showSerId.gridx = 1;
		gbc_showSerId.gridy = 3;
		
		gbc_showUsrId = new GridBagConstraints();
		gbc_showUsrId.fill = GridBagConstraints.HORIZONTAL;
		gbc_showUsrId.anchor = GridBagConstraints.WEST;
		gbc_showUsrId.fill = GridBagConstraints.VERTICAL;
		gbc_showUsrId.insets = new Insets(0, 0, 5, 5);
		gbc_showUsrId.gridx = 1;
		gbc_showUsrId.gridy = 4;
		
		
		showAuto = new JCheckBox("自动连接");
		panel.add(showAuto);
	}

	public static ServerSetterPanel getShowPane(ServerConfig config) {
		ServerSetterPanel ssp = new ServerSetterPanel(config);
		ssp.initShowPane();
		ssp.initInfo();
		return ssp;
	}
	
	public static void showSetDialog(ServerConfig config) {
		JDialog dd = new JDialog(MainFrame.INSTANCE, "服务器设置");
		
		ServerSetterPanel ssp = new ServerSetterPanel(config);
		ssp.initSetPane(dd);
		ssp.initInfo();
		
		dd.setContentPane(ssp);
		dd.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
		dd.pack();
		dd.setLocationRelativeTo(MainFrame.INSTANCE);
		dd.setVisible(true);
	}
	
	private void initShowPane() {
		showIp = new JLabel();
		add(showIp, gbc_showIp);
		
		showPort = new JLabel();
		add(showPort, gbc_showPort);
		
		showSerId = new JLabel();
		add(showSerId, gbc_showSerId);
		
		showUsrId = new JLabel();
		add(showUsrId, gbc_showUsrId);
		
		showAuto.setEnabled(false);
		
		btnSet = new JButton("设置");
		btnSet.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				showSetDialog(config);
			}
		});
		panel.add(btnSet);
	}

	private void initSetPane(JDialog dd) {
		inIp = new JTextField();
		add(inIp, gbc_showIp);
		
		inPort = new JTextField();
		add(inPort, gbc_showPort);
		
		inSerId = new JTextField();
		add(inSerId, gbc_showSerId);
		
		inUsrId = new JTextField();
		add(inUsrId, gbc_showUsrId);
		
		showAuto.setEnabled(true);
		
		JButton btnSave = new JButton("保存");
		btnSave.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				config.setAddress(inIp.getText());
				config.setPort(Integer.parseInt(inPort.getText()));
				config.setServerID(Integer.parseInt(inPort.getText()));
				config.setUserID(Integer.parseInt(inUsrId.getText()));
				config.auto = showAuto.isSelected();
			}
		});
		panel.add(btnSave);
		
		JButton btnCancel = new JButton("取消");
		btnCancel.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				dd.dispose();
			}
		});
		panel.add(btnCancel);
	}
	
	private void initInfo() {
		showAuto.setSelected(config.auto);
		if(showAuto.isEnabled()) {
			inIp.setText(config.getAddress());
			inPort.setText(""+config.getPort());
			inSerId.setText(""+config.getServerID());
			inUsrId.setText(""+config.getUserID());
		}
		else {
			showIp.setText(config.getAddress());
			showPort.setText(""+config.getPort());
			showSerId.setText(""+config.getServerID());
			showUsrId.setText(""+config.getUserID());
		}
	}
}
