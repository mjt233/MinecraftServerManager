package mjt233.msmc;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.charset.Charset;

public abstract class AccessClient{
	public static final Charset CHARSET = Charset.forName("UTF-8");
	public Socket socket;
	public AccessClient(String addr, int port) throws UnknownHostException, IOException {
		socket = new Socket(addr, port);
	}
	/**
	 * 读取数据帧信息.
	 * 调用该函数前请务必保障所有数据均已接收
	 * @return FrameInfo 帧信息对象,包含数据长度和操作码
	 * @throws IOException 
	 */
	public FrameInfo readFrame() throws IOException {
		FrameInfo info = new FrameInfo();
		FrameBuilder fb = new FrameBuilder();
		try {
			if ( socket.getInputStream().read(fb.frame, 0, 5) <= 0) {
				throw new IOException();
			}
		} catch (IOException e) {
			throw e;
		}
		fb.Analyze();
		System.out.println(String.format("FIN:%d opcode:%d length:%d", fb.FIN, fb.opcode, fb.length));
		info.length = fb.length;
		info.opcode = fb.opcode;
		return info;
	}
	
	public void stop() throws IOException {
		socket.close();
	}
	
	/**
	 * 从该对象中读取length个byte的数据
	 * 请务必保障要读取的长度准确,严格遵循数据帧的信息,否则将可能误读下一数据帧的信息
	 * @param buf		存放数据的byte数组
	 * @param length	要读取的长度
	 * @return			返回已读取的字节数
	 * @throws IOException
	 */
	public int read(byte[] buf,int length) throws IOException {
		return socket.getInputStream().read(buf, 0, length);
	}

	public void write(int opcode,byte[] buf) throws IOException {
		FrameBuilder fb = new FrameBuilder(opcode, buf.length);
		socket.getOutputStream().write(fb.frame, 0, 5);
		socket.getOutputStream().write(buf, 0, buf.length);
	}
}
