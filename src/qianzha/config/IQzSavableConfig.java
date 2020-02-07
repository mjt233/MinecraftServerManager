package qianzha.config;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.Charset;


public interface IQzSavableConfig extends IQzConfig {
	public static final Charset DEFAULT_CHARSET= Charset.forName("UTF-8");
	void save(OutputStream os) throws IOException;
	String toJSONString();
}
