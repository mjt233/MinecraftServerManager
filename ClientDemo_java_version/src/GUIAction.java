import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.IOException;


class GUIAction implements ActionListener{
	private AccessClient ac;
	private GUI gui;
	public GUIAction(AccessClient ac, GUI gui) {
		this.ac = ac;
		this.gui = gui;
	}
	@Override
	public void actionPerformed(ActionEvent e) {
		String str = gui.input.getText()+"\n";
		if(e.getSource() == gui.send ) {
			try {
				ac.write(0x0, str.getBytes());
			} catch (IOException e1) {
				gui.textArea.append("-------信息发送失败-------\n");
			}
		}
	}
	
}