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

import android.content.Context;

public class NetworkConfManager implements IConfConstant {
    private static final String TAG = "NetworkConfManager";

    private static NetworkConfManager instance = null;

    private Context mContext = null;

    private boolean isAvailable = false;

    private boolean isWepgConnected = false;

    private boolean isWifiEnabled = false;

    private boolean isCmEnabled = false;

    private NetworkConfManager(Context cont) {
        mContext = cont;
    }

    public static NetworkConfManager getInstance(Context cont) {
        if (instance == null) {
            instance = new NetworkConfManager(cont);
        }
        return instance;
    }

    public boolean isAvailable() {
        return isAvailable;
    }

    public boolean isWepgConnected() {
        return isWepgConnected;
    }

    public void setWepgConnected(boolean isConn) {
        isWepgConnected = isConn;
    }

    public boolean isWifiEnabled() {
        return isWifiEnabled;
    }

    public void setWifiEnabled(boolean isEnab) {
        isWifiEnabled = isEnab;
    }

    public boolean isCmEnabled() {
        return isCmEnabled;
    }

    public void setCmEnabled(boolean isEnab) {
        isCmEnabled = isEnab;
    }

    public synchronized boolean checkConnection() {
        isAvailable = false;

        //if (/* isWepgConnected == false && */(isWifiEnabled || isCmEnabled)) {
        if (isWifiEnabled || isCmEnabled) {

            //isWepgConnected = true;
            isAvailable = true;
            return true;
        }
        return false;
    }

    private static String getApplicationName(Context context) {
        int stringId = context.getApplicationInfo().labelRes;
        return context.getString(stringId);
    }

} // end of class
