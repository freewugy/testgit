/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-08-27 20:17:13 +0900 (수, 27 8월 2014) $
 * $LastChangedRevision: 12976 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice.conf;

import com.tvstorm.tv.siservice.util.HttpConnectTask;
import com.tvstorm.tv.siservice.util.HttpConnectTask.HttpConnInfo;
import com.tvstorm.tv.siservice.util.JLog;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStream;

//eschoi_20140323 | Spec. xxx

public class WepgXmlManager implements IConfConstant {
    private static final String TAG = "WepgXmlManager";

    private static final boolean USD_DEBUG = false;

    // @see ${workspace_loc:/TVSIService/jni/src/Android.mk}
    private static final boolean USE_WEPG_V2 = true;

    private static final String SERVER_XML_NAME = "server-list.conf";

    private static final String SERVER_XML = DATA_CONFIG_DIRECTORY + "/" + SERVER_XML_NAME;

    private static final String CHANNELMAP_TXT_NAME = "channelmap.txt";
    
    private static final String CHANNELMAP_XML_NAME = "channels.xml";

    private static final String CHANNELMAP_TXT = DATA_RUN_DIRECTORY + "/" + CHANNELMAP_TXT_NAME;

    // for service-list.conf
    private static final String TAG_SERVER_LIST = "server-list";

    private static final String TAG_SERVER = "server";

    private static final String ATTR_ID_NAME = "id";

    private static final String ATTR_ADDR_NAME = "address";

    private static final String ATTR_PORT_NAME = "port";

    private static final String ATTR_ID_VALUE_WEPG = "wepg";
    
    private static final String ATTR_ID_VALUE_WEBSI = "websi";

    private static final String DUMMY_ENDPOINT_IP_V1 = "http://139.228.191.243";

    private static final String DUMMY_ENDPOINT_IP_V2 = "http://139.228.191.244";
    
    private static final String DUMMY_WEBSI_IP = "http://139.228.191.244";

    private static final String DUMMY_ENDPOINT_PORT = "80";

    private static WepgXmlManager instance = null;

    private Context mContext = null;

    private XmlPullParser mParser = null;

    private boolean isOkloadXmlfile = false;

    private boolean isHttpConnected = false;

    private String mWebEPGUrl = null;
    
    private String mWebSIUrl = null;

    private WepgXmlManager(Context cont) {
        mContext = cont;

        loadFromXml();
    }

    public static WepgXmlManager getInstance(Context cont) {
        if (instance == null) {
            instance = new WepgXmlManager(cont);
        }
        return instance;
    }

    public boolean isOkloadXmlfile() {
        return isOkloadXmlfile;
    }

    public boolean isHttpConnected() {
        return isHttpConnected;
    }

    public synchronized void loadFromXml() {
        boolean isSucceed = true;
        File serverXmlFile = new File(SERVER_XML);

        if (serverXmlFile != null && serverXmlFile.exists()) {
            InputStream is = null;
            try {
                is = new FileInputStream(serverXmlFile);
            } catch (Exception e1) {
                JLog.e(TAG, e1.toString());

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

                            switch (eventType) {
                                case XmlPullParser.START_DOCUMENT:
                                case XmlPullParser.END_DOCUMENT:
                                case XmlPullParser.END_TAG:
                                case XmlPullParser.TEXT:
                                    if (USD_DEBUG) {
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
                                    if (USD_DEBUG) {
                                        Log.d(TAG, "START_TAG " + mParser.getName());
                                        for (int i = 0; i < mParser.getAttributeCount(); i++) {
                                            Log.d(TAG, i + ": + " + mParser.getAttributeName(i) + "/" + mParser.getAttributeValue(i));
                                        }
                                    }

                                    if (TAG_SERVER_LIST.equals(mParser.getName())) {
                                        clearServer();
                                    } else if (TAG_SERVER.equals(mParser.getName())) {
                                        loadServerFromXml(mParser.getAttributeCount());
                                    }
                                    break;

                            } // end of swtich

                            try {
                                eventType = mParser.next();
                            } catch (Exception e2) {
                                JLog.e(TAG, e2.toString());
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
            }  // end of isNull(is)
        } else {
            isSucceed = false;
        } // end of isNull(serverXmlFile)

        isOkloadXmlfile = isSucceed;
        if (isOkloadXmlfile != true) {
            clearServer();
        }
    }

    private void loadServerFromXml(int attrCount) {
        String val = mParser.getAttributeValue(null, ATTR_ID_NAME);
        if (val == null)
        	return;
        if (val.equals(ATTR_ID_VALUE_WEPG)) {

            String addr = mParser.getAttributeValue(null, ATTR_ADDR_NAME);
            String port = mParser.getAttributeValue(null, ATTR_PORT_NAME);

            if (addr != null && port != null) {
                //mWebEPGUrl = addr + ":" + port + "/EPG/" + CHANNELMAP_TXT_NAME;
            	mWebEPGUrl = addr + "/EPG/" + CHANNELMAP_TXT_NAME;
            }
        } else if (val.equals(ATTR_ID_VALUE_WEBSI)) {

            String addr = mParser.getAttributeValue(null, ATTR_ADDR_NAME);
            String port = mParser.getAttributeValue(null, ATTR_PORT_NAME);

            if (addr != null && port != null) {
            	//mWebSIUrl = addr + ":" + port + "/EPG/" + CHANNELMAP_XML_NAME;
            	mWebSIUrl = addr + "/EPG/" + CHANNELMAP_XML_NAME;
            }
        }
    }

    public String getWebEPGUrl() {
        if (mWebEPGUrl == null) {
            if (USE_WEPG_V2) {
                return DUMMY_ENDPOINT_IP_V2 + ":" + DUMMY_ENDPOINT_PORT + "/EPG/" + CHANNELMAP_TXT_NAME;
            } else {
                return DUMMY_ENDPOINT_IP_V1 + ":" + DUMMY_ENDPOINT_PORT + "/EPG/" + CHANNELMAP_TXT_NAME;
            }
        }
        return mWebEPGUrl;
    }

    public String getWebSIUrl() {
        if (mWebSIUrl == null) {
            return DUMMY_WEBSI_IP + ":" + DUMMY_ENDPOINT_PORT + "/EPG/" + CHANNELMAP_XML_NAME;
        }
        return mWebSIUrl;
    }

    public synchronized boolean checkConnection() {
        isHttpConnected = false;

        String urlStr = getWebEPGUrl();
        if (urlStr != null && urlStr.length() > 0) {
            HttpConnectTask task = new HttpConnectTask(mHandler);
            task.execute(urlStr);
            return true;
        }
        return false;
    }

    public synchronized void storeToXml(String doc) {
        if (USD_DEBUG) {
            if (doc == null || doc.length() <= 0) {
                return;
            }

            FileWriter fw = null;
            try {
                File f = new File(CHANNELMAP_TXT);
                if (f != null && f.exists()) {
                    f.delete();
                }

                f.createNewFile();
                f.setReadable(true, false);
                f.setWritable(true, false);

                fw = new FileWriter(f);
                fw.write(doc);
            } catch (Exception e1) {
                JLog.e(TAG, e1.toString());
            } finally {
                try {
                    if (fw != null) {
                        fw.flush();
                        fw.close();
                        fw = null;
                    }
                } catch (Exception e2) {
                    JLog.e(TAG, e2.toString());
                }
            }
        }
    }

    /**
     * NOTIFY: messages to communicate with sub module
     */
    public Handler mHandler = new Handler() {

        @Override
        public synchronized void handleMessage(Message msg) {
            if (instance == null || msg.obj == null)
                return;

            switch (msg.what) {
                case HttpConnectTask.MSG_CONN_RESPONSE:
                    HttpConnInfo info = (HttpConnInfo)msg.obj;
                    if (info.result == HttpConnectTask.MSG_CONN_RESULT_OK) {
                        isHttpConnected = true;
                        storeToXml(info.response);
                    }
                    JLog.i(TAG, getApplicationName(mContext) + "_checkWepgConnection=" + (isHttpConnected() ? "y" : "n") + " --!>");
                    break;
            }
        }

    };

    private static String getApplicationName(Context context) {
        int stringId = context.getApplicationInfo().labelRes;
        return context.getString(stringId);
    }

    private void clearServer() {
        if (mWebEPGUrl != null) {
            synchronized (mWebEPGUrl) {
                mWebEPGUrl = null;
            }
        }
    }

} // end of class
