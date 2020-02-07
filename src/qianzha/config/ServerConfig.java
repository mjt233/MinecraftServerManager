package qianzha.config;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.annotation.JSONField;
import com.alibaba.fastjson.serializer.SerializerFeature;

public class ServerConfig implements IQzSavableConfig {
	public static final String DEFAULT_ADDRESS = "www.xiaotao233.top";
	public static final int DEFAULT_PORT = 6636;
	public static final int DEFAULT_SID = 2;
	public static final int DEFAULT_UID = 12345;
	
	@JSONField(name = "Address")
	private String address;
	@JSONField(name = "Port")
	private int port;
	@JSONField(name = "SID")
	private int sID;
	@JSONField(name = "UID")
	private int uID;
	@JSONField(name = "AutoLink", defaultValue = "false")
	public boolean auto = false;
	
	public ServerConfig() {
		setDefaultConfig();
	}
	
	public ServerConfig(String address, int port, int serverID, int userID) {
		this.address = address;
		this.port = port;
		this.sID = serverID;
		this.uID = userID;
	}
	
	public String getAddress() {
		return address;
	}
	public void setAddress(String address) {
		this.address = address;
	}
	public int getPort() {
		return port;
	}
	public void setPort(int port) {
		this.port = port;
	}
	public int getServerID() {
		return sID;
	}
	public void setServerID(int serverID) {
		this.sID = serverID;
	}
	public int getUserID() {
		return uID;
	}
	public void setUserID(int userID) {
		this.uID = userID;
	}



	@Override
	public void setDefaultConfig() {
		address = DEFAULT_ADDRESS;
		port = DEFAULT_PORT;
		sID = DEFAULT_SID;
		uID = DEFAULT_UID;
	}

	@Override
	@Deprecated
	public void setConfig(IToConfig itc) {
	}

	@Override
	public void doConfig(IToConfig itc) {
		itc.setServer(this);
	}

	public void save(OutputStream os) throws IOException {
		JSON.writeJSONString(os, IQzSavableConfig.DEFAULT_CHARSET, this, SerializerFeature.PrettyFormat);
	}
	
	public String toJSONString() {
		return JSON.toJSONString(this, SerializerFeature.PrettyFormat);
	}
	
	public static ServerConfig read(InputStream is) throws IOException {
		ServerConfig ret = JSON.parseObject(is, IQzSavableConfig.DEFAULT_CHARSET, ServerConfig.class);
		return ret;
	}
	
	public static ServerConfig read(String json) {
		ServerConfig ret = new ServerConfig();
		ret = JSON.parseObject(json, ServerConfig.class);
		return ret;
	}

}
