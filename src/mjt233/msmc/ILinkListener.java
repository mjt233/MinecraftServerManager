package mjt233.msmc;

public interface ILinkListener {
	void linkSucceeded();
	void linkFailed();
	void linkRefused(String msg);
	void readSucceeded(String readed);
	void linkBreak();
}
