package qianzha.frame.config;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GraphicsEnvironment;
import java.awt.HeadlessException;
import java.awt.Window;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.Serializable;
import java.util.Locale;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRootPane;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.UIManager;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import qianzha.frame.MainFrame;
import qianzha.frame.component.SearchableList;

@SuppressWarnings("serial")
public class QZFontChooser extends JPanel {
	public static final String[] FONT_NAMES = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();
	public static final int[] FONT_STYLE = new int[] {Font.PLAIN, Font.BOLD, Font.ITALIC, Font.BOLD|Font.ITALIC};
	public static String[] FONT_STYLE_NAME = new String[] {"常规", "粗体", "斜体", "粗斜体"};
	private static String getStyle(int style) {
		return FONT_STYLE_NAME[style];
	}
	
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					MainFrame frame = new MainFrame();
					frame.setVisible(true);
					Font res = QZFontChooser.showDialog(frame.getConsolePanel(), "字体设置", frame.getFont());
					System.out.println(res);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}
	
	private JLabel eLabel;
	private SearchableList fname;
	private SearchableList fstyle;
	private JSpinner fsize;
	private ListSelectionListener listListener = new ListSelectionListener() {
		public void valueChanged(ListSelectionEvent e) {
			setShowFont();
		}
	};
	/**
	 * Create the panel.
	 */
	public QZFontChooser(Font initValue) {
		this.setLayout(new BorderLayout(0, 0));
		JPanel ePanel = new JPanel();
		ePanel.setBorder(new TitledBorder(null, "\u793A\u4F8B", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		this.add(ePanel, BorderLayout.SOUTH);
		{
			eLabel = new JLabel("示例文字 [AaBbCc] : 123");
			ePanel.add(eLabel);
		}
	
		JPanel panel = new JPanel();
		FlowLayout flowLayout = (FlowLayout) panel.getLayout();
		flowLayout.setAlignOnBaseline(true);
		this.add(panel, BorderLayout.CENTER);
		{
			fname = new SearchableList(FONT_NAMES, initValue.getName());
			fname.setBorder(new TitledBorder(null, "\u5B57\u4F53 (F)", TitledBorder.LEADING, TitledBorder.TOP, null, null));
			panel.add(fname);
		}
		{
			fstyle = new SearchableList(FONT_STYLE_NAME, getStyle(initValue.getStyle()));
			fstyle.setBorder(new TitledBorder(UIManager.getBorder("TitledBorder.border"), "\u5B57\u5F62 (Y)", TitledBorder.LEADING, TitledBorder.TOP, null, new Color(0, 0, 0)));
			panel.add(fstyle);
		}
		fname.addListSelectionListener(listListener);
		fstyle.addListSelectionListener(listListener);
		{
			JPanel fsize_panel = new JPanel();
			fsize_panel.setBorder(new TitledBorder(UIManager.getBorder("TitledBorder.border"), "\u5927\u5C0F (S)", TitledBorder.LEADING, TitledBorder.TOP, null, new Color(0, 0, 0)));
			panel.add(fsize_panel);
			{
				fsize = new JSpinner();
				fsize.setModel(new SpinnerNumberModel(initValue.getSize(), 9, 64, 1));
				fsize.setPreferredSize(new Dimension(50, 25));
				fsize.addChangeListener(new ChangeListener() {
					public void stateChanged(ChangeEvent e) {
						setShowFont();
					}
				});
				fsize_panel.add(fsize);
			}
		}
	}
	
	
	@SuppressWarnings("deprecation")
	public static Font showDialog(Component component,
        String title, Font initFont) {

        final QZFontChooser pane = new QZFontChooser(initFont != null?
                                               initFont : new Font(Font.SANS_SERIF, Font.PLAIN, 12));
        
        FontTracker ok = new FontTracker(pane);
//        JDialog dialog = createDialog(component, title, true, pane, ok, null);
//        public static JDialog createDialog(Component c, String title, boolean modal,
//                JColorChooser chooserPane, ActionListener okListener,
//                ActionListener cancelListener) throws HeadlessException {

        		Container container = component.getParent();
                while(container.getParent() != null) {
                	container = container.getParent();
                }
                
                FontChooserDialog dialog;
                if (container instanceof Frame) {
                    dialog = new FontChooserDialog((Frame)container, title, true, component, pane,
                    		ok, null);
                } else {
                    dialog = new FontChooserDialog((Dialog)container, title, true, component, pane,
                    		ok, null);
                }
                dialog.getAccessibleContext().setAccessibleDescription(title);
        
        dialog.addComponentListener(new ComponentAdapter() {
        	public void componentHidden(ComponentEvent e) {
                Window w = (Window)e.getComponent();
                w.dispose();
            }
        });

        dialog.show(); // blocks until user brings dialog down...

        return ok.getColor();
    }

	public Font getFontRes() {
		return new Font(FONT_NAMES[fname.getSelectedIndex()], FONT_STYLE[fstyle.getSelectedIndex()], (int)fsize.getValue());
	}

	public void setShowFont() {
		eLabel.setFont(getFontRes());
	}
}

@SuppressWarnings("serial")
class FontTracker implements ActionListener, Serializable {
    QZFontChooser chooser;
    Font font;

    public FontTracker(QZFontChooser c) {
        chooser = c;
    }

    public void actionPerformed(ActionEvent e) {
    	font = chooser.getFontRes();
    }

    public Font getColor() {
        return font;
    }
}

@SuppressWarnings({"serial", "deprecation"})
class FontChooserDialog extends JDialog {

    public FontChooserDialog(Dialog owner, String title, boolean modal,
        Component c, QZFontChooser chooserPane,
        ActionListener okListener, ActionListener cancelListener)
        throws HeadlessException {
        super(owner, title, modal);
        initFontChooserDialog(c, chooserPane, okListener, cancelListener);
    }

    public FontChooserDialog(Frame owner, String title, boolean modal,
        Component c, QZFontChooser chooserPane,
        ActionListener okListener, ActionListener cancelListener)
        throws HeadlessException {
        super(owner, title, modal);
        initFontChooserDialog(c, chooserPane, okListener, cancelListener);
    }
    
    protected void initFontChooserDialog(Component c, QZFontChooser chooserPane,
            ActionListener okListener, ActionListener cancelListener) {
            //setResizable(false);

            Locale locale = getLocale();
            String okString = UIManager.getString("ColorChooser.okText", locale);
            String cancelString = UIManager.getString("ColorChooser.cancelText", locale);
//            String resetString = UIManager.getString("ColorChooser.resetText", locale);

            Container contentPane = getContentPane();
            contentPane.setLayout(new BorderLayout());
            contentPane.add(chooserPane, BorderLayout.CENTER);

            /*
             * Create Lower button panel
             */
            JPanel buttonPane = new JPanel();
            buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
            JButton okButton = new JButton(okString);
            getRootPane().setDefaultButton(okButton);
            okButton.getAccessibleContext().setAccessibleDescription(okString);
            okButton.setActionCommand("OK");
            okButton.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
                    hide();
                }
            });
            if (okListener != null) {
                okButton.addActionListener(okListener);
            }
            buttonPane.add(okButton);

            JButton cancelButton = new JButton(cancelString);
            cancelButton.getAccessibleContext().setAccessibleDescription(cancelString);
            cancelButton.setActionCommand("cancel");
            cancelButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    hide();
                }
            });
            if (cancelListener != null) {
                cancelButton.addActionListener(cancelListener);
            }
            buttonPane.add(cancelButton);

//            JButton resetButton = new JButton(resetString);
//            resetButton.getAccessibleContext().setAccessibleDescription(resetString);
//            resetButton.addActionListener(new ActionListener() {
//               public void actionPerformed(ActionEvent e) {
//                   reset();
//               }
//            });
//            int mnemonic = SwingUtilities2.getUIDefaultsInt("ColorChooser.resetMnemonic", locale, -1);
//            if (mnemonic != -1) {
//                resetButton.setMnemonic(mnemonic);
//            }
//            buttonPane.add(resetButton);
            contentPane.add(buttonPane, BorderLayout.SOUTH);

            if (JDialog.isDefaultLookAndFeelDecorated()) {
                boolean supportsWindowDecorations =
                UIManager.getLookAndFeel().getSupportsWindowDecorations();
                if (supportsWindowDecorations) {
                    getRootPane().setWindowDecorationStyle(JRootPane.COLOR_CHOOSER_DIALOG);
                }
            }
            applyComponentOrientation(((c == null) ? getRootPane() : c).getComponentOrientation());

            pack();
            setLocationRelativeTo(c);

            this.addWindowListener(new WindowAdapter() {
            	public void windowClosing(WindowEvent e) {
                    cancelButton.doClick(0);
                    Window w = e.getWindow();
                    w.hide();
                }
			});
        }
}
