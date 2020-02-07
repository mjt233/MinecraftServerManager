package qianzha.frame.config;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JColorChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTextArea;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;

import qianzha.config.ConsoleConfig;
import qianzha.config.IToConfig;
import qianzha.config.WinConfig;

@SuppressWarnings("serial")
public class ConfigPanel extends JPanel {
	private JTextArea msgArea;
	private WinConfig config;
	private IToConfig toConfig;
	IConfigSetter<?>[] setters;
	/**
	 * Create the panel.
	 */
	public ConfigPanel(WinConfig config, IToConfig toConfig) {
		this.config = config;
		this.toConfig = toConfig;
		setLayout(new BorderLayout(0, 0));
		
		JPanel titlePanel = new JPanel();
		add(titlePanel, BorderLayout.NORTH);
		{
			GridBagLayout gbl_titlePanel = new GridBagLayout();
			gbl_titlePanel.columnWidths = new int[]{0, 100, 0, 0};
			gbl_titlePanel.rowHeights = new int[]{0, 0, 0, 0};
			gbl_titlePanel.columnWeights = new double[]{0.0, 0.0, 1.0, Double.MIN_VALUE};
			gbl_titlePanel.rowWeights = new double[]{0.0, 0.0, 1.0, Double.MIN_VALUE};
			titlePanel.setLayout(gbl_titlePanel);
			{
				JLabel lblNewLabel = new JLabel("设置");
				lblNewLabel.setFont(new Font("Microsoft YaHei UI", Font.PLAIN, 18));
				GridBagConstraints gbc_lblNewLabel = new GridBagConstraints();
				gbc_lblNewLabel.gridwidth = 2;
				gbc_lblNewLabel.insets = new Insets(0, 0, 5, 5);
				gbc_lblNewLabel.gridx = 0;
				gbc_lblNewLabel.gridy = 0;
				titlePanel.add(lblNewLabel, gbc_lblNewLabel);
			}
			{
				JSeparator separator = new JSeparator();
				GridBagConstraints gbc_separator = new GridBagConstraints();
				gbc_separator.gridwidth = 3;
				gbc_separator.fill = GridBagConstraints.HORIZONTAL;
				gbc_separator.insets = new Insets(0, 0, 5, 0);
				gbc_separator.gridx = 0;
				gbc_separator.gridy = 1;
				titlePanel.add(separator, gbc_separator);
			}
		}
		
		
		
		
		
		JPanel setPanel = new JPanel();
		{
			setPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
			add(setPanel, BorderLayout.CENTER);
			add(setPanel, BorderLayout.CENTER);
			GridBagLayout gbl_setPanel = new GridBagLayout();
			gbl_setPanel.columnWidths = new int[]{0, 100, 0, 0, 0, 0, 0, 0};
			gbl_setPanel.rowHeights = new int[]{0, 0, 0, 0, 0, 0, 0};
			gbl_setPanel.columnWeights = new double[]{0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, Double.MIN_VALUE};
			gbl_setPanel.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 1.0, 0.0, Double.MIN_VALUE};
			setPanel.setLayout(gbl_setPanel);

			// Look&Feel
			ConfigLAFSetter laf = new ConfigLAFSetter(config);
			{
				JPanel lafLabel = new JPanel();
				lafLabel.add(laf.getLabel());
				GridBagConstraints gbc_lafLabel = new GridBagConstraints();
				gbc_lafLabel.anchor = GridBagConstraints.EAST;
				gbc_lafLabel.insets = new Insets(0, 0, 5, 5);
				gbc_lafLabel.gridx = 1;
				gbc_lafLabel.gridy = 0;
				setPanel.add(lafLabel, gbc_lafLabel);
				
				JPanel lafCombo = new JPanel();
				lafCombo.add(laf.getComponent());
				GridBagConstraints gbc_lafCombo = new GridBagConstraints();
				gbc_lafCombo.insets = new Insets(0, 0, 5, 5);
				gbc_lafCombo.fill = GridBagConstraints.HORIZONTAL;
				gbc_lafCombo.gridx = 2;
				gbc_lafCombo.gridy = 0;
				setPanel.add(lafCombo, gbc_lafCombo);
				
			}
			
			{
				ConsoleConfig cp = config.getConsole();
				{
					JButton btnApply = new JButton("应用并关闭");
					GridBagConstraints gbc_btnApply = new GridBagConstraints();
					gbc_btnApply.anchor = GridBagConstraints.WEST;
					gbc_btnApply.insets = new Insets(0, 0, 5, 5);
					gbc_btnApply.gridx = 3;
					gbc_btnApply.gridy = 0;
					setPanel.add(btnApply, gbc_btnApply);
					btnApply.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent e) {
							laf.saveValue();
							toConfig.getFrame().dispose();
//							for(IConfigSetter<?> s: setters) {
//								s.saveValue();
//							}
						}
					});
				}
				JPanel consoleP = new JPanel();
				consoleP.setBorder(new TitledBorder(null, "\u63A7\u5236\u9762\u677F", TitledBorder.LEADING, TitledBorder.TOP, null, null));
				GridBagConstraints gbc_consoleP = new GridBagConstraints();
				gbc_consoleP.gridwidth = 5;
				gbc_consoleP.insets = new Insets(0, 0, 5, 5);
				gbc_consoleP.fill = GridBagConstraints.BOTH;
				gbc_consoleP.gridx = 1;
				gbc_consoleP.gridy = 2;
				setPanel.add(consoleP, gbc_consoleP);
				GridBagLayout gbl_consoleP = new GridBagLayout();
				gbl_consoleP.columnWidths = new int[]{0, 0, 0, 0};
				gbl_consoleP.rowHeights = new int[]{0, 0, 0, 0};
				gbl_consoleP.columnWeights = new double[]{1.0, 0.0, 1.0, Double.MIN_VALUE};
				gbl_consoleP.rowWeights = new double[]{1.0, 0.0, 0.0, Double.MIN_VALUE};
				consoleP.setLayout(gbl_consoleP);
				{
					msgArea = new JTextArea();
					msgArea.setEditable(false);
					msgArea.setText("======== Console Panel ========\n[Sample] [12:34:56] 示例文字");
					msgArea.setBackground(config.getConsole().getBgColor());
					msgArea.setForeground(config.getConsole().getFontColor());
					msgArea.setFont(config.getConsole().getFont());
					GridBagConstraints gbc_textArea = new GridBagConstraints();
					gbc_textArea.gridheight = 3;
					gbc_textArea.insets = new Insets(0, 0, 5, 0);
					gbc_textArea.fill = GridBagConstraints.BOTH;
					gbc_textArea.gridx = 2;
					gbc_textArea.gridy = 0;
					consoleP.add(msgArea, gbc_textArea);
				
					JLabel labelBgColor = new JLabel("背景颜色：");
					GridBagConstraints gbc_labelBgColor = new GridBagConstraints();
					gbc_labelBgColor.insets = new Insets(0, 0, 5, 5);
					gbc_labelBgColor.anchor = GridBagConstraints.EAST;
					gbc_labelBgColor.gridx = 0;
					gbc_labelBgColor.gridy = 0;
					consoleP.add(labelBgColor, gbc_labelBgColor);
				
					JButton btnBgColor = new JButton(" ");
					btnBgColor.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent e) {
							Color res = JColorChooser.showDialog(ConfigPanel.this, "颜色", config.getConsole().getBgColor());
							config.getConsole().setBgColor(res);
//							msgArea.setBackground(config.getConsole().getBgColor());
							consoleSampleUpdate();
						}
					});
					btnBgColor.setBackground(cp.getBgColor());
					GridBagConstraints gbc_btnBgColor = new GridBagConstraints();
					gbc_btnBgColor.anchor = GridBagConstraints.WEST;
					gbc_btnBgColor.insets = new Insets(0, 0, 5, 5);
					gbc_btnBgColor.gridx = 1;
					gbc_btnBgColor.gridy = 0;
					consoleP.add(btnBgColor, gbc_btnBgColor);
				
					JLabel labelFontColor = new JLabel("字体颜色：");
					GridBagConstraints gbc_labelFontColor = new GridBagConstraints();
					gbc_labelFontColor.anchor = GridBagConstraints.EAST;
					gbc_labelFontColor.insets = new Insets(0, 0, 5, 5);
					gbc_labelFontColor.gridx = 0;
					gbc_labelFontColor.gridy = 1;
					consoleP.add(labelFontColor, gbc_labelFontColor);
				
					JButton btnFontColor = new JButton(" ");
					btnFontColor.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent e) {
							Color res = JColorChooser.showDialog(ConfigPanel.this, "颜色", config.getConsole().getFontColor());
							config.getConsole().setFontColor(res);
//							msgArea.setForeground(config.getConsole().getFontColor());
							consoleSampleUpdate();
						}
					});
					btnFontColor.setBackground(cp.getFontColor());
					GridBagConstraints gbc_btnFontColor = new GridBagConstraints();
					gbc_btnFontColor.insets = new Insets(0, 0, 5, 5);
					gbc_btnFontColor.anchor = GridBagConstraints.WEST;
					gbc_btnFontColor.gridx = 1;
					gbc_btnFontColor.gridy = 1;
					consoleP.add(btnFontColor, gbc_btnFontColor);
				
					JLabel lblNewLabel_1 = new JLabel("字体样式：");
					GridBagConstraints gbc_lblNewLabel_1 = new GridBagConstraints();
					gbc_lblNewLabel_1.anchor = GridBagConstraints.EAST;
					gbc_lblNewLabel_1.insets = new Insets(0, 0, 0, 5);
					gbc_lblNewLabel_1.gridx = 0;
					gbc_lblNewLabel_1.gridy = 2;
					consoleP.add(lblNewLabel_1, gbc_lblNewLabel_1);
				
					JButton btnNewButton = new JButton("编辑");
					btnNewButton.addActionListener(new ActionListener() {
						public void actionPerformed(ActionEvent e) {
							Font res = QZFontChooser.showDialog(ConfigPanel.this, "字体", config.getConsole().getFont());
							config.getConsole().setFont(res);
//							msgArea.setFont(config.getConsole().getFont());
							consoleSampleUpdate();
						}
					});
					GridBagConstraints gbc_btnNewButton = new GridBagConstraints();
					gbc_btnNewButton.insets = new Insets(0, 0, 0, 5);
					gbc_btnNewButton.anchor = GridBagConstraints.WEST;
					gbc_btnNewButton.gridx = 1;
					gbc_btnNewButton.gridy = 2;
					consoleP.add(btnNewButton, gbc_btnNewButton);
				}
			}
			
			setters = new IConfigSetter<?>[] {
				laf
			};
		}
		JPanel buttonPane = new JPanel();
		add(buttonPane, BorderLayout.SOUTH);
		buttonPane.setLayout(new FlowLayout(FlowLayout.CENTER));
		{
			{
				JButton btnRestore = new JButton("恢复默认");
				btnRestore.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						for(IConfigSetter<?> setter: setters) {
							setter.setDefault();
						}
						config.setDefaultConfig();
						consoleSampleUpdate();
					}
				});
				buttonPane.add(btnRestore);
			}
		}
	}

	public void consoleSampleUpdate() {
		ConsoleConfig console = config.getConsole();
		msgArea.setBackground(console.getBgColor());
		msgArea.setForeground(console.getFontColor());
		msgArea.setFont(console.getFont());
		config.doConfig(toConfig);
	}
	
}
