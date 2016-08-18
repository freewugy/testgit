/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-01-10 17:34:00 +0900 (금, 10 1월 2014) $
 * $LastChangedRevision: 11091 $
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

public class StationXmlManager implements IConfConstant {
    private static final String TAG = "StationXmlManager";

    private static final String STATION_PROPERTIES_NAME = "station-list.conf";

    private static final String STATION_PROPERTIES_REAL_NAME = "station-list.conf.real";

    private static final String STATION_PROPERTIES = DATA_CONFIG_DIRECTORY + "/" + STATION_PROPERTIES_NAME;

    private static final String STATION_PROPERTIES_REAL = DATA_RUN_DIRECTORY + "/" + STATION_PROPERTIES_REAL_NAME;

    private static final String DEFAULT_CHANNEL_CALLSIGN = "";

    private static StationXmlManager instance = null;

    private Context mContext = null;

    private File mPropertiesFile = null;

    private Properties mProperties = null;

    private boolean isOkloadfile = false;

    private StationXmlManager(Context cont) {
        mContext = cont;

        initProperty(false);
        loadProperty();
    }

    public static StationXmlManager getInstance(Context cont) {
        if (instance == null) {
            instance = new StationXmlManager(cont);
        }
        return instance;
    }

    public void releaseInstance() {
        if (instance != null) {
            mPropertiesFile = null;
            mProperties = null;

            instance = null;
        }
    }

    public boolean isOkloadfile() {
        return isOkloadfile;
    }

    private synchronized void initProperty(boolean isForce) {
        File f = new File(STATION_PROPERTIES_REAL);

        if (f != null && f.exists()) {
            JLog.v(TAG, "start");
            FileInputStream fis = null;
            FileOutputStream fos = null;
            try {
                fis = new FileInputStream(f);
                fos = new FileOutputStream(STATION_PROPERTIES);
                if (fis != null && fos != null) {
                    int readcount = 0;
                    byte[] buffer = new byte[1024];
                    while ((readcount = fis.read(buffer, 0, 1024)) != -1) {
                        fos.write(buffer, 0, readcount);
                    }
                    return;
                }
            } catch (Exception e1) {
                JLog.e(TAG, e1.toString());
            } finally {
                try {
                    if (fos != null) {
                        fos.close();
                    }
                    if (fis != null) {
                        fis.close();
                    }
                } catch (Exception e2) {
                    JLog.e(TAG, e2.toString());
                }
            }
            JLog.v(TAG, "end");
        }

        f = new File(STATION_PROPERTIES);
        if (f != null && (f.exists() == false || isForce == true)) {
            JLog.v(TAG, "start");
            InputStream is = null;
            FileOutputStream fos = null;
            AssetManager assetManager = mContext.getResources().getAssets();
            try {
                is = assetManager.open(STATION_PROPERTIES_NAME, AssetManager.ACCESS_BUFFER);
                fos = new FileOutputStream(f);
                long filesize = is.available();
                byte[] tempdata = new byte[(int)filesize];
                is.read(tempdata);
                fos.write(tempdata);
            } catch (Exception e3) {
                JLog.e(TAG, e3.toString());
            } finally {
                try {
                    if (fos != null) {
                        fos.close();
                    }
                    if (is != null) {
                        is.close();
                    }
                } catch (Exception e4) {
                    JLog.e(TAG, e4.toString());
                }
            }
            JLog.v(TAG, "end");
        }
    }

    private synchronized void loadProperty() {
        boolean isSucceed = false;
        if (mPropertiesFile == null) {
            mPropertiesFile = new File(STATION_PROPERTIES);
        }

        if (mPropertiesFile != null) {
            if (mProperties == null) {
                mProperties = new Properties();
            }

            if (mProperties != null) {
                JLog.v(TAG, "start");
                FileInputStream fis = null;
                try {
                    fis = new FileInputStream(mPropertiesFile);
                    if (fis != null) {
                        mProperties.loadFromXML(fis);

                        isSucceed = true;
                    }
                } catch (Exception e1) {
                    JLog.e(TAG, e1.toString());

                    mPropertiesFile.delete();
                    mPropertiesFile = null;

                    initProperty(true);
                } finally {
                    try {
                        if (fis != null) {
                            fis.close();
                        }
                    } catch (Exception e2) {
                        JLog.e(TAG, e2.toString());
                    }
                }
                JLog.v(TAG, "end");
            }
        }

        isOkloadfile = isSucceed;
    }

    public String getChannelCallsign(int sid) {
        if (mProperties == null || sid == 0) {
            return DEFAULT_CHANNEL_CALLSIGN;
        }
        return mProperties.getProperty(sid + "");
    }

} // end of class
