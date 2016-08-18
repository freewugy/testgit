/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: ksh78 $
 * $LastChangedDate: 2014-06-06 15:26:59 +0900 (금, 06 6월 2014) $
 * $LastChangedRevision: 862 $
 * Description:
 * Note:
 *****************************************************************************/

#include "TDI.h"
#include "Logger.h"
#include "OhPlayerClientSIWrapper.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

using namespace tvstorm;

void playerLock();
void playerUnlock();

TDI_TunerCallback pcallbackfn = NULL;
extern tvstorm::OhPlayerClientSIWrapper* mOhPlayerClient;
int gCurrentFreqKHz = 0;
int gCurrentTunerId = 0;
int gCurrentChannel = 0;

bool checkTuner(std::string uri)
{
        char str[20];
        sprintf(str, "tuner%d", gCurrentTunerId);
        if(std::string::npos == uri.find(str)) {
                return false;
        }
        return true;
}

bool checkRec(std::string uri)
{
        char str[20];
        sprintf(str, "rc=1");
        if(std::string::npos == uri.find(str)) {
                return true;
        }
        return false;
}
bool checkSI(std::string uri)
{
        char str[20];
        sprintf(str, "sc=1");
        if(std::string::npos == uri.find(str)) {
                return false;
        }
        return true;
}
TDI_Error TDI_Demux_Reset();
void TDI_TVSMediaServerCallback(int aMsg, int aArg1, int aArg2, const char* aData)
{
        L_TEST("aMsg=%d aArg1=%d aArg2=%d aData=%s\n", aMsg, aArg1, aArg2, (char*)aData);

        if(aMsg == MEDIA_SERVICE_DIED) { // MEDIA_SERVICE_DIED = 400
                L_TEST("[SCAN ERROR] media service died : \n");
                playerLock();
#if 0
                if(mOhPlayerClient) {
                        L_TEST("begin_OhPlayerClient_close\n");
                        delete mOhPlayerClient;
                        L_TEST("end_OhPlayerClient_close\n");
                        mOhPlayerClient = NULL;
                }
#endif
                L_TEST("begin_OhPlayerClient_open\n");
                mOhPlayerClient = new OhPlayerClientSIWrapper;
                L_TEST("end_OhPlayerClient_open\n");
                if(mOhPlayerClient) {
                        L_TEST("begin_OhPlayerClient_setMediaServerCallback\n");
                        mOhPlayerClient->setMediaServerCallback(TDI_TVSMediaServerCallback);
                        L_TEST("end_OhPlayerClient_setMediaServerCallback\n");
                }
                playerUnlock();
                return;
        }
        if(aData != NULL) {
                std::string uri = (char*) aData;
                if(checkTuner(uri) == false)
                        return;
                if(checkRec(uri) == false)
                        return;
        }

        if(pcallbackfn) {
                if(aMsg == MEDIA_PLAY_STOPPED) { // MEDIA_PLAY_STOPPED = 300
                        TDI_Demux_Reset();
                        pcallbackfn(0, TDI_TUNER_EVENT_REQ_TUNE, -1, 0);
                } else if(aMsg == MEDIA_PLAY_STARTED) { // MEDIA_PLAY_STARTED = 301
                        gCurrentFreqKHz = aArg1;
                        gCurrentChannel = aArg2;
                } else if(aMsg == MEDIA_TUNER_LOCKED) { // MEDIA_TUNER_LOCKED = 330
                        gCurrentFreqKHz = aArg1;
                        //gCurrentChannel = aArg2;
                        pcallbackfn(0, TDI_TUNER_EVENT_SUCCESS_TUNE, gCurrentChannel, gCurrentFreqKHz);
                } else if(aMsg == MEDIA_TUNER_LOCK_FAILED) { // MEDIA_TUNER_LOCK_FAILED = 331
                        pcallbackfn(0, TDI_TUNER_EVENT_FAIL_TUNE, -1, gCurrentFreqKHz);
                } else if(aMsg == MEDIA_TUNER_SIGNAL_LOST) { // MEDIA_TUNER_SIGNAL_LOST = 332
                        if(aData != NULL) {
                                std::string uri = (char*) aData;
                                if(checkSI(uri)) {
                                        pcallbackfn(0, TDI_TUNER_EVENT_FAIL_TUNE, -1, gCurrentFreqKHz);
                                }
                        }

                }
#if 0
                else if(aMsg == MEDIA_TUNER_SIGNAL_ACQUIRE) { // MEDIA_TUNER_SIGNAL_ACQUIRE = 333
                        if(aData != NULL) {
                                std::string uri = (char*) aData;
                                if(checkSI(uri)) {
                                        pcallbackfn(0, TDI_TUNER_EVENT_SUCCESS_TUNE, gCurrentChannel, gCurrentFreqKHz);
                                }
                        }
                }
#endif
        } else {
                L_INFO("Tuner Callback is NULL\n");
        }
}

void transTunerParams(TDI_TunerSettings *pSettings, char* retUri)
{
#if 0	
        if(pSettings->type == TDI_TunerType_Terrestrial ||
                        pSettings->type == TDI_TunerType_Cable )
        {
                tunerParam->SourceType = TVS_SOURCE_TYPE_TUNER;
                tunerParam->FreqKHz = pSettings->parameter.rf.frequencyHz/1000;
                tunerParam->SymbolRate = pSettings->parameter.rf.symbolrate;
                tunerParam->BandWidth = (TVS_DVBT_BW)pSettings->parameter.rf.bandwidth;
        }
        else if(pSettings->type == TDI_TunerType_Satellite)
        {
                tunerParam->SourceType = TVS_SOURCE_TYPE_TUNER;
                tunerParam->FreqKHz = pSettings->parameter.sat.FreqKHz;
                tunerParam->SymbolRate = pSettings->parameter.sat.SymbolRate;
                tunerParam->TransSpec = (TVSTORM::TVS_TRANSPORT_SPEC) pSettings->parameter.sat.TransSpec;
                tunerParam->Rolloff = (TVSTORM::TVS_TUNER_SAT_ROLLOFF) pSettings->parameter.sat.Rolloff;
                tunerParam->Fec = (TVSTORM::TVS_TUNER_FEC_RATE) pSettings->parameter.sat.Fec;
                tunerParam->bPilot = pSettings->parameter.sat.bPilot;
                tunerParam->bSpectrum = pSettings->parameter.sat.bSpectrum;
                tunerParam->bScrambling = pSettings->parameter.sat.bScrambling;
                tunerParam->Polarization = (TVSTORM::TVS_TUNER_LNB_POLARIZATION) pSettings->parameter.sat.Polarization;
                tunerParam->bLLCEnabled = pSettings->parameter.sat.bLLCEnabled;
                tunerParam->bTone22khz = pSettings->parameter.sat.bTone22khz;
                tunerParam->Lnbmode = (TVSTORM::TVS_TUNER_LNB_MODE) pSettings->parameter.sat.Lnbmode;
                tunerParam->Lnb_preset = (TVSTORM::TVS_LNB_PRESET) pSettings->parameter.sat.Lnb_preset;

                tunerParam->Lnb_data.Type = (TVSTORM::TVS_DEMOD_LNB_TYPE)pSettings->parameter.sat.Lnb_data.Type;
                tunerParam->Lnb_data.Horizontal = (TVSTORM::TVS_DEMOD_LNB_VOLTAGE)pSettings->parameter.sat.Lnb_data.Horizontal;
                tunerParam->Lnb_data.Left = (TVSTORM::TVS_DEMOD_LNB_VOLTAGE)pSettings->parameter.sat.Lnb_data.Left;
                tunerParam->Lnb_data.Right = (TVSTORM::TVS_DEMOD_LNB_VOLTAGE)pSettings->parameter.sat.Lnb_data.Right;
                tunerParam->Lnb_data.Vertical = (TVSTORM::TVS_DEMOD_LNB_VOLTAGE)pSettings->parameter.sat.Lnb_data.Vertical;
                tunerParam->Lnb_data.uNumLNBs = pSettings->parameter.sat.Lnb_data.uNumLNBs;

                for(int i=0; i<TVS_MAX_LNBS; i++)
                {
                        tunerParam->Lnb_data.LNBSettings[i].uFreqMaxKHz = pSettings->parameter.sat.Lnb_data.LNBSettings[i].uFreqMaxKHz;
                        tunerParam->Lnb_data.LNBSettings[i].uLNB = pSettings->parameter.sat.Lnb_data.LNBSettings[i].uLNB;
                        tunerParam->Lnb_data.LNBSettings[i].iOrbitalPos = pSettings->parameter.sat.Lnb_data.LNBSettings[i].iOrbitalPos;
                        tunerParam->Lnb_data.LNBSettings[i].bOrbital22khz = pSettings->parameter.sat.Lnb_data.LNBSettings[i].bOrbital22khz;
                }

                tunerParam->SwitchData.bEnableSignalling = pSettings->parameter.sat.SwitchData.bEnableSignalling;
                tunerParam->SwitchData.uNumSwitchInputs = pSettings->parameter.sat.SwitchData.uNumSwitchInputs;
                for(int i=0; i<TVS_MAX_DISEQC_SWITCH_INPUTS; i++)
                {
                        tunerParam->SwitchData.SwitchInput[i].bInUse = pSettings->parameter.sat.SwitchData.SwitchInput[i].bInUse;
                        tunerParam->SwitchData.SwitchInput[i].nOrbitalPosition = pSettings->parameter.sat.SwitchData.SwitchInput[i].nOrbitalPosition;
                        tunerParam->SwitchData.SwitchInput[i].BurstType = (TVSTORM::TVS_TUNER_DISEQC_BURST_TYPE)pSettings->parameter.sat.SwitchData.SwitchInput[i].BurstType;
                }

                tunerParam->SwitchExData.uID10numSwitchInputs = pSettings->parameter.sat.SwitchExData.uID10numSwitchInputs;
                tunerParam->SwitchExData.uID11numSwitchInputs = pSettings->parameter.sat.SwitchExData.uID11numSwitchInputs;
                tunerParam->SwitchExData.BurstType = (TVSTORM::TVS_TUNER_DISEQC_BURST_TYPE)pSettings->parameter.sat.SwitchExData.BurstType;
                tunerParam->SwitchExData.uMotor_type = pSettings->parameter.sat.SwitchExData.uMotor_type;
                tunerParam->SwitchExData.uMotor_position = pSettings->parameter.sat.SwitchExData.uMotor_position;
                tunerParam->SwitchExData.uLongit.sat_longit = pSettings->parameter.sat.SwitchExData.uLongit.sat_longit;
                tunerParam->SwitchExData.uLongit.iLocalLongitude = pSettings->parameter.sat.SwitchExData.uLongit.iLocalLongitude;
                tunerParam->SwitchExData.uLongit.iLocalLatitude = pSettings->parameter.sat.SwitchExData.uLongit.iLocalLatitude;
        }
        else if(pSettings->type == TDI_TunerType_Ip)
        {
                tunerParam->SourceType = TVS_SOURCE_TYPE_IPTV;
        }

        if(pSettings->protocol == TDI_TunerProtocol_VSB)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_VSB;
        }
        else if(pSettings->protocol ==TDI_TunerProtocol_16QAM)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_QAM16;
        }
        else if(pSettings->protocol ==TDI_TunerProtocol_64QAM)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_QAM64;
        }
        else if(pSettings->protocol ==TDI_TunerProtocol_256QAM)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_QAM256;
        }
        else if(pSettings->protocol ==TDI_TunerProtocol_QPSK)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_QPSK;
        }
        else if(pSettings->protocol ==TDI_TUnerProrocol_8PSK)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_8PSK;
        }
        else if(pSettings->protocol ==TDI_TunerProtocol_DVBT)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_DVBT;
        }
        else if(pSettings->protocol ==TDI_TunerProtocol_DVBT2)
        {
                tunerParam->DemodType = TVS_DEMOD_TYPE_DVBT2;
        }

        tunerParam->AudioFormat = TVS_AUDIO_FORMAT_UNKNOWN;
        tunerParam->VideoFormat = TVS_VIDEO_FORMAT_UNKNOWN;
        tunerParam->AudioPid = 0x1fff;
        tunerParam->VideoPid = 0x1fff;
        tunerParam->PcrPid = 0x1fff;
        tunerParam->bIsPip = false;
#else
        if(pSettings->type == TDI_TunerType_Terrestrial || pSettings->type == TDI_TunerType_Cable) {
                if(pSettings->protocol == TDI_TunerProtocol_64QAM) {
                        sprintf(retUri, "tuner%d://qam64:%d?sr=%d&bw=%d&sc=1", pSettings->tunerId, pSettings->parameter.rf.frequencyHz / 1000,
                                        pSettings->parameter.rf.symbolrate / 1000, pSettings->parameter.rf.bandwidth);
                } else if(pSettings->protocol == TDI_TunerProtocol_128QAM) {
                        sprintf(retUri, "tuner%d://qam128:%d?sr=%d&bw=%d&sc=1", pSettings->tunerId, pSettings->parameter.rf.frequencyHz / 1000,
                                        pSettings->parameter.rf.symbolrate / 1000, pSettings->parameter.rf.bandwidth);
                } else if(pSettings->protocol == TDI_TunerProtocol_256QAM) {
                        sprintf(retUri, "tuner%d://qam256:%d?sr=%d&bw=%d&sc=1", pSettings->tunerId, pSettings->parameter.rf.frequencyHz / 1000,
                                        pSettings->parameter.rf.symbolrate / 1000, pSettings->parameter.rf.bandwidth);
                } else if(pSettings->protocol == TDI_TunerProtocol_16QAM) {
                        sprintf(retUri, "tuner%d://qam16:%d?sr=%d&bw=%d&sc=1", pSettings->tunerId, pSettings->parameter.rf.frequencyHz / 1000,
                                        pSettings->parameter.rf.symbolrate / 1000, pSettings->parameter.rf.bandwidth);
                } else if(pSettings->protocol == TDI_TunerProtocol_32QAM) {
                        sprintf(retUri, "tuner%d://qam32:%d?sr=%d&bw=%d&sc=1", pSettings->tunerId, pSettings->parameter.rf.frequencyHz / 1000,
                                        pSettings->parameter.rf.symbolrate / 1000, pSettings->parameter.rf.bandwidth);
                }
                gCurrentFreqKHz = pSettings->parameter.rf.frequencyHz / 1000;

                L_INFO("%s\n",retUri);
        }
#endif	
}

TDI_Error TDI_Tuner_Open(int *pTunerId)
{
        TDI_Error err = TDI_SUCCESS;

        playerLock();
        if(mOhPlayerClient == NULL) {
                TDI_System_Open(false);
        }
        playerUnlock();

        return err;
}

TDI_Error TDI_Tuner_Close(int tunerId)
{
        TDI_Error err = TDI_SUCCESS;

        return err;
}

TDI_Error TDI_Tuner_SetCallback(IN int tunerId, IN TDI_TunerCallback cbFunc)
{
        TDI_Error err = TDI_SUCCESS;

        pcallbackfn = cbFunc;
        playerLock();
        if(mOhPlayerClient) {
                L_TEST("begin_OhPlayerClient_setMediaServerCallback\n");
                mOhPlayerClient->setMediaServerCallback(TDI_TVSMediaServerCallback);
                L_TEST("end_OhPlayerClient_setMediaServerCallback\n");
        }
        playerUnlock();
        return err;
}

TDI_Error TDI_Tuner_UnsetCallback(IN int tunerId)
{
        TDI_Error err = TDI_SUCCESS;
        L_TEST("TDI_Tuner_UnsetCallback\n");
        return err;
}

static bool tunerequest = false;
TDI_Error TDI_Tuner_SetSettings(int tunerId, TDI_TunerSettings *pSettings)
{
        if(tunerequest == true) {
                return TDI_SUCCESS;
        }
        tunerequest = true;

        TDI_Error err = TDI_SUCCESS;
        int bsuccess = -1;

        playerLock();
        if(mOhPlayerClient == NULL) {
                TDI_System_Open(false);
        }
        playerUnlock();

        if(pSettings->type == TDI_TunerType_Ip) {
                char tunestr[200];
                sprintf(tunestr, "rtp://%s:%d?vp=62&vc=27&ap=63&ac=129&pp=62", pSettings->parameter.ip.ip, pSettings->parameter.ip.port);
                playerLock();
                if(mOhPlayerClient) {
                        bsuccess = mOhPlayerClient->startTune(tunestr);
                }
                playerUnlock();
        } else {
                pSettings->tunerId = tunerId;
                gCurrentTunerId = tunerId;
                char tunestr[200];
                transTunerParams(pSettings, tunestr);
                L_INFO("%s\n",tunestr);
                playerLock();
                if(mOhPlayerClient) {
                        L_TEST("begin_OhPlayerClient_startTune(%s)\n", tunestr);
                        bsuccess = mOhPlayerClient->startTune(tunestr);
                        L_TEST("end_OhPlayerClient_startTune(%s) bsuccess=%d\n", tunestr, bsuccess);
                }
                playerUnlock();
        }

        return err;
}

TDI_Error TDI_Tuner_Stop(int tunerId)
{
        L_INFO("=========IN=============\n");
        TDI_Error err = TDI_SUCCESS;

        if(tunerequest == false) {
                return TDI_SUCCESS;
        }
        tunerequest = false;

        playerLock();
        if(mOhPlayerClient) {
                L_TEST("begin_OhPlayerClient_stopTune\n");
                mOhPlayerClient->stopTune();
                L_TEST("end_OhPlayerClient_stopTune\n");
        }
        playerUnlock();

        return err;
}

TDI_Error TDI_Tuner_GetSettings(IN int tunerId, OUT TDI_TunerSettings *pSettings)
{
        TDI_Error err = TDI_SUCCESS;

        return err;
}

TDI_TunerState TDI_Tuner_GetState(IN int tunerId)
{
        TDI_TunerState ret = TDI_TUNER_UNKNOWN;

        return ret;
}

TDI_TunerType TDI_Tuner_GetType(IN int tunerId)
{
        TDI_TunerType ret = TDI_TunerType_Ip;

        return ret;
}

TDI_TunerProtocol TDI_Tuner_GetProtocol(IN int tunerId)
{
        TDI_TunerProtocol ret = TDI_TunerProtocol_Udp;

        return ret;
}

int TDI_Tuner_GetSNR(int tunerId)
{
        return 0;
}

int TDI_Tuner_GetPower(int tunerId)
{
        return 0;
}

int getCurrentFreqKHz()
{
        return gCurrentFreqKHz;
}
