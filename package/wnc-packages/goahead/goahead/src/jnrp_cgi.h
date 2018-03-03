#ifndef  JNRP_CGI_H
#define  JNRP_CGI_H

/* includes */

#include    "../uemf.h"
#include    "../wsIntrn.h"
#include    "../um.h"

//#include "jnrp_cgi_sql.h"
#include "jnrp_sql_lib.h"
// define
#define EXPORT_FILE     "/sysconfig/jnr-cfg.ascii"
#define IMPORT_DB_FILE  "/tmp/system.db"
// TODO: use export lib
#define EXPORT_CMD      "export_db -a /tmp/system.db /sysconfig/jnr-cfg.ascii; sync; sleep 1"
#define FACT_RESET_CMD  "/usr/local/bin/factory_reset.sh"

#define MAINMENU_FILE   "/usr/local/www/mainmenu.asp"
#define NAT_MAINMENU_FILE   "/usr/local/www/mainnatmenu.asp"
#define TOPMENU_FILE    "/usr/local/www/topmenu.asp"
#define LEFT_LTE_MENU_FILE      "/usr/local/www/leftLteMenu.asp"
#define LEFT_LAN_MENU_FILE      "/usr/local/www/leftLanMenu.asp"
#define LEFT_ADV_NAT_MENU_FILE      "/usr/local/www/leftAdvNatMenu.asp"
#define LEFT_WIFI_MENU_FILE      "/usr/local/www/leftWiFiMenu.asp"
#define LEFT_SYS_MENU_FILE      "/usr/local/www/leftSysMenu.asp"
#define LEFT_VOIP_MENU_FILE      "/usr/local/www/leftVoipMenu.asp"
#define LEFT_APP_MENU_FILE      "/usr/local/www/leftAppMenu.asp"
#define LEFT_ADV_MENU_FILE		"/usr/local/www/leftAdvMenu.asp"
#define LEFT_LAN_NAT_MENU_FILE      "/usr/local/www/leftLanNatMenu.asp"
#define LEFT_WIFI_AC_MENU_FILE      "/usr/local/www/leftWiFiACMenu.asp"
#define LEFT_WIFI_AC_BRIDGE_MENU_FILE      "/usr/local/www/leftWiFiAcBridgeMenu.asp"

//Poland WebUI skin --start
#ifdef __P02003_Dora
#define MAINMENU_CP_FILE   "/usr/local/www/mainmenu_cp.asp"
#define MAINMENU_PLK_FILE   "/usr/local/www/mainmenu_plk.asp"
#define NAT_MAINMENU_CP_FILE   "/usr/local/www/mainnatmenu_cp.asp"
#define NAT_MAINMENU_PLK_FILE   "/usr/local/www/mainnatmenu_plk.asp"
#endif
//Poland WebUI skin --end

typedef struct {
        /* Web ASP selected option value index */
        int value; // minute of timezone offset
        /* Web ASP selected option value index content */
        char_t *str;
} CGI_ENUMSEL_S;
// timezone table
static CGI_ENUMSEL_S web_Enum_TimeZoneList[] = {
#if defined(__P02003_Dora)
	{0,    "(GMT) Greenwich Mean Time : Edinburgh, London"},
    {-720, "(GMT-12:00) Eniwetok, Kwajalein"},
    {-660, "(GMT-11:00) Midway Island, Samoa"},
    {-600, "(GMT-10:00) Hawaii"},
    {-570, "(GMT-09:30) Marquesas Is"},
    {-540, "(GMT-09:00) Alaska"},
    {-480, "(GMT-08:00) Pacific Time(US); Tijuana"},
    {-420, "(GMT-07:00) Mountain Time, Arizona(US)"},
    {-360, "(GMT-06:00) MexicoCity(US),CentralTime(USA)"},
    {-300, "(GMT-05:00) EasternTime(USA), Lima, Indiana East"},
    {-270, "(GMT-04:30) Caracas"},
    {-240, "(GMT-04:00) Atlantic Time(Halifax Canada)"},
    {-210, "(GMT-03:30) Newfoundland"},
    {-180, "(GMT-03:00) Buenos Aires, Brasilia"},
    {-120, "(GMT-02:00) Mid-Atlantic"},
    {-60,  "(GMT-01:00) Azores, Cape Verde Is."},
    {+60,  "(GMT+01:00) Europe"},
    {+120, "(GMT+02:00) Athens, Istanbul, Minsk"},
    {+180, "(GMT+03:00) Baghdad, Kuwait, Moscow"},
    {+240, "(GMT+04:00) Abu Dhabi, Muscat"},
    {+270, "(GMT+04:30) Kabul"},
    {+300, "(GMT+05:00) Ekaterinburg, Islamabad, Karachi"},
    {+330, "(GMT+05:30) Bombay, Calcutta, Madras, Delhi"},
    {+360, "(GMT+06:00) Colombo"},
    {+390, "(GMT+06:30) Burma"},
    {+420, "(GMT+07:00) Bangkok, Hanoi, Jakarta"},
    {+480, "(GMT+08:00) Beijing, Chongqing, Hong Kong"},
    {+540, "(GMT+09:00) Osaka, Sapporo, Tokyo, Seoul"},
    {+570, "(GMT+09:30) Adelaide, Darwin"},
    {+600, "(GMT+10:00) Brisbane, Guam, Port Moresby"},
    {+630, "(GMT+10:30) Lord Howe Is."},
    {+660, "(GMT+11:00) Magadan, Solomon Is., New Caledonia"},
    {+690, "(GMT+11:30) Kingston(Norfolk Is)"},
    {+720, "(GMT+12:00) Auckland, Wellington, New Zealand, Fiji"},
    {+780, "(GMT+13:00) Tonga"},
    {+840, "(GMT+14:00) Kiribati, Western Samoa"}
#else
	{-720,"(GMT-12:00) Eniwetok, Kwajalein, International Date Line West"},
    {-660, "(GMT-11:00) Midway Island, Samoa"},
    {-600, "(GMT-10:00) Hawaii"},
    {-540, "(GMT-09:00) Alaska"},
    {-480, "(GMT-08:00) Pacific Time (US and Canada); Tijuana"},
    {-420, "(GMT-07:00) Arizona, Mountain Time (US and Canada), Chihuahua"},
    {-360,
     "(GMT-06:00) Central America, Central Time (US and Canada), Mexico City, Saskatchewan"},
    {-300,
     "(GMT-05:00) Bogota, Lima, Quito, Eastern Time (US and Canada), Indiana (East)"},
    {-270, "(GMT-04:30) Caracas"},
    {-240, "(GMT-04:00) Atlantic Time (Canada), Caracas, La Paz, Santiago"},
    {-210, "(GMT-03:30) Newfoundland"},
    {-180,
     "(GMT-03:00) Brasilia, Buenos Aires, Georgetown, Greenland, Montevideo"},
    {-120, "(GMT-02:00) Mid-Atlantic"},
    {-60, "(GMT-01:00) Azores, Cape Verde Is."},
    {0,
     "(GMT) Casablanca, Monrovia, Greenwich Mean Time: Dublin, Edinburgh, Lisbon, London"},
    {+60,
     "(GMT+01:00) Amsterdam, Berlin, Rome, Stockholm, Vienna, Prague, Brussels, Madrid, Paris"},
    {+120,
     "(GMT+02:00) Athens, Istanbul, Bucharest, Cairo, Harare, Pretoria, Helsinki, Jerusalem"},
    {+180,
     "(GMT+03:00) Baghdad, Kuwait, Riyadh, Moscow, St. Petersburg, Volgograd, Nairobi"},
    {+210, "(GMT+03:30) Tehran"},
    {+240,
     "(GMT+04:00) Abu Dhabi, Muscat, Baku, Tbilisi, Yerevan, Port Louis"},
    {+270, "(GMT+04:30) Kabul"},
    {+300, "(GMT+05:00) Ekaterinburg, Islamabad, Karachi, Tashkent"},
    {+330,
     "(GMT+05:30) Calcutta, Chennai, Mumbai, New Delhi, Sri Jayawardenepura"},
    {+345, "(GMT+05:45) Kathmandu"},
    {+360, "(GMT+06:00) Almaty, Novosibirsk, Astana, Dhaka"},
    {+390, "(GMT+06:30) Yangon (Rangoon)"},
    {+420, "(GMT+07:00) Bangkok, Hanoi, Jakarta, Krasnoyarsk"},
    {+480,
     "(GMT+08:00) Beijing, Hong Kong, Ulaan Bataar, Kuala Lumpur, Singapore, Perth, Taipei"},
    {+540, "(GMT+09:00) Osaka, Sapporo, Tokyo, Seoul, Yakutsk"},
    {+570, "(GMT+09:30) Adelaide, Darwin"},
    {+600,
     "(GMT+10:00) Brisbane, Canberra, Melbourne, Sydney, Guam, Hobart, Vladivostok"},
    {+660, "(GMT+11:00) Magadan, Solomon Is., New Caledonia"},
    {+720,
     "(GMT+12:00) Auckland, Wellington, Fiji, Kamchatka, Marshall Is."},
    {+780, "(GMT+13:00) Nuku'alofa"}

#endif
};

typedef struct {
	int value; 		// number of band offset, /* Web ASP band value index */
	char_t *str;	// string of band display, /* Web ASP band value index content */
} CGI_EBAND_P;
// Band transfer table
static CGI_EBAND_P web_Enum_BandValueList[] = {
	{0,  "BC 0"},     /*WNC_MAL_ACTIVE_BAND_BC_0*/
	{1,  "BC 1"},     /*WNC_MAL_ACTIVE_BAND_BC_1*/		
	{3,  "BC 3"},     /*WNC_MAL_ACTIVE_BAND_BC_3*/
	{4,  "BC 4"},     /*WNC_MAL_ACTIVE_BAND_BC_4*/
	{5,  "BC 5"},     /*WNC_MAL_ACTIVE_BAND_BC_5*/
	{6,  "BC 6"},     /*WNC_MAL_ACTIVE_BAND_BC_6*/
	{7,  "BC 7"},     /*WNC_MAL_ACTIVE_BAND_BC_7*/
	{8,  "BC 8"},     /*WNC_MAL_ACTIVE_BAND_BC_8*/
	{9,  "BC 9"},     /*WNC_MAL_ACTIVE_BAND_BC_9*/
	{10, "BC 10"},    /*WNC_MAL_ACTIVE_BAND_BC_10*/
	{11, "BC 11"},    /*WNC_MAL_ACTIVE_BAND_BC_11*/
	{12, "BC 12"},    /*WNC_MAL_ACTIVE_BAND_BC_12*/
	{13, "BC 13"},    /*WNC_MAL_ACTIVE_BAND_BC_13*/
	{14, "BC 14"},    /*WNC_MAL_ACTIVE_BAND_BC_14*/
	{15, "BC 15"},    /*WNC_MAL_ACTIVE_BAND_BC_15*/
	{16, "BC 16"},    /*WNC_MAL_ACTIVE_BAND_BC_16*/
	{17, "BC 17"},    /*WNC_MAL_ACTIVE_BAND_BC_17*/
	{18, "BC 18"},    /*WNC_MAL_ACTIVE_BAND_BC_18*/
	{19, "BC 19"}, 	  /*WNC_MAL_ACTIVE_BAND_BC_19*/
	{40, "GSM 450"},    		/*WNC_MAL_ACTIVE_BAND_GSM_450*/
	{41, "GSM 480"},    		/*WNC_MAL_ACTIVE_BAND_GSM_480*/
	{42, "GSM 750"},    		/*WNC_MAL_ACTIVE_BAND_GSM_750*/
	{43, "GSM 850"},    		/*WNC_MAL_ACTIVE_BAND_GSM_850*/
	{44, "GSM 900 Extended"},   /*WNC_MAL_ACTIVE_BAND_GSM_900_EXTENDED*/
	{45, "GSM 900 Primary"},    /*WNC_MAL_ACTIVE_BAND_GSM_900_PRIMARY*/
	{46, "GSM 900 Railways"},   /*WNC_MAL_ACTIVE_BAND_GSM_900_RAILWAYS*/
	{47, "GSM 1800"},   		/*WNC_MAL_ACTIVE_BAND_GSM_1800*/
	{48, "GSM 1900"}, 			/*WNC_MAL_ACTIVE_BAND_GSM_1900*/
	{80, "WCDMA 2100"},    		/*WNC_MAL_ACTIVE_BAND_WCDMA_2100*/
	{81, "WCDMA PCS 1900"},    	/*WNC_MAL_ACTIVE_BAND_WCDMA_PCS_1900*/
	{82, "WCDMA DCS 1800"},    	/*WNC_MAL_ACTIVE_BAND_WCDMA_DCS_1800*/
	{83, "WCDMA 1700 US"},    	/*WNC_MAL_ACTIVE_BAND_WCDMA_1700_US*/
	{84, "WCDMA 850"},          /*WNC_MAL_ACTIVE_BAND_WCDMA_850*/
	{85, "WCDMA 800"},          /*WNC_MAL_ACTIVE_BAND_WCDMA_800*/
	{86, "WCDMA 2600"},         /*WNC_MAL_ACTIVE_BAND_WCDMA_2600*/
	{87, "WCDMA 900 Japan"},   	/*WNC_MAL_ACTIVE_BAND_WCDMA_900*/
	{88, "WCDMA 1700 Japan"}, 	/*WNC_MAL_ACTIVE_BAND_WCDMA_1700_JAPAN*/
	{90, "WCDMA 1500 Japan"},   /*WNC_MAL_ACTIVE_BAND_WCDMA_1500_JAPAN*/
	{91, "WCDMA 850 Japan"}, 	/*WNC_MAL_ACTIVE_BAND_WCDMA_850_JAPAN*/	
	{120, "Band 1"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_1*/
	{121, "Band 2"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_2*/		
	{122, "Band 3"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_3*/
	{123, "Band 4"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_4*/
	{124, "Band 5"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_5*/
	{125, "Band 6"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_6*/
	{126, "Band 7"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_7*/
	{127, "Band 8"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_8*/
	{128, "Band 9"},  /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_9*/
	{129, "Band 10"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_10*/
	{130, "Band 11"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_11*/
	{131, "Band 12"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_12*/
	{132, "Band 13"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_13*/
	{133, "Band 14"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_14*/
	{134, "Band 17"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_134*/
	{135, "Band 33"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_135*/
	{136, "Band 34"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_136*/
	{137, "Band 35"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_137*/
	{138, "Band 36"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_138*/
	{139, "Band 37"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_139*/
	{140, "Band 38"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_140*/
	{141, "Band 39"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_141*/
	{142, "Band 40"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_142*/
	{143, "Band 18"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_143*/
	{144, "Band 19"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_144*/
	{145, "Band 20"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_145*/
	{146, "Band 21"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_146*/
	{147, "Band 24"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_147*/
	{148, "Band 25"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_148*/
	{149, "Band 41"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_149*/
	{150, "Band 42"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_150*/
	{151, "Band 43"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_151*/
	{152, "Band 23"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_152*/
	{153, "Band 26"}, /*WNC_MAL_ACTIVE_BAND_E_UTRA_OPERATING_BAND_153*/
	{200, "TDSCDMA Band A"},  /*WNC_MAL_ACTIVE_BAND_TDSCDMA_BAND_A*/
	{201, "TDSCDMA Band B"},  /*WNC_MAL_ACTIVE_BAND_TDSCDMA_BAND_B*/
	{202, "TDSCDMA Band C"},  /*WNC_MAL_ACTIVE_BAND_TDSCDMA_BAND_C*/
	{203, "TDSCDMA Band D"},  /*WNC_MAL_ACTIVE_BAND_TDSCDMA_BAND_D*/
	{204, "TDSCDMA Band E"},  /*WNC_MAL_ACTIVE_BAND_TDSCDMA_BAND_E*/
	{205, "TDSCDMA Band F"}   /*WNC_MAL_ACTIVE_BAND_TDSCDMA_BAND_F*/
};

enum
{
	LOGIN_DEFAULT=0,
	LOGIN_USERNAME_ERROR=1,
	LOGIN_PASSWORD_ERROR=2,
	LOGIN_NO_USERNAME=3,
	LOGIN_NO_PASSWORD=4,
	LOGIN_OK=5
};

// use DB trigger script to update relative tables
// need DB trgger script
//#define __USED_DB_TRIGGER__

// for goahead main function
extern void jnrpWebInit (char * dbfile);
const static char * pSep = "#";
const static char * pSep2 = ",";
char * pDBFile;

#ifdef __P02003_Dora
char edit_name[128];
#endif

#ifdef __USED_WEBS_DB__
static sqlite3 * pWebDb = NULL;
#endif
static SQLITE_BUSY_HANDLER_ARG   bhWebArg = {"", DB_MAX_RETRIES, NUM_MILLIS_TO_SLEEP};

// for utilities
extern long get_uptime();
extern char * getSplitStr(char * pSources, char * pSep, int nIdx);
extern char * getSplitStr2(char * pSources, char * pSep, int nIdx);


extern void initHomeWeb();
extern void initInternetWeb();
extern void initLanWeb();
extern void initVoipWeb();
extern void initApplicationWeb();

extern char * getIpAddress(char * pDBFile);

#define COMMAND_MAX	1024
#define SYSTEM_COMMAND_LOG "/var/system_command.log"

//Poland WebUI skin --start
#if defined(__P02003_Dora)
char g_sim_imsi[64];
extern void updateIMSIToDB();
#endif
//Poland WebUI skin --end

#endif
