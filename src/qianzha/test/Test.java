package qianzha.test;

import java.awt.GraphicsEnvironment;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

public class Test{
	JFrame frame = null;
//	ConsolePanel cp = null;
	
	public Test() {
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				createAndShowGUI();
			}
		});
	}
	
	void createAndShowGUI() {
        System.out.println("Created GUI on EDT? "+
                SwingUtilities.isEventDispatchThread());
        frame = new JFrame();
		frame.setVisible(true);
		frame.setResizable(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
//		cp = new ConsolePanel(new IConsoleSender() {
//			@Override
//			public void send(ActionEvent e, String text) {
//				cp.appendMsg("Enter Event", new Date(e.getWhen()), text);
//			}
//		});
//		frame.add(cp);
		
//		frame.add(new ConfigPanel(QzConfig.getDefaultConfig()));
		
		frame.pack();
//		frame.setSize(600, 800);
    }
	
	public static void main(String[] args) throws InterruptedException {
		GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
		ge.preferLocaleFonts();
		System.out.println(ge);
//		String lookAndFeel = "com.sun.java.swing.plaf.windows.WindowsLookAndFeel";
//		try {
//			UIManager.setLookAndFeel(lookAndFeel);
//		} catch (ClassNotFoundException | InstantiationException | IllegalAccessException
//				| UnsupportedLookAndFeelException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
//		
//		Test t = new Test();
//		Thread.sleep(1500);
//		
////		config.getConsole().setBgColor(Color.WHITE);
////		config.getConsole().setFontColor(Color.BLACK);
////		config.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
////		config.doConfig(t);
////		t.cp.appendMsg("ConfigTest", "New Config have applied");
////		t.cp.appendMsgLine(config.toJSONString());
//		for(LookAndFeelInfo laf : UIManager.getInstalledLookAndFeels()) {
////			t.cp.appendMsg("L&F", laf.getClassName());
//			System.out.println(laf.getClassName());
//		}
	}

	

}
