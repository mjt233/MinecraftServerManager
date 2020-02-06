import java.io.*;

import javax.swing.JFrame;
import javax.swing.JTextArea;
final class AccessType{
	static final int  SERVER = 1;
	static final int CONTROLLER = 2;
}
public class msmc{
	public static void main(String[] args) {
		GUI gui = new GUI();
		InfoShower is = new InfoShower("www.xiaotao233.top", 6636, 1, 12345, AccessType.CONTROLLER, gui.textArea);
		GUIAction aa = new GUIAction(is.ac, gui);
		gui.send.addActionListener(aa);
		new Thread(is).start();
	}
}


class InfoShower implements Runnable{
	public JTextArea textArea = null;
	public AccessClient ac;
	public InfoShower(String addr, int port, int SerID, int UsrID, int type, JTextArea joutput) {
		textArea = joutput;
		try {
			if( type == AccessType.SERVER ) {
				ac = new Server(addr, port, SerID, UsrID);
			}else {
				ac = new Controller(addr, port, SerID, UsrID);
			}
			show("接入成功\n");
		} catch (IOException e) {
			show("连接失败\n");
		} catch (AccessRefuseException e) {
			show("接入失败,原因:"+e.getMessage());
			e.printStackTrace();
		}
	}
	public void show(String str) {
		if( textArea != null ) {
			textArea.append(str);
			textArea.setCaretPosition(textArea.getText().length());
		}
		System.out.println(str);
	}
	@Override
	public void run() {
		byte[] buf = new byte[2048];
		int t = 0, count = 0;
		FrameInfo fInfo;
		try {
			while(true) {
				fInfo = ac.readFrame();
				t = fInfo.length;
				switch(fInfo.opcode) {
				case 0:
					while( t > 0 ) {
						count = ac.read(buf, t > 2048? 2048:t);
						if( count <= 0 ) {
							throw new IOException();
						}
						t -= count;
						show(new String(buf, 0, count)+"\n");
					}
					break;
				}
			}
		} catch (IOException e) {
			show("连接已断开\n");
		} catch (NullPointerException e) {
			show("发生错误\n");
		}
	}
}
