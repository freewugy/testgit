
package com.tvstorm.tv.siservice.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.SocketTimeoutException;
import java.net.URL;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLException;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

public class HttpRequestPoster {
    private static final String TAG = "HttpRequestPoster";

    private static final int MAX_CONNECTTION_TIMEOUT_MS = 3000;

    private static final String PROTOCOL_TYPE_HTTPS = "https";

    private HttpURLConnection mConn = null;

    private InputStream mIs = null;

    private InputStreamReader mIr = null;

    private BufferedReader mBr = null;

    private boolean mCancel = false;

    public void cancel() {
        mCancel = true;

        cleanConnection();
    }

    private boolean isCancelled() {
        return mCancel;
    }

    private void cleanConnection() {
        try {
            if (mBr != null) {
                mBr.close();
                mBr = null;
            }

            if (mIr != null) {
                mIr.close();
                mIr = null;
            }

            if (mIs != null) {
                mIs.close();
                mIs = null;
            }

            if (mConn != null) {
                mConn.disconnect();
                mConn = null;
            }
        } catch (Exception e) {
            JLog.e(TAG, e.toString());
        } finally {
            mCancel = false;
        }
    }

    private void trustAllHosts() {
        // Create a trust manager that does not validate certificate chains 
        TrustManager[] trustAllCerts = new TrustManager[] {
            new X509TrustManager() {
                public java.security.cert.X509Certificate[] getAcceptedIssuers() {
                    return new java.security.cert.X509Certificate[] {};
                }

                @Override
                public void checkClientTrusted(java.security.cert.X509Certificate[] chain, String authType)
                        throws java.security.cert.CertificateException {
                    // TODO Auto-generated method stub 
                }

                @Override
                public void checkServerTrusted(java.security.cert.X509Certificate[] chain, String authType)
                        throws java.security.cert.CertificateException {
                    // TODO Auto-generated method stub 
                }
            }
        };

        // Install the all-trusting trust manager 
        try {
            SSLContext sc = SSLContext.getInstance("TLS");
            sc.init(null, trustAllCerts, new java.security.SecureRandom());
            HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
        } catch (Exception e) {
            JLog.e(TAG, e.toString());
        }
    }

    final HostnameVerifier DO_NOT_VERIFY = new HostnameVerifier() {

        @Override
        public boolean verify(String arg0, SSLSession arg1) {
            // TODO Auto-generated method stub
            return true;
        }
    };

    public String getData(String requestUrl) throws SocketTimeoutException, IOException, Exception {
        boolean isSucceed = false;
        StringBuilder data = new StringBuilder();

        try {
            URL url = new URL(requestUrl);

            if (url.getProtocol().toLowerCase().equals(PROTOCOL_TYPE_HTTPS)) {
                trustAllHosts();
                HttpsURLConnection https = (HttpsURLConnection)url.openConnection();
                https.setHostnameVerifier(DO_NOT_VERIFY);

                mConn = https;
            } else {
                mConn = (HttpURLConnection)url.openConnection();
            }

            if (mConn != null) {
                mConn.setConnectTimeout(MAX_CONNECTTION_TIMEOUT_MS);
                // 안드로이드 버전에 따라서, HTTP 연결이 불안정하여 실패하는 경우가 종종 발생
                // connection pool 기능을 사용하지 않도록 하여 연결 문제가 해결되었다고 함
                System.setProperty("http.keepAlive", "false");

                mConn.connect();

                int responseCode = mConn.getResponseCode();
                if (responseCode == HttpURLConnection.HTTP_OK) {

                    mIs = mConn.getInputStream();
                    mIr = new InputStreamReader(mIs);
                    mBr = new BufferedReader(mIr);

                    String line;
                    while ((isCancelled() == false) && (line = mBr.readLine()) != null) {
                        data.append(line + "\n");
                    }

                    isSucceed = true;
                    if (isCancelled()) {
                        isSucceed = false;
                    }
                } else {
                    JLog.w(TAG, "+=============================================+");
                    JLog.w(TAG, "| HTTP Error Response                         |");
                    JLog.w(TAG, "+---------------------------------------------+");
                    JLog.w(TAG, "|> HTTP Request  URL [" + requestUrl + "]");
                    JLog.w(TAG, "|> HTTP Response Code[" + responseCode + "]");
                    JLog.w(TAG, "|> HTTP Response Msg [" + mConn.getResponseMessage() + "]");
                    JLog.w(TAG, "+=============================================+");
                }
            }
        } catch (SocketTimeoutException e1) {
            JLog.e(TAG, e1.toString());
        } catch (SSLException e2) {
            JLog.e(TAG, e2.toString());
        } catch (Exception e3) {
            JLog.e(TAG, e3.toString());
        } finally {
            cleanConnection();
        }

        if (isSucceed == false || data == null) {
            return null;
        }
        return data.toString();
    }

} // end of class
