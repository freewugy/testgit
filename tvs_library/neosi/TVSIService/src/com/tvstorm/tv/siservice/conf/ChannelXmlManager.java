/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-04-07 09:02:16 +0900 (월, 07 4월 2014) $
 * $LastChangedRevision: 11828 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice.conf;

import com.tvstorm.tv.si.channel.CAInfo;
import com.tvstorm.tv.si.channel.ChannelInfo;
import com.tvstorm.tv.si.channel.ESInfo;
import com.tvstorm.tv.si.channel.IChannelInfo;
import com.tvstorm.tv.si.channel.IESInfo;
import com.tvstorm.tv.si.channel.RetChannelList;
import com.tvstorm.tv.si.common.ISIResult;
import com.tvstorm.tv.si.common.ProductInfo;
import com.tvstorm.tv.si.common.RetProductList;
import com.tvstorm.tv.si.common.Utilities;
import com.tvstorm.tv.siservice.util.JLog;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmlpull.v1.XmlSerializer;

import android.content.Context;
import android.net.Uri;
import android.util.Log;
import android.util.Xml;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ChannelXmlManager implements IConfConstant {
	private static final String TAG = "ChannelXmlManager";

	private static final boolean USE_DEBUG = false;

	private static final String CHANNEL_XML_NAME = "channel.xml";

	private static final String CHANNEL_XML_REAL_NAME = "channel.xml.real";

	private static final String CHANNEL_XML = DATA_RUN_DIRECTORY + "/" + CHANNEL_XML_NAME;

	private static final String CHANNEL_XML_REAL = DATA_RUN_DIRECTORY + "/" + CHANNEL_XML_REAL_NAME;

	private static final String TAG_CHANNELS = "channels";

	private static final String TAG_CHANNEL = "channel";

	private static final String TAG_PRODUCTS = "products";

	private static final String TAG_PRODUCT = "product";
	
	private static final String TAG_VERSION = "version";

	private static ChannelXmlManager instance = null;

	private File mChannelXmlFile = null;

	private XmlPullParser mParser = null;

	private XmlSerializer mSerializer = null;

	private ArrayList<ChannelInfo> mChannels = new ArrayList<ChannelInfo>();

	private ArrayList<ProductInfo> mProducts = new ArrayList<ProductInfo>();

	private boolean isOkloadfile = false;

	private int mSdtVersion = 0xff;

	private ChannelConfManager mChannelConfManager = null;

	private ChannelXmlManager(Context cont) {
		mChannelConfManager  = ChannelConfManager.getInstance(cont);
		initXml();
		loadFromXml();
	}

	public static ChannelXmlManager getInstance(Context cont) {
		if (instance == null) {
			instance = new ChannelXmlManager(cont);
		}
		return instance;
	}

	public boolean isOkloadfile() {
		return isOkloadfile;
	}

	// 파일 복사
	// file(channel.xml.real)
	// save_fine(channel.xml)
	// eschoi_20130703 | not used
	private synchronized void initXml() {
		File f = new File(CHANNEL_XML_REAL);

		if (f != null && f.exists()) {
			JLog.v(TAG, "start");
			try {
				FileInputStream fis = new FileInputStream(f);
				FileOutputStream fos = new FileOutputStream(CHANNEL_XML);
				int readcount = 0;
				byte[] buffer = new byte[1024];
				while ((readcount = fis.read(buffer, 0, 1024)) != -1) {
					fos.write(buffer, 0, readcount);
				}
				fos.close();
				fis.close();
			} catch (Exception e) {
				JLog.e(TAG, e.toString());
			}
			JLog.v(TAG, "end");
		}
	}

	// loadFromXml
	// loadChListFromXml
	// loadPrdtListFromXml
	// getChannelList
	// getProductList
	public synchronized void loadFromXml() {
		boolean isSucceed = true;
		if (mChannelXmlFile == null) {
			mChannelXmlFile = new File(CHANNEL_XML);
		}

		if (mChannelXmlFile != null && mChannelXmlFile.exists()) {
			InputStream is = null;
			try {
				is = new FileInputStream(mChannelXmlFile);
			} catch (Exception e1) {
				JLog.e(TAG, e1.toString());

				mChannelXmlFile.delete();
				mChannelXmlFile = null;

				initXml();
				isSucceed = false;
			}

			if (is != null) {
				JLog.v(TAG, "start");
				try {
					if (mParser == null) {
						XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
						factory.setNamespaceAware(true);
						mParser = factory.newPullParser();
					}

					if (mParser != null) {
						mParser.setInput(is, "UTF-8");
						int eventType = mParser.getEventType();
						while (eventType != XmlPullParser.END_DOCUMENT) {
							boolean isValid = true;

							switch (eventType) {
								case XmlPullParser.START_DOCUMENT:
								case XmlPullParser.END_DOCUMENT:
								case XmlPullParser.END_TAG:
								case XmlPullParser.TEXT:
									if (USE_DEBUG) {
										if (eventType == XmlPullParser.START_DOCUMENT) {
											Log.d(TAG, "START_DOCUMENT");
										} else if (eventType == XmlPullParser.END_DOCUMENT) {
											Log.d(TAG, "END_DOCUMENT");
										} else if (eventType == XmlPullParser.END_TAG) {
											Log.d(TAG, "END_TAG " + mParser.getName());
										} else {
											Log.d(TAG, "TEXT " + mParser.getText());
										}
									}
									break;

								case XmlPullParser.START_TAG:
									if (USE_DEBUG) {
										Log.d(TAG, "START_TAG " + mParser.getName());
										for (int i = 0; i < mParser.getAttributeCount(); i++) {
											Log.d(TAG, i + ": + " + mParser.getAttributeName(i) + "/" + mParser.getAttributeValue(i));
										}
									}

									if (TAG_CHANNELS.equals(mParser.getName())) {
										clearChannels();
										isValid = loadVersionInfo();
										//Log.d(TAG, isValid ? "true":"false");
									} else if (TAG_CHANNEL.equals(mParser.getName())) {
										isValid = loadChListFromXml();
										//Log.d(TAG,  isValid? "true":"false");
									} else if (TAG_PRODUCTS.equals(mParser.getName())) {
										clearProducts();
										Log.d(TAG,  isValid? "true":"false");
									} else if (TAG_PRODUCT.equals(mParser.getName())) {
										isValid = loadPrdtListFromXml(mParser.getAttributeCount());
										Log.d(TAG,  isValid? "true":"false");
									}
									break;

							} // end of swtich

							if (isValid) {
								try {
									eventType = mParser.next();
								} catch (Exception e2) {
									JLog.e(TAG, e2.toString());
									isSucceed = false;
									break;
								}
							} else {
								JLog.w(TAG, "invalid index");
								isSucceed = false;
								break;
							}
						} // end of while
					} else {
						isSucceed = false;
					} // end of isNull(mParser)
				} catch (Exception e3) {
					JLog.e(TAG, e3.toString());
					isSucceed = false;
				}

				try {
					is.close();
				} catch (Exception e4) {
					JLog.e(TAG, e4.toString());
					isSucceed = false;
				}
				JLog.v(TAG, "end");
			} else {
				isSucceed = false;
			} // end of isNull(is)
		} else {
			isSucceed = false;
		} // end of isNull(mChannelXmlFile)

		isOkloadfile = isSucceed;
		if (isOkloadfile != true) {
			clearChannels();
			clearProducts();
		}
	}

	private int getIntAttr(int index) {
		int attr = 0;
		try {
			attr = Integer.parseInt(mParser.getAttributeValue(index));
		} catch (NumberFormatException e) {
			attr = 0;
		}
		return attr;
	}

	private int getIntAttrByName(String name) {
		int attr = 0;
		try {
			attr = Integer.parseInt(mParser.getAttributeValue(null, name));
		} catch (NumberFormatException e) {
			attr = 0;
		}
		return attr;
	}

	private String getStrAttr(int index) {
		return mParser.getAttributeValue(index);
	}

	private String getStrAttrByName(String name) {
		return mParser.getAttributeValue(null, name);
	}

	private boolean loadVersionInfo() {
		mSdtVersion  = getIntAttrByName(TAG_VERSION);
		if(mSdtVersion != 0xff) {
			return true;
		}
		return false;
	}
	
	private boolean loadChListFromXml() {
		// total 28
		// sid="1" suid="88.14.1" ctype="1" ch="1" cname="MiX"
		// ip_address="239.192.049.006" port="49220"
		// genre="0" local_area_code="0" running_status="4" vpid="1622"
		// vst="27"
		// apid="2622" ast="15" aplang="0" apid2="0" ast2="0" aplang2=""
		// ppid="1622"
		// res="0" pay="0" sample="0" image="" ca_id="0" ca_pid="0"
		ChannelInfo chInfo = new ChannelInfo();
		ESInfo esInfo;
		CAInfo caInfo;
		try {
			chInfo.setCategory(0); // default(0)
			chInfo.setLocalArea(0); // default(0)
			chInfo.setRunningStatus(IChannelInfo.CONST_RUNNING_STATUS_RUNNING); // default(4)

			int sid = getIntAttrByName("sid");
			chInfo.setChannelId(sid); // sid
			chInfo.setChannelUid(getStrAttrByName("suid")); // suid
			int ctype = getIntAttrByName("ctype");
			chInfo.setChannelType(ctype); // ctype
			int chNum = getIntAttrByName("ch"); // ch
			chInfo.setChannelNum(chNum);
			chInfo.setChannelName(getStrAttrByName("cname")); // cname
			
			/**
			 * For IPTV
			 */
			if (mChannelConfManager.getAccessPoint().equals("i")) {
				chInfo.setMulticastIP(getStrAttrByName("ip_address")); // ip_address
				chInfo.setMulticastPort(getIntAttrByName("port")); // port
			}
			
			String host = getStrAttrByName("demod"); // port
			String port = getStrAttrByName("freq"); // freq
			
			chInfo.setGenre(getIntAttrByName("genre")); // genre
			int vpid = getIntAttrByName("vpid"); // vpid
			int vst = getIntAttrByName("vst"); // vst
			int apid = getIntAttrByName("apid"); // apid
			int ast = getIntAttrByName("ast"); // ast
			int sr = getIntAttrByName("sr"); // sr
			int bw = getIntAttrByName("bw"); // bw
			String lang;
			lang = Utilities.convertStringToHex(getStrAttrByName("aplang")); // aplang
			if (lang == null || lang.isEmpty()) {
				lang = "0";
			}
			int alang = Integer.parseInt(lang, 16);
			int apid2 = getIntAttrByName("apid2"); // apid2
			int ast2 = getIntAttrByName("ast2"); // ast2
			lang = Utilities.convertStringToHex(getStrAttrByName("aplang2")); // aplang2
			if (lang == null || lang.isEmpty()) {
				lang = "0";
			}
			int alang2 = Integer.parseInt(lang, 16);

			int ppid = getIntAttrByName("ppid"); // ppid
			chInfo.setPcrPid(ppid);

			chInfo.setHDChannel(getIntAttrByName("res")); // res

			chInfo.setPayChannel(getIntAttrByName("pay")); // pay
			chInfo.setSampleTime(getIntAttrByName("sample")); // sample
			chInfo.setChannelImagePath(getStrAttrByName("image")); // image
			int caid = getIntAttrByName("ca_id"); // ca_id
			int capid = getIntAttrByName("ca_pid"); // ca_pid

			// EsInfoList
			ArrayList<ESInfo> eslist = new ArrayList<ESInfo>();
			esInfo = new ESInfo(vst, vpid, 0);
			eslist.add(esInfo);
			esInfo = new ESInfo(ast, apid, alang);
			eslist.add(esInfo);
			if (apid2 > 0) {
				esInfo = new ESInfo(ast2, apid2, alang2);
				eslist.add(esInfo);
			}
			chInfo.setESList(eslist);

			// CaInfoList
			chInfo.setFreeCaMode(0);
			ArrayList<CAInfo> calist = new ArrayList<CAInfo>();
			caInfo = new CAInfo(caid, capid);
			calist.add(caInfo);
			chInfo.setCAList(calist);

			String scheme = "";
			
			if (ip(host)) {
				scheme = String.format("tuner://%s:%d", chInfo.getMulticastIP(),
						chInfo.getMulticastPortNo()); // ip base
            } else if (rf(host)) {
                scheme = String.format("tuner://%s:%s", host, port); // rf base
            } else {
                scheme = String.format("dummy://%s:%s", host, port);
            }
			
			StringBuffer sb = new StringBuffer();
			sb.append(String.format("?ch=%d&pn=%d&ci=%d&cp=%d", chNum, sid, caid, capid));
			if (vpid > 0) {
				sb.append(String.format("&vp=%d&vc=%d", vpid, vst));
			}
			if (apid > 0) {
				sb.append(String.format("&ap=%d&ac=%d", apid, ast));
			}
			if (apid2 > 0) {
				sb.append(String.format("&ap2=%d&ac2=%d", apid2, ast2));
			}
			sb.append(String.format("&ct=%d&sn=0&sr=%d&bw=%d",ctype,sr,bw));
			chInfo.setChannelUri(scheme + sb.toString());
//			JLog.w(TAG, "channeluri" + chInfo.getChannelUri());
			mChannels.add(chInfo);
			
			return true;
		} catch (Exception e) {
			e.printStackTrace();
			JLog.e(TAG, e.toString());
		}

		return false;
	}

	private boolean loadPrdtListFromXml(int attrCount) {
		ProductInfo prdtInfo = new ProductInfo();
		try {
			prdtInfo.setProductId(getStrAttrByName("id"));
			prdtInfo.setProductDescription(getStrAttrByName("desc"));
			mProducts.add(prdtInfo);
			return true;
		} catch (Exception e) {
			JLog.e(TAG, e.toString());
		}
		return false;
	}

	public RetChannelList getChannelList(int tvRadioMode) {
		if (mChannels.isEmpty() == true) {
			JLog.w(TAG, "channels info is not exist");
			return new RetChannelList(ISIResult.SI_RESULT_ERR_INFO_IS_NOT_EXIST, new ArrayList<ChannelInfo>());
		}

		ArrayList<ChannelInfo> chList = new ArrayList<ChannelInfo>();
		switch (tvRadioMode) {
			case IChannelInfo.CONST_CHANNEL_MODE_TV:
				for (ChannelInfo chInfo : mChannels) {
					if (chInfo != null && chInfo.getChannelType() != IChannelInfo.CONST_CHANNEL_TYPE_DIGIT_RADIO) {
						chList.add(chInfo);
					}
				}
				break;
			case IChannelInfo.CONST_CHANNEL_MODE_RADIO:
				for (ChannelInfo chInfo : mChannels) {
					if (chInfo != null && chInfo.getChannelType() == IChannelInfo.CONST_CHANNEL_TYPE_DIGIT_RADIO) {
						chList.add(chInfo);
					}
				}
				break;

			default: // IChannelInfo.CONST_CHANNEL_MODE_ALL:
				chList = mChannels;
				break;
		}

		return new RetChannelList(ISIResult.SI_RESULT_OK, chList/* mChannels */);
	}

	public RetProductList getProductList() {
		if (mProducts.isEmpty() == true) {
			JLog.w(TAG, "products info is not exist");
			return new RetProductList(ISIResult.SI_RESULT_ERR_INFO_IS_NOT_EXIST,
					new ArrayList<ProductInfo>());
		}
		return new RetProductList(ISIResult.SI_RESULT_OK, mProducts);
	}

	// storeToXml
	// storeChListToXml
	// storePrdtListToXml
	// setChannelList
	// setProductList
	public synchronized void storeToXml(int sdtVersion, String default_sid) {
		if (mChannelXmlFile == null) {
			mChannelXmlFile = new File(CHANNEL_XML);
		}

		if (mChannelXmlFile != null) {
			FileOutputStream fos = null;
			try {
				fos = new FileOutputStream(mChannelXmlFile);
			} catch (Exception e1) {
				JLog.e(TAG, e1.toString());
			}

			if (fos != null) {
				JLog.v(TAG, "start");
				if (mSerializer == null) {
					mSerializer = Xml.newSerializer();
				}

				if (mSerializer != null) {
					try {
						mSerializer.setOutput(fos, "UTF-8");
						mSerializer.startDocument(null, Boolean.valueOf(true));
						mSerializer.setFeature(
								"http://xmlpull.org/v1/doc/features.html#indent-output", true);

						mSerializer.startTag(null, TAG_CHANNELS);
						mSerializer.attribute(null, "version", Integer.toString((sdtVersion)));
						mSerializer.attribute(null, "default_ch", default_sid);
						boolean isValid = true;
						for (ChannelInfo chInfo : mChannels) {
							isValid = storeChListToXml(chInfo);
							if (isValid == false) {
								break;
							}
						}
						mSerializer.endTag(null, TAG_CHANNELS);

						if (isValid) {
							mSerializer.startTag(null, TAG_PRODUCTS);
							for (ProductInfo prdtInfo : mProducts) {
								isValid = storePrdtListToXml(prdtInfo);
								if (isValid == false) {
									break;
								}
							}
							mSerializer.endTag(null, TAG_PRODUCTS);
						}

						mSerializer.endDocument();
						mSerializer.flush();
					} catch (Exception e2) {
						JLog.e(TAG, e2.toString());
					}
				} // end of isNull(mSerializer)

				try {
					fos.close();
				} catch (Exception e3) {
					JLog.e(TAG, e3.toString());
				}
				JLog.v(TAG, "end");
			} // end of isNull(fos)
		}
	}

	private boolean storeChListToXml(ChannelInfo chInfo) {
		if (chInfo != null) {
			// total 26
			// uid sid ctype ch cname host port genre
			// vpid apid apid2 ppid vst ast ast2 res
			// pay sample rating image product ca ca_id ca_pid <--24
			// sr bw
			try {
				String sr = "0", bw = "0", sn = "0";

				mSerializer.startTag(null, TAG_CHANNEL);
				mSerializer.attribute(null, "suid", chInfo.getChannelUid());
				mSerializer.attribute(null, "sid", chInfo.getChannelId() + "");
				mSerializer.attribute(null, "ctype", chInfo.getChannelType() + "");
				mSerializer.attribute(null, "ch", chInfo.getChannelNum() + "");
				mSerializer.attribute(null, "cname", chInfo.getChannelName());
				Uri uri = Uri.parse(chInfo.getChannelUri());
				String scheme = uri.getScheme();
				if (scheme != null) {
					if (scheme.compareTo("skbiptv") == 0 || scheme.compareTo("udp") == 0
							|| scheme.compareTo("rtp") == 0) { // ip base
						mSerializer.attribute(null, "ip", uri.getHost());
						mSerializer.attribute(null, "port", uri.getPort() + "");
					} else {
						if (scheme.length() > 0 	&& scheme.compareTo("tuner") == 0) { // rf  base
							mSerializer.attribute(null, "demod", uri.getHost());
							mSerializer.attribute(null, "freq", uri.getPort() + "");

							String query = uri.getQuery();
							if (query != null) {
								String[] params = query.split("&");
								for (String param : params) {
									String name = param.split("=")[0].trim();
									String value = param.split("=")[1].trim();
									if (name.compareTo("sr") == 0) {
										sr = value;
									}
									if (name.compareTo("bw") == 0) {
										bw = value;
									}
									if (name.compareTo("sn") == 0) {
										sn = value;
									}
								}
							} else {
								sr = "0";
								bw = "0";
								sn = "0";
							}
						} else { // etc ..
							mSerializer.attribute(null, "ip_address", uri.getHost());
							mSerializer.attribute(null, "port", uri.getPort() + "");
						}
					}
				} else {
					mSerializer.attribute(null, "ip_address", "0.0.0.0");
					mSerializer.attribute(null, "port", "0");
				}
				mSerializer.attribute(null, "gerne", chInfo.getGenre() + "");

				ArrayList<ESInfo> esInfoList = chInfo.getESList();
				int vpid = 0, vst = 0;
				int apid = 0, ast = 0, apid2 = 0, ast2 = 0;
				String alang = "", alang2 = "";
				for (ESInfo esinfo : esInfoList) {
					if (isVideoStream(esinfo.getStreamType())) {
						vpid = esinfo.getPid();
						vst = esinfo.getStreamType(); // last
					} else if (isAudioStream(esinfo.getStreamType())) {
						if (apid != 0) {
							alang2 = esinfo.getLang();
							apid2 = esinfo.getPid();
							ast2 = esinfo.getStreamType(); // last
						} else {
							alang = esinfo.getLang();
							apid = esinfo.getPid();
							ast = esinfo.getStreamType(); // first
						}
					}
				}
				mSerializer.attribute(null, "vpid", vpid + "");
				mSerializer.attribute(null, "apid", apid + "");
				mSerializer.attribute(null, "apid2", apid2 + "");
				mSerializer.attribute(null, "ppid", chInfo.getPcrPid() + "");
				mSerializer.attribute(null, "vst", vst + "");
				mSerializer.attribute(null, "ast", ast + "");
				mSerializer.attribute(null, "ast2", ast2 + "");
				mSerializer.attribute(null, "res", chInfo.isHDChannel() ? "1" : "0");
				mSerializer.attribute(null, "aplang", alang);
				mSerializer.attribute(null, "aplang2", alang2);

				mSerializer.attribute(null, "pay", chInfo.isPayChannel() ? "1" : "0");
				mSerializer.attribute(null, "sample", chInfo.getSampleTime() + "");
				mSerializer.attribute(null, "rating", chInfo.getRating() + "");
				mSerializer.attribute(null, "image", chInfo.getChannelImagePath());
				ArrayList<String> prdtIdList = chInfo.getProductIdList();
				String ids = "";
				int i = 0;
				for (String prdtId : prdtIdList) {
					if (i++ > 0)
						ids += ",";
					ids += prdtId.toString();
				}
				mSerializer.attribute(null, "product", ids);
				ArrayList<CAInfo> caInfoList = chInfo.getCAList();
				int casid = 0, capid = 0;
				for (CAInfo cainfo : caInfoList) {
					casid = cainfo.getCaSystemId();
					capid = cainfo.getCaPid();
				}
				mSerializer.attribute(null, "ca", chInfo.getFreeCaMode() + "");
				mSerializer.attribute(null, "ca_id", casid + "");
				mSerializer.attribute(null, "ca_pid", capid + "");
				mSerializer.attribute(null, "sn", sn);
				mSerializer.attribute(null, "sr", sr);
				mSerializer.attribute(null, "bw", bw); // last
				mSerializer.endTag(null, TAG_CHANNEL);
				return true;
			} catch (Exception e) {
				JLog.e(TAG, e.toString());
			}
		}
		return false;
	}

	private boolean storePrdtListToXml(ProductInfo prdtInfo) {
		if (prdtInfo != null) {
			try {
				mSerializer.startTag(null, TAG_PRODUCT);
				mSerializer.attribute(null, "id", prdtInfo.getProductId());
				mSerializer.attribute(null, "desc", prdtInfo.getProductDescription());
				mSerializer.endTag(null, TAG_PRODUCT);
				return true;
			} catch (Exception e) {
				JLog.e(TAG, e.toString());
			}
		}
		return false;
	}

	public void setChannelList(RetChannelList list) {
		clearChannels();
		synchronized (mChannels) {
			mChannels = list.getList();
		}
	}

	public void setProductList(RetProductList list) {
		clearProducts();
		synchronized (mProducts) {
			mProducts = list.getList();
		}
	}

	private boolean ip(String host) {
		Pattern p = Pattern
				.compile("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
		Matcher m = p.matcher(host);
		return m.find();
	}

	private boolean rf(String host) {
		if (host.compareTo("ntsc") == 0 || host.compareTo("vsb8") == 0
				|| host.compareTo("qam64") == 0 || host.compareTo("qam128") == 0
				|| host.compareTo("qam256") == 0) {
			return true;
		}
		return false;
	}

	private boolean isVideoStream(int streamType) {
		switch (streamType) {
		/* Video */
			case IESInfo.CONST_STREAM_TYPE_MPEG1Video:
			case IESInfo.CONST_STREAM_TYPE_MPEG2Video:
			case IESInfo.CONST_STREAM_TYPE_MPEG4Video:
			case IESInfo.CONST_STREAM_TYPE_H264Video:
				return true;
			default:
				break;
		}
		return false;
	}

	private boolean isAudioStream(int streamType) {
		switch (streamType) {
		/* Audio */
			case IESInfo.CONST_STREAM_TYPE_MPEG1Audio:
			case IESInfo.CONST_STREAM_TYPE_MPEG2Audio:
			case IESInfo.CONST_STREAM_TYPE_DVBAC3Audio:
			case IESInfo.CONST_STREAM_TYPE_AACAudio:
			case IESInfo.CONST_STREAM_TYPE_AC3Audio:
				return true;
			default:
				break;
		}
		return false;
	}

	private void clearChannels() {
		synchronized (mChannels) {
			if (mChannels.size() > 0) {
				mChannels.clear();
			}
		}
		mSdtVersion = 0xff;
	}

	private void clearProducts() {
		synchronized (mProducts) {
			if (mProducts.size() > 0) {
				mProducts.clear();
			}
		}
	}

} // end of class