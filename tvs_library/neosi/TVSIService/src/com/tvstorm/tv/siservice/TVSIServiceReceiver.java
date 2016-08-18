/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-06-13 23:22:00 +0900 (금, 13 6월 2014) $
 * $LastChangedRevision: 12467 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

import com.tvstorm.tv.siservice.conf.IConfConstant;
import com.tvstorm.tv.siservice.conf.NetworkConfManager;
import com.tvstorm.tv.siservice.util.JLog;
import com.tvstorm.tv.systemframework.network.IEthernet;

public class TVSIServiceReceiver extends BroadcastReceiver implements IConfConstant {
	private static final String TAG = "TVSIServiceReceiver";

	private static final boolean USD_DEBUG = true;

	private static final String ACTION_SLEEP_WAKEUP = "com.tvstorm.tv.SLEEP_WAKEUP";

	private static final String ACTION_HIDDEN_MENU = "com.tvstorm.tv.HIDDEN_MENU";

	private static final String ACTION_NETSTATE_CHANGED = "com.tvstorm.tv.systemframework.network.NetStateChangedEvent";

	private static final String EXTRA_SLEEP_WAKEUP = "isSleep";

	private static final String EXTRA_HIDDEN_MENU = "isHidden";

	private Object mLock = new Object();

	@Override
	public void onReceive(Context context, Intent intent) {
		JLog.i(TAG, intent.getAction());
		if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
			Log.d(TAG, "onReceive: " + intent.getAction());

			Intent serviceIntent = new Intent();
			serviceIntent.setClassName(BIND_INTENT_ACTION, BIND_INTENT_ACTION + ".TVSIService");

			context.startService(serviceIntent);
		} else if (intent.getAction().equalsIgnoreCase(ACTION_SLEEP_WAKEUP)) {
			Log.d(TAG,
					"onReceive:"
							+ intent.getAction()
							+ "/status="
							+ (intent.getBooleanExtra(EXTRA_SLEEP_WAKEUP, false) ? "sleep"
									: "wakeup"));

		} else if (intent.getAction().equalsIgnoreCase(ACTION_HIDDEN_MENU)) {
			Log.d(TAG,
					"onReceive:"
							+ intent.getAction()
							+ "/status="
							+ (intent.getBooleanExtra(EXTRA_HIDDEN_MENU, false) ? "outside"
									: "inside"));

		} else if (intent.getAction().equalsIgnoreCase(ACTION_NETSTATE_CHANGED)) {
			Log.d(TAG, "onReceive:" + intent.getAction());			
			int networkType = intent.getIntExtra(IEthernet.NET_TYPE_INFO,
					IEthernet.NETWORK_TYPE_UNKNWON);
			int state = intent.getIntExtra(IEthernet.NET_STATE_INFO,
					IEthernet.NETWORK_STATE_UNKNOWN);
			if (USD_DEBUG) {
				Log.d(TAG, "--------------------------------------------------");
				Log.d(TAG, "networkType = " + getNetworkName(networkType));
				Log.d(TAG, "state = " + state); // getNetworkState(state));
				Log.d(TAG, "--------------------------------------------------");
			}
			
			NetworkConfManager netManager = NetworkConfManager.getInstance(context);
			boolean isPrevConnected = netManager.checkConnection();
			if (networkType == IEthernet.NETWORK_TYPE_WIFI) {
				netManager.setWifiEnabled(state == 1 ? true : false);
			} else if (networkType == IEthernet.NETWORK_TYPE_ETHERNET) {
				netManager.setCmEnabled(state == 1 ? true : false);
			}
			if (isPrevConnected == false && netManager.checkConnection()) {
				JLog.i(TAG,
						getApplicationName(context) + "_checkNetConnection="
								+ (netManager.checkConnection() ? "y" : "n"));

				TVSIServiceManager siManager = TVSIService.getSIManager();
				if (siManager != null) {
					synchronized (mLock) {
						siManager.setNetworkConnectionInfo(true);
					}
				}
			}
				
		} else if (intent.getAction().equals(ConnectivityManager.CONNECTIVITY_ACTION)) {
            Log.d(TAG, "onReceive:" + intent.getAction());
            
            if (USD_DEBUG) {
                ConnectivityManager conn = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);
                if (true/* USD_DEBUG */) {
                    NetworkInfo[] nets = conn.getAllNetworkInfo();
                    for (NetworkInfo net : nets) {
                        Log.d(TAG, "--------------------------------------------------");
                        Log.d(TAG, "net.getType = " + net.getType());
                        Log.d(TAG, "net.getTypeName = " + net.getTypeName());
                        Log.d(TAG, "net.getSubtype = " + net.getSubtype());
                        Log.d(TAG, "net.getSubtypeName = " + net.getSubtypeName());
                        Log.d(TAG, "net.getExtraInfo = " + net.getExtraInfo());
                        Log.d(TAG, "net.getReason = " + net.getReason());
                        Log.d(TAG, "net.getState = " + net.getState());
                        Log.d(TAG, "net.isAvailable = " + net.isAvailable());
                        Log.d(TAG, "net.isConnected = " + net.isConnected());
                        Log.d(TAG, "net.isConnectedOrConnecting = " + net.isConnectedOrConnecting());
                        Log.d(TAG, "net.isFailover = " + net.isFailover());
                        Log.d(TAG, "net.isRoaming = " + net.isRoaming());
                        Log.d(TAG, "--------------------------------------------------");
                    }
                }
                conn.getNetworkInfo(ConnectivityManager.TYPE_WIFI).isConnected();
                JLog.i(TAG, getApplicationName(context) + "wifi_checkNetConnection="
                        + (conn.getNetworkInfo(ConnectivityManager.TYPE_WIFI).isConnected() ? "y" : "n"));
                conn.getNetworkInfo(ConnectivityManager.TYPE_ETHERNET).isConnected();
                JLog.i(TAG, getApplicationName(context) + "eth_checkNetConnection="
                        + (conn.getNetworkInfo(ConnectivityManager.TYPE_ETHERNET).isConnected() ? "y" : "n"));
                
                TVSIServiceManager siManager = TVSIService.getSIManager();
                if( conn.getNetworkInfo(ConnectivityManager.TYPE_WIFI).isConnected() || 
                		conn.getNetworkInfo(ConnectivityManager.TYPE_ETHERNET).isConnected() ){
                		JLog.i(TAG, "Network (WIFI or ETHERNET) Connection");                		
                		 if (siManager != null) {
                			synchronized (mLock) {
                				siManager.setNetworkConnectionInfo(true);
                			}
                		}
                	} else {
                		JLog.i(TAG, "Network Connection Failed !!!");
                		if (siManager != null) {
                			synchronized (mLock) {
                				siManager.setNetworkConnectionInfo(false);
                			}
                		}
                	}
            }
        }
		
	}

	private static String getApplicationName(Context context) {
		int stringId = context.getApplicationInfo().labelRes;
		return context.getString(stringId);
	}

	private static String getNetworkName(int netType) {
		if (netType == IEthernet.NETWORK_TYPE_WIFI) {
			return "NETWORK_TYPE_WIFI";
		} else if (netType == IEthernet.NETWORK_TYPE_ETHERNET) {
			return "NETWORK_TYPE_ETHERNET";
		} else {
			return "NETWORK_TYPE_UNKNWON";
		}
	}

	private static String getNetworkState(int netState) {
		if (netState == IEthernet.NETWORK_STATE_ENABLED) {
			return "NETWORK_STATE_ENABLED";
		} else {
			return "NETWORK_STATE_ENABLED";
		}
	}

} // end of class
