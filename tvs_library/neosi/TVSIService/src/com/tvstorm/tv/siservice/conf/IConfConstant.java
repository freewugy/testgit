/*****************************************************************************
 * Copyright (c) TVStorm, Inc. http://www.tvstorm.com. All rights reserved.
 * TVSTORM PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 *
 * $LastChangedBy: eschoi $
 * $LastChangedDate: 2014-03-25 15:31:58 +0900 (화, 25 3월 2014) $
 * $LastChangedRevision: 11720 $
 * Description:
 * Note:
 *****************************************************************************/

package com.tvstorm.tv.siservice.conf;

public interface IConfConstant {
    // Intent
    public static final String BIND_INTENT_ACTION = "com.tvstorm.tv.siservice";

    // Directory
    public static final String DATA_HOME_DIRECTORY = "/data/lkn_home";

    public static final String DATA_CONFIG_DIRECTORY = DATA_HOME_DIRECTORY + "/config";

    public static final String DATA_RUN_DIRECTORY = DATA_HOME_DIRECTORY + "/run";

    public static final String CONFIG_SI_DIRECTORY = DATA_CONFIG_DIRECTORY + "/si";

    public static final String RUN_SI_DIRECTORY = DATA_RUN_DIRECTORY + "/si";

    /**
     * EPGUri
     */
    // provider name
    public static final String EPG_PROVIDER_NAME_LINKNET = "lkn";

    public static final String EPG_PROVIDER_NAME_LINKNET_IPTV = "lki";
    
    public static final String EPG_PROVIDER_NAME_SKB = "skb";

    // demod
    public static final String DEMOD_VSB8 = "vsb8";

    public static final String DEMOD_QAM16 = "qam16";

    public static final String DEMOD_QAM32 = "qam32";

    public static final String DEMOD_QAM64 = "qam64";

    public static final String DEMOD_QAM128 = "qam128";

    public static final String DEMOD_QAM256 = "qam256";

    public static final String AP_IP = "i";

    public static final String AP_TERRESTRIAL = "t";

    public static final String AP_CABLE = "c";

    public static final String AP_SATELITE = "s";

    public static final String AP_FILE = "f";

} // end of class
