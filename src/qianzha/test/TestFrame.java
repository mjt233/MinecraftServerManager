package qianzha.test;

import java.awt.BorderLayout;
import java.awt.EventQueue;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JRadioButton;
import javax.swing.JScrollBar;
import javax.swing.JSpinner;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.JToggleButton;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.border.EmptyBorder;

@SuppressWarnings("serial")
public class TestFrame extends JFrame {

	{
		String lookAndFeel = UIManager.getSystemLookAndFeelClassName();
		try {
			UIManager.setLookAndFeel(lookAndFeel);
		} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
				| UnsupportedLookAndFeelException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private JPanel contentPane;
	private JTextField textField;
	private JPasswordField passwordField_1;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					TestFrame frame = new TestFrame();
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the frame.
	 */
	public TestFrame() {
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 450, 300);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		contentPane.setLayout(new BorderLayout(0, 0));
		
		JTabbedPane tabbedPane = new JTabbedPane(JTabbedPane.LEFT);
		tabbedPane.setTabLayoutPolicy(JTabbedPane.SCROLL_TAB_LAYOUT);
		contentPane.add(tabbedPane, BorderLayout.CENTER);
		
		JPanel panel = new JPanel();
		tabbedPane.addTab("New tab", null, panel, null);
		panel.setLayout(null);
		panel.setBorder(new EmptyBorder(5, 5, 5, 5));
		
		JButton button = new JButton("New button");
		button.setBounds(33, 53, 93, 23);
		panel.add(button);
		
		JLabel label = new JLabel("New label");
		label.setBounds(136, 57, 54, 15);
		panel.add(label);
		
		JRadioButton radioButton = new JRadioButton("New radio button");
		radioButton.setBounds(33, 82, 121, 23);
		panel.add(radioButton);
		
		JCheckBox checkBox = new JCheckBox("New check box");
		checkBox.setBounds(33, 104, 103, 23);
		panel.add(checkBox);
		
		JToggleButton toggleButton = new JToggleButton("New toggle button");
		toggleButton.setBounds(33, 133, 135, 23);
		panel.add(toggleButton);
		
		textField = new JTextField();
		textField.setText("ffff");
		textField.setColumns(10);
		textField.setBounds(33, 166, 66, 21);
		panel.add(textField);
		
		passwordField_1 = new JPasswordField();
		passwordField_1.setBounds(148, 166, 83, 21);
		panel.add(passwordField_1);
		
		JScrollBar scrollBar_1 = new JScrollBar();
		scrollBar_1.setBounds(417, 0, 17, 261);
		panel.add(scrollBar_1);
		
		JSpinner spinner = new JSpinner();
		spinner.setBounds(56, 197, 93, 22);
		panel.add(spinner);
		
		JPanel panel_1 = new JPanel();
		panel_1.setBorder(new EmptyBorder(5, 5, 5, 5));
		tabbedPane.addTab("New tab", null, panel_1, null);
		panel_1.setLayout(new BorderLayout(0, 0));
	}
}
