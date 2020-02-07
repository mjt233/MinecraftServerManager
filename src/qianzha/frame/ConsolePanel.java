package qianzha.frame;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;

@SuppressWarnings("serial")
public class ConsolePanel extends JPanel {
	public static final SimpleDateFormat DATE_FORMAT_HHmmss = new SimpleDateFormat("HH:mm:ss");
	public static final Color DEFAULT_BG_COLOR = Color.DARK_GRAY;
	public static final Color DEFAULT_FONT_COLOR = Color.GREEN;
	
	public static final Font DEFAULT_FONT = new Font("Microsoft YaHei UI", Font.PLAIN, 12);
	
	private JTextField inputField;
	private JTextArea msgArea;
	private JPanel bottomPanel;
	private JLabel inputLabel;
	private JButton sendButton;
	
	private LinkedList<IConsoleSender> senders;
	/**
	 * Create the panel.
	 */
	public ConsolePanel() {
		senders = new LinkedList<IConsoleSender>();
		setLayout(new BorderLayout(0, 0));
		
		msgArea = new JTextArea();
		msgArea.setEditable(false);
		msgArea.setText("======== Console Panel ========");
		
		JScrollPane msgJscroll = new JScrollPane(msgArea);
		msgJscroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		add(msgJscroll, BorderLayout.CENTER);
		
		bottomPanel = new JPanel();
		add(bottomPanel, BorderLayout.SOUTH);
		bottomPanel.setLayout(new BorderLayout(0, 0));
		
		inputField = new JTextField();
		bottomPanel.add(inputField);
		
		inputLabel = new JLabel(" > ");
		inputLabel.setLabelFor(inputField);
		bottomPanel.add(inputLabel, BorderLayout.WEST);
		
		sendButton = new JButton("发送");
		bottomPanel.add(sendButton, BorderLayout.EAST);
		

		ActionListener senderListener = new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				// Enter Event
				sendInput(e);
			}
		};
		inputField.addActionListener(senderListener);
		sendButton.addActionListener(senderListener);
		
		initStyle();
	}
	
	protected void sendInput(ActionEvent e) {
//		appendMsg("Enter Event", new Date(e.getWhen()), getInputText());
		for(IConsoleSender s : senders) {
			s.send(e, this, getInputText());
		}
		inputField.setText("");
	}
	
	public void addSenders(IConsoleSender sender) {
		senders.add(sender);
	}
	public List<IConsoleSender> getSenders() { return this.senders; }
	public boolean removeSender(IConsoleSender o) { return this.senders.remove(o); };
	
	public void clear() {
		this.msgArea.setText("");
	}
	
	public void appendMsgLine(String line) {
		msgArea.append("\n"+line);
		msgArea.setCaretPosition(msgArea.getText().length());
//		messageArea.paintImmediately(messageArea.getBounds());
	}
	public void appendTestMsg(String msg) {
		appendMsg("TestMsg", msg);
	}
	public void appendMsg(String src, String msg) {
		appendMsg(src, new Date(), msg);
	}
	public void appendMsg(String src, Date time, String msg) {
		String msgLine = "["+src+"] [" + DATE_FORMAT_HHmmss.format(time) + "] " + msg;
		System.out.println(msgLine);
		appendMsgLine(msgLine);
	}

	public String getConsoleText() {
		return this.msgArea.getText();
	}
	
	public String getInputText() {
		return this.inputField.getText();
	}
	
	// Style Setting
	public void initStyle() {
		setBackground(DEFAULT_BG_COLOR);
		setFontColor(DEFAULT_FONT_COLOR);
		if(msgArea!=null)msgArea.setFont(DEFAULT_FONT);
	}


	@Override
	public void setBackground(Color c) {
		super.setBackground(c);
		if(msgArea!=null) msgArea.setBackground(c);
	}
	
	public void setFontColor(Color c) {
		setForeground(c);
		if(msgArea!=null) msgArea.setForeground(c);
	}
	
	public Color getFontColor() {
		return getForeground();
	}
	
	public void setFont(Font f) {
		super.setFont(f);
		if(msgArea!=null) {
			msgArea.setFont(f);
			inputField.setFont(f);
		}
	}

}
