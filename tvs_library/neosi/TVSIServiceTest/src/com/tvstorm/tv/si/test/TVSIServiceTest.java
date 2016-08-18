/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-10-20 18:03:42 +0900 (월, 20 10월 2014) $
 * $LastChangedRevision: 13308 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.si.test;

import com.tvstorm.tv.ITVService;
import com.tvstorm.tv.TVContextManager;
import com.tvstorm.tv.si.SIService;
import com.tvstorm.tv.si.channel.CAInfo;
import com.tvstorm.tv.si.channel.ChannelInfo;
import com.tvstorm.tv.si.channel.ESInfo;
import com.tvstorm.tv.si.channel.IChannelInfo;
import com.tvstorm.tv.si.channel.IESInfo;
import com.tvstorm.tv.si.channel.IRetChannelList;
import com.tvstorm.tv.si.common.ISIResult;
import com.tvstorm.tv.si.common.ISIServiceMsg;
import com.tvstorm.tv.si.program.IRetProgramList;
import com.tvstorm.tv.si.program.ProgramInfo;
import com.tvstorm.tv.si.program.ProgramLinkInfo;
import com.tvstorm.tv.si.program.Programs;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.*;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class TVSIServiceTest extends Activity {
    private static final String TAG = "TVSIServiceTest";

    private static final int MSG_CONNECTED_SI_REMOTE_SERVICE = 100;

    private static final int MSG_DISCONNECTED_SI_REMOTE_SERVICE = 101;

    private static final int MSG_READY_TO_START = 200;

    private static final int MSG_START_CHANNEL_SCAN = 300;

    private static final int MSG_STOP_CHANNEL_SCAN = 301;

    private static final int MSG_PROGRESS_CHANNEL_SCAN = 302;

    private static final int MSG_COMPLETE_CHANNEL_SCAN = 303;

    private static final int MSG_UPDATE_CHANNEL_INFO = 400;

    private static final int MSG_UPDATE_PROGRAM_INFO = 401;

    private static final int MSG_CHANGE_PROGRAM = 402;

    private static final int MSG_CHANGE_CHANNEL = 403;

    private static final int MSG_LOCAL_AREACODE_CHANGED = 404;

    private static final int MSG_DUMP_CHANNEL_INFO = 500;

    private static final int MSG_DUMP_PROGRAM_INFO = 501;

    private static final int MSG_START_VIDEO = 600;

    // scan mode
    private static final int CONST_SCANMODE_POSTEL_AUTO = 0;

    private static final int CONST_SCANMODE_POSTEL_MANUAL = 1;

    private static final int CONST_SCANMODE_LGEQA_AUTO = 2;

    private static final int CONST_SCANMODE_LGEQA_MANUAL = 3;

    private static final int CONST_SCANMODE_MANUAL_CHANNEL = 4;

    private static final int CONST_SCANMODE_CHANNEL = 5;

    // bandwidth
    private static final String CONST_BANDWIDTH_6MHZ = "6";

    private static final String CONST_BANDWIDTH_7MHZ = "7";

    private static final String CONST_BANDWIDTH_8MHZ = "8";

    // modulation
    private static final String CONST_MODULATION_16QAM = "16";

    private static final String CONST_MODULATION_32QAM = "32";

    private static final String CONST_MODULATION_64QAM = "64";

    private static final String CONST_MODULATION_128QAM = "128";

    private static final String CONST_MODULATION_256QAM = "256";

    // tuner id
    private static final int CONST_TUNERID_1 = 0;

    private static final int CONST_TUNERID_2 = 1;

    // newtork id
    private static final String CONST_LOCAL_AREACODE_JAKARTA = "88";

    // input control
    ToggleButton connectToggleButton;

    Spinner scanmodeSpinner;

    EditText freqEditText;

    EditText symborateEditText;

    Spinner bandwidthSpinner;

    Spinner modulationSpinner;

    Spinner tuneridSpinner;

    EditText networkidEditText;

    ToggleButton startToggleButton;

    Button channelButton;

    Button programButton;

    Button channelUpButton;

    Button channelDownButton;

    CheckBox channelCheckBox;

    CheckBox programCheckBox;

    // view control
    TextView tvVersion;

    TextView tvStatusTextView;

    TextView tvChannelInfoTextView;

    TextView tvProgramInfoTextView;

    VideoView videoView;

    TextView tvCurChannelInfoTextView;

    private SIService siService = null;

    private boolean mIsConnected = false;

    private boolean mIsScanning = false;

    private int mScanMode = CONST_SCANMODE_CHANNEL;

    private String mBandwidth = CONST_BANDWIDTH_8MHZ; // index 2

    private String mModulation = CONST_MODULATION_64QAM; // index 2

    private int mTunerId = CONST_TUNERID_1;

    private Map<String, ChannelInfo> mChannelList = Collections.synchronizedMap(new LinkedHashMap<String, ChannelInfo>());

    private List<Programs> mProgramlList = Collections.synchronizedList(new ArrayList<Programs>());

    private int mChIndex = -1;

    private String mChUri = "tuner" + mTunerId + "://passthrough";

    private int mChNumber = 0;

    private int mChannelInfoUpdateCount = 0;

    private int mProgramInfoUpdateCount = 0;

    private int mProgramChangedCount = 0;

    private static final String[] channelInfoList = new String[] {
            "Get Default Channel", "Get Channel List",
    };

    private static final String[] programInfoList = new String[] {
            "Get Current Program", //
            "Get Program List", "Get Program List by Index", "Get Program List by Time", //
            "Get Multi-Channel Program List", "Get Multi-Channel Program List by Index", "Get Multi-Channel Program List by Time", //
            "Get Program List by Name", "Get Program List by Series",
    };

    private static final String[] prgmMonitorList = new String[] {
        "Set Current Channel",
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ArrayAdapter<CharSequence> adapter;
        connectToggleButton = (ToggleButton)findViewById(R.id.connectToggleButton);
        adapter = ArrayAdapter.createFromResource(this, R.array.array_scanmode, R.layout.spinner_item);
        //adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        scanmodeSpinner = (Spinner)findViewById(R.id.scanmodeSpinner);
        scanmodeSpinner.setAdapter(adapter);

        freqEditText = (EditText)findViewById(R.id.freqEditText);
        symborateEditText = (EditText)findViewById(R.id.symbolrateEditText);
        adapter = ArrayAdapter.createFromResource(this, R.array.array_badnwidth, R.layout.spinner_item);
        //adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        bandwidthSpinner = (Spinner)findViewById(R.id.bandwidthSpinner);
        bandwidthSpinner.setAdapter(adapter);

        adapter = ArrayAdapter.createFromResource(this, R.array.array_modulation, R.layout.spinner_item);
        //adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        modulationSpinner = (Spinner)findViewById(R.id.modulationSpinner);
        modulationSpinner.setAdapter(adapter);

        adapter = ArrayAdapter.createFromResource(this, R.array.array_tunerid, R.layout.spinner_item);
        //adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        tuneridSpinner = (Spinner)findViewById(R.id.tuneridSpinner);
        tuneridSpinner.setAdapter(adapter);

        networkidEditText = (EditText)findViewById(R.id.networkidEditText);
        startToggleButton = (ToggleButton)findViewById(R.id.startToggleButton);
        channelButton = (Button)findViewById(R.id.channelButton);
        programButton = (Button)findViewById(R.id.programButton);
        channelUpButton = (Button)findViewById(R.id.channelUpButton);
        channelDownButton = (Button)findViewById(R.id.channelDownButton);
        channelCheckBox = (CheckBox)findViewById(R.id.cb_channel);
        programCheckBox = (CheckBox)findViewById(R.id.cb_program);

        tvVersion = (TextView)findViewById(R.id.tv_version);
        tvVersion.setText("Version : " + getCurrentVersion(this));
        tvStatusTextView = (TextView)findViewById(R.id.tvStatusTextView);
        tvChannelInfoTextView = (TextView)findViewById(R.id.tvChannelInfoTextView);
        tvProgramInfoTextView = (TextView)findViewById(R.id.tvProgramInfoTextView);
        videoView = (VideoView)findViewById(R.id.videoView);
        tvCurChannelInfoTextView = (TextView)findViewById(R.id.tvCurChannelInfoTextView);

        initButtonProcess();
        disableConnectedResources();

        // default
        scanmodeSpinner.setSelection(mScanMode); // CHANNEL SEARCH
        tuneridSpinner.setSelection(mTunerId); // Tuner#1

        StringBuffer sb = new StringBuffer();
        sb.append("===================================\n");
        sb.append(" onCreate():" + this.getClass().getSimpleName() + "_v" + getCurrentVersion(this) + "\n");
        sb.append("===================================");
        Log.i(TAG, sb.toString());
        Toast.makeText(this, sb.toString(), Toast.LENGTH_LONG).show();

        // init focus
        connectToggleButton.requestFocus();
    }

    @Override
    protected void onDestroy() {
        stopVideo();
        if (mIsScanning) {
            stopChannelScan();
        }

        disconnectSIService();

        StringBuffer sb = new StringBuffer();
        sb.append("===================================\n");
        sb.append(" onDestroy():" + this.getClass().getSimpleName() + "\n");
        sb.append("===================================");
        Log.i(TAG, sb.toString());
        Toast.makeText(this, sb.toString(), Toast.LENGTH_LONG).show();
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        StringBuffer sb = new StringBuffer();
        sb.append("===================================\n");
        sb.append(" onPause():" + this.getClass().getSimpleName() + "\n");
        sb.append("===================================");
        Log.i(TAG, sb.toString());
        super.onPause();
    }

    @Override
    protected void onResume() {
        StringBuffer sb = new StringBuffer();
        sb.append("===================================\n");
        sb.append(" onResume()" + this.getClass().getSimpleName() + "\n");
        sb.append("===================================");
        Log.i(TAG, sb.toString());
        super.onResume();
    }

    private void initButtonProcess() {
        connectToggleButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (connectToggleButton.isChecked()) {
                    if (mIsConnected) {
                        return;
                    }
                    tvStatusTextView.setText("onClick Connect Button");
                    connectSIService();
                } else {
                    if (mIsConnected == false) {
                        return;
                    }
                    tvStatusTextView.setText("onClick Disconnect Button");
                    disconnectSIService();
                }
            }

        });

        scanmodeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String item = (String)parent.getItemAtPosition(position);
                if (item.equals("POSTEL_AUTO")) {
                    mScanMode = CONST_SCANMODE_POSTEL_AUTO;
                    setAutoScan4PostelResources();
                } else if (item.equals("POSTEL_MANUAL")) {
                    mScanMode = CONST_SCANMODE_POSTEL_MANUAL;
                    setManualScan4PostelResources();
                } else if (item.equals("LGEQA_AUTO")) {
                    mScanMode = CONST_SCANMODE_LGEQA_AUTO;
                    setAutoScan4LgeqaResources();
                } else if (item.equals("LGEQA_MANUAL")) {
                    mScanMode = CONST_SCANMODE_LGEQA_MANUAL;
                    setManualScan4LgeqaResources();
                } else if (item.equals("MANUAL_CHANNEL")) {
                    mScanMode = CONST_SCANMODE_MANUAL_CHANNEL;
                    setManualChannelScanResources();
                } else if (item.equals("CHANNEL")) {
                    mScanMode = CONST_SCANMODE_CHANNEL;
                    setChannelScanResources();
                }
                tvStatusTextView.setText("onItemSelected(" + position + ") : ScanMode=" + getScanMode(mScanMode));
            }

            @Override
            public void onNothingSelected(AdapterView<?> arg0) {
                // TODO Auto-generated method stub

            }

        });

        bandwidthSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String item = (String)parent.getItemAtPosition(position);
                if (item.equals("8MHz")) {
                    mBandwidth = CONST_BANDWIDTH_8MHZ;
                } else if (item.equals("7MHz")) {
                    mBandwidth = CONST_BANDWIDTH_7MHZ;
                } else if (item.equals("6MHz")) {
                    mBandwidth = CONST_BANDWIDTH_6MHZ;
                }
                tvStatusTextView.setText("onItemSelected(" + position + ") : BandWidth=" + mBandwidth);
            }

            @Override
            public void onNothingSelected(AdapterView<?> arg0) {
                // TODO Auto-generated method stub

            }

        });

        modulationSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String item = (String)parent.getItemAtPosition(position);
                if (item.equals("16QAM")) {
                    mModulation = CONST_MODULATION_16QAM;
                } else if (item.equals("32QAM")) {
                    mModulation = CONST_MODULATION_32QAM;
                } else if (item.equals("64QAM")) {
                    mModulation = CONST_MODULATION_64QAM;
                } else if (item.equals("128QAM")) {
                    mModulation = CONST_MODULATION_128QAM;
                } else if (item.equals("256QAM")) {
                    mModulation = CONST_MODULATION_256QAM;
                }
                tvStatusTextView.setText("onItemSelected(" + position + ") : Modulation=" + mModulation);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // TODO Auto-generated method stub

            }

        });

        tuneridSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                String item = (String)parent.getItemAtPosition(position);
                if (item.equals("Tuner#1")) {
                    mTunerId = CONST_TUNERID_1;
                } else if (item.equals("Tuner#2")) {
                    mTunerId = CONST_TUNERID_2;
                }
                tvStatusTextView.setText("onItemSelected(" + position + ") : TunerId=" + mTunerId);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                // TODO Auto-generated method stub

            }

        });

        startToggleButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (startToggleButton.isChecked()) {
                    if (siService == null || mIsConnected == false) {
                        return;
                    }
                    if (mIsScanning) {
                        return;
                    }
                    // stop video
                    stopVideo();

                    tvStatusTextView.setText("onClick Start Scan Button");
                    startChannelScan();
                } else {
                    if (siService == null || mIsConnected == false) {
                        return;
                    }
                    if (mIsScanning == false) {
                        return;
                    }
                    tvStatusTextView.setText("onClick Stop Scan Button");
                    stopChannelScan();
                }
            }
        });

        channelButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (siService == null || mIsConnected == false) {
                    return;
                }
                tvStatusTextView.setText("onClick Channel Button");
                createChannelInfoDlg();
            }
        });

        programButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (siService == null || mIsConnected == false) {
                    return;
                }
                tvStatusTextView.setText("onClick Program Button");
                createProgramInfoDlg();
            }

        });

        channelUpButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (siService == null || mIsConnected == false) {
                    return;
                }
                tvStatusTextView.setText("onClick CH+ Button");
                ChannelInfo[] list = currentChannelList();

                if (list != null) {
                    int next = (mChIndex + 1);
                    if (next >= list.length)
                        next = 0;

                    mChIndex = next;
                    //startVideo(list[mChIndex].getChannelUri());
                    mHandler.sendMessage(mHandler.obtainMessage(MSG_START_VIDEO));
                }
            }

        });

        channelDownButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                if (siService == null || mIsConnected == false) {
                    return;
                }
                tvStatusTextView.setText("onClick CH- Button");
                ChannelInfo[] list = currentChannelList();

                if (list != null) {
                    int prev = (mChIndex - 1);
                    if (prev < 0)
                        prev = list.length - 1;

                    mChIndex = prev;
                    //startVideo(list[mChIndex].getChannelUri());
                    mHandler.sendMessage(mHandler.obtainMessage(MSG_START_VIDEO));
                }
            }

        });

    }

    private void enableConnectedResources() {
        scanmodeSpinner.setVisibility(View.VISIBLE);
        freqEditText.setVisibility(View.VISIBLE);
        symborateEditText.setVisibility(View.VISIBLE);
        bandwidthSpinner.setVisibility(View.VISIBLE);
        modulationSpinner.setVisibility(View.VISIBLE);
        tuneridSpinner.setVisibility(View.VISIBLE);
        networkidEditText.setVisibility(View.VISIBLE);
        startToggleButton.setVisibility(View.VISIBLE);
        channelButton.setVisibility(View.VISIBLE);
        programButton.setVisibility(View.VISIBLE);
        channelUpButton.setVisibility(View.VISIBLE);
        channelDownButton.setVisibility(View.VISIBLE);
        channelCheckBox.setVisibility(View.VISIBLE);
        programCheckBox.setVisibility(View.VISIBLE);
    }

    private void disableConnectedResources() {
        scanmodeSpinner.setVisibility(View.GONE);
        freqEditText.setVisibility(View.GONE);
        symborateEditText.setVisibility(View.GONE);
        bandwidthSpinner.setVisibility(View.GONE);
        modulationSpinner.setVisibility(View.GONE);
        tuneridSpinner.setVisibility(View.GONE);
        networkidEditText.setVisibility(View.GONE);
        startToggleButton.setVisibility(View.GONE);
        channelButton.setVisibility(View.GONE);
        programButton.setVisibility(View.GONE);
        channelUpButton.setVisibility(View.GONE);
        channelDownButton.setVisibility(View.GONE);
        channelCheckBox.setVisibility(View.GONE);
        programCheckBox.setVisibility(View.GONE);
    }

    private void removePendingMessages() {
        //mHandler.removeCallbacksAndMessages(null);
        if (mHandler.hasMessages(MSG_CONNECTED_SI_REMOTE_SERVICE))
            mHandler.removeMessages(MSG_CONNECTED_SI_REMOTE_SERVICE);
        if (mHandler.hasMessages(MSG_DISCONNECTED_SI_REMOTE_SERVICE))
            mHandler.removeMessages(MSG_DISCONNECTED_SI_REMOTE_SERVICE);
        if (mHandler.hasMessages(MSG_READY_TO_START))
            mHandler.removeMessages(MSG_READY_TO_START);
        if (mHandler.hasMessages(MSG_START_CHANNEL_SCAN))
            mHandler.removeMessages(MSG_START_CHANNEL_SCAN);
        if (mHandler.hasMessages(MSG_STOP_CHANNEL_SCAN))
            mHandler.removeMessages(MSG_STOP_CHANNEL_SCAN);
        if (mHandler.hasMessages(MSG_PROGRESS_CHANNEL_SCAN))
            mHandler.removeMessages(MSG_PROGRESS_CHANNEL_SCAN);
        if (mHandler.hasMessages(MSG_COMPLETE_CHANNEL_SCAN))
            mHandler.removeMessages(MSG_COMPLETE_CHANNEL_SCAN);
        if (mHandler.hasMessages(MSG_UPDATE_CHANNEL_INFO))
            mHandler.removeMessages(MSG_UPDATE_CHANNEL_INFO);
        if (mHandler.hasMessages(MSG_UPDATE_PROGRAM_INFO))
            mHandler.removeMessages(MSG_UPDATE_PROGRAM_INFO);
        if (mHandler.hasMessages(MSG_CHANGE_PROGRAM))
            mHandler.removeMessages(MSG_CHANGE_PROGRAM);
        if (mHandler.hasMessages(MSG_CHANGE_CHANNEL))
            mHandler.removeMessages(MSG_CHANGE_CHANNEL);
        if (mHandler.hasMessages(MSG_LOCAL_AREACODE_CHANGED))
            mHandler.removeMessages(MSG_LOCAL_AREACODE_CHANGED);
        if (mHandler.hasMessages(MSG_DUMP_CHANNEL_INFO))
            mHandler.removeMessages(MSG_DUMP_CHANNEL_INFO);
        if (mHandler.hasMessages(MSG_DUMP_PROGRAM_INFO))
            mHandler.removeMessages(MSG_DUMP_PROGRAM_INFO);
        if (mHandler.hasMessages(MSG_START_VIDEO))
            mHandler.removeMessages(MSG_START_VIDEO);
    }

    private static String getCurrentVersion(Context context) {
        try {
            PackageInfo pi = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
            return pi.versionName;
        } catch (NameNotFoundException e) {
            Log.e(TAG, e.toString());
            return "0.1.0";
        }
    }

    private void setAutoScan4PostelResources() {
        //                      | POSTEL(AUTO)
        // Freq.(start to end)  | 47 to 862MHz
        // Bandwidth            | 8MHz
        // Modulation           | 64QAM    
        // Symbolrate           | 6875Ksps
        // Tuner#1              | Selectable
        // Tuner#2              | not need yet
        freqEditText.setText("47"); // 47 to 862(no input)
        freqEditText.setEnabled(false);
        bandwidthSpinner.setSelection(2); // 8MHZ(고정)
        bandwidthSpinner.setEnabled(false);
        modulationSpinner.setSelection(2); // 64QAM(고정)
        modulationSpinner.setEnabled(false);
        tuneridSpinner.setSelection(CONST_TUNERID_1); // Tuner#1(고정)
        tuneridSpinner.setEnabled(false);
        networkidEditText.setText("88"); // not use(no input)
        networkidEditText.setEnabled(false);

        Toast.makeText(getApplicationContext(), "POSTEL(AUTO):Freq.(47 to 862MHz)/Mod(64QAM)", Toast.LENGTH_SHORT).show();
    }

    private void setManualScan4PostelResources() {
        //                      | POSTEL(MANUAL)
        // Freq.(start to end)  | 47 to 862MHz
        // Bandwidth            | 8MHz
        // Modulation           | 64QAM
        // Symbolrate           | 6875Ksps
        // Tuner#1              | Selectable
        // Tuner#2              | not need yet
        freqEditText.setText("466"); // 47 to 862MHz(input)
        freqEditText.setEnabled(true);
        bandwidthSpinner.setSelection(2); // 8MHZ(고정)
        bandwidthSpinner.setEnabled(false);
        modulationSpinner.setSelection(2); // 64QAM(고정)
        modulationSpinner.setEnabled(false);
        tuneridSpinner.setSelection(CONST_TUNERID_1); // Tuner#1(고정)
        tuneridSpinner.setEnabled(false);
        networkidEditText.setText("88"); // not use(no input)
        networkidEditText.setEnabled(false);

        Toast.makeText(getApplicationContext(), "POSTEL(MANUAL):Freq.(47 to 862MHz)/Mod(64QAM)", Toast.LENGTH_SHORT).show();
    }

    private void setAutoScan4LgeqaResources() {
        //                      | LGEQA(AUTO)
        // Freq.(start to end)  | 54 to 998MHz
        // Bandwidth            | 8MHz
        // Modulation           | 256QAM
        // Symbolrate           | 6875Ksps
        // Tuner#1              | Selectable
        // Tuner#2              | Selectable
        freqEditText.setText("54"); // 54 to 998(no input)
        freqEditText.setEnabled(false);
        bandwidthSpinner.setSelection(2); // 8MHZ(고정)
        bandwidthSpinner.setEnabled(false);
        modulationSpinner.setSelection(4); // 256QAM(고정)
        modulationSpinner.setEnabled(false);
        tuneridSpinner.setSelection(CONST_TUNERID_1); // Tuner#1(가변)
        tuneridSpinner.setEnabled(true);
        networkidEditText.setText("88"); // not use(no input)
        networkidEditText.setEnabled(false);

        Toast.makeText(getApplicationContext(), "LGEQA(AUTO):Freq.(54 to 998MHz)/Mod(256QAM)", Toast.LENGTH_SHORT).show();
    }

    private void setManualScan4LgeqaResources() {
        //                      | LGEQA(MANUAL)
        // Freq.(start to end)  | 47 to 1002MHz
        // Bandwidth            | 6/7/8MHz
        // Modulation           | 16/32/64/128/256QAM
        // Symbolrate           | 6875Ksps
        // Tuner#1              | Selectable
        // Tuner#2              | Selectable
        freqEditText.setText("466"); // 47 to 1002MHz(input)
        freqEditText.setEnabled(true);
        bandwidthSpinner.setSelection(2); // 8MHZ(가변)
        bandwidthSpinner.setEnabled(true);
        modulationSpinner.setSelection(2); // 64QAM(가변)
        modulationSpinner.setEnabled(true);
        tuneridSpinner.setSelection(CONST_TUNERID_1); // Tuner#1(가변)
        tuneridSpinner.setEnabled(true);
        networkidEditText.setText("88"); // not use(no input)
        networkidEditText.setEnabled(false);

        Toast.makeText(getApplicationContext(), "LGEQA(MANUAL):Freq.(47 to 1002MHz)/Mod(16/32/64/128/256QAM)", Toast.LENGTH_SHORT).show();
    }

    private void setManualChannelScanResources() {
        //                      | LINKNET(MANUAL_CHANNEL)
        // Freq.(start to end)  | 47 to 862MHz
        // Bandwidth            | 8MHz
        // Modulation           | 64/128/256QAM
        // Symbolrate           | 6875Ksps
        // Tuner#1              | Selectable
        // Tuner#2              | net need yet
        freqEditText.setText("466"); // 47 to 862MHz(input)
        freqEditText.setEnabled(true);
        bandwidthSpinner.setSelection(2); // 8MHZ(고정)
        bandwidthSpinner.setEnabled(false);
        modulationSpinner.setSelection(2); // 64QAM(가변)
        modulationSpinner.setEnabled(true);
        tuneridSpinner.setSelection(CONST_TUNERID_1); // Tuner#1(고정)
        tuneridSpinner.setEnabled(false);
        networkidEditText.setText("88"); // 88(input)
        networkidEditText.setEnabled(true);

        Toast.makeText(getApplicationContext(), "LINKNET(MANUAL_CHANNEL):Freq.(47 to 862MHz)/Mod(64/128/256QAM)", Toast.LENGTH_SHORT).show();
    }

    private void setChannelScanResources() {
        //                      | LINKNET(CHANNEL)
        // Freq.(start to end)  | 466MHz
        // Bandwidth            | 8MHz
        // Modulation           | 64QAM
        // Symbolrate           | 6875Ksps
        // Tuner#1              | Selectable
        // Tuner#2              | net need yet
        freqEditText.setText("466"); // 466MHz(no input)
        freqEditText.setEnabled(false);
        bandwidthSpinner.setSelection(2); // 8MHZ(고정)
        bandwidthSpinner.setEnabled(false);
        modulationSpinner.setSelection(2); // 64QAM(고정)
        modulationSpinner.setEnabled(false);
        tuneridSpinner.setSelection(CONST_TUNERID_1); // Tuner#1(고정)
        tuneridSpinner.setEnabled(false);
        networkidEditText.setText("88"); // not use(no input)
        networkidEditText.setEnabled(false);

        Toast.makeText(getApplicationContext(), "LINKNET(CHANNEL):Freq.(466MHz)/Mod(64QAM)", Toast.LENGTH_SHORT).show();
    }

    /**
     * Connect SIService <BR>
     * Get SIService from TVContextManager, and then connect <BR>
     * Wait listener for SIService connected notification
     */
    private void connectSIService() {
        // eschoi_20130530 | use public context
        // (주의) Warning("W/ActivityManager( 1533): Binding with unknown activity")
        // getInstance(Context)에서 this에 의한 Context로는 제대로 바인드되지 않는다.
        // Public context를 반환하는 getApplicationContext()를 사용하도록 하자.
        TVContextManager tvContextManager = TVContextManager.getInstance(getApplicationContext());
        siService = (SIService)tvContextManager.getService(ITVService.SIService);

        ////////////////////////////////////////
        siService.connect(new TVSIServiceListener());
        ////////////////////////////////////////
    }

    /**
     * Disconnect SIService <BR>
     * Release SIService connection <BR>
     * Wait listener for SIService disconnected notification
     */
    private void disconnectSIService() {
        if (siService != null) {
            ////////////////////////////////////////
            siService.disconnect();
            ////////////////////////////////////////
        }
    }

    /**
     * Register SIService listener <BR>
     * Receive notification for SIService connection <BR>
     * - onTVServiceConnected() <BR>
     * - onTVServiceDisconnected()
     */
    public class TVSIServiceListener extends SIService.ConnectionListener {

        @Override
        public void onTVServiceConnected() {
            StringBuffer sb = new StringBuffer();
            sb.append("onTVServiceConnected to " + ITVService.SIService);
            Log.d(TAG, "\n\n" + sb.toString() + "\n\n");
            mIsConnected = true;

            tvStatusTextView.setText(sb.toString());
            mHandler.sendEmptyMessage(MSG_CONNECTED_SI_REMOTE_SERVICE);
        }

        @Override
        public void onTVServiceDisconnected() {
            StringBuffer sb = new StringBuffer();
            sb.append("onTVServiceDisconnected to " + ITVService.SIService);
            Log.d(TAG, "\n\n" + sb.toString() + "\n\n");
            mIsConnected = false;

            disableConnectedResources();
            removePendingMessages();

            tvStatusTextView.setText(sb.toString());
            mHandler.sendEmptyMessage(MSG_DISCONNECTED_SI_REMOTE_SERVICE);
        }

    };

    /**
     * Start Channel scan <BR>
     * Wait the notification message for channel scan start
     */
    private void startChannelScan() {
        if (siService != null) {
            String str0 = freqEditText.getText().toString();
            String str1 = symborateEditText.getText().toString();
            String str2 = mBandwidth.toString();
            String str3 = mModulation.toString();
            String str4 = networkidEditText.getText().toString();

            int freq = str0.isEmpty() ? 466 : Integer.parseInt(str0);
            int sr = str1.isEmpty() ? 6875 : Integer.parseInt(str1);
            int bw = str2.isEmpty() ? 8 : Integer.parseInt(str2);
            int demod = str3.isEmpty() ? 64 : Integer.parseInt(str3);
            int nid = str4.isEmpty() ? 88 : Integer.parseInt(str4);

            int tid = mTunerId;

            switch (mScanMode) {
                case CONST_SCANMODE_POSTEL_AUTO:
                    siService.startAutoScan(47, 862, sr, bw, demod, tid); // demod=64(64QAM)
                    break;
                case CONST_SCANMODE_POSTEL_MANUAL:
                    siService.startManualScan(freq, sr, bw, demod, tid); // demod=64(64QAM)
                    break;
                case CONST_SCANMODE_LGEQA_AUTO:
                    siService.startAutoScan(54, 998, sr, bw, demod, tid); // demod=256(256QAM)
                    break;
                case CONST_SCANMODE_LGEQA_MANUAL:
                    siService.startManualScan(freq, sr, bw, demod, tid); // demod=16/32/64/128/256QAM
                    break;
                case CONST_SCANMODE_MANUAL_CHANNEL:
                    siService.startManualChannelScan(freq, sr, bw, demod, nid);
                    break;
                case CONST_SCANMODE_CHANNEL:
                    siService.startChannelScan(false);
                    break;

                default:
                    Log.w(TAG, "not suppoted scan mode:" + mScanMode);
                    break;
            }
        }
    }

    /**
     * Stop Channel scan <BR>
     * Wait the notification message for channel scan stop
     */
    private void stopChannelScan() {
        if (siService != null) {
            ////////////////////////////////////////
            siService.stopChannelScan();
            ////////////////////////////////////////
        }
    }

    /**
     * Create Channel search dialog
     */
    private void createChannelSearchDlg() {
        final Context ctx = getApplicationContext();
        AlertDialog.Builder alertChannelInfoBuilder = new AlertDialog.Builder(this);
        alertChannelInfoBuilder.setTitle("Select Action!");
        alertChannelInfoBuilder.setCancelable(true);

        // FTI(First-Time-Installation
        alertChannelInfoBuilder.setPositiveButton("Channel Search", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                startChannelScan();
            }
        });

        // Booting
        alertChannelInfoBuilder.setNegativeButton("Channel Views ", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                // getDefaultChannel
                String chUid = siService.getDefaultChannel();

                if (chUid != null && chUid.isEmpty() == false) {
                    Log.w(TAG, "chUid=" + chUid);
                    Toast.makeText(ctx, "chUid=" + chUid, Toast.LENGTH_LONG).show();
                } else {
                    Log.w(TAG, "chUid is null. invalid!");
                }
                // getChannelList
                IRetChannelList ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL/* tvRadioMode */);

                if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                    // only dump
                    int saveChList = 1; // 0:false, 1:true
                    int startVideo = 0; // 0:false, 1:true
                    mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_CHANNEL_INFO, saveChList, startVideo, ret));
                }
                // channelChange
            }
        });

        alertChannelInfoBuilder.show();
    }

    /**
     * Create Channel info dialog
     */
    private void createChannelInfoDlg() {
        final Context ctx = getApplicationContext();
        LayoutInflater inflater = (LayoutInflater)this.getSystemService(LAYOUT_INFLATER_SERVICE);
        View layout = inflater.inflate(R.layout.custorm_dialog0, (ViewGroup)findViewById(R.id.layout_root));

        AlertDialog.Builder alertChannelInfoBuilder = new AlertDialog.Builder(this);
        alertChannelInfoBuilder.setTitle("Channel Info.");
        alertChannelInfoBuilder.setCancelable(true);
        alertChannelInfoBuilder.setView(layout);

        final RadioButton allRadioButton = (RadioButton)layout.findViewById(R.id.radio_ALL);
        final RadioButton tvRadioButton = (RadioButton)layout.findViewById(R.id.radio_TV);
        final RadioButton radiolRadioButton = (RadioButton)layout.findViewById(R.id.radio_RADIO);
        allRadioButton.toggle();

        alertChannelInfoBuilder.setSingleChoiceItems(channelInfoList, 0, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.v(TAG, "Items onClick : " + channelInfoList[which]);
                switch (which) {
                    case 0:
                        /**
                         * Get the default channel
                         */
                        String chUid = siService.getDefaultChannel();

                        if (chUid != null && chUid.isEmpty() == false) {
                            Log.w(TAG, "chUid=" + chUid);
                            Toast.makeText(ctx, "chUid=" + chUid, Toast.LENGTH_LONG).show();
                        } else {
                            Log.w(TAG, "fail to " + channelInfoList[which]);
                        }
                        break;

                    case 1:
                        /**
                         * Get the channel list
                         */
                        IRetChannelList ret;
                        if (tvRadioButton.isChecked()) {
                            ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_TV);
                        } else if (radiolRadioButton.isChecked()) {
                            ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_RADIO);
                        } else {
                            ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL);
                        }

                        if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                            // only dump
                            int saveChList = 1; // 0:false, 1:true
                            int startVideo = 0; // 0:false, 1:true
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_CHANNEL_INFO, saveChList, startVideo, ret));
                        } else {
                            Log.w(TAG, "fail to " + channelInfoList[which]);
                        }
                        break;

                    default:
                        break;
                }
            }
        });

        alertChannelInfoBuilder.show();
    }

    /**
     * Create Program Info Dialog
     */
    private void createProgramInfoDlg() {
        final Context ctx = getApplicationContext();
        LayoutInflater inflater = (LayoutInflater)this.getSystemService(LAYOUT_INFLATER_SERVICE);
        View layout = inflater.inflate(R.layout.custorm_dialog1, (ViewGroup)findViewById(R.id.layout_root));

        AlertDialog.Builder alertProgramInfoBuilder = new AlertDialog.Builder(this);
        alertProgramInfoBuilder.setTitle("Program Info.");
        alertProgramInfoBuilder.setCancelable(true);
        alertProgramInfoBuilder.setView(layout);

        final EditText etChNum = (EditText)layout.findViewById(R.id.et_chNum01);
        final EditText etChNumList = (EditText)layout.findViewById(R.id.et_chNumList01);
        final EditText etStIndex = (EditText)layout.findViewById(R.id.et_stIndex);
        final EditText etRqCount = (EditText)layout.findViewById(R.id.et_rqCount);
        final EditText etStTime = (EditText)layout.findViewById(R.id.et_stTime);
        final EditText etEdTime = (EditText)layout.findViewById(R.id.et_edTime);
        final EditText etRqName = (EditText)layout.findViewById(R.id.et_rqName);
        final CheckBox cbDescFlag = (CheckBox)layout.findViewById(R.id.cb_descFlag);

        alertProgramInfoBuilder.setSingleChoiceItems(programInfoList, 0, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.v(TAG, "Items onClick : " + programInfoList[which]);

                String chNum = etChNum.getText().toString();
                String chNums = etChNumList.getText().toString();
                int startIdx = Integer.valueOf(etStIndex.getText().toString());
                int requestCnt = Integer.valueOf(etRqCount.getText().toString());
                int startTime = Integer.valueOf(etStTime.getText().toString());
                int endTime = Integer.valueOf(etEdTime.getText().toString());
                Date startDate = convertStringToDate(etStTime.getText().toString());
                Date endDate = convertStringToDate(etEdTime.getText().toString());
                String requestName = etRqName.getText().toString();
                boolean descFlag = cbDescFlag.isChecked();
                int seasonNo = Integer.valueOf(etStIndex.getText().toString());
                int episodeNo = Integer.valueOf(etRqCount.getText().toString());
                Log.d(TAG, "params : " + chNum + "/" + chNums + "/" + startIdx + "/" + requestCnt + "/" + startTime + "/" + endTime + "/"
                        + requestName + "/" + (descFlag == true ? "true" : "false"));

                String chUid = "";
                String[] chUids = null;
                StringBuffer sb = new StringBuffer();
                IRetProgramList ret = null;
                switch (which) {
                    case 0:
                        /**
                         * Get the current program of channel
                         */
                        chUid = convertChNumToChUid(chNum);
                        if (chUid.isEmpty() == false) {
                            sb.append("ChNum=" + chNum + "(" + chUid + ")");
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getCurrentProgram(chUid);
                        }
                        break;

                    case 1:
                        /**
                         * Get the all program of channel
                         */
                        chUid = convertChNumToChUid(chNum);
                        if (chUid.isEmpty() == false) {
                            sb.append("ChNum=" + chNum + "(" + chUid + ")");
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getProgramList(chUid);
                        }
                        break;

                    case 2:
                        /**
                         * Get the part program of channel <BR>
                         * Return program list as long as the number of counts from start index.
                         */
                        chUid = convertChNumToChUid(chNum);
                        if (chUid.isEmpty() == false) {
                            sb.append("ChNum=" + chNum + "(" + chUid + ")\n");
                            sb.append("StartIndex=" + startIdx + "\n");
                            sb.append("RequestCount=" + requestCnt);
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getProgramListByIndex(chUid, startIdx, requestCnt);
                        }
                        break;

                    case 3:
                        /**
                         * Get the part program of channel <BR>
                         * Return program list from start to end time.
                         */
                        chUid = convertChNumToChUid(chNum);
                        if (chUid.isEmpty() == false) {
                            sb.append("ChNum=" + chNum + "(" + chUid + ")\n");
                            sb.append("Start/EndTime=" + dumpEventTime(startDate, endDate));
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getProgramListByTime(chUid, startDate, endDate);
                        }
                        break;

                    case 4:
                        // eschoi_20130509 | test for max_channel_count(8)
                        //chNumList = "1|2|3|5|6|7|8|9";
                        /**
                         * Get the all program of multi-channel
                         */
                        chUids = convertChNumToChUid(convertStringToStringArray(chNums, "\\|"));
                        if (chUids.length > 0) {
                            sb.append("ChNums=" + chNums + "(" + chUids.toString() + ")");
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getMultiChannelProgramList(chUids);
                        }
                        break;

                    case 5:
                        /**
                         * Get the part program of multi-channel <BR>
                         * Return program list as long as the number of counts from start index.
                         */
                        chUids = convertChNumToChUid(convertStringToStringArray(chNums, "\\|"));
                        if (chUids.length > 0) {
                            sb.append("ChNums=" + chNums + "(" + chUids.toString() + ")\n");
                            sb.append("StartIndex=" + startIdx + "\n");
                            sb.append("RequestCount=" + requestCnt);
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getMultiChannelProgramListByIndex(chUids, startIdx, requestCnt);
                        }
                        break;

                    case 6:
                        /**
                         * Get the part program of multi-channel <BR>
                         * Return program list from start to end time.
                         */
                        chUids = convertChNumToChUid(convertStringToStringArray(chNums, "\\|"));
                        if (chUids.length > 0) {
                            sb.append("ChNums=" + chNums + "(" + chUids.toString() + ")\n");
                            sb.append("Start/EndTime=" + dumpEventTime(startDate, endDate));
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getMultiChannelProgramListByTime(chUids, startDate, endDate);
                        }
                        break;

                    case 7:
                        // eschoi_20140107 | test
                        /**
                         * Get the part program of channel <BR>
                         * Return program list including name.
                         */
                        if (chNum.equals("0") || chNum.equals(IChannelInfo.CONST_DEFAULT_CHANNEL_UID)) {
                            chUid = IChannelInfo.CONST_DEFAULT_CHANNEL_UID;
                        } else {
                            chUid = convertChNumToChUid(chNum);
                        }
                        if (requestName.isEmpty())
                            requestName = "What's The Big Idea?";
                        if (chUid.isEmpty() == false) {
                            Date curTime = new Date();
                            sb.append("ChNum=" + chNum + "(" + chUid + ")\n");
                            sb.append("requestName=" + requestName + "\n");
                            sb.append("descFlag=" + (descFlag == true ? "true" : "false"));
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getProgramListByName(chUid, curTime, requestName, descFlag);
                        }
                        break;

                    case 8:
                        // eschoi_20141001 | test
                        /**
                         * Get the part program of channel <BR>
                         * Return program list including name with next series.
                         */
                        if (chNum.equals("0") || chNum.equals(IChannelInfo.CONST_DEFAULT_CHANNEL_UID)) {
                            chUid = IChannelInfo.CONST_DEFAULT_CHANNEL_UID;
                        } else {
                            chUid = convertChNumToChUid(chNum);
                        }
                        if (startDate.getTime() == 0)
                            startDate = new Date();
                        if (requestName.isEmpty())
                            requestName = "What's The Big Idea?";
                        if (chUid.isEmpty() == false) {
                            sb.append("ChNum=" + chNum + "(" + chUid + ")\n");
                            sb.append("reauestTime=" + dumpEventTime(startDate, endDate));
                            sb.append("requestName=" + requestName + "\n");
                            sb.append("seasonNo/episodeNo=" + seasonNo + "/" + episodeNo);
                            Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                            ret = siService.getProgramListBySeries(chUid, startDate, requestName, seasonNo, episodeNo);
                        }
                        break;

                    default:
                        break;
                }

                if (ret == null) {
                    Log.w(TAG, "return program list is null");
                    return;
                }

                if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                    mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_PROGRAM_INFO, ret));
                } else {
                    Log.w(TAG, "fail to " + programInfoList[which]);
                }
            }
        });

        alertProgramInfoBuilder.show();
    }

    /**
     * Create Program Monitor Dialog
     */
    private void createProgramMonitorDlg() {
        final Context ctx = getApplicationContext();
        LayoutInflater inflater = (LayoutInflater)this.getSystemService(LAYOUT_INFLATER_SERVICE);
        View layout = inflater.inflate(R.layout.custorm_dialog2, (ViewGroup)findViewById(R.id.layout_root));

        AlertDialog.Builder alertPrgmMonitorBuilder = new AlertDialog.Builder(this);
        alertPrgmMonitorBuilder.setTitle("Program Monitor");
        alertPrgmMonitorBuilder.setCancelable(true);
        alertPrgmMonitorBuilder.setView(layout);

        final EditText etChNum = (EditText)layout.findViewById(R.id.et_chNum02);

        alertPrgmMonitorBuilder.setSingleChoiceItems(prgmMonitorList, 0, new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface dialog, int which) {
                Log.v(TAG, "Items onClick : " + prgmMonitorList[which]);

                int chNum = Integer.parseInt(etChNum.getText().toString());
                Log.d(TAG, "params : " + chNum);

                String chUid = "";
                StringBuffer sb = new StringBuffer();
                switch (which) {
                    case 0:
                        /**
                         * Set the current channel
                         */
                        chUid = mChannelList.get(chNum + "").getChannelUid();
                        sb.append("ChNum=" + chNum + "(" + chUid + ")\n");
                        sb.append(chNum > 0 ? "available" : "not available");
                        Toast.makeText(ctx, sb.toString(), Toast.LENGTH_LONG).show();
                        if (chNum > 0) {
                            siService.setCurrentChannel(chUid);

                            mChNumber = chNum;
                            mProgramChangedCount = 0;
                        }
                        break;

                    default:
                        break;
                }
            }
        });

        alertPrgmMonitorBuilder.show();
    }

    /**
     * Start/Stop Video
     */
    private ChannelInfo[] currentChannelList() {
        ChannelInfo[] resultInfos = null;
        if (mChannelList.size() > 0) {
            resultInfos = new ChannelInfo[mChannelList.size()];
            Set keySet = mChannelList.keySet();
            Iterator itr = keySet.iterator();
            int index = 0;
            while (itr.hasNext()) {
                Object key = itr.next();
                ChannelInfo chInfo = (ChannelInfo)mChannelList.get(key);
                if (chInfo != null && chInfo.getChannelId() > 0) {
                    resultInfos[index++] = chInfo;
                }
            }
        }

        return resultInfos;
    }

    private void startVideo(String uri) {
        mChUri = uri;

        Log.w(TAG, "setVideoPath_start");
        videoView.setVideoPath(mChUri);
        videoView.start();

        String message = String.format("#%d - %s", mChIndex, mChUri);
        tvCurChannelInfoTextView.setText(message);
        Log.d(TAG, message);
        //Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    private void stopVideo() {
        Log.w(TAG, "stopPlayback");
        videoView.stopPlayback();

        String message = String.format("#%d - %s", mChIndex, mChUri);
        tvCurChannelInfoTextView.setText(message);
        Log.d(TAG, message);
        //Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    private TVSIServiceMsgReceiver mReceiver = null;

    /**
     * Register SIService listener <BR>
     * Receive notification for SIService connection <BR>
     * - onTVServiceConnected() <BR>
     * - onTVServiceDisconnected()
     */
    /**
     * Register SIService receiver <BR>
     * Receive notification for SIService event. <BR>
     * - ISIServiceMsg.SUBMSG_READY_TO_START <BR>
     * - ISIServiceMsg.SUBMSG_START_CHANNEL_SCAN <BR>
     * - ISIServiceMsg.SUBMSG_STOP_CHANNEL_SCAN <BR>
     * - ISIServiceMsg.SUBMSG_PROGRESS_CHANNEL_SCAN <BR>
     * - ISIServiceMsg.SUBMSG_COMPLETE_CHANNEL_SCAN <BR>
     * - ISIServiceMsg.SUBMSG_UPDATE_CHANNEL_INFO <BR>
     * - ISIServiceMsg.SUBMSG_UPDATE_PROGRAM_INFO <BR>
     * - ISIServiceMsg.SUBMSG_CHANGE_PROGRAM <BR>
     */
    public class TVSIServiceMsgReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            Intent i = intent;
            //Log.d(TAG, "\n\n" + "onReceive from Broadcaster : i=" + intent + "\n\n");

            String[] msg = new String[] {
                    null, null, null
            };

            if (i.getAction().equals(ISIServiceMsg.ACTION_SIService)) {
                if (i.hasExtra(ISIServiceMsg.MAINMSG)) {
                    msg = i.getStringArrayExtra(ISIServiceMsg.MAINMSG);
                } else {
                    return;
                }

                if (msg != null) {
                    //Log.d(TAG, "subMsg=" + msg[0] + ": chUid=" + msg[1]);
                    String subMsg = msg[0];
                    if (subMsg != null && subMsg.isEmpty() == false) {
                        Log.d(TAG, "\n\n" + msg[0].toString() + "\n\n");

                        if (msg[0].equals(ISIServiceMsg.SUBMSG_READY_TO_START)) {
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_READY_TO_START));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_START_CHANNEL_SCAN)) {
                            if (msg[1] == null || msg[1].isEmpty()) {
                                return;
                            }
                            int total = Integer.parseInt(msg[1]);
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_START_CHANNEL_SCAN, total));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_STOP_CHANNEL_SCAN)) {
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_STOP_CHANNEL_SCAN));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_PROGRESS_CHANNEL_SCAN)) {
                            if (msg[1] == null || msg[1].isEmpty() || msg[2] == null || msg[2].isEmpty()) {
                                return;
                            }
                            int progress = Integer.parseInt(msg[1]);
                            int found = Integer.parseInt(msg[2]);
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_PROGRESS_CHANNEL_SCAN, progress, found));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_COMPLETE_CHANNEL_SCAN)) {
                            if (msg[1] == null || msg[1].isEmpty()) {
                                return;
                            }
                            int found = Integer.parseInt(msg[1]);
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_COMPLETE_CHANNEL_SCAN, found));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_UPDATE_CHANNEL_INFO)) {
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_UPDATE_CHANNEL_INFO));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_UPDATE_PROGRAM_INFO)) {
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_UPDATE_PROGRAM_INFO));
                        } else if (msg[0].equals(ISIServiceMsg.SUBMSG_CHANGE_PROGRAM)) {
                            if (msg[1] == null || msg[1].isEmpty()) {
                                return;
                            }
                            String chUid = msg[1];
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_CHANGE_PROGRAM, chUid));
                        }
                    }
                }
            }
        }
    };

    private Handler mHandler = new Handler() {

        @Override
        public synchronized void handleMessage(Message msg) {
            if (siService == null) {
                Log.w(TAG, ITVService.SIService + " is null");
                return;
            }

            StringBuffer sb = new StringBuffer();
            switch (msg.what) {
                case MSG_CONNECTED_SI_REMOTE_SERVICE:
                    // registerReceiver
                    mReceiver = new TVSIServiceMsgReceiver();
                    IntentFilter filter = new IntentFilter();
                    filter.addAction(ISIServiceMsg.ACTION_SIService);
                    ////////////////////////////////////////
                    registerReceiver(mReceiver, filter);
                    ////////////////////////////////////////
                    Log.d(TAG, "registerReceiver: filter action(" + ISIServiceMsg.ACTION_SIService + ")");

                    enableConnectedResources();

                    // setLocalAreaCode eschoi_20140327 | must!!!
                    if (mHandler.hasMessages(MSG_LOCAL_AREACODE_CHANGED) == true) {
                        mHandler.removeMessages(MSG_LOCAL_AREACODE_CHANGED);
                    }
                    mHandler.sendEmptyMessage(MSG_LOCAL_AREACODE_CHANGED);

                    // setCurrentChannel eschoi_20140117 | must!!!
                    if (mHandler.hasMessages(MSG_CHANGE_CHANNEL) == true) {
                        mHandler.removeMessages(MSG_CHANGE_CHANNEL);
                    }
                    mHandler.sendEmptyMessage(MSG_CHANGE_CHANNEL);
                    break;

                case MSG_DISCONNECTED_SI_REMOTE_SERVICE:
                    // unregisterReceiver
                    if (mReceiver != null) {
                        Log.d(TAG, "unregisterReceiver: filter action(" + ISIServiceMsg.ACTION_SIService + ")");
                        unregisterReceiver(mReceiver);
                        mReceiver = null;
                    }

                    mChannelInfoUpdateCount = 0;
                    tvChannelInfoTextView.setText("Channel Info.");
                    mProgramInfoUpdateCount = 0;
                    tvProgramInfoTextView.setText("Program Info.");
                    tvCurChannelInfoTextView.setText("Current Channel Info.");
                    //siService = null;
                    break;

                case MSG_READY_TO_START: {
                    tvStatusTextView.setText(getString(msg.what));
                    // get default channel uid.
                    String chUid = IChannelInfo.CONST_DEFAULT_CHANNEL_UID;
                    chUid = siService.getDefaultChannel();
                    if (chUid == null || chUid.isEmpty()) {
                        chUid = IChannelInfo.CONST_DEFAULT_CHANNEL_UID;
                    }
                    Toast.makeText(getApplicationContext(), "서비스가 준비되었습니다." + chUid, Toast.LENGTH_LONG).show();

                    // get updated channel info.
                    IRetChannelList ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL/* tvRadioMode */);
                    if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                        if (mHandler.hasMessages(MSG_DUMP_CHANNEL_INFO)) {
                            mHandler.removeMessages(MSG_DUMP_CHANNEL_INFO);
                        }
                        // only dump
                        int saveChList = 1; // 0:false, 1:true
                        int startVideo = 0; // 0:false, 1:true
                        mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_CHANNEL_INFO, saveChList, startVideo, ret));
                    }
                }
                    break;
                case MSG_START_CHANNEL_SCAN:
                    sb.append(getString(msg.what) + " total=" + msg.obj);
                    Log.d(TAG, sb.toString());
                    tvStatusTextView.setText(sb.toString());

                    int total = (Integer)msg.obj;
                    if (total == 0) { // invalid network
                        Toast.makeText(getApplicationContext(), "스캔할 수 없습니다. freq(0)", Toast.LENGTH_LONG).show();
                        stopChannelScan();
                        return;
                    }

                    mIsScanning = true;
                    break;
                case MSG_STOP_CHANNEL_SCAN: {
                    sb.append(getString(msg.what));
                    Log.d(TAG, sb.toString());
                    tvStatusTextView.setText(sb.toString());

                    // get updated channel info.
                    IRetChannelList ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL/* tvRadioMode */);
                    if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                        if (mHandler.hasMessages(MSG_DUMP_CHANNEL_INFO)) {
                            mHandler.removeMessages(MSG_DUMP_CHANNEL_INFO);
                        }
                        // only dump
                        int saveChList = 1; // 0:false, 1:true
                        int startVideo = 1; // 0:false, 1:true
                        mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_CHANNEL_INFO, saveChList, startVideo, ret));
                    }

                    mIsScanning = false;
                }
                    break;
                case MSG_PROGRESS_CHANNEL_SCAN:
                    sb.append(getString(msg.what) + " progress=" + msg.arg1 + " found=" + msg.arg2);
                    Log.d(TAG, sb.toString());
                    tvStatusTextView.setText(sb.toString());
                    break;
                case MSG_COMPLETE_CHANNEL_SCAN:
                    sb.append(getString(msg.what) + " found=" + msg.obj);
                    Log.d(TAG, sb.toString());
                    tvStatusTextView.setText(sb.toString());

                    mIsScanning = false;
                    // enable start Scan button
                    startToggleButton.toggle();

                    // get updated channel info.
                    int found = (Integer)msg.obj;
                    if (found > 0) {
                        IRetChannelList ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL/* tvRadioMode */);
                        if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                            if (mHandler.hasMessages(MSG_DUMP_CHANNEL_INFO)) {
                                mHandler.removeMessages(MSG_DUMP_CHANNEL_INFO);
                            }
                            // start video
                            int saveChList = 1; // 0:false, 1:true
                            int startVideo = 1; // 0:false, 1:true
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_CHANNEL_INFO, saveChList, startVideo, ret));
                        }
                    }
                    break;

                case MSG_UPDATE_CHANNEL_INFO:
                    // getChannelList
                    mChannelInfoUpdateCount++;
                    tvChannelInfoTextView.setText("Channel updated count : " + mChannelInfoUpdateCount);
                    sb.append(getString(msg.what) + "(" + mChannelInfoUpdateCount + ")");
                    Log.d(TAG, "\n\n*** " + sb.toString() + " ***\n\n");
                    Toast.makeText(getApplicationContext(), sb.toString(), Toast.LENGTH_LONG).show();

                    // get updated channel info.
                    //IRetChannelList ret = siService.getChannelList(IChannelInfo.CONST_CHANNEL_MODE_ALL/* tvRadioMode */);
                    //if (ret.getResult() == ISIResult.SI_RESULT_OK) {
                    //    if (mHandler.hasMessages(MSG_DUMP_CHANNEL_INFO)) {
                    //        mHandler.removeMessages(MSG_DUMP_CHANNEL_INFO);
                    //    }
                    //    // only dump
                    //    int saveChList = 1; // 0:false, 1:true
                    //    int startVideo = 0; // 0:false, 1:true
                    //    mHandler.sendMessage(mHandler.obtainMessage(MSG_DUMP_CHANNEL_INFO, saveChList, startVideo, ret));
                    //}
                    break;

                case MSG_UPDATE_PROGRAM_INFO:
                    // getProgramList
                    if (mChannelInfoUpdateCount == 0) {
                        Log.w(TAG, "chinfo was not yet updated");
                    }
                    mProgramInfoUpdateCount++;
                    tvProgramInfoTextView.setText("Program updated count : " + mProgramInfoUpdateCount);
                    sb.append(getString(msg.what) + "(" + mProgramInfoUpdateCount + ")");
                    Log.d(TAG, "\n\n*** " + sb.toString() + " ***\n\n");
                    Toast.makeText(getApplicationContext(), sb.toString(), Toast.LENGTH_LONG).show();
                    break;

                case MSG_CHANGE_PROGRAM:
                    break;

                case MSG_CHANGE_CHANNEL: {
                    String chUid = IChannelInfo.CONST_DEFAULT_CHANNEL_UID;
                    if (mChannelList.size() > 0) {
                        if (mChannelList.containsKey(mChNumber)) {
                            chUid = mChannelList.get(mChNumber + "").getChannelUid();
                        }
                    }
                    sb.append("ChNum=" + mChNumber + "(" + chUid + ")\n");
                    sb.append(mChNumber > 0 ? "available" : "not available");
                    Toast.makeText(getApplicationContext(), sb.toString(), Toast.LENGTH_LONG).show();

                    ////////////////////////////////////////
                    siService.setCurrentChannel(chUid);
                    ////////////////////////////////////////
                }
                    break;

                case MSG_LOCAL_AREACODE_CHANGED:
                    /**
                     * Client는 SIService에 연결후, 지역의 네트워크 아이디를 SIService에 알려주어야 한다. <BR>
                     * SIService는 지역코드를 수신후, READY_TO_START를 전송한다. <BR>
                     */
                    ////////////////////////////////////////
                    siService.setLocalAreaCode(CONST_LOCAL_AREACODE_JAKARTA); // default network id(88)
                    ////////////////////////////////////////
                    break;

                case MSG_DUMP_CHANNEL_INFO:
                    if (msg.obj == null) {
                        Log.w(TAG, "not available handler");
                        return;
                    }
                    IRetChannelList retChList = (IRetChannelList)msg.obj;

                    //////////////////////////////
                    if (!mChannelList.isEmpty()) {
                        mChannelList.clear();
                    }
                    mChIndex = -1;
                    mChUri = "tuner" + mTunerId + "://passthrough";
                    //////////////////////////////

                    // saveChList
                    ArrayList<ChannelInfo> chList = retChList.getList();
                    int totalCh = chList.size();
                    if (totalCh > 0) {
                        ChannelInfo chInfo;

                        Log.d(TAG, "================================================");
                        Log.d(TAG, " Get Channel Info. List(" + totalCh + ")");
                        Log.d(TAG, "================================================");
                        for (int idx = 0; idx < totalCh; idx++) {
                            chInfo = chList.get(idx);

                            dumpChannelInfo(idx, chInfo, channelCheckBox.isChecked());
                            if (!mChannelList.containsKey(chInfo.getChannelNum() + "")) {
                                mChannelList.put(chInfo.getChannelNum() + "", chInfo);
                            } else {
                                Log.d(TAG, "ChannelList(" + chInfo.getChannelNum() + ") duplicated so that it would be skipped ... ");
                            }
                        }
                        Log.d(TAG, "================================================" + totalCh);
                    }
                    Toast.makeText(getApplicationContext(), "totalCh=" + totalCh, Toast.LENGTH_LONG).show();

                    // startVideo
                    if (msg.arg2 == 1) {
                        if (mHandler.hasMessages(MSG_START_VIDEO)) {
                            mHandler.removeMessages(MSG_START_VIDEO);
                        }
                        if (totalCh > 0) {
                            mChIndex = 0;
                            mHandler.sendMessage(mHandler.obtainMessage(MSG_START_VIDEO));
                        } else {
                            String message = String.format("#%d - %s", mChIndex, mChUri);
                            tvCurChannelInfoTextView.setText(message);
                        }
                    }
                    break;

                case MSG_DUMP_PROGRAM_INFO:
                    if (msg.obj == null) {
                        Log.w(TAG, "not available handler");
                        return;
                    }
                    IRetProgramList retPrgmList = (IRetProgramList)msg.obj;

                    //////////////////////////////
                    if (mProgramlList.size() > 0) {
                        mProgramlList.clear();
                    }
                    //////////////////////////////

                    // 1.
                    mProgramlList = retPrgmList.getList();
                    int multiChCnt = mProgramlList.size();
                    if (multiChCnt > 0) {
                        Programs prgms;

                        Log.d(TAG, "================================================");
                        Log.d(TAG, "Get Program Info. List");
                        Log.d(TAG, "================================================");
                        for (int i = 0; i < multiChCnt; i++) {
                            // 2.
                            prgms = mProgramlList.get(i);
                            String chUid = prgms.getChannelUid();
                            Log.d(TAG, "ChannelUid[" + chUid + "]");
                            ArrayList<ProgramInfo> prgmInfoList = new ArrayList<ProgramInfo>();
                            // 3.
                            prgmInfoList = prgms.getProgramInfoList();
                            for (int j = 0; j < prgmInfoList.size(); j++) {
                                ProgramInfo prgmInfo = prgmInfoList.get(j);

                                dumpProgramInfo(i, j, prgmInfo, programCheckBox.isChecked());
                                dumpPrgmLinkInfo(prgmInfo, false /* mCbProgram.isChecked() */);
                            }
                            if (multiChCnt - i > 1) { // not last
                                Log.d(TAG, "------------------------------------------------");
                            }
                        }
                        Log.d(TAG, "================================================");

                    }
                    break;

                case MSG_START_VIDEO:
                    if (mChIndex < 0) {
                        Log.w(TAG, "not available index");
                        return;
                    }

                    ChannelInfo[] list = currentChannelList();

                    if (list != null) {
                        startVideo(list[mChIndex].getChannelUri());
                    }
                    break;

                default:
                    Log.d(TAG, getString(msg.what));
                    break;
            }
        }

        private String getString(int what) {
            switch (what) {
                case MSG_CONNECTED_SI_REMOTE_SERVICE:
                    return "MSG_CONNECTED_SI_REMOTE_SERVICE";
                case MSG_DISCONNECTED_SI_REMOTE_SERVICE:
                    return "MSG_DISCONNECTED_SI_REMOTE_SERVICE";
                case MSG_READY_TO_START:
                    return "MSG_READY_TO_START";
                case MSG_START_CHANNEL_SCAN:
                    return "MSG_START_CHANNEL_SCAN";
                case MSG_STOP_CHANNEL_SCAN:
                    return "MSG_STOP_CHANNEL_SCAN";
                case MSG_PROGRESS_CHANNEL_SCAN:
                    return "MSG_PROGRESS_CHANNEL_SCAN";
                case MSG_COMPLETE_CHANNEL_SCAN:
                    return "MSG_COMPLETE_CHANNEL_SCAN";
                case MSG_UPDATE_CHANNEL_INFO:
                    return "MSG_UPDATE_CHANNEL_INFO";
                case MSG_UPDATE_PROGRAM_INFO:
                    return "MSG_UPDATE_PROGRAM_INFO";
                case MSG_CHANGE_PROGRAM:
                    return "MSG_CHANGE_PROGRAM";
                case MSG_CHANGE_CHANNEL:
                    return "MSG_CHANGE_CHANNEL";
                case MSG_LOCAL_AREACODE_CHANGED:
                    return "MSG_LOCAL_AREACODE_CHANGED";
                case MSG_DUMP_CHANNEL_INFO:
                    return "MSG_DUMP_CHANNEL_INFO";
                case MSG_DUMP_PROGRAM_INFO:
                    return "MSG_DUMP_PROGRAM_INFO";
                case MSG_START_VIDEO:
                    return "MSG_START_VIDEO";
                default:
                    return "not supported message=" + what;
            }
        }

    };

    private String dumpEventTime(Date fromDate, Date toDate) {
        Calendar cl = Calendar.getInstance();

        StringBuffer sb = new StringBuffer();
        cl.setTime(fromDate);
        // Month is 0 based so add 1
        sb.append(" " + cl.get(Calendar.YEAR) + "/" + (cl.get(Calendar.MONTH) + 1) + "/" + cl.get(Calendar.DATE));
        sb.append(" " + cl.get(Calendar.HOUR_OF_DAY) + ":" + cl.get(Calendar.MINUTE) + ":" + cl.get(Calendar.SECOND));
        sb.append(" ~");
        cl.setTime(toDate);
        // Month is 0 based so add 1
        sb.append(" " + cl.get(Calendar.YEAR) + "/" + (cl.get(Calendar.MONTH) + 1) + "/" + cl.get(Calendar.DATE));
        sb.append(" " + cl.get(Calendar.HOUR_OF_DAY) + ":" + cl.get(Calendar.MINUTE) + ":" + cl.get(Calendar.SECOND));
        return sb.toString();
    }

    private void dumpChannelInfo(int idx, ChannelInfo channel, boolean inDetail) {
        StringBuffer sb = new StringBuffer();
        if (inDetail) {
            sb.append("---------------------------\n");
            sb.append(" Channel Information\n");
            sb.append("---------------------------\n");
            sb.append(" uid         : " + channel.getChannelUid() + "\n");
            sb.append(" type        : " + channel.getChannelType() + "\n");
            sb.append(" id          : " + channel.getChannelId() + "/" + channel.getChannelCallsign() + "\n");
            sb.append(" name        : " + channel.getChannelNum() + "/" + channel.getChannelName() + "\n");
            sb.append(" uri         : " + channel.getChannelUri() + "\n");
            sb.append(" genre       : " + channel.getGenre() + "/" + channel.getCategory() + "\n");
            sb.append(" area        : " + channel.getLocalArea() + "\n");
            sb.append(" rating      : " + channel.getRating() + "\n");
            sb.append(" hd channel  : " + channel.isHDChannel() + "\n");
            sb.append(" image path  : " + channel.getChannelImagePath() + "\n");
            sb.append(" pay channel : " + channel.isPayChannel() + "\n");
            sb.append(" sample time : " + channel.getSampleTime() + "\n");
            sb.append(" ESInfo      : " + String.format("pcr=0x%04X", channel.getPcrPid()) + "\n");
            ArrayList<ESInfo> esInfoList = channel.getESList();
            if (esInfoList.size() > 0) {
                for (ESInfo esinfo : esInfoList) {
                    if (isVideoStream(esinfo.getStreamType())) {
                        sb.append(String.format("  %s=0x%04X(type=0x%04X)\n", "v", esinfo.getPid(), esinfo.getStreamType()));
                    } else if (isAudioStream(esinfo.getStreamType())) {
                        sb.append(String.format("  %s=0x%04X(type=0x%04X,lang=%s)\n", "a", esinfo.getPid(), esinfo.getStreamType(), esinfo.getLang()));
                    } else {
                        sb.append(String.format("  %s=0x%04X(type=0x%04X)\n", "?", esinfo.getPid(), esinfo.getStreamType()));
                    }
                }
            }
            sb.append(" CAInfo      : ");
            ArrayList<CAInfo> caInfoList = channel.getCAList();
            if (caInfoList.size() > 0) {
                for (CAInfo cainfo : caInfoList) {
                    int i = 0;
                    if (i++ > 0)
                        sb.append(",");
                    sb.append(String.format("0x%04X(pid=0x%04X)", cainfo.getCaSystemId(), cainfo.getCaPid()));
                }
            }
            sb.append("\n");
            sb.append(" ProductId   : ");
            ArrayList<String> prdtIdList = channel.getProductIdList();
            if (prdtIdList.size() > 0) {
                sb.append("\n  id=");
                int i = 0;
                for (String prdtId : prdtIdList) {
                    if (i++ > 0)
                        sb.append(",");
                    sb.append(prdtId.toString());
                }
            }
            sb.append("\n");
            sb.append("---------------------------\n");
        } else {
            sb.append("rsl=\"" + (channel.isHDChannel() ? "HD" : "SD") + "\" ");
            sb.append("uid=\"" + channel.getChannelUid() + "\" ");
            sb.append("name=\"" + channel.getChannelNum() + "/" + channel.getChannelName() + "\" ");
            sb.append("uri=\"" + channel.getChannelUri() + "\"\n");
        }
        Log.d(TAG, sb.toString());
    }

    private void dumpProgramInfo(int chidx, int prgmidx, ProgramInfo program, boolean inDetail) {
        StringBuffer sb = new StringBuffer();
        if (inDetail) {
            sb.append("---------------------------\n");
            sb.append(" Program Information\n");
            sb.append("---------------------------\n");
            sb.append(" uid         : " + program.getChannelUid() + "\n");
            sb.append(" time        : " + dumpEventTime(program.getStartTime(), program.getEndTime()) + "\n");
            sb.append(" duration(s) : " + program.getDuration() + "\n");
            sb.append(" name        : " + program.getProgramId() + "/" + program.getProgramName() + "\n");
            sb.append(" short desc  : " + program.getProgramShortDescription() + "\n");
            sb.append(" ext desc    : " + program.getProgramDescription() + "\n");
            sb.append(" rating      : " + program.getRating() + "\n");
            sb.append(" genre/catego: " + program.getContentNibble1() + "/" + program.getContentNibble2() + "\n");
            sb.append(" season/episo: " + program.getSeason() + "/" + program.getEpisodeNo() + "\n");
            sb.append(" image path  : " + program.getProgramImagePath() + "\n");
            sb.append(" audio type  : " + program.getAudioType() + "\n");
            sb.append(" price       : " + program.getPrice() + "\n");
            sb.append(" director    : " + program.getDirector() + "\n");
            sb.append(" actors      : " + program.getActors() + "\n");
            sb.append(" rsl         : " + program.isHDProgram() + "\n");
            sb.append(" dolby audio : " + program.isDolbyAudio() + "\n");
            sb.append(" caption     : " + program.isCaption() + "\n");
            sb.append(" dvs         : " + program.isDvs() + "\n");
            //sb.append("---------------------------\n");
        } else {
            sb.append("series=\"" + program.getSeason() + "/" + program.getEpisodeNo() + "\" ");
            sb.append("uid=\"" + program.getChannelUid() + "\" ");
            sb.append("time=\"" + dumpEventTime(program.getStartTime(), program.getEndTime()) + "\" ");
            sb.append("name=\"" + program.getProgramId() + "/" + program.getProgramName() + "\"\n");
        }
        Log.d(TAG, sb.toString());
    }

    private void dumpPrgmLinkInfo(ProgramInfo program, boolean inDetail) {
        if (inDetail) {
            StringBuffer sb = new StringBuffer();
            ArrayList<ProgramLinkInfo> prgmLinkInfoList = program.getProgramLinkInfoList();
            if (prgmLinkInfoList.size() > 0) {
                sb.append(" LinkInfo    : " + "\n");
                for (ProgramLinkInfo prgmLinkInfo : prgmLinkInfoList) {
                    sb.append("  service type="
                            + String.format("0x%02X(Dec:%d)", prgmLinkInfo.getLinkedServiceType(), prgmLinkInfo.getLinkedServiceType()) + "\n");
                    sb.append("  service text=" + prgmLinkInfo.getLinkedServiceText() + "\n");
                    sb.append("  button type =" + String.format("0x%02X(Dec:%d)", prgmLinkInfo.getButtonType(), prgmLinkInfo.getButtonType()) + "\n");
                    sb.append("  button path =" + prgmLinkInfo.getButtonImagePath() + "\n");
                    sb.append("  display time=" + dumpEventTime(prgmLinkInfo.getDisplayStartTime(), prgmLinkInfo.getDisplayEndTime()) + "\n");
                    sb.append("  cmenu      =" + prgmLinkInfo.getCMenuValue() + "\n");
                    sb.append("  vas item id  =" + prgmLinkInfo.getVasItemId() + "\n");
                    sb.append("  vas path    =" + prgmLinkInfo.getVasPath() + "\n");
                    sb.append("  vas service id= " + prgmLinkInfo.getVasServiceId() + "\n");
                }
            }
            sb.append("---------------------------\n");
            Log.d(TAG, sb.toString());
        }
    }

    private boolean isVideoStream(int streamType) {
        switch (streamType) {
        /* Video */
            case IESInfo.CONST_STREAM_TYPE_MPEG1Video:
            case IESInfo.CONST_STREAM_TYPE_MPEG2Video:
            case IESInfo.CONST_STREAM_TYPE_MPEG4Video:
            case IESInfo.CONST_STREAM_TYPE_H264Video:
                return true;
            default:
                break;
        }
        return false;
    }

    private boolean isAudioStream(int streamType) {
        switch (streamType) {
        /* Audio */
            case IESInfo.CONST_STREAM_TYPE_MPEG1Audio:
            case IESInfo.CONST_STREAM_TYPE_MPEG2Audio:
            case IESInfo.CONST_STREAM_TYPE_DVBAC3Audio:
            case IESInfo.CONST_STREAM_TYPE_AACAudio:
            case IESInfo.CONST_STREAM_TYPE_AC3Audio:
                return true;
            default:
                break;
        }
        return false;
    }

    private Date convertStringToDate(String time) {
        if (time == null) {
            return new Date();
        }
        // yyMMddhhmm
        //String[] tokens = time.split("^[0-9][0-9]&");

        // calendar
        int year = 2000 + Integer.parseInt(time.substring(0, 2)/* tokens[0] */); // after 2000
        int month = Integer.parseInt(time.substring(2, 4)/* tokens[1] */);
        int date = Integer.parseInt(time.substring(4, 6)/* tokens[2] */);
        // time
        int hourOfDay = Integer.parseInt(time.substring(6, 8)/* tokens[3] */);
        int minute = Integer.parseInt(time.substring(8)/* tokens[4] */);
        int second = 0;

        // set Time : Month is 0 based so delete 1
        Calendar cal = Calendar.getInstance();
        cal.set(year, month - 1, date, hourOfDay, minute, second);
        return cal.getTime(); // new GregorianCalendar(year, month - 1, date, hourOfDay, minute, second).getTime();
    }

    private String[] convertStringToStringArray(String str, String regularExpression) {
        if (str == null || regularExpression == null) {
            return new String[0]; // ReturnEmptyArrayRatherThanNull
        }
        if (str.isEmpty() || regularExpression.isEmpty()) {
            return new String[0]; // ReturnEmptyArrayRatherThanNull
        }

        String[] tokens = str.split(regularExpression);
        return tokens;
    }

    private String convertChNumToChUid(String chNum) {
        if (chNum == null || chNum.isEmpty()) {
            return "";
        }

        String token = "";
        if (mChannelList.containsKey(chNum)) {
            token = mChannelList.get(chNum).getChannelUid();
        }

        return token;
    }

    private String[] convertChNumToChUid(String[] chNums) {
        if (chNums == null || chNums.length == 0) {
            return new String[0]; // ReturnEmptyArrayRatherThanNull
        }

        int i = 0;
        String[] tokens = new String[chNums.length];
        for (String chNum : chNums) {
            if (chNum == null || chNum.isEmpty()) {
                continue;
            }
            if (mChannelList.containsKey(chNum)) {
                tokens[i++] = mChannelList.get(chNum).getChannelUid();
            } else {
                tokens[i++] = IChannelInfo.CONST_DEFAULT_CHANNEL_UID; // default channel_uid(0.0.0)
            }
        }

        if (i != chNums.length) {
            Log.w(TAG, "invalid length(" + i + "/" + chNums.length + ")");
        }

        return tokens;
    }

    private static String getScanMode(int mode) {
        switch (mode) {
            case CONST_SCANMODE_POSTEL_AUTO:
                return "POSTEL_AUTO";
            case CONST_SCANMODE_POSTEL_MANUAL:
                return "POSTEL_MANUAL";
            case CONST_SCANMODE_LGEQA_AUTO:
                return "LGEQA_AUTO";
            case CONST_SCANMODE_LGEQA_MANUAL:
                return "LGEQA_MANUAL";
            case CONST_SCANMODE_MANUAL_CHANNEL:
                return "MANUAL_CHANNEL";
            case CONST_SCANMODE_CHANNEL:
                return "CHANNEL";

            default:
                return "not supported mode=" + mode;
        }
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_CHANNEL_UP) {
            if (siService == null || mIsConnected == false) {
                return true;
            }
            //tvStatusTextView.setText("onClick CH+ Button");
            ChannelInfo[] list = currentChannelList();

            if (list != null) {
                int next = 1;
                if (mChIndex > 0) {
                    next = mChIndex == 1 ? 28 : 1;
                }

                mChIndex = next;
                startVideo(list[mChIndex].getChannelUri());
                //mHandler.sendMessage(mHandler.obtainMessage(MSG_START_VIDEO));
            }
        }
        return true;
    }

} // end of class
