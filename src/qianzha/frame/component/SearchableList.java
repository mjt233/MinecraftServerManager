package qianzha.frame.component;

import java.awt.BorderLayout;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

@SuppressWarnings("serial")
public class SearchableList extends JPanel {
	private JTextField textField;
	private JList<String> jlist;
	private JScrollPane sp;
	/**
	 * Create the panel.
	 */
	@SuppressWarnings({ "unchecked", "rawtypes" })
	public SearchableList(String[] list, String initValue) {
//		this.setBorder(new TitledBorder(UIManager.getBorder("TitledBorder.border"), title, TitledBorder.LEADING, TitledBorder.TOP, null, new Color(0, 0, 0)));
		this.setLayout(new BorderLayout(0, 0));
		
		jlist = new JList(list) {
			public void setSelectedIndex(int index) {
				super.setSelectedIndex(index);
				if(sp != null && !jlist.hasFocus()) {
					JScrollBar bar = sp.getVerticalScrollBar();
					bar.setValue( bar.getMaximum() /list.length * index);
				}
			}
		};
		jlist.addListSelectionListener(new ListSelectionListener() {
			public void valueChanged(ListSelectionEvent e) {
				String value = (String) jlist.getSelectedValue();
				if(textField != null && !textField.hasFocus()) {
					textField.setText(value);
				}
			}
		});
		sp = new JScrollPane(jlist);
		this.add(sp);
		
		textField = new JTextField();
		textField.getDocument().addDocumentListener(new ListSearcher() {
			public void insertUpdate(DocumentEvent e) {
				find(textField.getText(), list, jlist);
				jlist.repaint();
			}
		});
		textField.addKeyListener(new KeyListener() {
			public void keyTyped(KeyEvent e) {}
			public void keyReleased(KeyEvent e) {}
			public void keyPressed(KeyEvent e) {
				int index = jlist.getSelectedIndex();
				switch (e.getKeyCode()) {
				case KeyEvent.VK_UP:
					if(index > 0) jlist.setSelectedIndex(index - 1);
					textUpdate();
					break;
				case KeyEvent.VK_DOWN:
					if(index < jlist.getModel().getSize()) jlist.setSelectedIndex(index + 1);
					textUpdate();
					break;
				default:
					break;
				}
			}
			private void textUpdate() {
				String value = jlist.getSelectedValue();
				textField.setText(value);
				textField.select(0, value.length());
			}
		});
		this.add(textField, BorderLayout.NORTH);
		textField.setColumns(10);
		
		setSelectedValue(initValue);
	}

	public void setSelectedIndex(int index) {
		jlist.setSelectedIndex(index);
	}
	
	public void setSelectedValue(Object value) {
		jlist.setSelectedValue(value, true);
	}
	
	public int getSelectedIndex() {
		return jlist.getSelectedIndex();
	}
	public void addListSelectionListener(ListSelectionListener listener) {
		jlist.addListSelectionListener(listener);
	}
	
	public static boolean startWithIgnoreCase(String str, String start) {
		if(start.length() > str.length()) {
			return false;
		}
		return str.substring(0, start.length()).equalsIgnoreCase(start);
	}
	
	abstract class ListSearcher implements DocumentListener {
		private Thread find;
		void find(String tofind, String[] list, JList<?> jList) {
			if(find != null) find.interrupt();
			find = new Thread(new Runnable() {
				public void run() {
					for(int i=0; i<list.length; i++) {
						if(Thread.currentThread().isInterrupted()) break;
						if(startWithIgnoreCase(list[i], tofind)) {
							jList.setSelectedIndex(i);
							break;
						}
					}
				}
			});
			find.start();
		}
		public void removeUpdate(DocumentEvent e) {}
		public void changedUpdate(DocumentEvent e) {}
		/**
		 * @see #find(String, String[], JList, JScrollBar)
		 */
		public abstract void insertUpdate(DocumentEvent e);
	}
}
