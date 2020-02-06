package mjt233.msmc;

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