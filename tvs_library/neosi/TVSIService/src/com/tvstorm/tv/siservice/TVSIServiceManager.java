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

import android.app.AlarmManager;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.util.Log;
import android.os.SystemProperties;

import com.tvstorm.tv.si.channel.IChannelInfo;
import com.tvstorm.tv.si.channel.RetChannelList;
import com.tvstorm.tv.si.common.ISIResult;
import com.tvstorm.tv.si.common.ISIServiceMsg;
import com.tvstorm.tv.si.program.RetProgramList;
import com.tvstorm.tv.siservice.TVSIServiceManager.ScanInfo;
import com.tvstorm.tv.siservice.conf.ChannelConfManager;
import com.tvstorm.tv.siservice.conf.ChannelXmlManager;
import com.tvstorm.tv.siservice.conf.IConfConstant;
import com.tvstorm.tv.siservice.conf.NetworkConfManager;
import com.tvstorm.tv.siservice.util.JLog;

import java.util.Calendar;
import java.util.Date;



public class TVSIServiceManager implements IConfConstant {
	private static final String TAG = "TVSIServiceManager";

    private static final boolean USE_CHANNEL_XML = false;

	private static final long MAX_SET_SYSTEM_TIME_GAP = 20; // in seconds
	
	private static final String SI_SERVICE_INIT = "tvs.si.initialize";
	
	private static final String SI_DEFAULT_CH = "tvs.si.default.ch";

	private boolean isReadyChInfo = false;

	private boolean isUpdatedChInfo = false;

	private boolean isUpdatedPrgmInfo = false;
	
	private boolean isOnScanning = false;
	
	private boolean isInitialized = false;

	private int sdtVersion = 0xFF;
	
	private String default_sid = "0";
	
	static {
		System.loadLibrary("tvs_duesi");
		System.loadLibrary("tvs_jnisi");
	}

	private Context mContext;

	private Intent mIntent = new Intent(ISIServiceMsg.ACTION_SIService);

	private MessageHandler mHandler = null;

	private Object mLock = new Object();
	
	public interface TVSIServiceManagerEventListener{
		public void onInitialized();
	}

	public TVSIServiceManager(Context context) {
		mContext = context;
		mHandler = new MessageHandler();
	}

	public boolean isReadyChInfo() {
		return isReadyChInfo;
	}

	public boolean isUpdatedChInfo() {
		return isUpdatedChInfo;
	}

	public boolean isUpdatedPrgmInfo() {
		return isUpdatedPrgmInfo;
	}

	public boolean isOnScanning() {
        return isOnScanning;
    }
	
	public void cleanUp() {
		if (mHandler.hasMessages(MSG_CHECK_CHANNEL_INFO) == true) {
			mHandler.removeMessages(MSG_CHECK_CHANNEL_INFO);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_CHANNEL_UPDATED) == true) {
			mHandler.removeMessages(MSG_NOTIFY_CHANNEL_UPDATED);
		}
		if (mHandler.hasMessages(MSG_UPDATE_CHANNEL_XML) == true) {
			mHandler.removeMessages(MSG_UPDATE_CHANNEL_XML);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_PROGRAM_UPDATED) == true) {
			mHandler.removeMessages(MSG_NOTIFY_PROGRAM_UPDATED);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_PROGRAM_CHANGED) == true) {
			mHandler.removeMessages(MSG_NOTIFY_PROGRAM_CHANGED);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_SCAN_READY) == true) {
			mHandler.removeMessages(MSG_NOTIFY_SCAN_READY);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_SCAN_PROCESSING) == true) {
			mHandler.removeMessages(MSG_NOTIFY_SCAN_PROCESSING);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_SCAN_COMPLETED) == true) {
			mHandler.removeMessages(MSG_NOTIFY_SCAN_COMPLETED);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_SCAN_STOP) == true) {
			mHandler.removeMessages(MSG_NOTIFY_SCAN_STOP);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_CHANNEL_READY) == true) {
			mHandler.removeMessages(MSG_NOTIFY_CHANNEL_READY);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_TDT_DATE) == true) {
			mHandler.removeMessages(MSG_NOTIFY_TDT_DATE);
		}
		if (mHandler.hasMessages(MSG_NOTIFY_ERROR_WEPG_CONN) == true) {
			mHandler.removeMessages(MSG_NOTIFY_ERROR_WEPG_CONN);
		}

		isReadyChInfo = false;
		isUpdatedChInfo = false;
		isUpdatedPrgmInfo = false;
		isOnScanning = false;
		sdtVersion = 0xFF;
		default_sid ="0";
	}

	// //////////////////////////////////////////////////////////////////////////
	// Native Setters/Getters API
	// //////////////////////////////////////////////////////////////////////////
	public native int start(String providerUri, int mode); // mode(0: not used
															// 1:
															// normal(HomeTS),
															// 2: abnormal(e,g
															// watchingTV))

	public native int stop();

	public native int restart(String providerUri, int mode); // mode(0: not used
																// 1:
																// normal(HomeTS),
																// 2:
																// abnormal(e,g
																// watchingTV))

	public native int startAutoScan(String providerUri, int startFreqMHz, int endFreqMHz,
			int tunerId);

	public native int startManualScan(String providerUri, int freqMHz, int tunerId);

	public native int startManualChannelScan(String providerUri, int freqMHz, int networkId);

	public native int startScan(String providerUri, int mode); // mode(0: not
																// used 1:
																// fti(first-time-installation),
																// 2 button
																// event by
																// setting)

	public native int stopScan();

	public native int setCurrentChannel(String channelUid);

	public native int setNetworkConnectionInfo(boolean isConnected);

	public native String getDefaultChannel();

	public native RetChannelList getChannelList(int tvRadioMode);

	public native RetProgramList getCurrentProgram(String channelUid);

	public native RetProgramList getProgramList(String channelUid);

	public native RetProgramList getProgramListByIndex(String channelUid, int startIndex,
			int requestCount);

	public native RetProgramList getProgramListByName(String channelUid, int requestTime,
			String requestName, boolean descFlag);

	public native RetProgramList getProgramListBySeries(String channelUid, int requestTime,
			String requestName, int seasonNo, int episodeNo);

	public native RetProgramList getProgramListByTime(String channelUid, int startTime, int endTime);

	public native RetProgramList getMultiChannelProgramList(String[] channelUids);

	public native RetProgramList getMultiChannelProgramListByIndex(String[] channelUids,
			int startIndex, int requestCount);

	public native RetProgramList getMultiChannelProgramListByTime(String[] channelUids,
			int startTime, int endTime);

	// //////////////////////////////////////////////////////////////////////////
	// Notification Event
	// //////////////////////////////////////////////////////////////////////////
	public void notifyChannelReady(int version) {
	
//		ChannelConfManager confManager = ChannelConfManager.getInstance(mContext);
//		
//		if (confManager.getSI_INIT().equalsIgnoreCase("n")) {
//			confManager.setSI_INIT("y");
//		}
		
        if(version > 0) {
            sdtVersion = version;
        }
		if (isOnScanning) {
            JLog.i(TAG, "on_Scannning._not_need_to_notify_channel_ready");
        } else {
        	SystemProperties.set(SI_SERVICE_INIT, "true");
			mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_CHANNEL_READY));
//			isInitialized = true;
        }
	    
	}
	
	public void notifyDefaultChannel(String default_channel) {
	
		SystemProperties.set(SI_DEFAULT_CH, default_channel);
		Log.w(TAG, "setDefaultChannel(uid=" + default_channel + ")");
	}

	public void notifyChannelUpdated(int version, int default_ch) {
		    
        if(version > 0) {
            sdtVersion = version;
        }
		default_sid = Integer.toString(default_ch);
		
		if (USE_CHANNEL_XML) {
			// 채널이 업데이트 되면, 1. 정보유효검사 2. 알림 3. channel.xml갱신
			if (mHandler.hasMessages(MSG_CHECK_CHANNEL_INFO) == true) {
				mHandler.removeMessages(MSG_CHECK_CHANNEL_INFO);
			}
			mHandler.sendMessage(mHandler.obtainMessage(MSG_CHECK_CHANNEL_INFO, 0));
		} else {
			// needtoUpdate == false ? do not write the channelxml
			JLog.i(TAG, "notify_channel_updated");
			if (isOnScanning) {
	            JLog.i(TAG, "on_Scannning._not_need_to_notify_channel_updated");
	        } else {
	        	mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_CHANNEL_UPDATED, true));			
	        }
		}
	}

    public void notifyProgramUpdated() {
        if (isOnScanning) {
            JLog.i(TAG, "on_Scannning._not_need_to_notify_program_updated");
        } else {
            mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_PROGRAM_UPDATED));
        }
    }

    public void notifyChangeProgram(int channelNumber) {
        if (isOnScanning) {
            JLog.i(TAG, "on_Scannning._not_need_to_notify_program_changed");
        } else {
            mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_PROGRAM_CHANGED, channelNumber));
        }
    }

    public void notifyChannelUpdated(boolean needtoUpdate) {
        if (isOnScanning) {
            JLog.i(TAG, "on_Scannning._not_need_to_notify_channel_updated");
        } else {
            // needtoUpdate == false ? do not write the channelxml
            mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_CHANNEL_UPDATED, needtoUpdate));
        }
    }

	public void notifyScanReady(int total) {
		mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_SCAN_READY,
				new ScanInfo(total, 0, 0)));
	}

	public void notifyScanProcessing(int progress, int found) {
		mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_SCAN_PROCESSING, new ScanInfo(0,
				progress, found)));
	}

	public void notifyScanCompleted(int found) {
		mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_SCAN_COMPLETED, new ScanInfo(0, 0,
				found)));
	}

	public void notifyScanStop() {
		mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_SCAN_STOP, new ScanInfo(0, 0, 0)));
	}

	void notifyTDTDate(int year, int month, int date, int hour, int minute, int second) {
		// set Time : Month is 0 based so delete 1
		Calendar cal = Calendar.getInstance();
		cal.set(year, month - 1, date, hour, minute, second);
		mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_TDT_DATE, cal));
	}
 
    public void notifyChannelPMT() {
        if (isOnScanning) {
            JLog.i(TAG, "on_Scannning._not_need_to_notify_channel_pmt");
        } else {
            // needtoUpdate == false ? do not write the channelxml
            mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_CHANNEL_UPDATED, false));
        }
    }

	public void notifyErrorWepgConn() {
		mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_ERROR_WEPG_CONN));
	}

    public void sendBroadcastMessage(int type) {
        if (isOnScanning) {
            JLog.i(TAG, "on_Scannning._not_send_message");
            return;
        }
        String[] msgArray = new String[] {
                null, null, null
        };

		JLog.i(TAG, "msg=" + mHandler.getString(type));
		synchronized (mLock) {
			switch (type) {
				case MSG_NOTIFY_CHANNEL_READY:
					msgArray[0] = ISIServiceMsg.SUBMSG_READY_TO_START;
					break;
				case MSG_NOTIFY_CHANNEL_UPDATED:
					msgArray[0] = ISIServiceMsg.SUBMSG_UPDATE_CHANNEL_INFO;
					break;
				case MSG_NOTIFY_PROGRAM_UPDATED:
					msgArray[0] = ISIServiceMsg.SUBMSG_UPDATE_PROGRAM_INFO;
					break;
				default:
					return;
			} // end of switch
			mIntent.putExtra(ISIServiceMsg.MAINMSG, msgArray);
			mContext.sendBroadcast(mIntent);
		}
	}

    public void sendBroadcastMessage(int type, int version) { // not use
//        if (isOnScanning) {
//            JLog.i(TAG, "on_Scannning._not_send_message");
//            return;
//        }
        String[] msgArray = new String[] {
                null, null, null
        };

		JLog.i(TAG, "msg=" + mHandler.getString(type) + " || version=" +version);
		synchronized (mLock) {
			switch(type) {
			case MSG_NOTIFY_CHANNEL_READY:
				msgArray[0] = ISIServiceMsg.SUBMSG_READY_TO_START;
				msgArray[1] = Integer.toString(version); // sdt_version
				break;
			case MSG_NOTIFY_CHANNEL_UPDATED :
				msgArray[0] = ISIServiceMsg.SUBMSG_UPDATE_CHANNEL_INFO;
				msgArray[1] = Integer.toString(version); // sdt_version
				break;
			case MSG_NOTIFY_PROGRAM_CHANGED :
				msgArray[0] = ISIServiceMsg.SUBMSG_CHANGE_PROGRAM;
				msgArray[1] = Integer.toString(version); // channel_number
				break;
			default :
				break;
			} // end of switch
			mIntent.putExtra(ISIServiceMsg.MAINMSG, msgArray);
			mContext.sendBroadcast(mIntent);
		}
	}
	
	/*public void sendBroadcastMessage(int type, int channel_number) { // not use
		String[] msgArray = new String[] {
				null, null, null
		};

		JLog.i(TAG, "msg=" + mHandler.getString(type));
		if (type == MSG_NOTIFY_PROGRAM_CHANGED) {
			synchronized (mLock) {
				msgArray[0] = ISIServiceMsg.SUBMSG_CHANGE_PROGRAM;
				msgArray[1] = Integer.toString(channel_number);
				mIntent.putExtra(ISIServiceMsg.MAINMSG, msgArray);
				mContext.sendBroadcast(mIntent);
			}
		}
	}*/

	public void sendBroadcastMessage(int type, ScanInfo scanInfo) {
		String[] msgArray = new String[] {
				null, null, null
		};

		JLog.i(TAG, "msg=" + mHandler.getString(type));
		synchronized (mLock) {
			switch (type) {
				case MSG_NOTIFY_SCAN_READY:
					msgArray[0] = ISIServiceMsg.SUBMSG_START_CHANNEL_SCAN;
					msgArray[1] = scanInfo.mTotal;
					break;
				case MSG_NOTIFY_SCAN_PROCESSING:
					msgArray[0] = ISIServiceMsg.SUBMSG_PROGRESS_CHANNEL_SCAN;
					msgArray[1] = scanInfo.mProgress;
					msgArray[2] = scanInfo.mFound;
					break;
				case MSG_NOTIFY_SCAN_COMPLETED:
					msgArray[0] = ISIServiceMsg.SUBMSG_COMPLETE_CHANNEL_SCAN;
					msgArray[1] = scanInfo.mFound;
					break;
				case MSG_NOTIFY_SCAN_STOP:
					msgArray[0] = ISIServiceMsg.SUBMSG_STOP_CHANNEL_SCAN;
					break;

				default:
					return;
			} // end of switch
			mIntent.putExtra(ISIServiceMsg.MAINMSG, msgArray);
			mContext.sendBroadcast(mIntent);
		}
	}

	private static final int MSG_CHECK_CHANNEL_INFO = 0;

	private static final int MSG_NOTIFY_CHANNEL_UPDATED = 1;

	private static final int MSG_UPDATE_CHANNEL_XML = 2;

	private static final int MSG_NOTIFY_PROGRAM_UPDATED = 11;

	private static final int MSG_NOTIFY_PROGRAM_CHANGED = 12;

	private static final int MSG_NOTIFY_SCAN_READY = 21;

	private static final int MSG_NOTIFY_SCAN_PROCESSING = 22;

	private static final int MSG_NOTIFY_SCAN_COMPLETED = 23;

	private static final int MSG_NOTIFY_SCAN_STOP = 24;

	private static final int MSG_NOTIFY_CHANNEL_READY = 31;

	private static final int MSG_NOTIFY_TDT_DATE = 32;

	private static final int MSG_NOTIFY_ERROR_WEPG_CONN = 91;

	class ScanInfo {

		String mTotal = "0";

		String mProgress = "0";

		String mFound = "0";

		public ScanInfo(int total, int progress, int found) {
			mTotal = Integer.toString(total);
			mProgress = Integer.toString(progress);
			mFound = Integer.toString(found);
		}
	}; // end if class

	/**
	 * 채널정보의 유효검사. 유효데이터인 경우, channel.xml갱신. 아니면 알림안함
	 * 
	 * @param channnels
	 * @return 유효여부
	 */
	private boolean isValid(RetChannelList channnels) {
		if (channnels == null) {
			return false;
		}
		if (channnels.getResult() == ISIResult.SI_RESULT_OK) {
			ChannelXmlManager channelManager = ChannelXmlManager.getInstance(mContext);
			channelManager.setChannelList(channnels);
			return true;
		}
		return false;
	}

	class MessageHandler extends Handler {

		@Override
		public synchronized void handleMessage(Message msg) {

			JLog.d(TAG, "msg=" + getString(msg.what));
			switch (msg.what) {
				case MSG_CHECK_CHANNEL_INFO: {
						if (isValid(getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL))) {
							mHandler.sendMessage(mHandler.obtainMessage(MSG_NOTIFY_CHANNEL_UPDATED, true));
						}
					}
					break;
				case MSG_NOTIFY_CHANNEL_UPDATED: {
					if (msg.obj == null) {
						JLog.w(TAG, "msg=" + mHandler.getString(msg.what) + ":" + "null");
						return;
					}

					isUpdatedChInfo = true;
					
//					if(sdtVersion == 0xff)
					// ////////////////////////////////////////////////
					sendBroadcastMessage(MSG_NOTIFY_CHANNEL_UPDATED, sdtVersion);
					// ////////////////////////////////////////////////

					boolean needtoUpdate = (Boolean)msg.obj;
					if (needtoUpdate) {
						if (mHandler.hasMessages(MSG_UPDATE_CHANNEL_XML) == true) {
							mHandler.removeMessages(MSG_UPDATE_CHANNEL_XML);
						}
						mHandler.sendMessage(mHandler.obtainMessage(MSG_UPDATE_CHANNEL_XML, sdtVersion, 0, default_sid));
					}
				}
					break;
				case MSG_UPDATE_CHANNEL_XML: {
						if (USE_CHANNEL_XML) {
							ChannelXmlManager channelManager = ChannelXmlManager.getInstance(mContext);
//							channelManager.storeToXml(msg.arg1, msg.obj.toString()); //wjpark 2015.10.23
						}
					}
					break;
				case MSG_NOTIFY_PROGRAM_UPDATED: {
						isUpdatedPrgmInfo = true;
						// ////////////////////////////////////////////////
						sendBroadcastMessage(MSG_NOTIFY_PROGRAM_UPDATED);
						// ////////////////////////////////////////////////
					}
					break;
				case MSG_NOTIFY_PROGRAM_CHANGED: {
						if (msg.obj == null) {
							JLog.w(TAG, "msg=" + mHandler.getString(msg.what) + ":" + "null");
							return;
						}
	
						int ch = (Integer)msg.obj;
						if (ch > 0) {
							// ////////////////////////////////////////////////
							sendBroadcastMessage(MSG_NOTIFY_PROGRAM_CHANGED, ch);
							// ////////////////////////////////////////////////
						}
					}
					break;
				case MSG_NOTIFY_SCAN_READY:
				case MSG_NOTIFY_SCAN_PROCESSING: {
					if (msg.obj == null) {
                        JLog.w(TAG, "msg=" + mHandler.getString(msg.what) + ":" + "null");
                        return;
                    }

                    isOnScanning = true;
                    isUpdatedChInfo = true;
                    /////////////////////////////////////////////////
                    sendBroadcastMessage(msg.what, (ScanInfo)msg.obj);
                    //////////////////////////////////////////////////
				}
                    break;
					
				case MSG_NOTIFY_SCAN_COMPLETED:
				case MSG_NOTIFY_SCAN_STOP: {
					if (msg.obj == null) {
                        JLog.w(TAG, "msg=" + mHandler.getString(msg.what) + ":" + "null");
                        return;
                    }

                    isOnScanning = false;
                    isUpdatedChInfo = true;
                    //////////////////////////////////////////////////
                    sendBroadcastMessage(msg.what, (ScanInfo)msg.obj);
                    //////////////////////////////////////////////////
				}
                    break;

				case MSG_NOTIFY_TDT_DATE: {
					if (msg.obj == null) {
						JLog.w(TAG, "msg=" + mHandler.getString(msg.what) + ":" + "null");
						return;
					}
					Calendar cal = (Calendar)msg.obj;
					if (cal.getTimeInMillis() > 0) {
						ChannelConfManager confManager = ChannelConfManager.getInstance(mContext);

						if (confManager.getUseTDT().equalsIgnoreCase("y")) {
							// set AutoTime disable
							if (isAutoTimeEnabled()) {
								Boolean b = setAutoTimeDisable();
								JLog.i(TAG,
										"set_auto_time_disable(setAutoTimeDisable=" + b.toString()
												+ "/isAutoTimeEnabled=" + isAutoTimeEnabled() + ")");
							}

							// 시스템으로부터 현재시간(ms) 가져오기
							long now = System.currentTimeMillis();
							JLog.i(TAG, "get_system_time(" + now + "ms:" + dumpSystemTime(now)
									+ ")");

							// TDT으로부터 현재시간(ms) 가져오기
							long tdt = cal.getTimeInMillis();

							// 시스템시간설정 타임갭 계산하기
							long diff_time = (Math.abs(now - tdt)) / 1000;

							if (diff_time > MAX_SET_SYSTEM_TIME_GAP) {
								// set Time : Month is 0 based so delete 1
								// Calendar cal = Calendar.getInstance();
								// cal.set(2014, 3 - 1, 17, 21, 26, 00);
								// //cal.set(year, month - 1, date, hourOfDay,
								// minute, second);
								AlarmManager am = (AlarmManager)mContext
										.getSystemService(Context.ALARM_SERVICE);
								am.setTime(tdt);
								JLog.i(TAG, "set_system_time(" + tdt + "ms:" + dumpSystemTime(tdt)
										+ ")");
							}
						}
					}
				}
					break;

				case MSG_NOTIFY_ERROR_WEPG_CONN: {
					NetworkConfManager netManager = NetworkConfManager.getInstance(mContext);
					netManager.setWepgConnected(false);
				}
					break;
					
					
				 case MSG_NOTIFY_CHANNEL_READY: {
	                    isReadyChInfo = true;
	                    //////////////////////////////////////////////////
	                    sendBroadcastMessage(MSG_NOTIFY_CHANNEL_READY, sdtVersion);
	                    //////////////////////////////////////////////////
	                }
	                    break;

				default:
					break;
			}
		}

		private String getString(int what) {
			switch (what) {
				case MSG_CHECK_CHANNEL_INFO:
					return "MSG_CHECK_CHANNEL_INFO";
				case MSG_NOTIFY_CHANNEL_UPDATED:
					return "MSG_NOTIFY_CHANNEL_UPDATED";
				case MSG_UPDATE_CHANNEL_XML:
					return "MSG_UPDATE_CHANNEL_XML";
				case MSG_NOTIFY_PROGRAM_UPDATED:
					return "MSG_NOTIFY_PROGRAM_UPDATED";
				case MSG_NOTIFY_PROGRAM_CHANGED:
					return "MSG_NOTIFY_PROGRAM_CHANGED";
				case MSG_NOTIFY_SCAN_READY:
					return "MSG_NOTIFY_SCAN_READY";
				case MSG_NOTIFY_SCAN_PROCESSING:
					return "MSG_NOTIFY_SCAN_PROCESSING";
				case MSG_NOTIFY_SCAN_COMPLETED:
					return "MSG_NOTIFY_SCAN_COMPLETED";
				case MSG_NOTIFY_SCAN_STOP:
					return "MSG_NOTIFY_SCAN_STOP";
				case MSG_NOTIFY_CHANNEL_READY:
					return "MSG_NOTIFY_CHANNEL_READY";
				case MSG_NOTIFY_TDT_DATE:
					return "MSG_NOTIFY_TDT_DATE";
				case MSG_NOTIFY_ERROR_WEPG_CONN:
					return "MSG_NOTIFY_ERROR_WEPG_CONN";

				default:
					return "not supported message=" + what;
			}
		}

		private String dumpSystemTime(long milliseconds) {
			Calendar cl = Calendar.getInstance();

			StringBuffer sb = new StringBuffer();
			cl.setTimeInMillis(milliseconds);
			// Month is 0 based so add 1
			sb.append(" " + cl.get(Calendar.YEAR) + "/" + (cl.get(Calendar.MONTH) + 1) + "/"
					+ cl.get(Calendar.DATE));
			sb.append(" " + cl.get(Calendar.HOUR_OF_DAY) + ":" + cl.get(Calendar.MINUTE) + ":"
					+ cl.get(Calendar.SECOND));
			sb.append(" ");
			return sb.toString();
		}

		private boolean isAutoTimeEnabled() {
			return Settings.Global.getInt(mContext.getContentResolver(), Settings.Global.AUTO_TIME,
					0) > 0;
		}

		private boolean setAutoTimeDisable() {
			return Settings.Global.putInt(mContext.getContentResolver(), Settings.Global.AUTO_TIME,
					0);
		}

	};

	private String dumpTDTTime(Date date) {
		Calendar cl = Calendar.getInstance();

		StringBuffer sb = new StringBuffer();
		cl.setTime(date);
		// Month is 0 based so add 1
		sb.append(" " + cl.get(Calendar.YEAR) + "/" + (cl.get(Calendar.MONTH) + 1) + "/"
				+ cl.get(Calendar.DATE));
		sb.append(" " + cl.get(Calendar.HOUR_OF_DAY) + ":" + cl.get(Calendar.MINUTE) + ":"
				+ cl.get(Calendar.SECOND));
		sb.append(" ");
		return sb.toString();
	}

} // end of class