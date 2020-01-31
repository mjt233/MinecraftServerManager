import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

abstract class AccessClient{
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

class FrameInfo{
	public int opcode,length;
}

class FrameBuilder{
	public byte[] frame = new byte[5];
	public int opcode = 0,length = 0, FIN;
	public FrameBuilder() {};
	public FrameBuilder(int opcode, int length) {
		build(opcode, length);
	}
	public void build() {
		build(opcode, length);
	}
	public void build(int opcode, int length) {
		this.length = length;
		this.opcode = opcode;
		this.FIN = 1;
		frame[0] = (byte)0x80;
		frame[0] += opcode;
		frame[4] = (byte) (length & 0xff);  
		frame[3] = (byte) (length >> 8 & 0xff);  
		frame[2] = (byte) (length >> 16 & 0xff);  
		frame[1] = (byte) (length >> 24 & 0xff);
	}
	public void Analyze(byte[] frame_data) {
		FIN =( frame_data[0] & 0x80 ) == 0x80 ? 1:0;
		opcode = frame_data[0] & 0x7F;
		length = frame_data[4] & 0xFF |
	            (frame_data[3] & 0xFF) << 8 |
	            (frame_data[2] & 0xFF) << 16 |
	            (frame_data[1] & 0xFF) << 24;
	}
	
	public void Analyze() {
		Analyze(frame);
	}
}