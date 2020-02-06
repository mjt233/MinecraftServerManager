package mjt233.msmc;

import java.io.IOException;


import qianzha.frame.ConsolePanel;

public class InfoShower implements Runnable{
	public static final int  SERVER = 1;
	public static final int CONTROLLER = 2;
	public ConsolePanel console = null;
	public AccessClient ac;
	public InfoShower(String addr, int port, int SerID, int UsrID, int type, ConsolePanel joutput) {
		console = joutput;
		try {
			if( type == InfoShower.SERVER ) {
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
		if( console != null ) {
			console.appendMsgLine(str);
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
