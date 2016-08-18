/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-03-24 14:30:02 +0900 (월, 24 3월 2014) $
 * $LastChangedRevision: 11713 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice.util;

import android.os.AsyncTask;
import android.os.Handler;

import java.net.SocketTimeoutException;
import java.util.Timer;
import java.util.TimerTask;

public class HttpConnectTask extends AsyncTask<String, Void, Integer> {
    private static final String TAG = "HttpConnectTask";

    private static final int DEFAULT_CONN_CHECK_INTERVAL_MS = 3000;

    // for handler
    public static final int MSG_CONN_RESPONSE = 0;

    public static final int MSG_CONN_RESULT_OK = 0;

    public static final int MSG_CONN_RESULT_GENERAL_ERR = -1;

    public static final int MSG_CONN_RESULT_NETWORK_SOCKET_ERR = -2;

    private Handler mHandler = null;

    private HttpConnInfo mConnInfo = null;

    private HttpRequestPoster mHttpRequestPoster = null;

    private Timer mConnTimer = null;

    public class HttpConnInfo {

        public String request = null;

        public String response = null;

        public int result = 0;

    };

    public HttpConnectTask(Handler handler) {
        mHandler = handler;
    }

    @Override
    protected void onCancelled() {
        super.onCancelled();

        stopConnTimer();
        if (mHttpRequestPoster != null) {
            mHttpRequestPoster.cancel();
            mHttpRequestPoster = null;
        }
        if (mConnInfo != null) {
            mConnInfo = null;
        }
    }

    @Override
    protected Integer doInBackground(String... params) {
        int result = MSG_CONN_RESULT_OK;
        String downloadDoc = null;

        try {
            startConnTimer();

            if (mHttpRequestPoster == null) {
                mHttpRequestPoster = new HttpRequestPoster();
            }
            downloadDoc = mHttpRequestPoster.getData(params[0]);
        } catch (SocketTimeoutException e1) {
            JLog.e(TAG, e1.toString());
            result = MSG_CONN_RESULT_NETWORK_SOCKET_ERR;
        } catch (Exception e2) {
            JLog.e(TAG, e2.toString());
            result = MSG_CONN_RESULT_GENERAL_ERR;
        }

        if (mConnInfo == null) {
            mConnInfo = new HttpConnInfo();
        }
        mConnInfo.request = params[0];
        mConnInfo.response = downloadDoc;

        return result;
    }

    private void startConnTimer() {
        stopConnTimer();

        if (mConnTimer == null) {
            mConnTimer = new Timer();

            mConnTimer.schedule(new TimerTask() {

                @Override
                public boolean cancel() {
                    return super.cancel();
                }

                @Override
                public void run() {
                    if (getStatus() != AsyncTask.Status.FINISHED) {
                        onCancelled();

                        sendConnMessage(MSG_CONN_RESULT_NETWORK_SOCKET_ERR);
                    }
                }

            }, DEFAULT_CONN_CHECK_INTERVAL_MS);
        }
    }

    private void stopConnTimer() {
        if (mConnTimer != null) {
            mConnTimer.cancel();
            mConnTimer = null;
        }
    }

    @Override
    protected void onPostExecute(Integer result) {
        JLog.d(TAG, "=============================================================");
        JLog.d(TAG, "> REQ [" + mConnInfo.request + "]");
        JLog.d(TAG, "-------------------------------------------------------------");
        JLog.d(TAG, "> RES [" + mConnInfo.response + "]");
        JLog.d(TAG, "=============================================================");

        sendConnMessage(result);

        stopConnTimer();
        if (mHttpRequestPoster != null) {
            mHttpRequestPoster.cancel();
            mHttpRequestPoster = null;
        }
        if (mConnInfo != null) {
            mConnInfo = null;
        }
    }

    private void sendConnMessage(Integer result) {
        if (mConnInfo != null) {
            mConnInfo.result = result;
            mHandler.sendMessage(mHandler.obtainMessage(MSG_CONN_RESPONSE, mConnInfo));
        }
    }

} // end of class
