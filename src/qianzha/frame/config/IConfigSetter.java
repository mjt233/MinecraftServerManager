package qianzha.frame.config;

import javax.swing.JComponent;
import javax.swing.JLabel;

public interface IConfigSetter<T> {
	
	JComponent getComponent();
	JLabel getLabel();
	
	T getOldValue();
	T getValue();
	void saveValue();
	void saveOldValue();
	
	void setDefault();
}
