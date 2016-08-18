/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-10-20 18:03:10 +0900 (월, 20 10월 2014) $
 * $LastChangedRevision: 13307 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice;

import com.tvstorm.tv.si.ISIService;
import com.tvstorm.tv.si.channel.IChannelInfo;
import com.tvstorm.tv.si.channel.RetChannelList;
import com.tvstorm.tv.si.common.ISIResult;
import com.tvstorm.tv.si.program.IProgramInfo;
import com.tvstorm.tv.si.program.ProgramInfo;
import com.tvstorm.tv.si.program.Programs;
import com.tvstorm.tv.si.program.RetProgramList;
import com.tvstorm.tv.siservice.TVSIServiceManager.TVSIServiceManagerEventListener;
import com.tvstorm.tv.siservice.conf.ChannelConfManager;
import com.tvstorm.tv.siservice.conf.ChannelXmlManager;
import com.tvstorm.tv.siservice.conf.IConfConstant;
import com.tvstorm.tv.siservice.conf.WepgXmlManager;
import com.tvstorm.tv.siservice.util.JLog;

import android.os.SystemProperties;
//import android.os.Build;

import android.app.Service;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.os.IBinder;
import android.os.RemoteException;
import android.text.TextUtils;
import android.util.Log;
import android.provider.Settings;
import android.content.ContentResolver;

import java.util.ArrayList;
import java.util.Date;

public class TVSIService extends Service implements IConfConstant, TVSIServiceManagerEventListener {
	private static final String TAG = "TVSIService";

	private static TVSIServiceManager mSIManager = null;

	private static Object mLock = new Object();

	String mEPGUri = "";

	boolean isOnBinding = false;

	boolean writeChXml = false;
	
	private static final String SI_SERVICE_INIT = "tvs.si.initialize";
	
	private static final String SI_DEFAULT_CH = "tvs.si.default.ch";
	
	public static TVSIServiceManager getSIManager() {
		synchronized (mLock) {
			return mSIManager;
		}
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		Log.d(TAG, "onConfigurationChanged");
	}

	@Override
	public void onCreate() {
		super.onCreate();
		Log.d(TAG, "onCreate");

		// 로그설정 초기화
		String appName = getApplicationName(getApplicationContext());
		Context appContext = getApplicationContext();
		JLog.setAppName(appName);
		JLog.setOutput(JLog.LOG_ENABLED);

		// 버전 확인
		JLog.i(TAG, appName + "_v" + getCurrentVersion(appContext));

		// 라이브러리 확인
		String hasSystemSharedLibraryInstalled = (hasSystemSharedLibraryInstalled(appContext,
				"tvcontextmanager") ? "y" : "n");
		JLog.i(TAG, appName + "_hasSystemSharedLibraryInstalled=" + hasSystemSharedLibraryInstalled);

		String isOkloadfile = "n";
		// 속성정보 가져오기
		ChannelConfManager confManager = ChannelConfManager.getInstance(appContext);
		isOkloadfile = (confManager.isOkloadfile() ? "y" : "n");
		String ap = confManager.getAccessPoint();
		String ip = confManager.getIPAddress();
		String port = confManager.getIPPort();
		String demod = confManager.getDemod();
		String freqkHz = confManager.getFreqkHz();
		String fpath = confManager.getFilePath();
		String fname = confManager.getFileName();
		String pn = confManager.getEPGProviderName();
		String db = confManager.getDBType();
		String la = confManager.getLocalAreaCodes();
		String sr = confManager.getSymbolRate();
		String bw = confManager.getBandWidth();
		String wepg = confManager.getWebEpg();
		String websi = confManager.getWebSI();
		String tdt = confManager.getUseTDT();
		String xml = confManager.getUseXML();
		
		StringBuffer sb = new StringBuffer();
		sb.append("\n========================================\n");
		sb.append(" " + appName + "_loadProperties=" + isOkloadfile + "\n");
		sb.append("========================================\n");
		sb.append(" access point : " + ap + "\n");
		sb.append("----------------------------------------\n");
		sb.append(" ip address   : " + ip + "\n");
		sb.append(" ip port      : " + port + "\n");
		sb.append(" demod        : " + demod + "\n");
		sb.append(" freqkHz      : " + freqkHz + "\n");
		sb.append(" file path    : " + fpath + "\n");
		sb.append(" file name    : " + fname + "\n");
		sb.append("----------------------------------------\n");
		sb.append(" provider name: " + pn + "\n");
		sb.append(" db type      : " + db + "\n");
		sb.append("----------------------------------------\n");
		sb.append(" local area   : " + la + "\n");
		sb.append(" symbol rate  : " + sr + "\n");
		sb.append(" band width   : " + bw + "\n");
		sb.append(" web epg      : " + wepg + "\n");
		sb.append(" web si       : " + websi + "\n");		
		sb.append(" use tdt      : " + tdt + "\n");
		sb.append(" use xml      : " + xml + "\n");
		// sb.append(" write chxml  : " + chxml + "\n");
		sb.append("========================================\n");
		JLog.i(TAG, sb.toString());

		// 채널정보 가져오기
		ChannelXmlManager channelManager = ChannelXmlManager.getInstance(appContext);
		isOkloadfile = (channelManager.isOkloadfile() ? "y" : "n");
		JLog.i(TAG, appName + "_loadChannelXml=" + isOkloadfile);

		// 웹서버정보 가져오기
		WepgXmlManager wepgManager = WepgXmlManager.getInstance(appContext);
		isOkloadfile = (wepgManager.isOkloadXmlfile() ? "y" : "n");
		JLog.i(TAG, appName + "_loadServerXml=" + isOkloadfile);

		// EPGUri정보 가져오기
		StringBuffer uri = new StringBuffer();
		// //////////////////////////////////////
		uri.append(ap); // *ap
		if (ap.equals(AP_IP)) { // *loc (ip:port or demod:freqkHz or
								// fpath:fname)
			uri.append("://" + ip + ":" + port);
		} else if (ap.equals(AP_FILE)) {
			uri.append("://" + fpath + ":" + fname);
		} else {
			uri.append("://" + getDemodName(Integer.parseInt(demod)) + ":" + freqkHz);
		}

		uri.append("?" + "pn=" + pn); // *pn
		uri.append("&" + "db=" + db); // *db

		if (pn.equals(EPG_PROVIDER_NAME_SKB)) { // *la
			uri.append("&" + "la=" + la);
		} else if (pn.equals(EPG_PROVIDER_NAME_LINKNET)) {
			uri.append("&" + "sr=" + sr); // *sr
			uri.append("&" + "bw=" + bw); // *bw
		}

		if (wepg.equalsIgnoreCase("y")) {
			uri.append("&" + "wepg=" + wepgManager.getWebEPGUrl()); // *wepg
		}

		if (websi.equalsIgnoreCase("y")) {
			uri.append("&" + "websi=" + wepgManager.getWebSIUrl()); // *websi
		}

		if (xml.equalsIgnoreCase("y")) {
			uri.append("&" + "xml=" + xml); // *xml
		}

		// //////////////////////////////////////
		JLog.i(TAG, appName + "_EPGUri=" + uri.toString());
		mEPGUri = uri.toString();

		mSIManager = new TVSIServiceManager(getApplicationContext());

		if (mSIManager != null) {
			synchronized (mLock) {
				
				boolean result = "true".equals(SystemProperties.get(SI_SERVICE_INIT, "false"));
		        if (result) {
		        	JLog.i(TAG, "SI start mode 2");
		        	mSIManager.start(mEPGUri, 2); // 1: normal 2: abnormal(not go to home ts)
		        }
		        else
		        {
		        	JLog.i(TAG, "SI start first");
		        	mSIManager.start(mEPGUri, 1/* intent != null ? 1 : 2 */); // 1:
		        }

			}
		}
	}

	private static String getDemodName(int demod) {
		switch (demod) {
			case 8:
				return DEMOD_VSB8;
			case 16:
				return DEMOD_QAM16;
			case 32:
				return DEMOD_QAM32;
			case 64:
				return DEMOD_QAM64;
			case 128:
				return DEMOD_QAM128;
			case 256:
				return DEMOD_QAM256;
			default:
				return DEMOD_QAM64;
		}
	}

	private boolean hasSystemSharedLibraryInstalled(Context ctx, String libraryName) {
		boolean hasLibraryInstalled = false;
		if (!TextUtils.isEmpty(libraryName)) {
			String[] installedLibraries = ctx.getPackageManager().getSystemSharedLibraryNames();
			if (installedLibraries != null) {
				for (String s : installedLibraries) {
					if (libraryName.equals(s)) {
						hasLibraryInstalled = true;
						break;
					}
				}
			}
		}
		return hasLibraryInstalled;
	}

	private static String getCurrentVersion(Context context) {
		try {
			PackageInfo pi = context.getPackageManager()
					.getPackageInfo(context.getPackageName(), 0);
			return pi.versionName;
		} catch (NameNotFoundException e) {
			Log.e(TAG, e.toString());
			return "0.0.0";
		}
	}

	private static String getApplicationName(Context context) {
		int stringId = context.getApplicationInfo().labelRes;
		return context.getString(stringId);
	}

	@Override
	public void onDestroy() {
		Log.d(TAG, "onDestroy");

		if (mSIManager != null) {
			synchronized (mLock) {
				mSIManager.stop();
			}
			mSIManager = null;
		}

		super.onDestroy();
	}

	@Override
	public void onLowMemory() {
		Log.d(TAG, "onLowMemory");

		super.onLowMemory();
	}

	@Override
	public void onRebind(Intent intent) {
		super.onRebind(intent);
		String i = (intent == null) ? "null" : intent.toString();
		Log.d(TAG, "onRebind:" + i);
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		String i = (intent == null) ? "null" : intent.toString();
		Log.d(TAG, "onStartCommand:" + i + "," + flags + "," + startId);

		//부팅 이후에 한번이라도 CHANNEL UPDATE 이벤트를 보낸 경우 true 리턴.
		
//		ChannelConfManager confManager = ChannelConfManager.getInstance(getApplicationContext());
		
		
		return Service.START_STICKY; // START_REDELIVER_INTENT
	}

	@Override
	public IBinder onBind(Intent intent) {
		String i = (intent == null) ? "null" : intent.toString();
		String action = intent.getAction();
		Log.d(TAG, "onBind:" + i + " action:" + action);

		return new ISIService.Stub() {

			@Override
			public synchronized String getDefaultChannel() throws RemoteException {
				String uid = IChannelInfo.CONST_DEFAULT_CHANNEL_UID;
				Log.w(TAG, "call_getDefaultChannel");
				
				String default_channel  = SystemProperties.get(SI_DEFAULT_CH,"");
				if(default_channel == null)
					default_channel = uid;
				Log.w(TAG, "call_getDefaultChannel(uid=" + default_channel + ")");
				return default_channel;
				
//				if (mSIManager == null) {
//					JLog.w(TAG, "siManager is null");
//					return uid;
//				}
//				synchronized (mLock) {
//					uid = mSIManager.getDefaultChannel();					
//				}
//				Log.w(TAG, "call_getDefaultChannel(uid=" + uid + ")");
//				return uid;
			}

			@Override
			public synchronized RetChannelList getChannelList(int tvRadioMode)throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getChannelList(tvRadioMode=" + tvRadioMode + ")");				
				RetChannelList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager_is_null._getChannelList_from_channelxml");
					ChannelXmlManager channelManager = ChannelXmlManager.getInstance(getApplicationContext());
					ret = channelManager.getChannelList(tvRadioMode);
				}
				
				if (mSIManager != null && mSIManager.isReadyChInfo()) {
					synchronized (mLock) {
						ret = mSIManager.getChannelList(tvRadioMode);

						if (ret.getResult() != ISIResult.SI_RESULT_OK) {
							JLog.w(TAG, "result_is_not_ok._getChannelList_from_channelxml");
							ChannelXmlManager channelManager = ChannelXmlManager.getInstance(getApplicationContext());
							ret = channelManager.getChannelList(tvRadioMode);
						} else {
							// ////////////////////////////////////////////////
							// temp
							if (writeChXml && tvRadioMode == IChannelInfo.CONST_CHANNEL_MODE_ALL) {
								JLog.w(TAG, "result_is_ok._getChannelList_to_channelxml");
//								ChannelXmlManager channelManager = ChannelXmlManager.getInstance(getApplicationContext());
//								channelManager.setChannelList(ret);
//								channelManager.storeToXml(0, "");
//								wjpark 2015.10.24
								writeChXml = false;
							}
							// ////////////////////////////////////////////////
						}
					}
				} else {
					Log.w(TAG, "Because ChannelList of SIManager is NOT READY, Get ChannelList From channel.xml");
					//ret = mSIManager.getChannelList(tvRadioMode);
					ChannelXmlManager channelManager = ChannelXmlManager.getInstance(getApplicationContext());
					ret = channelManager.getChannelList(tvRadioMode);
				}
				Log.w(TAG, "call_getChannelList(tvRadioMode=" + tvRadioMode + "):L="
						+ ret.getList().size() + ":T=" + (System.currentTimeMillis() - curTick)
						+ " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getCurrentProgram(String channelUid)
					throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getCurrentProgram(channelUid=" + channelUid + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				synchronized (mLock) {
					ret = mSIManager.getCurrentProgram(channelUid);
				}
				Log.w(TAG,
						"call_getCurrentProgram(channelUid=" + channelUid + "):T="
								+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getMultiChannelProgramList(String[] channelUids)
					throws RemoteException {
				StringBuffer uids = new StringBuffer();
				for (int i = 0; i < channelUids.length; i++) {
					if (i > 0) {
						uids.append("|");
					}
					String uid = channelUids[i];
					if (uid == null) {
						uid = "";
					}
					uids.append(uid);
				}
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getMultiChannelProgramList(channelUids=" + uids + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUids == null) {
					JLog.w(TAG, "channelUids is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUids.length <= 0) {
					JLog.w(TAG, "channelUids is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				for (String channelUid : channelUids) {
					if (channelUid == null) {
						JLog.w(TAG, "channelUid is null");
						ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
								new ArrayList<Programs>());
					}
					if (channelUid.isEmpty()) {
						JLog.w(TAG, "channelUid is empty");
						ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
								new ArrayList<Programs>());
					}
				}
				synchronized (mLock) {
					ret = mSIManager.getMultiChannelProgramList(channelUids);
				}
				Log.w(TAG,
						"call_getMultiChannelProgramList(channelUids=" + uids + "):T="
								+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getMultiChannelProgramListByIndex(
					String[] channelUids, int startIndex, int requestCount) throws RemoteException {
				StringBuffer uids = new StringBuffer();
				for (int i = 0; i < channelUids.length; i++) {
					if (i > 0) {
						uids.append("|");
					}
					String uid = channelUids[i];
					if (uid == null) {
						uid = "";
					}
					uids.append(uid);
				}
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getMultiChannelProgramListByIndex(channelUids=" + uids + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUids == null) {
					JLog.w(TAG, "channelUids is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUids.length <= 0) {
					JLog.w(TAG, "channelUids is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				for (String channelUid : channelUids) {
					if (channelUid == null) {
						JLog.w(TAG, "channelUid is null");
						ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
								new ArrayList<Programs>());
					}
					if (channelUid.isEmpty()) {
						JLog.w(TAG, "channelUid is empty");
						ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
								new ArrayList<Programs>());
					}
				}
				synchronized (mLock) {
					ret = mSIManager.getMultiChannelProgramListByIndex(channelUids, startIndex,
							requestCount);
				}
				Log.w(TAG, "call_getMultiChannelProgramListByIndex(channelUids=" + uids + "):T="
						+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getMultiChannelProgramListByTime(
					String[] channelUids, int startTime, int endTime) throws RemoteException {
				StringBuffer uids = new StringBuffer();
				for (int i = 0; i < channelUids.length; i++) {
					if (i > 0) {
						uids.append("|");
					}
					String uid = channelUids[i];
					if (uid == null) {
						uid = "";
					}
					uids.append(uid);
				}
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getMultiChannelProgramListByTime(channelUids=" + uids + "):T="
						+ (System.currentTimeMillis() - curTick) + " ms");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUids == null) {
					JLog.w(TAG, "channelUids is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUids.length <= 0) {
					JLog.w(TAG, "channelUids is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				for (String channelUid : channelUids) {
					if (channelUid == null) {
						JLog.w(TAG, "channelUid is null");
						ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
								new ArrayList<Programs>());
					}
					if (channelUid.isEmpty()) {
						JLog.w(TAG, "channelUid is empty");
						ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
								new ArrayList<Programs>());
					}
				}
				synchronized (mLock) {
					ret = mSIManager.getMultiChannelProgramListByTime(channelUids, startTime,
							endTime);
				}
				Log.w(TAG, "call_getMultiChannelProgramListByTime(channelUids=" + uids + "):T="
						+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getProgramList(String channelUid)
					throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getProgramList(channelUid=" + channelUid + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				synchronized (mLock) {
					ret = mSIManager.getProgramList(channelUid);
				}
				Log.w(TAG,
						"call_getProgramList(channelUid=" + channelUid + "):T="
								+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getProgramListByIndex(String channelUid,
					int startIndex, int requestCount) throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getProgramListByIndex(channelUid=" + channelUid + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				synchronized (mLock) {
					ret = mSIManager.getProgramListByIndex(channelUid, startIndex, requestCount);
				}
				Log.w(TAG,
						"call_getProgramListByIndex(channelUid=" + channelUid + "):T="
								+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getProgramListByName(String channelUid,
					int requestTime, String requestName, boolean descFlag) throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getProgramListByName(channelUid=" + channelUid + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (requestName == null) {
					JLog.w(TAG, "requestName is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (requestName.isEmpty()) {
					JLog.w(TAG, "requestName is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				synchronized (mLock) {
					ret = mSIManager.getProgramListByName(channelUid, requestTime, requestName,
							descFlag);
				}
				Log.w(TAG,
						"call_getProgramListByName(channelUid=" + channelUid + "):T="
								+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public RetProgramList getProgramListBySeries(String channelUid, int requestTime,
					String requestName, int seasonNo, int episodeNo) throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getProgramListBySeries(channelUid=" + channelUid + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (requestName == null) {
					JLog.w(TAG, "requestName is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (requestName.isEmpty()) {
					JLog.w(TAG, "requestName is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				synchronized (mLock) {
					ret = mSIManager.getProgramListBySeries(channelUid, requestTime, requestName,
							seasonNo, episodeNo);
				}
				Log.w(TAG, "call_getProgramListBySeries(channelUid=" + channelUid + "):T="
						+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized RetProgramList getProgramListByTime(String channelUid,
					int startTime, int endTime) throws RemoteException {
				long curTick = System.currentTimeMillis();
				Log.w(TAG, "call_getProgramListByTime(channelUid=" + channelUid + ")");
				RetProgramList ret;
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_GENERAL_FAILURE,
							new ArrayList<Programs>());
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					ret = new RetProgramList(ISIResult.SI_RESULT_ERR_INVALID_PARAMETERS,
							new ArrayList<Programs>());
				}
				synchronized (mLock) {
					ret = mSIManager.getProgramListByTime(channelUid, startTime, endTime);
				}
				Log.w(TAG,
						"call_getProgramListByTime(channelUid=" + channelUid + "):T="
								+ (System.currentTimeMillis() - curTick) + " ms");
				return ret;
			}

			@Override
			public synchronized void startAutoScan(int startFreqMHz, int endFreqMHz,
					int symbolRate, int bandWidth, int modulation, int tunerId)
					throws RemoteException {
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					return;
				}
				synchronized (mLock) {
					mSIManager.startAutoScan(
							makeEPGUri(getApplicationContext(), symbolRate, bandWidth, modulation),
							startFreqMHz, endFreqMHz, tunerId);
				}
			}

			@Override
			public synchronized void startManualScan(int freqMHz, int symbolRate, int bandWidth,
					int modulation, int tunerId) throws RemoteException {
				if (mSIManager == null) {
                        JLog.w(TAG, "siManager is null");
                        return;
                    }
                    synchronized (mLock) {
                        mSIManager.startManualScan(makeEPGUri(getApplicationContext(), symbolRate, bandWidth, modulation), freqMHz, tunerId);
                    }
			}

			@Override
			public synchronized void startManualChannelScan(int freqMHz, int symbolRate,
					int bandWidth, int modulation, int networkId) throws RemoteException {
				if (mSIManager == null) {
                        JLog.w(TAG, "siManager is null");
                        return;
                    }
                    synchronized (mLock) {
                        mSIManager.startManualChannelScan(makeEPGUri(getApplicationContext(), symbolRate, bandWidth, modulation), freqMHz, networkId);
                    }
			}

			@Override
            public synchronized void startChannelScan(boolean duringFti) throws RemoteException {
                if (mSIManager == null) {
                    JLog.w(TAG, "siManager is null");
                    return;
                }
                synchronized (mLock) {
                	ChannelConfManager confManager = ChannelConfManager.getInstance(getApplicationContext());
                		
            		if (confManager.getUseXML().equalsIgnoreCase("y")) {
            			 JLog.w(TAG, "Dummy EPG mode not available Channel Scan");
                		mSIManager.notifyScanReady(0);
                	} else {
                		mSIManager.startScan(mEPGUri, duringFti ? 1 : 2); // 1: fti(first-time-install) 2: ui/ux
                	}
                }
            }

			@Override
            public synchronized void stopChannelScan() throws RemoteException {
                if (mSIManager == null) {
                    JLog.w(TAG, "siManager is null");
                    return;
                }
                synchronized (mLock) {
                    mSIManager.stopScan();
                }
            }

			@Override
			public synchronized void setLocalAreaCodes(String areaCodes) throws RemoteException {
				Log.w(TAG, "call_setLocalAreaCodes(areaCodes=" + areaCodes + ")");
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					return;
				}
				if (areaCodes == null) {
					JLog.w(TAG, "areaCodes is null");
					return;
				}
				if (areaCodes.isEmpty()) {
					JLog.w(TAG, "areaCodes is empty");
					return;
				}

				if (mSIManager.isReadyChInfo()) {
					Log.w(TAG, "When binding, channel info was already ready!");
					mSIManager.notifyChannelReady(0);
				}
				if (mSIManager.isUpdatedChInfo()) {
					Log.w(TAG, "When binding, channel info was already updated!");
					mSIManager.notifyChannelUpdated(false);
				}
				if (mSIManager.isUpdatedPrgmInfo()) {
					Log.w(TAG, "When binding, program info was already updated!");
					mSIManager.notifyProgramUpdated();
				}
			}

			@Override
			public synchronized void setCurrentChannel(String channelUid) throws RemoteException {
				Log.w(TAG, "call_setCurrentChannel(channelUid=" + channelUid + ")");
				if (mSIManager == null) {
					JLog.w(TAG, "siManager is null");
					return;
				}
				if (channelUid == null) {
					JLog.w(TAG, "channelUid is null");
					return;
				}
				if (channelUid.isEmpty()) {
					JLog.w(TAG, "channelUid is empty");
					return;
				}
				
				if (channelUid.equals(IChannelInfo.CONST_DEFAULT_CHANNEL_UID) == false) {
					synchronized (mLock) {
						mSIManager.setCurrentChannel(channelUid);
					}
				}
			}

		};
	}

	@Override
	public boolean onUnbind(Intent intent) {
		String i = (intent == null) ? "null" : intent.toString();

		String action = intent.getAction();
		Log.d(TAG, "onUnbind:" + i + " action:" + action);

		isOnBinding = false;

		return super.onUnbind(intent);
	}

	private int[] convertStringToIntegerArray(String str, String regularExpression) {
		if (str == null || regularExpression == null) {
			return new int[0]; // ReturnEmptyArrayRatherThanNull
		}
		if (str.isEmpty() || regularExpression.isEmpty()) {
			return new int[0]; // ReturnEmptyArrayRatherThanNull
		}

		int values[] = null;
		String[] tokens = str.split(regularExpression);
		values = new int[tokens.length];
		for (int i = 0; i < tokens.length; i++) {
			values[i] = Integer.parseInt(tokens[i]);
		}
		return values;
	}

	private String makeCallsign(String name) {
		if (name != null && name.isEmpty() == false) {
			return name.trim().replaceAll("\\p{Space}", "").toUpperCase();
		}
		return "";
	}

	private String makeEPGUri(Context context, int symbolRate, int bandWidth, int modulation) {
		ChannelConfManager confManager = ChannelConfManager.getInstance(context);
		String ap = confManager.getAccessPoint();
		String ip = confManager.getIPAddress();
		String port = confManager.getIPPort();
		String demod = confManager.getDemod();
		if (modulation > 0) {
			demod = Integer.toString(modulation); // convert to input value
		}
		String freqkHz = confManager.getFreqkHz();
		String fpath = confManager.getFilePath();
		String fname = confManager.getFileName();
		String pn = confManager.getEPGProviderName();
		String db = confManager.getDBType();
		String la = confManager.getLocalAreaCodes();
		String sr = confManager.getSymbolRate();
		if (symbolRate > 0) {
			sr = Integer.toString(symbolRate); // convert to input value
		}
		String bw = confManager.getBandWidth();
		if (bandWidth > 0) {
			bw = Integer.toString(bandWidth); // convert to input value
		}

		StringBuffer uri = new StringBuffer();
		// //////////////////////////////////////
		uri.append(ap); // *ap
		if (ap.equals(AP_IP)) { // *loc (ip:port or demod:freqkHz or
								// fpath:fname)
			uri.append("://" + ip + ":" + port);
		} else if (ap.equals(AP_FILE)) {
			uri.append("://" + fpath + ":" + fname);
		} else {
			uri.append("://" + getDemodName(Integer.parseInt(demod)) + ":" + freqkHz);
		}

		uri.append("?" + "pn=" + pn); // *pn
		uri.append("&" + "db=" + db); // *db

		if (pn.equals(EPG_PROVIDER_NAME_SKB)) { // *la
			uri.append("&" + "la=" + la);
		} else {
			uri.append("&" + "sr=" + sr); // *sr
			uri.append("&" + "bw=" + bw); // *bw
		}
		// //////////////////////////////////////
		JLog.i(TAG, "Uri=" + uri.toString());
		return uri.toString();
	}

	// DUMMY
	public static RetProgramList getDummyCurrentProgram(String channelUid) {
		ArrayList<Programs> programsList = new ArrayList<Programs>();
		Date time;

		Programs programs = new Programs();
		programs.setChannelUid(channelUid);

		ProgramInfo programInfo = new ProgramInfo();
		programInfo.setChannelUId(channelUid);
		String[] tokens = channelUid.split("\\.");
		programInfo.setChannelId(Integer.parseInt(tokens[2]));
		programInfo.setProgramId(1426);
		programInfo.setProgramName("상속자들 제19회");
		programInfo.setProgramShortDescription("이민호, 박신혜");
		programInfo.setProgramDescription("왕관을 쓰려는자, 그 무게를 견뎌라");
		programInfo.setRunningStatus(IProgramInfo.CONST_RUNNING_STATUS_UNDEFINED);
		programInfo.setFreeCaMode(IProgramInfo.CONST_FREECA_MODE_FREE);
		programInfo.setRating(IProgramInfo.CONST_RATING_ALL);
		programInfo.setContentNibble1(0);
		programInfo.setContentNibble2(0);
		programInfo.setContentUserNibble1(0);
		programInfo.setContentUserNibble2(0);
		programInfo.setProgramImagePath("");
		programInfo.setAudioType(IProgramInfo.CONST_AUDIO_TYPE_AC3);
		programInfo.setPrice("0");
		programInfo.setDirector("");
		programInfo.setActors("");

		time = new Date();
		programInfo.setStartTime(time);
		if (channelUid.equals("1.100.14")) {
			Date endTime = new Date(time.getTime() + 30 * 60 * 1000);
			programInfo.setEndTime(endTime);
			programInfo.setDuration(30 * 60);
		} else {
			Date endTime = new Date(time.getTime() + 30 * 60 * 1000);
			programInfo.setEndTime(endTime);
			programInfo.setDuration(30 * 60);
		}
		programInfo.setHDProgram(IProgramInfo.CONST_VEDIO_RESULTION_HD);
		programInfo.setDolbyAudio(IProgramInfo.CONST_DOLBY_YES);
		programInfo.setCaption(IProgramInfo.CONST_CAPTION_NO);
		programInfo.setDvs(IProgramInfo.CONST_DVS_NO);
		// programInfo.setProgramLinkInfoList(ArrayList<ProgramLinkInfo> list);
		// programInfo.addProgramLinkInfo(ProgramLinkInfo linkInfo);

		programs.getProgramInfoList().add(programInfo);

		programsList.add(programs);
		RetProgramList retProgramList = new RetProgramList(ISIResult.SI_RESULT_OK, programsList);

		return retProgramList;
	}

	public static RetProgramList getDummyProgramList(String channelUid) {
		ArrayList<Programs> programsList = new ArrayList<Programs>();
		Date time;

		Programs programs = new Programs();
		programs.setChannelUid(channelUid);

		for (int i = 0; i < 2; i++) {
			ProgramInfo programInfo = new ProgramInfo();

			programInfo.setChannelUId(channelUid);
			String[] tokens = channelUid.split("\\.");
			programInfo.setChannelId(Integer.parseInt(tokens[2]));
			if (i == 0) {
				programInfo.setProgramId(1426);
				programInfo.setProgramName("상속자들 제19회");
				programInfo.setProgramShortDescription("이민호, 박신혜");
				programInfo.setProgramDescription("왕관을 쓰려는자, 그 무게를 견뎌라");

			} else {
				programInfo.setProgramId(1427);
				programInfo.setProgramName("상속자들 제20회");
				programInfo.setProgramShortDescription("이민호, 박신혜");
				programInfo.setProgramDescription("[최종회] 왕관을 쓰려는자, 그 무게를 견뎌라");
			}
			programInfo.setRunningStatus(IProgramInfo.CONST_RUNNING_STATUS_UNDEFINED);
			programInfo.setFreeCaMode(IProgramInfo.CONST_FREECA_MODE_FREE);
			programInfo.setRating(IProgramInfo.CONST_RATING_ALL);
			programInfo.setContentNibble1(0);
			programInfo.setContentNibble2(0);
			programInfo.setContentUserNibble1(0);
			programInfo.setContentUserNibble2(0);
			programInfo.setProgramImagePath("");
			programInfo.setAudioType(IProgramInfo.CONST_AUDIO_TYPE_AC3);
			programInfo.setPrice("0");
			programInfo.setDirector("");
			programInfo.setActors("");

			time = new Date();
			if (i == 0) {
				Date startTime = time;
				programInfo.setStartTime(startTime);
				Date endTime = new Date(startTime.getTime() + 30 * 60 * 1000);
				programInfo.setEndTime(endTime);
				programInfo.setDuration(30 * 60);
			} else {
				Date startTime = new Date(time.getTime() + 30 * 60 * 1000);
				programInfo.setStartTime(startTime);
				Date endTime = new Date(startTime.getTime() + 30 * 60 * 1000);
				programInfo.setEndTime(endTime);
				programInfo.setDuration(30 * 60);
			}
			programInfo.setHDProgram(IProgramInfo.CONST_VEDIO_RESULTION_HD);
			programInfo.setDolbyAudio(IProgramInfo.CONST_DOLBY_YES);
			programInfo.setCaption(IProgramInfo.CONST_CAPTION_NO);
			programInfo.setDvs(IProgramInfo.CONST_DVS_NO);
			// programInfo.setProgramLinkInfoList(ArrayList<ProgramLinkInfo>
			// list);
			// programInfo.addProgramLinkInfo(ProgramLinkInfo linkInfo);

			programs.getProgramInfoList().add(programInfo);
		}

		programsList.add(programs);
		RetProgramList retProgramList = new RetProgramList(ISIResult.SI_RESULT_OK, programsList);

		return retProgramList;
	}

	public static RetProgramList getDummyMultiChannelProgramList(String[] channelUids) {
		ArrayList<Programs> programsList = new ArrayList<Programs>();
		Date time;

		for (int j = 0; j < channelUids.length; j++) {
			Programs programs = new Programs();
			if (j % 2 == 0) {
				programs.setChannelUid(channelUids[j]);
			} else {
				programs.setChannelUid(channelUids[j]);
			}

			for (int i = 0; i < 2; i++) {
				ProgramInfo programInfo = new ProgramInfo();

				if (j % 2 == 0) {
					programInfo.setChannelUId(channelUids[j]);
					String[] tokens = channelUids[j].split("\\.");
					programInfo.setChannelId(Integer.parseInt(tokens[2]));
					if (i == 0) {
						programInfo.setProgramId(1426);
						programInfo.setProgramName("상속자들 제19회");
						programInfo.setProgramShortDescription("이민호, 박신혜");
						programInfo.setProgramDescription("왕관을 쓰려는자, 그 무게를 견뎌라");

					} else {
						programInfo.setProgramId(1427);
						programInfo.setProgramName("상속자들 제20회");
						programInfo.setProgramShortDescription("이민호, 박신혜");
						programInfo.setProgramDescription("[최종회] 왕관을 쓰려는자, 그 무게를 견뎌라");
					}
				} else {
					programInfo.setChannelUId(channelUids[j]);
					String[] tokens = channelUids[j].split("\\.");
					programInfo.setChannelId(Integer.parseInt(tokens[2]));
					if (i == 0) {
						programInfo.setProgramId(1426);
						programInfo.setProgramName("기황후 제13회");
						programInfo.setProgramShortDescription("하지원, 지창욱");
						programInfo.setProgramDescription("기황후.. 그녀는 공녀로 끌려간 고려의 여인이었다");

					} else {
						programInfo.setProgramId(1427);
						programInfo.setProgramName("기황후 제14회");
						programInfo.setProgramShortDescription("하지원, 지창욱");
						programInfo.setProgramDescription("[15회예고포함] 기황후.. 그녀는 공녀로 끌려간 고려의 여인이었다");
					}
				}
				programInfo.setRunningStatus(IProgramInfo.CONST_RUNNING_STATUS_UNDEFINED);
				programInfo.setFreeCaMode(IProgramInfo.CONST_FREECA_MODE_FREE);
				programInfo.setRating(IProgramInfo.CONST_RATING_ALL);
				programInfo.setContentNibble1(0);
				programInfo.setContentNibble2(0);
				programInfo.setContentUserNibble1(0);
				programInfo.setContentUserNibble2(0);
				programInfo.setProgramImagePath("");
				programInfo.setAudioType(IProgramInfo.CONST_AUDIO_TYPE_AC3);
				programInfo.setPrice("0");
				programInfo.setDirector("");
				programInfo.setActors("");

				time = new Date();
				if (i == 0) {
					Date startTime = time;
					programInfo.setStartTime(startTime);
					Date endTime = new Date(startTime.getTime() + 30 * 60 * 1000);
					programInfo.setEndTime(endTime);
					programInfo.setDuration(30 * 60);
				} else {
					Date startTime = new Date(time.getTime() + 30 * 60 * 1000);
					programInfo.setStartTime(startTime);
					Date endTime = new Date(startTime.getTime() + 30 * 60 * 1000);
					programInfo.setEndTime(endTime);
					programInfo.setDuration(30 * 60);
				}
				programInfo.setHDProgram(IProgramInfo.CONST_VEDIO_RESULTION_HD);
				programInfo.setDolbyAudio(IProgramInfo.CONST_DOLBY_YES);
				programInfo.setCaption(IProgramInfo.CONST_CAPTION_NO);
				programInfo.setDvs(IProgramInfo.CONST_DVS_NO);
				// programInfo.setProgramLinkInfoList(ArrayList<ProgramLinkInfo>
				// list);
				// programInfo.addProgramLinkInfo(ProgramLinkInfo linkInfo);

				programs.getProgramInfoList().add(programInfo);
			}

			programsList.add(programs);
		}

		RetProgramList retProgramList = new RetProgramList(ISIResult.SI_RESULT_OK, programsList);

		return retProgramList;
	}

	@Override
	public void onInitialized() {
		
	}

} // end of class
