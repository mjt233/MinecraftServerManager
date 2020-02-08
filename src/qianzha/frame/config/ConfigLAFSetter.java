package qianzha.frame.config;

import java.awt.Component;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.DefaultListCellRenderer;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;

import qianzha.config.WinConfig;
import qianzha.frame.icon.IconQz;

@SuppressWarnings("serial")
public class ConfigLAFSetter implements IConfigSetter<String> {

	public static final LookAndFeelInfo[] lafs = UIManager.getInstalledLookAndFeels();
	public static final String DEFAULT_VALUE = UIManager.getSystemLookAndFeelClassName();
	
	private JLabel lafLabel;
	private String lafCurrent = UIManager.getLookAndFeel().getClass().getName();
	private JComboBox<?> lafCom;
	private WinConfig config;
	private String oldValue;
	/**
	 * Create the panel.
	 */
	@SuppressWarnings({ "unchecked", "rawtypes" })
	public ConfigLAFSetter(WinConfig config) {
		this.config = config;
		oldValue = config.getLookAndFeel();
		
		lafLabel = new JLabel("Look&Feel");
		
		lafCom = new JComboBox(lafs);
		lafCom.setRenderer(new DefaultListCellRenderer() {
			@Override
			public Component getListCellRendererComponent(JList<?> list, Object value, int index, boolean isSelected,
					boolean cellHasFocus) {
				return super.getListCellRendererComponent(list, ((LookAndFeelInfo)value).getName(), index, isSelected, cellHasFocus);
			}
		});
		lafCom.addItemListener(new ItemListener() {
			@Override
			public void itemStateChanged(ItemEvent e) {
				if(ItemEvent.SELECTED == e.getStateChange()) {
					lafJudge();
				}
			}
		});
		
		
		for(int i=lafs.length-1; i>=0; i--) {
			if(config.getLookAndFeel().equals(lafs[i].getClassName())) {
				lafCom.setSelectedIndex(i);
			}
		}
	}
	
	private void lafJudge() {
//		lafLabel, lafCurrent, ((LookAndFeelInfo)e.getItem()).getClassName()
		String newLaf = getValue();
		if(lafCurrent.equals(newLaf)) {
			lafLabel.setIcon(null);
			lafLabel.setToolTipText("");
		}
		else {
			lafLabel.setIcon(IconQz.Warning);
			lafLabel.setToolTipText("该变更需要重启程序");
		}
	}
	
	@Override
	public JComboBox<?> getComponent() {
		return lafCom;
	}

	@Override
	public JLabel getLabel() {
		return lafLabel;
	}
	
	@Override
	public String getOldValue() {
		return oldValue;
	}
	
	@Override
	public String getValue() {
		return ((LookAndFeelInfo)lafCom.getSelectedItem()).getClassName();
	}
	
	@Override
	public void saveOldValue() {
		config.setLookAndFeel(getOldValue());
	}
	
	@Override
	public void saveValue() {
		config.setLookAndFeel(getValue());
	}
	
	
	@Override
	public void setDefault() {
		for(int i=lafs.length-1; i>=0; i--) {
			if(DEFAULT_VALUE.equals(lafs[i].getClassName())) {
				lafCom.setSelectedIndex(i);
				break;
			}
		}
	}

}
