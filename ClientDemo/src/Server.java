import java.io.IOException;
import java.net.UnknownHostException;

class Server extends AccessClient{
	public Server(String addr, int port, int SerID, int UsrID) throws UnknownHostException, IOException, AccessRefuseException {
		super(addr, port);
		socket.getOutputStream().write(String.format("SER%5sE%5sE", SerID, UsrID).getBytes());
		byte[] b = new byte[2];
		socket.getInputStream().read(b);
		if( !new String(b).equals("OK") ) {
			int count = 0;
			byte[] buf = new byte[30];
			count = socket.getInputStream().read(buf);
			throw new AccessRefuseException(new String(b)+new String(buf, 0, count));
		}
	}
	
	
}