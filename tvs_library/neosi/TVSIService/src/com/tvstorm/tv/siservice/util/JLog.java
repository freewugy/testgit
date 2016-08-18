/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2013-07-15 19:33:46 +0900 (월, 15 7월 2013) $
 * $LastChangedRevision: 9085 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice.util;

import android.util.Log;

public class JLog {

    public static final boolean LOG_ENABLED = true;

    public static final boolean LOG_DISABLE = false;

    public static String mAppName = "";

    public static boolean misEnabled = true;

    // 태그 우선순의 Verbose < Debug < Info < Waring < Error < Fatal < Slient
    // JLog format > 로그레벌/어플명(PID): 클래스명:메소드명(라인번호):메세지
    // 디버깅 로그레벌 설정 Verbose, Debug
    // setprop log.tag.TVSIService VERBOSE
    // setprop log.tag.TVSIService DEBUG
    // adb logcat TVSIService:D *:S
    public static void setAppName(String appName) {
        mAppName = appName;
    }

    public static String getAppName() {
        return mAppName;
    }

    public static void setOutput(boolean isEnabled) {
        misEnabled = isEnabled;
    }

    public static boolean getOutput() {
        return misEnabled;
    }

    public static void v(String tag, String msg) {
        if (misEnabled && Log.isLoggable(mAppName, Log.VERBOSE)) {
            String className = tag;
            String methodName = new Exception().getStackTrace()[1].getMethodName();
            int lineNumber = new Exception().getStackTrace()[1].getLineNumber();

            Log.v(mAppName, className + ":" + methodName + "(" + lineNumber + "):" + msg);
        }
    }

    public static void d(String tag, String msg) {
        if (misEnabled && Log.isLoggable(mAppName, Log.DEBUG)) {
            String className = tag;
            String methodName = new Exception().getStackTrace()[1].getMethodName();
            int lineNumber = new Exception().getStackTrace()[1].getLineNumber();

            Log.d(mAppName, className + ":" + methodName + "(" + lineNumber + "):" + msg);
        }
    }

    public static void i(String tag, String msg) {
        if (misEnabled) {
            String className = tag;
            String methodName = new Exception().getStackTrace()[1].getMethodName();
            int lineNumber = new Exception().getStackTrace()[1].getLineNumber();

            Log.i(mAppName, className + ":" + methodName + "(" + lineNumber + "):" + msg);
        }
    }

    public static void w(String tag, String msg) {
        if (misEnabled) {
            String className = tag;
            String methodName = new Exception().getStackTrace()[1].getMethodName();
            int lineNumber = new Exception().getStackTrace()[1].getLineNumber();

            Log.w(mAppName, className + ":" + methodName + "(" + lineNumber + "):" + msg);
        }
    }

    public static void e(String tag, String msg) {
        if (misEnabled) {
            String className = tag;
            String methodName = new Exception().getStackTrace()[1].getMethodName();
            int lineNumber = new Exception().getStackTrace()[1].getLineNumber();

            Log.e(mAppName, className + ":" + methodName + "(" + lineNumber + "):" + msg);
        }
    }

    // debug log for developers
    public static void t(String tag, String msg) {
        String className = tag;
        String methodName = new Exception().getStackTrace()[1].getMethodName();
        int lineNumber = new Exception().getStackTrace()[1].getLineNumber();

        Log.d(mAppName, className + ":" + methodName + "(" + lineNumber + "):" + msg);
    }

} // end of class
