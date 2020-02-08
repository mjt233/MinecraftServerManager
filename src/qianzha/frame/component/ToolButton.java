package qianzha.frame.component;

import javax.swing.Icon;
import javax.swing.JButton;
import javax.swing.UIManager;

@SuppressWarnings("serial")
public class ToolButton extends JButton {
	{
		this.setBackground(UIManager.getColor("Panel.background"));
	}
	public ToolButton(Icon icon) {
		super(icon);
	}
	public ToolButton(Icon icon, String tooltip) {
		super(icon);
		setToolTipText(tooltip);
	}
	
}
