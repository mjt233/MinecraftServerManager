package mjt233.msmc;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

class GUI extends JFrame{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public JTextArea textArea = new JTextArea();
	public JTextField input = new JTextField();
	public JButton send = new JButton("发送");
	private JScrollPane jsp = new JScrollPane(textArea);
	public GUI() {
		super("hia!");
		setSize(800, 600);
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		textArea.setAutoscrolls(true);
		add(jsp);
		add(input);
		add(send);
		setLayoutStyle();
		setLocationRelativeTo(null);
		setVisible(true);
	}
	private void setLayoutStyle() {
		GridBagLayout layout = new GridBagLayout();
		GridBagConstraints c = new GridBagConstraints();
		setLayout(layout);
		c.fill = GridBagConstraints.BOTH;
		textArea.setBackground(Color.black);
		textArea.setForeground(Color.white);
		c.gridwidth = 0;
		c.weightx = 1;
		c.weighty = 1;
		layout.setConstraints(jsp, c);
		c.gridwidth = 1;
		c.weightx = 1;
		c.weighty = 0;
		layout.setConstraints(input, c);
		c.gridwidth = 0;
		c.weightx = 0;
		layout.setConstraints(send, c);
	}
}