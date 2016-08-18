/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-04-02 20:16:24 +0900 (수, 02 4월 2014) $
 * $LastChangedRevision: 11777 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice.conf;

import com.tvstorm.tv.siservice.util.JLog;

import android.content.Context;
import android.content.res.AssetManager;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.Properties;

public class ChannelConfManager implements IConfConstant {
    private static final String TAG = "ChannelConfManager";

    private static final String TVSDB_PROPERTIES_NAME = "channel.conf";

    private static final String TVSDB_PROPERTIES = DATA_CONFIG_DIRECTORY + "/" + TVSDB_PROPERTIES_NAME;

    private static final String TVSDB_PROPERTIES_OLD = DATA_RUN_DIRECTORY + "/" + TVSDB_PROPERTIES_NAME;

    // Scheme
    public static final String ACCESS_POINT = "ap";

    // Host/Port
    public static final String IP_ADDRESS = "ip";

    public static final String IP_PORT = "port";

    public static final String RF_DEMOD = "demod";

    public static final String RF_FREQKHZ = "freqkHz";

    public static final String FILE_PATH = "fpath";

    public static final String FILE_NAME = "fname";

    // mandatory Query
    public static final String EPG_PROVIDER_NAME = "pn";

    public static final String DB_TYPE = "db";

    // optional Query
    public static final String LOCALAREA_CODES = "la";

    public static final String SYMBOL_RATE = "sr";

    public static final String BAND_WIDTH = "bw";

    public static final String WEB_EPG = "wepg";

    public static final String WEB_SI = "websi";

    public static final String USE_TDT = "tdt";
    
    public static final String USE_XML = "xml";
    
    public static final String SI_INIT = "tvs.si.initialize";

    // default
    private static final String DEFAULT_ACCESS_POINT = "c"; // i (udp), t (dvb-t), c (dvb-c), s (dvb-s), f (file)

    private static final String DEFAULT_IP_ADDRESS = "239.192.60.3";

    private static final String DEFAULT_IP_PORT = "49200";

    private static final String DEFAULT_RF_DEMOD = "64"; // 8 (vsb8), 16 (qam16), 32 (qam32), 64 (qam64), 128 (qam128), 256 (qam256)

    private static final String DEFAULT_RF_FREQKHZ = "466000";

    private static final String DEFAULT_FILE_PATH = DATA_HOME_DIRECTORY;

    private static final String DEFAULT_FILE_NAME = "sample.ts";

    private static final String DEFAULT_EPG_PROVIDER_NAME = "lkn"; // skb, lkn

    private static final String DEFAULT_DB_TYPE = "f"; // f (file) m (memory) 

    private static final String DEFAULT_LOCALAREA_CODES = "1|41|61"; // for seoul mbc|kbs1|sbs

    private static final String DEFAULT_SYMBOL_RATE = "6875"; // unit:ksps

    private static final String DEFAULT_BAND_WIDTH = "8"; // unit:MHz

    private static final String DEFAULT_WEB_EPG = "n"; // y (use) n (not use)

    private static final String DEFAULT_WEB_SI = "n"; // y (use) n (not use)

    private static final String DEFAULT_USE_TDT = "y"; // y (use) n (not use)

    private static final String DEFAULT_USE_XML = "n"; // y (use) n (not use)
    
    private static final String DEFAULT_SI_INIT= "n"; // y (use) n (not use)
    
    private static ChannelConfManager instance = null;

    private Context mContext = null;

    private File mPropertiesFile = null;

    private Properties mProperties = null;

    private boolean isOkloadfile = false;

    private ChannelConfManager(Context cont) {
        mContext = cont;

        initProperty(false);
        loadProperty();
    }

    public static ChannelConfManager getInstance(Context cont) {
        if (instance == null) {
            instance = new ChannelConfManager(cont);
        }
        return instance;
    }

    public boolean isOkloadfile() {
        return isOkloadfile;
    }

    private synchronized void initProperty(boolean isForce) {
        File f = new File(TVSDB_PROPERTIES_OLD);

        if (f != null && f.exists()) {
            f.delete();
        }

        f = new File(TVSDB_PROPERTIES);
        if (f != null && (f.exists() == false || isForce == true)) {
            JLog.v(TAG, "start");
            AssetManager assetManager = mContext.getResources().getAssets();
            try {
                InputStream is = assetManager.open(TVSDB_PROPERTIES_NAME, AssetManager.ACCESS_BUFFER);
                FileOutputStream fos = new FileOutputStream(f);
                long filesize = is.available();
                byte[] tempdata = new byte[(int)filesize];
                is.read(tempdata);
                fos.write(tempdata);
                fos.close();
                is.close();
            } catch (Exception e) {
                JLog.e(TAG, e.toString());
            }
            JLog.v(TAG, "end");
        }
    }

    private synchronized void loadProperty() {
        boolean isSucceed = false;
        if (mPropertiesFile == null) {
            mPropertiesFile = new File(TVSDB_PROPERTIES);
        }

        if (mPropertiesFile != null) {
            if (mProperties == null) {
                mProperties = new Properties();
            }

            if (mProperties != null) {
                JLog.v(TAG, "start");
                try {
                    FileInputStream fis = new FileInputStream(mPropertiesFile);
                    if (fis != null) {
                        mProperties.loadFromXML(fis);
                        fis.close();

                        isSucceed = true;
                    }
                } catch (Exception e) {
                    JLog.e(TAG, e.toString());

                    mPropertiesFile.delete();
                    mPropertiesFile = null;

                    initProperty(true);
                    mProperties.setProperty(ACCESS_POINT, DEFAULT_ACCESS_POINT);
                    mProperties.setProperty(IP_ADDRESS, DEFAULT_IP_ADDRESS);
                    mProperties.setProperty(IP_PORT, DEFAULT_IP_PORT);
                    mProperties.setProperty(RF_DEMOD, DEFAULT_RF_DEMOD);
                    mProperties.setProperty(RF_FREQKHZ, DEFAULT_RF_FREQKHZ);
                    mProperties.setProperty(FILE_PATH, DEFAULT_FILE_PATH);
                    mProperties.setProperty(FILE_NAME, DEFAULT_FILE_NAME);
                    mProperties.setProperty(EPG_PROVIDER_NAME, DEFAULT_EPG_PROVIDER_NAME);
                    mProperties.setProperty(DB_TYPE, DEFAULT_DB_TYPE);
                    mProperties.setProperty(LOCALAREA_CODES, DEFAULT_LOCALAREA_CODES);
                    mProperties.setProperty(SYMBOL_RATE, DEFAULT_SYMBOL_RATE);
                    mProperties.setProperty(BAND_WIDTH, DEFAULT_BAND_WIDTH);
                    mProperties.setProperty(WEB_EPG, DEFAULT_WEB_EPG);
                    mProperties.setProperty(WEB_SI, DEFAULT_WEB_SI);
                    mProperties.setProperty(USE_TDT, DEFAULT_USE_TDT);
                    mProperties.setProperty(USE_XML, DEFAULT_USE_XML);
                    mProperties.setProperty(SI_INIT, DEFAULT_SI_INIT);
                }
                JLog.v(TAG, "end");
            }
        }

        isOkloadfile = isSucceed;
    }

    private synchronized void storeProperty() {
        if (mPropertiesFile == null) {
            mPropertiesFile = new File(TVSDB_PROPERTIES);
        }

        if (mPropertiesFile != null) {
            if (mProperties == null) {
                mProperties = new Properties();
            }

            if (mProperties != null) {
                JLog.v(TAG, "start");
                try {
                    FileOutputStream fos = new FileOutputStream(mPropertiesFile);
                    mProperties.storeToXML(fos, "DVBSI-PROPERTIES");
                    fos.getFD().sync();
                    fos.close();
                } catch (Exception e) {
                    JLog.e(TAG, e.toString());
                }
                JLog.v(TAG, "end");
            }
        }
    }

    // getters
    public String getAccessPoint() {
        if (mProperties == null) {
            return DEFAULT_ACCESS_POINT;
        }
        String value = mProperties.getProperty(ACCESS_POINT);
        if (value == null || value.isEmpty()) {
            return DEFAULT_ACCESS_POINT;
        }
        return value;
    }

    public String getIPAddress() {
        if (mProperties == null) {
            return DEFAULT_IP_ADDRESS;
        }
        String value = mProperties.getProperty(IP_ADDRESS);
        if (value == null || value.isEmpty()) {
            return DEFAULT_IP_ADDRESS;
        }
        return value;

    }

    public String getIPPort() {
        if (mProperties == null) {
            return DEFAULT_IP_PORT;
        }
        String value = mProperties.getProperty(IP_PORT);
        if (value == null || value.isEmpty()) {
            return DEFAULT_IP_PORT;
        }
        return value;

    }

    public String getDemod() {
        if (mProperties == null) {
            return DEFAULT_RF_DEMOD;
        }
        String value = mProperties.getProperty(RF_DEMOD);
        if (value == null || value.isEmpty()) {
            return DEFAULT_RF_DEMOD;
        }
        return value;
    }

    public String getFreqkHz() {
        if (mProperties == null) {
            return DEFAULT_RF_FREQKHZ;
        }
        String value = mProperties.getProperty(RF_FREQKHZ);
        if (value == null || value.isEmpty()) {
            return DEFAULT_RF_FREQKHZ;
        }
        return value;
    }

    public String getFilePath() {
        if (mProperties == null) {
            return DEFAULT_FILE_PATH;
        }
        String value = mProperties.getProperty(FILE_PATH);
        if (value == null || value.isEmpty()) {
            return DEFAULT_FILE_PATH;
        }
        return value;
    }

    public String getFileName() {
        if (mProperties == null) {
            return DEFAULT_FILE_NAME;
        }
        String value = mProperties.getProperty(FILE_NAME);
        if (value == null || value.isEmpty()) {
            return DEFAULT_FILE_NAME;
        }
        return value;
    }

    public String getEPGProviderName() {
        if (mProperties == null) {
            return DEFAULT_EPG_PROVIDER_NAME;
        }
        String value = mProperties.getProperty(EPG_PROVIDER_NAME);
        if (value == null || value.isEmpty()) {
            return DEFAULT_EPG_PROVIDER_NAME;
        }
        return value;
    }

    public String getDBType() {
        if (mProperties == null) {
            return DEFAULT_DB_TYPE;
        }
        String value = mProperties.getProperty(DB_TYPE);
        if (value == null || value.isEmpty()) {
            return DEFAULT_DB_TYPE;
        }
        return value;
    }

    public String getLocalAreaCodes() {
        if (mProperties == null) {
            return DEFAULT_LOCALAREA_CODES;
        }
        String value = mProperties.getProperty(LOCALAREA_CODES);
        if (value == null || value.isEmpty()) {
            return DEFAULT_LOCALAREA_CODES;
        }
        return value;
    }

    public String getSymbolRate() {
        if (mProperties == null) {
            return DEFAULT_SYMBOL_RATE;
        }
        String value = mProperties.getProperty(SYMBOL_RATE);
        if (value == null || value.isEmpty()) {
            return DEFAULT_SYMBOL_RATE;
        }
        return value;
    }

    public String getBandWidth() {
        if (mProperties == null) {
            return DEFAULT_BAND_WIDTH;
        }
        String value = mProperties.getProperty(BAND_WIDTH);
        if (value == null || value.isEmpty()) {
            return DEFAULT_BAND_WIDTH;
        }
        return value;
    }

    public String getWebEpg() {
        if (mProperties == null) {
            return DEFAULT_WEB_EPG;
        }
        String value = mProperties.getProperty(WEB_EPG);
        if (value == null || value.isEmpty()) {
            return DEFAULT_WEB_EPG;
        }
        return value;
    }

    public String getWebSI() {
        if (mProperties == null) {
            return DEFAULT_WEB_SI;
        }
        String value = mProperties.getProperty(WEB_SI);
        if (value == null || value.isEmpty()) {
            return DEFAULT_WEB_SI;
        }
        return value;
    }

    public String getUseTDT() {
        if (mProperties == null) {
            return DEFAULT_USE_TDT;
        }
        String value = mProperties.getProperty(USE_TDT);
        if (value == null || value.isEmpty()) {
            return DEFAULT_USE_TDT;
        }
        return value;
    }

    public String getUseXML() {
        if (mProperties == null) {
            return DEFAULT_USE_XML;
        }
        String value = mProperties.getProperty(USE_XML);
        if (value == null || value.isEmpty()) {
            return DEFAULT_USE_XML;
        }
        return value;
    }
    
    public String getSI_INIT() {
        if (mProperties == null) {
            return DEFAULT_SI_INIT;
        }
        String value = mProperties.getProperty(SI_INIT);
        if (value == null || value.isEmpty()) {
            return DEFAULT_SI_INIT;
        }
        return value;
    }
    
    public void setSI_INIT(String value) {
    	mProperties.setProperty(SI_INIT, value);
    }
    

} // end of class
