#define _CRT_SECURE_NO_WARNINGS

/*
TODO: 
- Localization 
- Perfomance optimization 
- Clean da fooking coderino 
- Build in Handle Leaker rather than external process. 
*/

#include <Windows.h>
#include <iostream>
#include <Psapi.h>
#include <memory>
#include <vector>
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <fstream>
#include <WinInet.h>
#include <Iphlpapi.h>
#include <tuple>
#include <chrono>
#include <stdio.h>
#include <list>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "tinyxml2.h"

#include "XorStr.h"
#include "Offsets.h"
#include "Globals.h"
#include "Memory.h"
#include "SDK.h"
#include "CRender.h"
#include "Utils.h"
#include "LocalPlayer.h"

#include "Strings.h"
#include "Config.h"

#include "Crypto.h"

#include "menu\imgui.h"
#include "menu\imgui_impl_dx9.h"

#include "InputSystem.h"

#include "VMProtectSDK.h"

#pragma comment(lib, "wininet.lib")
#pragma comment( lib, "psapi" )
#pragma comment(lib, "iphlpapi.lib")

#define LOADERCODE "ASD"
#define VERSION "v1.2"
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
using namespace std::chrono;

std::wstring GameName = L"TslGame.exe";///*TslGame.exe*/XorStr<0x0A, 12, 0x2FC22B61>("\x5E\x78\x60\x4A\x6F\x62\x75\x3F\x77\x6B\x71" + 0x2FC22B61).s;

//std::vector<std::string> CachedGNames(200000); // Big size
//std::vector<std::wstring> CachedLootNames(200000); // Big size
std::vector<std::string> CachedPlayerNames(1700); // Determine the max PlayerID for this

std::vector<std::wstring> Weapons = { L"M16A4", L"R1895", L"SCAR-L", L"AKM", L"M416", L"P1911", L"M249", L"Micro UZI", L"P92", L"Kar98k", L"UMP9", L"S1897", L"S686", L"Vector", L"P18C", L"SKS", L"VSS", L"Mini" }; // L"S12K"
bool bWeapon[20];

std::vector<std::wstring> Attachements = { L"Suppressor", L"Flash Hider", L"Shotgun Choke", L"Ext. QuickDraw Mag", L"Vertical Foregrip", L"Angled Foregrip", L"Bullet Loops", L"Cheek Pad ", L"Pan", L"Compensator" };
bool bAttachements[30];

std::vector<std::wstring> Ammo = { L"12 Gauge", L".45 ACP", L"5.56mm", L"7.62mm", L"9mm" };
bool bAmmo[6];

std::vector<std::wstring> Medical = { L"First Aid Kit", L"Bandage", L"Med Kit", L"Painkiller", L"Energy Drink" };
bool bMedical[6];

std::vector<std::wstring> Scopes = { L"4x Scope", L"2x Scope", L"Red Dot Sight", L"Holographic Sight", L"8x Scope" };
bool bScopes[6];

FCameraCacheEntry pLocalCameraCahceEntry;

CRender* g_pRender;
CMaths* g_pMaths = new CMaths();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

MARGINS margin; 

DWORD_PTR GNamesOffset;
DWORD_PTR UWorldOffSet;
DWORD_PTR gNamesOffSet;
DWORD_PTR GWorldAddress;
DWORD dUWorldOffset;

sLocalPlayer g_pLocalPlayer;

Config config;

ATslPlayerState LocalPlayerState;
AActor LocalPlayer;

DWORD_PTR pEntityList; 
int iCount;

std::ofstream namesDump;
std::ofstream lootDump;

bool bDrawESP = true;

high_resolution_clock::time_point RenderEnd; 
high_resolution_clock::time_point RenderBegin;

std::string username, password;

std::string g_hwid;
std::string HDSerial;
std::string MacAddress;

bool bVerified = false;

std::string requestUpdate();

Crypto crypto;

char* repHTTPS;

std::vector<std::string> Lootfilters;

int Diffx = 0, Diffy = 0;

InputSystem* g_pInputSystem = new InputSystem();

bool DrawMenu = false;

std::vector<char*> CustomLoot_Items;
int  CustomLoot_SelectedIndex = 1;

char aimkey[256];
char buff[1024];
Vector LocalPlayerPosition;
const char* Fonts[] = { "Tahoma", "Comic Sans MS", "Impact", "Wingdings", "Verdana" };

MSG msg;

Cache g_pCache;

Vector AimLocation;

int mousex, mousey;
bool keys[256];
BYTE KeyStates[256];

int TemScreenW = 0, TempScreenH = 0;

bool bFoundTarget = false;

int gcd(int a, int b) {
	return b == 0 ? a : gcd(b, a % b);
}

void verify()
{
	std::string reps = requestUpdate();

	if (reps.find("-") != std::string::npos)
	{
		bVerified = false;
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "LOL NOPE", NULL, NULL);
		Utils::Selfdestruct();
		ExitProcess(0);
	}
	if (reps.find("*") != std::string::npos)
	{
		bVerified = false;
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "Invalid login information", NULL, NULL);
		ExitProcess(0);
	}
	if (reps.find("%") != std::string::npos)
	{
		bVerified = false;
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "Unknown ERROR", NULL, NULL);
		ExitProcess(0);
	}
	if (reps.find(")(") != std::string::npos) 
	{
		bVerified = false;
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "No Subscription", NULL, NULL);
		ExitProcess(0);
	}

	if (reps.find("()") != std::string::npos)
	{
		bVerified = true;
	} else {
		bVerified = false;
	}

}

void SaveSettings()
{
	tinyxml2::XMLDocument cfg;
	cfg.LoadFile("config.cfg");

	std::string AllSettings;

	AllSettings += "<config>";

	//Visuals
	{
		AllSettings += "<visuals>";
			AllSettings += "<d1st>" + std::to_string(config.bDistanceESP) + "</d1st>";
			AllSettings += "<l1ne>" + std::to_string(config.bLineESP) + "</l1ne>";
			AllSettings += "<h3lath>" + std::to_string(config.bPlayerHealthESP) + "</h3lath>";
			AllSettings += "<nam3>" + std::to_string(config.bPlayerNameESP) + "</nam3>";
			AllSettings += "<RadA1>" + std::to_string(config.bRadarESP) + "</RadA1>";
			AllSettings += "<Sh0wL00t>" + std::to_string(config.bShowLoot) + "</Sh0wL00t>";
			AllSettings += "<V3h1cl3>" + std::to_string(config.bVehicleESP) + "</V3h1cl3>";

			AllSettings += "<D0wned>" + std::to_string(config.bDowned) + "</D0wned>";
			//Downed Options
			{
				AllSettings += "<D0wnedOptions>";
					AllSettings += "<DownedDistance>" + std::to_string(config.bDownedDistance) + "</DownedDistance>";
					AllSettings += "<DownedHP>" + std::to_string(config.bDownedHP) + "</DownedHP>";
					AllSettings += "<DownedTimeToDie>" + std::to_string(config.bDownedTimeToDie) + "</DownedTimeToDie>";
				AllSettings += "</D0wnedOptions>";
			}

			AllSettings += "<ShowLocalMarker>" + std::to_string(config.bShowLocalMarker) + "</ShowLocalMarker>";
			AllSettings += "<ShowTeamMarker>" + std::to_string(config.bShowTeamMarker) + "</ShowTeamMarker>";
			AllSettings += "<Sk31eton>" + std::to_string(config.bSkeletonESP) + "</Sk31eton>";
			AllSettings += "<Box>" + std::to_string(config.BoxESP) + "</Box>";
			AllSettings += "<boxtype>" + std::to_string(config.BoxType) + "</boxtype>";
			AllSettings += "<drawbones>" + std::to_string(config.bDrawBones) + "</drawbones>";
			AllSettings += "<AirDrop>" + std::to_string(config.bAirDrop) + "</AirDrop>";
			AllSettings += "<l00t>" + std::to_string(config.bLootESP) + "</l00t>";
			AllSettings += "<l00tdist>" + std::to_string(config.bLootDistanceESP) + "</l00tdist>";

			//Loot options
			{
				AllSettings += "<l00toptions>";

					AllSettings += "<wepsoptions>";
						for(auto i = 0; i < Weapons.size(); i++)
						{
							std::string wepname = "wep"; 
							wepname += std::to_string(i);
							AllSettings += "<" + wepname + ">" + std::to_string(bWeapon[i]) + "</" + wepname + ">";
						}
					AllSettings += "</wepsoptions>";

					AllSettings += "<attachoptions>";
						for (auto i = 0; i < Attachements.size(); i++)
						{
							std::string attach = "attach";
							attach += std::to_string(i);
							AllSettings += "<" + attach + ">" + std::to_string(bAttachements[i]) + "</" + attach + ">";
						}
					AllSettings += "</attachoptions>";

					AllSettings += "<ammooptions>";
						for (int i = 0; i < Ammo.size(); i++)
						{
							std::string ammo = "Ammo";
							ammo += std::to_string(i);
							AllSettings += "<" + ammo + ">" + std::to_string(bAmmo[i]) + "</" + ammo + ">";
						}
					AllSettings += "</ammooptions>";

					AllSettings += "<medicaloptions>";
						for (int i = 0; i < Medical.size(); i++)
						{
							std::string medicals = "Medical";
							medicals += std::to_string(i);
							AllSettings += "<" + medicals + ">" + std::to_string(bMedical[i]) + "</" + medicals + ">";
						}
					AllSettings += "</medicaloptions>";

					AllSettings += "<scopesoptions>";
					for (int i = 0; i < Scopes.size(); i++)
					{
						std::string Scope = "Scopes";
						Scope += std::to_string(i);
						AllSettings += "<" + Scope + ">" + std::to_string(bScopes[i]) + "</" + Scope + ">";
					}
					AllSettings += "</scopesoptions>";

					AllSettings += "<nofilter>" + std::to_string(config.bLootFilterNone) + "</nofilter>";
					AllSettings += "<L1filter>" + std::to_string(config.bLootFilterL1) + "</L1filter>";
					AllSettings += "<L2ilter>" + std::to_string(config.bLootFilterL2) + "</L2ilter>";
					AllSettings += "<L3ilter>" + std::to_string(config.bLootFilterL3) + "</L3ilter>";
					AllSettings += "<wepsilter>" + std::to_string(config.bLootFilterGuns) + "</wepsilter>";
					AllSettings += "<attachilter>" + std::to_string(config.bLootFilterAttachments) + "</attachilter>";
					AllSettings += "<ammoilter>" + std::to_string(config.bAmmoLoot) + "</ammoilter>";
					AllSettings += "<medicalilter>" + std::to_string(config.bMedicalLoot) + "</medicalilter>";
					AllSettings += "<scopesilter>" + std::to_string(config.bScopesLoot) + "</scopesilter>";
					AllSettings += "<customilter>" + std::to_string(config.bCustomFilter) + "</customilter>";
					
				AllSettings += "</l00toptions>";
			}

		AllSettings += "</visuals>";
	}

	//Removals
	{
		AllSettings += "<removals>";
			AllSettings += "<norec>" + std::to_string(config.bNoRecoil) + "</norec>";
			AllSettings += "<nosway>" + std::to_string(config.bNoSway) + "</nosway>";
			AllSettings += "<FastFireRate>" + std::to_string(config.bFastFireRate) + "</FastFireRate>";
			AllSettings += "<FastBu11et>" + std::to_string(config.bFastBullets) + "</FastBu11et>";
			AllSettings += "<muzzlez>" + std::to_string(config.bNoMuzzle) + "</muzzlez>";
			AllSettings += "<nspread>" + std::to_string(config.bNoSpread) + "</nspread>";
		AllSettings += "</removals>";
	}

	//Aimbot
	{
		AllSettings += "<a1mbotoptions>";
			AllSettings += "<A1mb0t>" + std::to_string(config.bAimbot) + "</A1mb0t>";
			AllSettings += "<A1mb0tf0v>" + std::to_string(config.FOV) + "</A1mb0tf0v>";
			AllSettings += "<drawfov>" + std::to_string(config.bDrawFOV) + "</drawfov>";
			AllSettings += "<A1mb0tsm00th>" + std::to_string(config.Smooth) + "</A1mb0tsm00th>";
			AllSettings += "<A1mb0tkey>" + config.aimkey + "</A1mb0tkey>";
			AllSettings += "<A1mb0tvecpred>" + std::to_string(config.bVelocityPrediction) + "</A1mb0tvecpred>";
			AllSettings += "<a1mbone>" + std::to_string(config.aimbotBone) + "</a1mbone>";

			AllSettings += "<a1mlockshow>" + std::to_string(config.ShowLockIndication) + "</a1mlockshow>";

			{
				AllSettings += "<lockoptions>";
					AllSettings += "<a1mbonevis>" + std::to_string(config.ShowAimLockBone) + "</a1mbonevis>";
					AllSettings += "<a1mlockshowType>" + std::to_string(config.LockIndicationType) + "</a1mlockshowType>";
					AllSettings += "<a1imlocktarget>" + std::to_string(config.locktarget) + "</a1imlocktarget>";
					AllSettings += "<a1mlocktextcol>" + Utils::saveColor(config.AimbotLockColour) + "</a1mlocktextcol>";
					AllSettings += "<a1mlockbonecol>" + Utils::saveColor(config.AimbotLockBoneColour) + "</a1mlockbonecol>";
					AllSettings += "<a1mlockbonerad>" + std::to_string(config.BoneLockrad) + "</a1mlockbonerad>";
				AllSettings += "</lockoptions>";
			}
		AllSettings += "</a1mbotoptions>";
	}

	//Coloured Options
	{
		AllSettings += "<colourOptions>";
			AllSettings += "<SkelCol>" + Utils::saveColor(config.SkelColorIMGUI) + "</SkelCol>";
			AllSettings += "<boxCol>" + Utils::saveColor(config.BoxColorIMGUI) + "</boxCol>";
			AllSettings += "<skelwidth>" + std::to_string(config.SkeletonWidth) + "</skelwidth>";
			AllSettings += "<boxwidth>" + std::to_string(config.BoxWidth) + "</boxwidth>";

			AllSettings += "<dist280>" + Utils::saveColor(config.Distance280IMGUI) + "</dist280>";
			AllSettings += "<dist560>" + Utils::saveColor(config.Distance560IMGUI) + "</dist560>";
			AllSettings += "<distm560>" + Utils::saveColor(config.DistanceMorethan560IMGUI) + "</distm560>";

			AllSettings += "<fovcol>" + Utils::saveColor(config.DrawFOVIMGUI) + "</fovcol>";

			AllSettings += "<l00tdef>" + Utils::saveColor(config.LootDefaultColor) + "</l00tdef>";
			AllSettings += "<l00tl1>" + Utils::saveColor(config.LootL1) + "</l00tl1>";
			AllSettings += "<l00tl2>" + Utils::saveColor(config.LootL2) + "</l00tl2>";
			AllSettings += "<l00tl3>" + Utils::saveColor(config.LootL3) + "</l00tl3>";
			AllSettings += "<l00tGuns>" + Utils::saveColor(config.LootGuns) + "</l00tGuns>";
			AllSettings += "<l00tAttachements>" + Utils::saveColor(config.LootAttachements) + "</l00tAttachements>";
			AllSettings += "<l00tMedical>" + Utils::saveColor(config.LootMedical) + "</l00tMedical>";
			AllSettings += "<l00tAmmo>" + Utils::saveColor(config.LootAmmo) + "</l00tAmmo>";
			AllSettings += "<l00tScopes>" + Utils::saveColor(config.LootScopes) + "</l00tScopes>";
			AllSettings += "<l00tCustom>" + Utils::saveColor(config.LootCustom) + "</l00tCustom>";

		AllSettings += "</colourOptions>";
	}

	//Text Options
	{
		AllSettings += "<TextOptions>";
			AllSettings += "<Multiplier>" + std::to_string(config.TextMultiplier) + "</Multiplier>";
			AllSettings += "<Font>" + std::to_string(config.TextFont) + "</Font>";
			AllSettings += "<Outline>" + std::to_string(config.TextOutile) + "</Outline>";
		AllSettings += "</TextOptions>";
	}

	//Login Options
	{
		AllSettings += "<LoginOptions>";
			AllSettings += "<autologin>" + std::to_string(config.bAutoLogin) + "</autologin>";
			AllSettings += "<username>" + config.username + "</username>";
			AllSettings += "<password>" + config.password + "</password>";
		AllSettings += "</LoginOptions>";
	}

	//Debug Options
	{
		AllSettings += "<DebugOptions>";
			AllSettings += "<debug>" + std::to_string(config.bDebugMode) + "</debug>";
			AllSettings += "<l0g1ds>" + std::to_string(config.bLogIds) + "</l0g1ds>";
		AllSettings += "</DebugOptions>";
	}

	//Misc Options
	{
		AllSettings += "<MiscOptions>";
			AllSettings += "<AutoScale>" + std::to_string(config.AutoScale) + "</AutoScale>";
			AllSettings += "<ScreenH>" + std::to_string(config.ScreenH) + "</ScreenH>";
			AllSettings += "<ScreenW>" + std::to_string(config.ScreenW) + "</ScreenW>";
			AllSettings += "<ovoffset>" + std::to_string(config.flOverlayOffset) + "</ovoffset>";
		AllSettings += "</MiscOptions>";
	}

	//Optimization
	{
		AllSettings += "<Optimization>";
			AllSettings += "<SleepRat3>" + std::to_string(config.iSleepRate) + "</SleepRat3>";
		AllSettings += "</Optimization>";
	}

	AllSettings += "</config>";

	cfg.Parse(AllSettings.c_str());

	cfg.SaveFile(std::string("config.cfg").c_str());
}

void LoadSettings()
{
	tinyxml2::XMLDocument cfg;
	cfg.LoadFile("config.cfg");

	tinyxml2::XMLElement* root = cfg.RootElement();
	if (!root)
		std::cout << "Failed to load config" << std::endl;

	//Visuals
	{
		tinyxml2::XMLElement* Visuals = root->FirstChildElement("visuals");

		config.bDistanceESP = Utils::toBool(Visuals->FirstChildElement("d1st")->GetText());
		config.bLineESP = Utils::toBool(Visuals->FirstChildElement("l1ne")->GetText());

		config.bPlayerHealthESP = Utils::toBool(Visuals->FirstChildElement("h3lath")->GetText());
		config.bPlayerNameESP = Utils::toBool(Visuals->FirstChildElement("nam3")->GetText());
		config.bRadarESP = Utils::toBool(Visuals->FirstChildElement("RadA1")->GetText());
		config.bShowLoot = Utils::toBool(Visuals->FirstChildElement("Sh0wL00t")->GetText());
		config.bVehicleESP = Utils::toBool(Visuals->FirstChildElement("V3h1cl3")->GetText());

		config.bDowned = Utils::toBool(Visuals->FirstChildElement("D0wned")->GetText());

		//Downed
		{
			tinyxml2::XMLElement* DownedOptions = Visuals->FirstChildElement("D0wnedOptions");

			config.bDownedDistance = Utils::toBool(DownedOptions->FirstChildElement("DownedDistance")->GetText());
			config.bDownedHP = Utils::toBool(DownedOptions->FirstChildElement("DownedHP")->GetText());
			config.bDownedTimeToDie = Utils::toBool(DownedOptions->FirstChildElement("DownedTimeToDie")->GetText());
		}

		config.bShowLocalMarker = Utils::toBool(Visuals->FirstChildElement("ShowLocalMarker")->GetText());
		config.bShowTeamMarker = Utils::toBool(Visuals->FirstChildElement("ShowTeamMarker")->GetText());
		config.bSkeletonESP = Utils::toBool(Visuals->FirstChildElement("Sk31eton")->GetText());
		config.BoxESP = Utils::toBool(Visuals->FirstChildElement("Box")->GetText());
		config.BoxType = std::stoi(Visuals->FirstChildElement("boxtype")->GetText());
		config.bDrawBones = Utils::toBool(Visuals->FirstChildElement("drawbones")->GetText());
		config.bAirDrop = Utils::toBool(Visuals->FirstChildElement("AirDrop")->GetText());

		// LOOT
		config.bLootESP = Utils::toBool(Visuals->FirstChildElement("l00t")->GetText());
		config.bLootDistanceESP = Utils::toBool(Visuals->FirstChildElement("l00tdist")->GetText());

		//Loot options
		{
			tinyxml2::XMLElement* LootOptions = Visuals->FirstChildElement("l00toptions");

			tinyxml2::XMLElement* weps = LootOptions->FirstChildElement("wepsoptions");
			for (auto i = 0; i < Weapons.size(); i++)
			{
				std::string wepname = "wep";
				wepname += std::to_string(i);
				bWeapon[i]  = Utils::toBool(weps->FirstChildElement(wepname.c_str())->GetText());
			}

			tinyxml2::XMLElement* attach = LootOptions->FirstChildElement("attachoptions");
			for (auto i = 0; i < Attachements.size(); i++)
			{
				std::string AttachSName = "attach";
				AttachSName += std::to_string(i);
				bAttachements[i] = Utils::toBool(attach->FirstChildElement(AttachSName.c_str())->GetText());
			}

			tinyxml2::XMLElement* ammochild = LootOptions->FirstChildElement("ammooptions");
			for (auto i = 0; i < Ammo.size(); i++)
			{
				std::string sAmmoName = "Ammo";
				sAmmoName += std::to_string(i);
				bAmmo[i] = Utils::toBool(ammochild->FirstChildElement(sAmmoName.c_str())->GetText());
			}

			tinyxml2::XMLElement* medicalchild = LootOptions->FirstChildElement("medicaloptions");
			for (auto i = 0; i < Medical.size(); i++)
			{
				std::string sMedicals = "Medical";
				sMedicals += std::to_string(i);
				bMedical[i] = Utils::toBool(medicalchild->FirstChildElement(sMedicals.c_str())->GetText());
			}

			tinyxml2::XMLElement* scopeschild = LootOptions->FirstChildElement("scopesoptions");
			for (auto i = 0; i < Scopes.size(); i++)
			{
				std::string sScopes = "Scopes";
				sScopes += std::to_string(i);
				bScopes[i] = Utils::toBool(scopeschild->FirstChildElement(sScopes.c_str())->GetText());
			}

			config.bLootFilterNone = Utils::toBool(LootOptions->FirstChildElement("nofilter")->GetText());
			config.bLootFilterL1 = Utils::toBool(LootOptions->FirstChildElement("L1filter")->GetText());
			config.bLootFilterL2 = Utils::toBool(LootOptions->FirstChildElement("L2ilter")->GetText());
			config.bLootFilterL3 = Utils::toBool(LootOptions->FirstChildElement("L3ilter")->GetText());
			config.bLootFilterGuns = Utils::toBool(LootOptions->FirstChildElement("wepsilter")->GetText());
			config.bLootFilterAttachments = Utils::toBool(LootOptions->FirstChildElement("attachilter")->GetText());
			config.bAmmoLoot = Utils::toBool(LootOptions->FirstChildElement("ammoilter")->GetText());
			config.bMedicalLoot = Utils::toBool(LootOptions->FirstChildElement("medicalilter")->GetText());
			config.bScopesLoot = Utils::toBool(LootOptions->FirstChildElement("scopesilter")->GetText());
			config.bCustomFilter = Utils::toBool(LootOptions->FirstChildElement("customilter")->GetText());
		}
	}

	//Text Options
	{
		tinyxml2::XMLElement* TextOptions = root->FirstChildElement("TextOptions");

		config.TextMultiplier = std::stof(TextOptions->FirstChildElement("Multiplier")->GetText());
		config.TextFont = std::stof(TextOptions->FirstChildElement("Font")->GetText());
		config.TextOutile = Utils::toBool(TextOptions->FirstChildElement("Outline")->GetText());
	}

	//Removals
	{
		tinyxml2::XMLElement* Removals = root->FirstChildElement("removals");

		config.bNoRecoil = Utils::toBool(Removals->FirstChildElement("norec")->GetText());
		config.bNoSway = Utils::toBool(Removals->FirstChildElement("nosway")->GetText());
		config.bFastFireRate = Utils::toBool(Removals->FirstChildElement("FastFireRate")->GetText());
		config.bFastBullets = Utils::toBool(Removals->FirstChildElement("FastBu11et")->GetText());
		config.bNoMuzzle = Utils::toBool(Removals->FirstChildElement("muzzlez")->GetText());
		config.bNoSpread = Utils::toBool(Removals->FirstChildElement("nspread")->GetText());
	}
	
	//Aimbot
	{
		tinyxml2::XMLElement* AimbotOptions = root->FirstChildElement("a1mbotoptions");

		config.bAimbot = Utils::toBool(AimbotOptions->FirstChildElement("A1mb0t")->GetText());
		config.FOV = std::stof(AimbotOptions->FirstChildElement("A1mb0tf0v")->GetText());
		config.bDrawFOV = Utils::toBool(AimbotOptions->FirstChildElement("drawfov")->GetText());
		config.Smooth = std::stof(AimbotOptions->FirstChildElement("A1mb0tsm00th")->GetText());
		config.bVelocityPrediction = AimbotOptions->FirstChildElement("A1mb0tvecpred")->GetText();
		config.aimbotBone = std::stoi(AimbotOptions->FirstChildElement("a1mbone")->GetText());

		config.aimkey = AimbotOptions->FirstChildElement("A1mb0tkey")->GetText();
		strcpy(aimkey, config.aimkey.c_str());

		config.ShowLockIndication = Utils::toBool(AimbotOptions->FirstChildElement("a1mlockshow")->GetText());
		{
			tinyxml2::XMLElement* LockOptions = AimbotOptions->FirstChildElement("lockoptions");

			config.ShowAimLockBone = Utils::toBool(LockOptions->FirstChildElement("a1mbonevis")->GetText());
			config.LockIndicationType = std::stoi(LockOptions->FirstChildElement("a1mlockshowType")->GetText());
			config.locktarget = Utils::toBool(LockOptions->FirstChildElement("a1imlocktarget")->GetText());

			config.AimbotLockColour = Utils::toColor(LockOptions->FirstChildElement("a1mlocktextcol")->GetText());
			config.AimbotLockBoneColour = Utils::toColor(LockOptions->FirstChildElement("a1mlockbonecol")->GetText());
			config.BoneLockrad = std::atof(LockOptions->FirstChildElement("a1mlockbonerad")->GetText());
		}
	}

	// Colors
	{
		tinyxml2::XMLElement* colourOptions = root->FirstChildElement("colourOptions");

		config.SkelColorIMGUI = Utils::toColor(colourOptions->FirstChildElement("SkelCol")->GetText());
		config.BoxColorIMGUI = Utils::toColor(colourOptions->FirstChildElement("boxCol")->GetText());
		config.SkeletonWidth = std::stoi(colourOptions->FirstChildElement("skelwidth")->GetText());
		config.BoxWidth = std::stoi(colourOptions->FirstChildElement("boxwidth")->GetText());

		config.Distance280IMGUI = Utils::toColor(colourOptions->FirstChildElement("dist280")->GetText());
		config.Distance560IMGUI = Utils::toColor(colourOptions->FirstChildElement("dist560")->GetText());
		config.DistanceMorethan560IMGUI = Utils::toColor(colourOptions->FirstChildElement("distm560")->GetText());

		config.DrawFOVIMGUI = Utils::toColor(colourOptions->FirstChildElement("fovcol")->GetText());

		config.LootDefaultColor = Utils::toColor(colourOptions->FirstChildElement("l00tdef")->GetText());
		config.LootL1 = Utils::toColor(colourOptions->FirstChildElement("l00tl1")->GetText());
		config.LootL2 = Utils::toColor(colourOptions->FirstChildElement("l00tl2")->GetText());
		config.LootL3 = Utils::toColor(colourOptions->FirstChildElement("l00tl3")->GetText());
		config.LootGuns = Utils::toColor(colourOptions->FirstChildElement("l00tGuns")->GetText());
		config.LootAttachements = Utils::toColor(colourOptions->FirstChildElement("l00tAttachements")->GetText());
		config.LootMedical = Utils::toColor(colourOptions->FirstChildElement("l00tMedical")->GetText());
		config.LootAmmo = Utils::toColor(colourOptions->FirstChildElement("l00tAmmo")->GetText());
		config.LootScopes = Utils::toColor(colourOptions->FirstChildElement("l00tScopes")->GetText());
		config.LootCustom = Utils::toColor(colourOptions->FirstChildElement("l00tCustom")->GetText());
	}

	//Login 
	{
		tinyxml2::XMLElement* LoginOptions = root->FirstChildElement("LoginOptions");

		config.bAutoLogin = Utils::toBool(LoginOptions->FirstChildElement("autologin")->GetText());
		config.username = LoginOptions->FirstChildElement("username")->GetText();
		config.password = LoginOptions->FirstChildElement("password")->GetText();
	}

	//Debug
	{
		tinyxml2::XMLElement* DebugOptions = root->FirstChildElement("DebugOptions");

		config.bDebugMode = Utils::toBool(DebugOptions->FirstChildElement("debug")->GetText());
		config.bLogIds = Utils::toBool(DebugOptions->FirstChildElement("l0g1ds")->GetText());
	}

	//Misc Options
	{
		tinyxml2::XMLElement* MiscOptions = root->FirstChildElement("MiscOptions");

		config.AutoScale = Utils::toBool(MiscOptions->FirstChildElement("AutoScale")->GetText());

		if (config.AutoScale)
		{
			Utils::GetDesktopResolution(config.ScreenW, config.ScreenH);
		}
		else
		{
			config.ScreenH = std::atoi(MiscOptions->FirstChildElement("ScreenH")->GetText());
			config.ScreenW = std::atoi(MiscOptions->FirstChildElement("ScreenW")->GetText());
		}

		g_pMaths->UpdateScreenResolution(config.ScreenH, config.ScreenW);

		int ScreenSizeW = 0, ScreenSizeH = 0;
		Utils::GetDesktopResolution(ScreenSizeW, ScreenSizeH);
		Diffx = ScreenSizeW - config.ScreenW;
		Diffy = ScreenSizeH - config.ScreenH;
		//margin = { ScreenSizeW / 2 - config.ScreenH / 2, ScreenSizeH / 2 - config.ScreenH / 2, config.ScreenW, config.ScreenH };
		margin = { 0, 0, config.ScreenW, config.ScreenH };

		config.flOverlayOffset = std::atoi(MiscOptions->FirstChildElement("ovoffset")->GetText());
	}

	//Optimization
	{
		tinyxml2::XMLElement* OptimizationOptions = root->FirstChildElement("Optimization");

		config.iSleepRate = atoi(OptimizationOptions->FirstChildElement("SleepRat3")->GetText());
	}
}

std::string getNameFromId(int ID) {
	DWORD64 fNamePtr = mem->Read<DWORD64>(GNamesOffset + (ID / 0x4000) * 8);
	DWORD64 fName = mem->Read<DWORD64>(fNamePtr + 8 * (ID % 0x4000));
	char name[64];
	ZeroMemory(name, sizeof(name));
	if (ReadProcessMemory(global->hProcess, (LPVOID)(fName + 0x10), name, sizeof(name) - 2, NULL) != 0)
	{
		return std::string(name);
	}
	else {
		return std::string("FAIL");
	}
}

int GetClassNameIndex(std::string _class) 
{
	for (int i = 0; i < 200000; i++)
	{
		std::string temp = getNameFromId(i);
		if (_class == temp)
		{
			return i;
			break;
		}
	}

	return NULL;
}

DWORD_PTR GetUWorld(DWORD_PTR GWorldAddress, DWORD_PTR gWorldOffset)
{
	DWORD_PTR uworldtemp = mem->Read<DWORD_PTR>(GWorldAddress + gWorldOffset); // Sig scan address + Ofset out come (Base + Uworld Offset)
	std::cout << std::hex << "uworldtemp " << uworldtemp << std::endl;
	
	DWORD_PTR owninggameinstance = mem->Read<DWORD_PTR>(uworldtemp + off_OwningGameInstance); // Owining Instanc
	std::cout << std::hex << "owninggameinstance " << owninggameinstance << std::endl;
	DWORD_PTR localplayerarray = mem->Read<DWORD_PTR>(owninggameinstance + off_ULocalPlayerArray); // Locl Player Array
	std::cout << std::hex << "localplayerarray " << localplayerarray << std::endl;
	global->pLocalPlayer = mem->Read<DWORD_PTR>(localplayerarray); // Local Player Pointer
	std::cout << std::hex << "global->pLocalPlayer  " << global->pLocalPlayer << std::endl;
	DWORD_PTR viewportclient = mem->Read<DWORD_PTR>(global->pLocalPlayer + off_ViewportClien); // ViewPortClient
	std::cout << std::hex << "viewportclient " << viewportclient << std::endl;
	
	return mem->Read<DWORD_PTR>(viewportclient + off_UWorld); // UWorld Pointer
}

std::string requestUpdate()
{
	VMProtectBeginUltra("requestUpdate");
	char* tempresp = (char*)Utils::DoPostRequest(
		L"p2cheat.comlawl", L"rip/restinpeperonis.php",
		std::string("szQvUG8x5gvC3bcB=") + crypto.base64_encode(username) + // Standard HWID
		std::string("&HF9LUtqzb4gW7WuP=") + crypto.base64_encode(password) + // Standard HWID
		std::string("&TXFSmteP6sUH7wgv=") + crypto.base64_encode(Utils::ws2s(Utils::GetPcName())) + // PC Name
		std::string("&J3tsbh6gx72xDM2s=") + crypto.base64_encode(LOADERCODE) + // Loader Code
		std::string("&PTrZTm9Rb8rXzD7r=") + crypto.base64_encode(HDSerial) + //HD Serial
		std::string("&YQNLGt3Yf3N6Q4Dt=") + crypto.base64_encode(std::string(MacAddress)) + //Mac Address
		std::string("&LeGrAqVQ2P88fGvw=") + crypto.base64_encode(VERSION) // Version
	);

	if (!tempresp)
	{
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "Was unable to verify information.\n Please try again.", NULL, NULL);
		ExitProcess(0);
	}
	else {
		std::string reps(tempresp);

		if (reps.size() > 0)
		{
			reps = reps.erase(reps.size() - 5);
		}

		return reps;
	}
	VMProtectEnd();
}

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	VMProtectBeginUltra("WindowProc");
	if (ImGui_ImplDX9_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	ImGuiIO& io = ImGui::GetIO();

	switch (message)
	{
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	VMProtectEnd();

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void exitCleanup()
{
	VMProtectBeginUltra("exitCleanup");
	g_pRender->Release();
	delete mem;
	ExitProcess(0);
	VMProtectEnd();
}

void TickSystem()
{
	VMProtectBeginUltra("TickSystem");
	while (true)
	{
		verify();

		if (bVerified == false)
		{
			exitCleanup();
		}

		Sleep(Utils::rangeRandomAlg2(290, 650) * 1000); //OUCH
	}
	VMProtectEnd();
	
}

void OtherStuffThread()
{
	while (true)
	{
		pEntityList = mem->Read<DWORD_PTR>((DWORD_PTR)global->pULevel + off_EntityList); // POINTER to entity list
		iCount = mem->Read<int>((DWORD_PTR)global->pULevel + off_EntityList + 0x8); // Array Size
		
		if(iCount > 7000)
		{
			iCount = 100;
		}

		LocalPlayer = mem->Read<AActor>((DWORD_PTR)mem->Read<DWORD_PTR>((DWORD_PTR)g_pLocalPlayer.LocalPlayerController + 0x0418));
		LocalPlayerState = LocalPlayer.GetPlayerState();
		pLocalCameraCahceEntry = mem->Read<FCameraCacheEntry>((DWORD_PTR)g_pLocalPlayer.pLocalPlayerCameraManager + off_CameraCache);

		if (GetAsyncKeyState(VK_ADD) & 1)
		{
			exitCleanup();
		}

		if (GetAsyncKeyState(VK_F5) & 1)
		{
			LoadSettings();
			std::cout << "R3load3d3d config" << std::endl;

			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F1) & 1)
		{
			bDrawESP = !bDrawESP;
			std::cout << "Dr5w: " << std::string(bDrawESP ? "Yeah" : "Nah") << std::endl;

			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F2) & 1)
		{
			config.bVehicleESP = !config.bVehicleESP;
			std::cout << "V3h1c13: " << std::string(config.bVehicleESP ? "Yeah" : "Nah") << std::endl;

			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F3) & 1)
		{
			config.bLootESP = !config.bLootESP;
			std::cout << "L000t: " << std::string(config.bLootESP ? "Yeah" : "Nah") << std::endl;

			Sleep(100);
		}

		if (GetAsyncKeyState(VK_F4) & 1)
		{
			config.bDebugMode = !config.bDebugMode;
			std::cout << "D3bug: " << std::string(config.bDebugMode ? "Yeah" : "Nah") << std::endl;

			Sleep(100);
		}

		if ((LocalPlayer.Health > 0 && LocalPlayer.Health <= 100) && LocalPlayer.BasePointer != NULL) // && LocalPlayerState.BasePointer != NULL
		{
			if (config.bNoRecoil || config.bNoSpread || config.bNoSway || config.bFastFireRate || config.bFastBullets)
			{
				if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0)
				{
					AWeaponProcessor lpWeaponProcessor = LocalPlayer.GetWeaponProcessor();

					if (lpWeaponProcessor.BasePointer != NULL && lpWeaponProcessor.EquippedWeapons != NULL)
					{
						if (lpWeaponProcessor.CurrentWeaponIndex >= 0 && lpWeaponProcessor.CurrentWeaponIndex <= 3)
						{
							DWORD_PTR Weapon = mem->Read<DWORD_PTR>(lpWeaponProcessor.EquippedWeapons + lpWeaponProcessor.CurrentWeaponIndex * sizeof(DWORD_PTR));

							if (Weapon == NULL)
								continue;

							FWeaponGunData WeaponGunConfig = mem->Read<FWeaponGunData>((DWORD_PTR)Weapon + off_WeaponGunConfig);

							if (config.bFastFireRate)
							{
								WeaponGunConfig.TimeBetweenShots = 0.03f;
							}

							if (config.bNoRecoil || config.bNoMuzzle || config.bNoSpread || config.bFastBullets)
							{
								FWeaponGunAnim WeaponGunAnim = mem->Read<FWeaponGunAnim>((DWORD_PTR)Weapon + off_WeaponGunAnim);

								if (config.bNoMuzzle)
								{
									WeaponGunAnim.CharacterFire = NULL;
								}

								if (config.bNoRecoil || config.bNoSpread || config.bFastBullets)
								{
									FTrajectoryWeaponData WeaponTrajectoryWeaponData = mem->Read<FTrajectoryWeaponData>((DWORD_PTR)Weapon + off_WeaponTrajectoryWeaponData);

									if (config.bFastBullets)
									{
										WeaponTrajectoryWeaponData.InitialSpeed = 999999;
									}

									if (config.bNoRecoil)
									{
										FRecoilInfo RecoilInfo = mem->Read<FRecoilInfo>((DWORD_PTR)Weapon + off_FRecoilInfo);

										WeaponTrajectoryWeaponData.RecoilPatternScale = 0;
										WeaponTrajectoryWeaponData.RecoilRecoverySpeed = 0;
										WeaponTrajectoryWeaponData.RecoilSpeed = 0;

										WeaponGunAnim.ShotCameraShake = NULL;
										WeaponGunAnim.ShotCameraShakeADS = NULL;
										WeaponGunAnim.ShotCameraShakeIronsight = NULL;

										RecoilInfo.VerticalRecoilMin = 0;
										RecoilInfo.VerticalRecoilMax = 0;
										RecoilInfo.RecoilValue_Climb = 0;
										RecoilInfo.RecoilValue_Fall = 0;
										RecoilInfo.RecoilModifier_Stand = 0;
										RecoilInfo.RecoilModifier_Crouch = 0;
										RecoilInfo.RecoilModifier_Prone = 0;
										RecoilInfo.RecoilSpeed_Horizontal = 0;
										RecoilInfo.RecoilSpeed_Vertical = 0;
										RecoilInfo.RecoverySpeed_Vertical = 0;
										RecoilInfo.VerticalRecoveryModifier = 0;

										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_FRecoilInfo), &RecoilInfo, sizeof(FRecoilInfo), 0);
									}

									if (config.bNoSpread)
									{
										FWeaponDeviationData WeaponDeviationConfig = mem->Read<FWeaponDeviationData>((DWORD_PTR)Weapon + off_WeaponDeviationConfig);

										WeaponTrajectoryWeaponData.WeaponSpread = 0;
										WeaponTrajectoryWeaponData.AimingSpreadModifier = 0;
										WeaponTrajectoryWeaponData.FiringSpreadBase = 0;
										WeaponTrajectoryWeaponData.ProneRecoveryTime = 0;
										WeaponTrajectoryWeaponData.ScopingSpreadModifier = 0;

										WeaponGunConfig.FiringBulletsSpread = 0;

										WeaponDeviationConfig.DeviationBase = 0;
										WeaponDeviationConfig.DeviationBaseADS = 0;
										WeaponDeviationConfig.DeviationBaseAim = 0;
										WeaponDeviationConfig.DeviationMax = 0;
										WeaponDeviationConfig.DeviationMaxMove = 0;
										WeaponDeviationConfig.DeviationMinMove = 0;
										WeaponDeviationConfig.DeviationMoveMaxReferenceVelocity = 0;
										WeaponDeviationConfig.DeviationMoveMinReferenceVelocity = 0;
										WeaponDeviationConfig.DeviationMoveMultiplier = 0;
										WeaponDeviationConfig.DeviationRecoilGain = 0;
										WeaponDeviationConfig.DeviationRecoilGainADS = 0;
										WeaponDeviationConfig.DeviationRecoilGainAim = 0;
										WeaponDeviationConfig.DeviationStanceCrouch = 0;
										WeaponDeviationConfig.DeviationStanceJump = 0;
										WeaponDeviationConfig.DeviationStanceProne = 0;
										WeaponDeviationConfig.DeviationStanceStand = 0;

										float buf = 0.f;
										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_RecoilSpreadScale), &buf, sizeof(float), 0);  /* RecoilSpreadScale */
										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_WalkSpread), &buf, sizeof(float), 0); /* WalkSpread */
										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_RunSpread), &buf, sizeof(float), 0); /* RunSpread */
										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_JumpSpread), &buf, sizeof(float), 0); /* JumpSpread */

										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_WeaponDeviationConfig), &WeaponDeviationConfig, sizeof(FWeaponDeviationData), 0);
									}

									if (config.bNoSpread || config.bFastBullets)
										WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_WeaponTrajectoryWeaponData), &WeaponTrajectoryWeaponData, sizeof(FTrajectoryWeaponData), 0);
								}

								if (config.bNoMuzzle || config.bNoRecoil)
									WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_WeaponGunAnim), &WeaponGunAnim, sizeof(FWeaponGunAnim), 0);
							}

							if(config.bFastFireRate || config.bNoSpread)
								WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_WeaponGunConfig), &WeaponGunConfig, sizeof(FWeaponGunData), 0);

							if (config.bNoSway)
							{
								FWeaponData WeaponConfig = mem->Read<FWeaponData>((DWORD_PTR)Weapon + off_WeaponConfig);

								WeaponConfig.SwayModifier_Crouch = 0;
								WeaponConfig.SwayModifier_Movement = 0;
								WeaponConfig.SwayModifier_Pitch = 0;
								WeaponConfig.SwayModifier_Prone = 0;
								WeaponConfig.SwayModifier_Stand = 0;
								WeaponConfig.SwayModifier_YawOffset = 0;

								WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)Weapon + off_WeaponConfig), &WeaponConfig, sizeof(FWeaponData), 0);
							}
						}
					}
				}
			}
			

			if (config.bAimbot) //config.bAimbot
			{
				if ((GetAsyncKeyState(Utils::getKeycode(config.aimkey.c_str())) & 0x8000) != 0) //   
				{
					FRotator LocalAngles = mem->Read<FRotator>((DWORD_PTR)g_pLocalPlayer.LocalPlayerController + off_ControlRotation);

					FRotator flBestAngDelta = FRotator(0, 0, 0);
					float FOV = config.FOV;
					bFoundTarget = false;

					Vector EntityLocation;
					for (int nIndex = 0; nIndex < iCount; nIndex++)
					{
						AActor Actor = mem->Read<AActor>((DWORD_PTR)mem->Read<DWORD_PTR>(pEntityList + nIndex * sizeof(DWORD_PTR)));
						int ActorID = Actor.ActorID;

						if (ActorID == g_pCache.PlayerFemale_A || ActorID == g_pCache.PlayerFemale_A_C || ActorID == g_pCache.PlayerMale_A ||
							ActorID == g_pCache.PlayerMale_A_C && (Actor.Health > 0 && Actor.Health <= 100) && Actor.Health != 0)
						{
							ATslPlayerState PlayerState = Actor.GetPlayerState();

							//if (PlayerState.BasePointer == NULL) // Cars
								//continue;

							if (LocalPlayerState.PlayerID == PlayerState.PlayerID) // Dont need local player
								continue;

							if (PlayerState.TeamNumber == LocalPlayerState.TeamNumber) // Dont need squad
								continue;

							switch (config.aimbotBone)
							{
							case 0:
								EntityLocation = Actor.GetBoneWithRotation(Bone::forehead);
								break;
							case 1:
								EntityLocation = Actor.GetBoneWithRotation(Bone::neck_01);
								break;
							case 2:
								EntityLocation = Actor.GetBoneWithRotation(75);
								break;
							case 3:
								EntityLocation = Actor.GetBoneWithRotation(174);
								break;
							case 4:
								EntityLocation = Actor.GetBoneWithRotation(175);
								break;
							default:
								EntityLocation = Actor.GetBoneWithRotation(6);
							}


							if (config.bVelocityPrediction || config.bBulletDropPrediction)
							{
								AWeaponProcessor lpWeaponProcessor = LocalPlayer.GetWeaponProcessor();
								if (lpWeaponProcessor.BasePointer != NULL)
								{
									DWORD_PTR Weapon = mem->Read<DWORD_PTR>(lpWeaponProcessor.EquippedWeapons + lpWeaponProcessor.CurrentWeaponIndex * sizeof(DWORD_PTR));
									if (Weapon != NULL)
									{
										FTrajectoryWeaponData WeaponTrajectoryWeaponData = mem->Read<FTrajectoryWeaponData>((DWORD_PTR)Weapon + off_WeaponTrajectoryWeaponData); // Used to get bullet speed

										float Distance = (LocalPlayerPosition - EntityLocation).length() / 100.f; // Distance to player

										if(config.bVelocityPrediction)
										{
											Vector Velocity = Actor.GetRootComponent().ComponentVelocity;
											Velocity = Velocity * (Distance / WeaponTrajectoryWeaponData.InitialSpeed); // Velocity * Travel time

											EntityLocation = EntityLocation + Velocity; // Plus it to my initial cordinets
										}
										
										if(config.bBulletDropPrediction)
										{

										}
									}
								}
							}

							static bool bDistance = false;
							if(config.MaxDist != 0)
							{
								Vector vecRealiverPos = LocalPlayerPosition - EntityLocation;
								float flDistance = vecRealiverPos.length() / 100;

								if (flDistance <= config.MaxDist)
								{
									bDistance = true;
								}
								else {
									bDistance = false;
								}
							}
							else {
								bDistance = true;
							}

							if (bDistance)
							{
								Vector delta = EntityLocation - pLocalCameraCahceEntry.POV.Location;
								FRotator angDelta = (delta.ToFRotator() - LocalAngles).Clamp();

								if (angDelta.Length() <= FOV)
								{
									FOV = (float)angDelta.Length();
									flBestAngDelta = angDelta;
									AimLocation = EntityLocation;
									bFoundTarget = true;
								}
							}
						}
						else continue;
					}

					if (bFoundTarget)
					{
						if (config.Smooth != 0)
						{
							flBestAngDelta = flBestAngDelta * (config.Smooth / 100);
						}
						flBestAngDelta.Clamp();


						WriteProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)g_pLocalPlayer.LocalPlayerController + off_ControlRotation), &(LocalAngles + flBestAngDelta), sizeof(Vector), 0);
					}
				}
				 else {
					 bFoundTarget = false;
				}
			}
		}

		Sleep(config.iSleepRate);
	}
}

void RenderAndUpdateThread()
{
	auto io = ImGui::GetIO();
	auto duration = 0;
	io.DeltaTime = 1.0f / 120.f;
	io.ImeWindowHandle = global->hWnd;

	while (true) {
		if (DrawMenu)
		{
			RenderBegin = high_resolution_clock::now();
		}

		g_pRender->Clear();
		g_pRender->Begin();

		if (bDrawESP)
		{
			if (config.bPlayerHealthESP || config.bPlayerNameESP || config.bDistanceESP || config.bLineESP || config.bDebugMode || config.bLootESP || config.bRadarESP || config.bDowned || config.bShowLoot
				|| config.bShowTeamMarker || config.bSkeletonESP || config.bAirDrop || config.BoxESP)
			{
				/* Local Player Crap */
				LocalPlayerPosition = LocalPlayer.GetRootComponent().EntityLocation;

				for (auto nIndex = 0; nIndex < iCount; nIndex++)
				{
					AActor Actor = mem->Read<AActor>((DWORD_PTR)mem->Read<DWORD_PTR>(pEntityList + nIndex * sizeof(DWORD_PTR)));

					int ActorID = Actor.ActorID;

					if (config.bDebugMode) // Ouch dont use 24/7
					{
						Vector EntityLocation = Actor.GetRootComponent().EntityLocation; //Entity Location
						Vector EntityTransformedLocation;
						if (!g_pMaths->WorldToScreen(EntityLocation, pLocalCameraCahceEntry, EntityTransformedLocation)) continue;

						if (EntityTransformedLocation.x > config.ScreenW || EntityTransformedLocation.y > config.ScreenH) continue;

						g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, D3DCOLOR_ARGB(255, 255, 0, 0), 2, "[%d | %s]", (int)ActorID, (std::string)getNameFromId(ActorID));
					}

					if (Actor.IsPlayer(g_pCache)
						|| ActorID == g_pCache.DroppedItemInteractionComponent
						|| ActorID == g_pCache.DroppedItemGroup

						|| ActorID == g_pCache.DeathDropItemPackage_C
						|| ActorID == g_pCache.Boat_PG117_C
						|| ActorID == g_pCache.Dacia_A_01_C
						|| ActorID == g_pCache.Dacia_A_02_C
						|| ActorID == g_pCache.Dacia_A_03_C
						|| ActorID == g_pCache.BP_Motorbike_04_SideCar_C
						|| ActorID == g_pCache.BP_Motorbike_03_C
						|| ActorID == g_pCache.BP_Motorbike_04_C
						|| ActorID == g_pCache.Buggy_A_01_C
						|| ActorID == g_pCache.Buggy_A_03_C
						|| ActorID == g_pCache.Buggy_A_02_C
						|| ActorID == g_pCache.Dacia_A_04_C
						|| ActorID == g_pCache.Uaz_B_01_C
						|| ActorID == g_pCache.Uaz_A_01_C
						|| ActorID == g_pCache.Uaz_C_01_C
						|| ActorID == g_pCache.Carapackage_RedBox_C) //  && !config.bDebugMode
					{
						ATslPlayerState PlayerState = Actor.GetPlayerState();

						if (LocalPlayerState.PlayerID == PlayerState.PlayerID) // Filter out my self. 
							continue;

						if (PlayerState.TeamNumber == LocalPlayerState.TeamNumber)
						{
							if (config.bShowTeamMarker)
							{
								if (PlayerState.bShowMapMarker) // If player Marker Exsists
								{
									float flwaypointdist = (LocalPlayerPosition - PlayerState.MapMarkerPosition).length() / 100;
									Vector SquadWayPointTransformedLocation;
									if (!g_pMaths->WorldToScreen(PlayerState.MapMarkerPosition, pLocalCameraCahceEntry, SquadWayPointTransformedLocation)) continue;

									g_pRender->DrawFontText(SquadWayPointTransformedLocation.x, SquadWayPointTransformedLocation.y, D3DCOLOR_ARGB(255, 0, 255, 0), "Waypoint: %d m", (int)flwaypointdist);
								}
							}
							continue;
						}

						int i_font_type = 0;
						int offset = -10; // Reset offset to -10 for each actor

						USceneComponent ActorRoot = Actor.GetRootComponent();

						Vector EntityLocation = ActorRoot.EntityLocation; //Entity Location

						if (config.bLootESP || (config.bDebugMode && config.bLogIds))
						{
							if (ActorID == g_pCache.DroppedItemInteractionComponent || ActorID == g_pCache.DroppedItemGroup) // Only items / weapons
							{
								if (Actor.iDroppedItemGroupCount != 0)
								{
									for (int j = 0; j < Actor.iDroppedItemGroupCount; j++)
									{
										UDroppedItemInteractionComponent pADroppedItemGroup = mem->Read<UDroppedItemInteractionComponent>(Actor.DroppedItemGroupArray + j * 0x8);

										Vector vecRelativePositionDropedItemLocation = mem->Read<Vector>(pADroppedItemGroup.BasePointer + 0x01E0);

										Vector vecItemTransformedLocation;
										if (!g_pMaths->WorldToScreen(EntityLocation + vecRelativePositionDropedItemLocation, pLocalCameraCahceEntry, vecItemTransformedLocation)) continue;

										if (vecItemTransformedLocation.x > config.ScreenW || vecItemTransformedLocation.y > config.ScreenH) continue;

										Vector vecItemRealiverPos = LocalPlayerPosition - (EntityLocation + vecRelativePositionDropedItemLocation);
										float flDistanceToItem = vecItemRealiverPos.length() / 100;

										DWORD Itemcol = 0;

										if ((int)flDistanceToItem <= 75)
										{
											DWORD_PTR pUItem = mem->Read<DWORD_PTR>(pADroppedItemGroup.BasePointer + 0x0448);
											DWORD_PTR pUItemFString = mem->Read<DWORD_PTR>(pUItem + 0x40);
											DWORD_PTR pItemName = mem->Read<DWORD_PTR>(pUItemFString + 0x28);
											//int ItemID = mem->Read<int>(pUItem + 0x18);
											wchar_t* szItemName[64];
											if (ReadProcessMemory(global->hProcess, (LPVOID)((DWORD_PTR)pItemName), &szItemName, sizeof(wchar_t*) * 64, NULL) != 0)
											{
												bool draw = false;
												wchar_t szItemNamew[64];
												swprintf(szItemNamew, sizeof(szItemNamew) / sizeof(*szItemNamew), L"%ws", szItemName);

												if (!config.bLootFilterNone)
												{

													if ((std::wstring(szItemNamew).find(L"Level 1") != std::wstring::npos || std::wstring(szItemNamew).find(L"Lv.1") != std::wstring::npos) && config.bLootFilterL1) {
														Itemcol = D3DCOLOR_RGBA((int)(config.LootL1.x*255.0f), (int)(config.LootL1.y*255.0f), (int)(config.LootL1.z*255.0f), 255);
														draw = true;
													}

													if ((std::wstring(szItemNamew).find(L"Level 2") != std::wstring::npos || std::wstring(szItemNamew).find(L"Lv.2") != std::wstring::npos) && config.bLootFilterL2) {
														Itemcol = D3DCOLOR_RGBA((int)(config.LootL2.x*255.0f), (int)(config.LootL2.y*255.0f), (int)(config.LootL2.z*255.0f), 255);
														draw = true;
													}

													if ((std::wstring(szItemNamew).find(L"Level 3") != std::wstring::npos || std::wstring(szItemNamew).find(L"Lv.3") != std::wstring::npos) && config.bLootFilterL3) {
														Itemcol = D3DCOLOR_RGBA((int)(config.LootL3.x*255.0f), (int)(config.LootL3.y*255.0f), (int)(config.LootL3.z*255.0f), 255);
														draw = true;
													}

													if (config.bLootFilterGuns)
													{
														for (int i = 0; i < Weapons.size(); i++)
														{
															if (std::wstring(szItemNamew).find(Weapons[i]) != std::wstring::npos && bWeapon[i] == true) {
																Itemcol = D3DCOLOR_RGBA((int)(config.LootGuns.x*255.0f), (int)(config.LootGuns.y*255.0f), (int)(config.LootGuns.z*255.0f), 255);
																draw = true;
																break;
															}
														}
													}

													if (config.bLootFilterAttachments)
													{
														for (int i = 0; i < Attachements.size(); i++)
														{
															if (std::wstring(szItemNamew).find(Attachements[i]) != std::wstring::npos && bAttachements[i] == true) {
																Itemcol = D3DCOLOR_RGBA((int)(config.LootAttachements.x*255.0f), (int)(config.LootAttachements.y*255.0f), (int)(config.LootAttachements.z*255.0f), 255);
																draw = true;
																break;
															}
														}
													}

													if (config.bAmmoLoot)
													{
														for (int i = 0; i < Ammo.size(); i++)
														{
															if (std::wstring(szItemNamew).find(Ammo[i]) != std::wstring::npos && bAmmo[i] == true) {
																Itemcol = D3DCOLOR_RGBA((int)(config.LootAmmo.x*255.0f), (int)(config.LootAmmo.y*255.0f), (int)(config.LootAmmo.z*255.0f), 255);
																draw = true;
																break;
															}
														}
													}

													if (config.bMedicalLoot)
													{
														for (int i = 0; i < Medical.size(); i++)
														{
															if (std::wstring(szItemNamew).find(Medical[i]) != std::wstring::npos  && bMedical[i] == true) {
																Itemcol = D3DCOLOR_RGBA((int)(config.LootMedical.x*255.0f), (int)(config.LootMedical.y*255.0f), (int)(config.LootMedical.z*255.0f), 255);
																draw = true;
																break;
															}
														}
													}

													if (config.bScopesLoot)
													{
														for (int i = 0; i < Scopes.size(); i++)
														{
															if (std::wstring(szItemNamew).find(Scopes[i]) != std::wstring::npos && bScopes[i] == true) {
																Itemcol = D3DCOLOR_RGBA((int)(config.LootScopes.x*255.0f), (int)(config.LootScopes.y*255.0f), (int)(config.LootScopes.z*255.0f), 255);
																draw = true;
																break;
															}
														}
													}

													if (draw)
													{
														if (config.bLootDistanceESP)
														{
															g_pRender->DrawFontText(vecItemTransformedLocation.x, vecItemTransformedLocation.y, Itemcol, "%ws  %dm", szItemName, (int)flDistanceToItem);
														}
														else {
															g_pRender->DrawFontText(vecItemTransformedLocation.x, vecItemTransformedLocation.y, Itemcol, "%ws", szItemName);
														}

														continue;
													}
												}
												else
												{
													if (config.bLootDistanceESP)
													{
														g_pRender->DrawFontText(vecItemTransformedLocation.x, vecItemTransformedLocation.y, Itemcol, "%ws  %dm", szItemName, (int)flDistanceToItem);
													}
													else {
														g_pRender->DrawFontText(vecItemTransformedLocation.x, vecItemTransformedLocation.y, Itemcol, "%ws", szItemName);
													}

													continue;
												}

											}
										}
										else
											continue;

									}
								}

								continue; //If freezes
							}
						}

						Vector EntityTransformedLocation;
						if (!g_pMaths->WorldToScreen(EntityLocation, pLocalCameraCahceEntry, EntityTransformedLocation)) continue;

						Vector vecRealiverPos = LocalPlayerPosition - EntityLocation;
						float flDistance = vecRealiverPos.length() / 100;

						if (ActorID == g_pCache.Carapackage_RedBox_C && config.bAirDrop)
						{
							g_pRender->DrawFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, D3DCOLOR_ARGB(255, 255, 20, 147), "[Airdrop %.fm]", (float)flDistance);
							continue;
						}

						if (config.bVehicleESP && flDistance <= 600)
						{
							DWORD color = D3DCOLOR_ARGB(255, 0, 255, 255);

							if (ActorID == g_pCache.Uaz_A_01_C || ActorID == g_pCache.Uaz_C_01_C || ActorID == g_pCache.Uaz_B_01_C)
							{
								g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, color, 2, "[UAZ %dm]", (int)flDistance);
								continue; //If freezes
							}

							if (ActorID == g_pCache.Dacia_A_01_C || ActorID == g_pCache.Dacia_A_02_C || ActorID == g_pCache.Dacia_A_03_C
								|| ActorID == g_pCache.Dacia_A_04_C)
							{
								g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, color, 2, "[Dacia %dm]", (int)flDistance);
								continue; //If freezes
							}

							if (ActorID == g_pCache.BP_Motorbike_03_C || ActorID == g_pCache.BP_Motorbike_04_C)
							{
								g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, color, 2, "[Motorbike %dm]", (int)flDistance);
								continue; //If freezes
							}

							if (ActorID == g_pCache.BP_Motorbike_04_SideCar_C)
							{
								g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, color, 2, "[Motorbike Sidecar %dm]", (int)flDistance);
								continue; //If freezes
							}

							if (ActorID == g_pCache.Buggy_A_03_C || ActorID == g_pCache.Buggy_A_02_C || ActorID == g_pCache.Buggy_A_01_C)
							{
								g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, color, 2, "[Buggy %dm]", (int)flDistance);
								continue; //If freezes
							}

							if (ActorID == g_pCache.Boat_PG117_C)
							{
								g_pRender->DrawOutlinedFontText(EntityTransformedLocation.x, EntityTransformedLocation.y, color, 2, "[Boat %dm]", (int)flDistance);
								continue; //If freezes
							}
						}

						if (ActorID == g_pCache.PlayerFemale_A || ActorID == g_pCache.PlayerFemale_A_C || ActorID == g_pCache.PlayerMale_A ||
							ActorID == g_pCache.PlayerMale_A_C || ActorID == g_pCache.DeathDropItemPackage_C)
						{
							if (config.bRadarESP)
							{
								float RadarX = config.ScreenW * 0.848958333f;
								float RadarY = config.ScreenH * 0.73796296296f;
								float Radarsize = config.ScreenH * 00.23148148148f; //250

								float RadarX2 = RadarX + Radarsize;
								float RadarY2 = RadarY + Radarsize;
								float RadarX_C = RadarX + Radarsize / 2;
								float RadarY_C = RadarY + Radarsize / 2;

								Vector Delta = EntityLocation - LocalPlayerPosition;
								Vector2 PosToScreen = Vector2(RadarX_C + Radarsize * Delta.x / 42000.f, RadarY_C + Radarsize * Delta.y / 42000.f);
								if (PosToScreen.x > RadarX && PosToScreen.x < RadarX2 && PosToScreen.y > RadarY && PosToScreen.y < RadarY2)
								{
									g_pRender->DrawRect(PosToScreen.x, PosToScreen.y, 5, 5, D3DCOLOR_ARGB(255, 255, 0, 0));
								}
							}

							switch (Actor.GetCharacterMovement().StanceMode)
							{
							case 1:
								EntityLocation.z += 30;
								break;

							case 2:
								EntityLocation.z += 10;
								break;

							case 3:
								EntityLocation.z -= 15;
								break;
							default:
								break;
							}

							Vector PlayerTransformedLocation;
							if (!g_pMaths->WorldToScreen(EntityLocation, pLocalCameraCahceEntry, PlayerTransformedLocation)) continue;

							DWORD dDistanceColor = 0;

							if ((int)flDistance < 280)
							{
								i_font_type = 1;
								dDistanceColor = D3DCOLOR_RGBA((int)(config.Distance280IMGUI.x*255.0f), (int)(config.Distance280IMGUI.y*255.0f), (int)(config.Distance280IMGUI.z*255.0f), 255);
							}

							if ((int)flDistance >= 560)
							{
								i_font_type = 3;
								dDistanceColor = D3DCOLOR_RGBA((int)(config.DistanceMorethan560IMGUI.x*255.0f), (int)(config.DistanceMorethan560IMGUI.y*255.0f), (int)(config.DistanceMorethan560IMGUI.z*255.0f), 255);
							}

							if ((int)flDistance > 279 && (int)flDistance < 560)
							{
								i_font_type = 2;
								dDistanceColor = D3DCOLOR_RGBA((int)(config.Distance560IMGUI.x*255.0f), (int)(config.Distance560IMGUI.y*255.0f), (int)(config.Distance560IMGUI.z*255.0f), 255);
							}

							float flPlayerHealth = Actor.Health;

							if (flPlayerHealth == 0 && Actor.GroggyHealth >= 1 && config.bDowned) // If the Actor is Down
							{
								g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y, D3DCOLOR_ARGB(255, 39, 174, 96), i_font_type, "[D0WN3D]");

								int offset = 10;
								if (config.bDownedHP)
								{
									g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y + offset, D3DCOLOR_ARGB(255, 39, 174, 96), i_font_type, "[HP: %d]", (int)Actor.GroggyHealth);
									offset += 10;
								}

								if (config.bDownedDistance)
								{
									g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y + offset, D3DCOLOR_ARGB(255, 39, 174, 96), i_font_type, "[%dm]", (int)flDistance);
									offset += 10;
								}

								if (config.bDownedTimeToDie)
								{
									float timetodie = Actor.GroggyHealth / Actor.DecreaseGroggyHealthPerSecond;
									g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y + offset, D3DCOLOR_ARGB(255, 39, 174, 96), i_font_type, "[%d s]", (int)timetodie);
								}

								continue;
							}

							if (ActorID == g_pCache.DeathDropItemPackage_C && config.bShowLoot)
							{
								g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y, D3DCOLOR_ARGB(255, 39, 174, 96), i_font_type, "[DED L00T]");
								continue;
							}

							if ((flPlayerHealth > 0 && flPlayerHealth <= 100) && flPlayerHealth != 0 && ActorID != g_pCache.DeathDropItemPackage_C) // If the Actor is alive
							{
								if (config.bSkeletonESP && (int)flDistance <= 100)
								{
									std::vector<int> skeletonUpper{
										forehead, Head, neck_01
									};
									std::vector<int> skeletonRightArm{
										neck_01, upperarm_r, lowerarm_r, hand_r
									};
									std::vector<int> skeletonLeftArm{
										neck_01, upperarm_l, lowerarm_l, hand_l
									};
									std::vector<int> skeletonSpine{
										neck_01, spine_01, spine_02, pelvis
									};
									std::vector<int> skeletonLowerRight{
										pelvis, thigh_r, calf_r, foot_r
									};
									std::vector<int> skeletonLowerLeft{
										pelvis, thigh_l, calf_l, foot_l
									};
									std::vector<std::vector<int>> skeleton{
										skeletonUpper, skeletonLeftArm, skeletonRightArm, skeletonSpine, skeletonLowerRight, skeletonLowerLeft
									};

									for each (auto part in skeleton)
									{
										Vector previousBone(0, 0, 0);
										for each(auto b in part)
										{
											auto currentBone = Actor.GetBoneWithRotation(b);
											if (previousBone.x == 0) //first bone no draw happens
											{
												previousBone = currentBone;
												continue;
											}
											else
											{
												Vector previousBoneScreen, currentBoneScreen;
												if (!g_pMaths->WorldToScreen(previousBone, pLocalCameraCahceEntry, previousBoneScreen)) continue;
												if (!g_pMaths->WorldToScreen(currentBone, pLocalCameraCahceEntry, currentBoneScreen)) continue;

												g_pRender->DrawLine(previousBoneScreen.x, previousBoneScreen.y, currentBoneScreen.x, currentBoneScreen.y, config.SkeletonWidth, D3DCOLOR_RGBA((int)(config.SkelColorIMGUI.x*255.0f), (int)(config.SkelColorIMGUI.y*255.0f), (int)(config.SkelColorIMGUI.z*255.0f), 255));

												previousBone = currentBone;
											}
										}
									}
								}

								if (config.bPlayerHealthESP)
								{
									int G = (int)((float)(255 * flPlayerHealth) / 100.0);
									int R = 255 - G;

									g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y + offset, D3DCOLOR_ARGB(255, R, G, 0), i_font_type, "[HP %d]", static_cast<int>(flPlayerHealth));
									offset += 12;
								}

								if (config.bPlayerNameESP)
								{
									std::string PlayerName = "";

									if (CachedPlayerNames[PlayerState.PlayerID] != "")
									{
										PlayerName = CachedPlayerNames[PlayerState.PlayerID];
									}
									else {
										std::string tempName = PlayerState.GetName();
										CachedPlayerNames[PlayerState.PlayerID] = tempName;
										PlayerName = tempName;
									}

									g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y + offset, D3DCOLOR_ARGB(255, 192, 57, 43), i_font_type, "[%s]", static_cast<std::string>(PlayerName));
									offset += 12;
								}

								if (config.bDistanceESP)
								{
									g_pRender->DrawOutlinedFontText(PlayerTransformedLocation.x, PlayerTransformedLocation.y + offset, dDistanceColor, i_font_type, "[%.fm]", static_cast<float>(flDistance));
									offset += 12;
								}

								if (config.bLineESP)
								{
									g_pRender->DrawLine(config.ScreenW / 2, config.ScreenH, PlayerTransformedLocation.x, PlayerTransformedLocation.y, 1, D3DCOLOR_ARGB(255, 192, 57, 43));
								}

								if (config.BoxESP && (int)flDistance <= 100 && config.BoxType != -1)
								{
									Vector headTransformed;
									Vector RLegTransformed;

									if (!g_pMaths->WorldToScreen(Actor.GetBoneWithRotation(59), pLocalCameraCahceEntry, headTransformed)) continue;
									if (!g_pMaths->WorldToScreen(Actor.GetBoneWithRotation(167), pLocalCameraCahceEntry, RLegTransformed)) continue;

									float Width = abs(RLegTransformed.y - headTransformed.y) / 1.5f;
									float Height = abs(RLegTransformed.y - headTransformed.y);

									DWORD boxcolor = D3DCOLOR_RGBA((int)(config.BoxColorIMGUI.x*255.0f), (int)(config.BoxColorIMGUI.y*255.0f), (int)(config.BoxColorIMGUI.z*255.0f), 255);

									if (config.BoxType == 0)
									{
										g_pRender->Draw2dBox(headTransformed.x - Width / 2, headTransformed.y, Width, Height, config.BoxWidth, boxcolor);
									}

									if (config.BoxType == 1)
									{
										g_pRender->DrawCornerBox(headTransformed.x - Width / 2, headTransformed.y, Width, Height, config.BoxWidth, boxcolor);
									}

									if (config.BoxType == 2)
									{
										EntityLocation = ActorRoot.RelativeLocation;
										Vector bounds = ActorRoot.Bounds;
										float top = EntityLocation.z + bounds.z;
										float bottom = EntityLocation.z - bounds.z;

										float front = EntityLocation.x + bounds.x;
										float behind = EntityLocation.x - bounds.x;

										float left = EntityLocation.y + bounds.y;
										float right = EntityLocation.y - bounds.y;

										Vector top1, top2, top3, top4, bot1, bot2, bot3, bot4;
										g_pMaths->WorldToScreen(Vector(EntityLocation.x + bounds.x, EntityLocation.y + bounds.y, 0), pLocalCameraCahceEntry, top1);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x - bounds.x, EntityLocation.y + bounds.y, 0), pLocalCameraCahceEntry, top2);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x + bounds.x, EntityLocation.y - bounds.y, 0), pLocalCameraCahceEntry, top4);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x - bounds.x, EntityLocation.y - bounds.y, 0), pLocalCameraCahceEntry, top3);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x + bounds.x, EntityLocation.y + bounds.y, 0), pLocalCameraCahceEntry, bot1);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x - bounds.x, EntityLocation.y + bounds.y, 0), pLocalCameraCahceEntry, bot2);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x + bounds.x, EntityLocation.y - bounds.y, 0), pLocalCameraCahceEntry, bot4);
										g_pMaths->WorldToScreen(Vector(EntityLocation.x - bounds.x, EntityLocation.y - bounds.y, 0), pLocalCameraCahceEntry, bot3);

										std::list<std::pair<Vector, Vector>> topLines = { { top1,top2 },{ top2,top3 },{ top3,top4 },{ top4,top1 }, };
										std::list<std::pair<Vector, Vector>> botLines = { { bot1,bot2 },{ bot2,bot3 },{ bot3,bot4 },{ bot4,bot1 }, };
										std::list<std::pair<Vector, Vector>> sideLines = { { top1,bot1 },{ top2,bot2 },{ top3,bot3 },{ top4,bot4 }, };

										for each(auto p in topLines) {
											g_pRender->DrawLine(p.first.x, p.first.y, p.second.x, p.second.y, config.BoxWidth, boxcolor);
										}
										for each(auto p in botLines) {
											g_pRender->DrawLine(p.first.x, p.first.y, p.second.x, p.second.y, config.BoxWidth, boxcolor);
										}
										for each(auto p in sideLines) {
											g_pRender->DrawLine(p.first.x, p.first.y, p.second.x, p.second.y, config.BoxWidth, boxcolor);
										}
									}
								}

								if (config.bDrawBones)
								{
									for (int i = 0; i < Bone::MAX; i++)
									{
										Vector TransfomedBone;
										if (!g_pMaths->WorldToScreen(Actor.GetBoneWithRotation(i), pLocalCameraCahceEntry, TransfomedBone)) continue;

										g_pRender->DrawFontText(TransfomedBone.x, TransfomedBone.y, D3DCOLOR_ARGB(255, 255, 25, 255), "%d", static_cast<int>(i));

									}
								}

								continue;
							}
							else continue;
						}
						else continue;

						continue;
					}
					else continue;
				}

			}
			else {
				//std::cout << "Getting UWorld" << std::endl;
				//global->pUWorld = GetUWorld(GWorldAddress, dUWorldOffset);
			}

			if (config.bShowLocalMarker)
			{
				if (LocalPlayerState.bShowMapMarker) // If player Marker Exsists
				{
					float flwaypointdist = (LocalPlayer.GetRootComponent().RelativeLocation - LocalPlayerState.MapMarkerPosition).length() / 100.f;
					Vector LocalWayPointTransformedLocation;
					if (g_pMaths->WorldToScreen(LocalPlayerState.MapMarkerPosition, pLocalCameraCahceEntry, LocalWayPointTransformedLocation))
						g_pRender->DrawFontText(LocalWayPointTransformedLocation.x, LocalWayPointTransformedLocation.y, D3DCOLOR_ARGB(255, 0, 255, 0), "Waypoint: %d m", (int)flwaypointdist);
				}
			}

			if (config.bDrawFOV)
			{
				float pxR = tan((DEG2RAD(config.FOV))) / tan(DEG2RAD(pLocalCameraCahceEntry.POV.FOV) / 2) * (config.ScreenW / 2);
				g_pRender->DrawCircle(config.ScreenW / 2, config.ScreenH / 2, pxR, M_PI / 180, D3DCOLOR_RGBA((int)(config.DrawFOVIMGUI.x*255.0f), (int)(config.DrawFOVIMGUI.y*255.0f), (int)(config.DrawFOVIMGUI.z*255.0f), 255));
			}

			if (config.ShowLockIndication && bFoundTarget)
			{
				DWORD AimbotLockColor = D3DCOLOR_RGBA((int)(config.AimbotLockColour.x*255.0f), (int)(config.AimbotLockColour.y*255.0f), (int)(config.AimbotLockColour.z*255.0f), 255);
				//Player
				if (config.LockIndicationType == 0)
				{
					Vector transformed;
					if (g_pMaths->WorldToScreen(AimLocation, pLocalCameraCahceEntry, transformed))
					{
						g_pRender->DrawFontText(transformed.x, transformed.y, AimbotLockColor, "*LOCKED*");
					}
				}
				//Static
				if (config.LockIndicationType == 1)
				{
					g_pRender->DrawFontText((config.ScreenW / 2) - 10, 20, AimbotLockColor, "*LOCKED*");
				}

				if (config.ShowAimLockBone)
				{
					Vector transformed;
					if (g_pMaths->WorldToScreen(AimLocation, pLocalCameraCahceEntry, transformed))
					{
						float halfBoneRad = config.BoneLockrad / 2;
						g_pRender->DrawCircle(transformed.x - halfBoneRad, transformed.y - halfBoneRad, config.BoneLockrad, M_PI / 180, D3DCOLOR_RGBA((int)(config.AimbotLockBoneColour.x*255.0f), (int)(config.AimbotLockBoneColour.y*255.0f), (int)(config.AimbotLockBoneColour.z*255.0f), 255));
					}
				}
			}
		}

		if (DrawMenu)
		{
			ImGui_ImplDX9_NewFrame();

			//Tab
			ImGui::Begin("Oh me God Mega", nullptr, ImVec2(405, 496), -1.0F);
			ImGui::Text("Welcome back, %s", (std::string)username);

			if (ImGui::CollapsingHeader("Visuals"))
			{
				if (ImGui::TreeNode("Player"))
				{
					ImGui::Checkbox("Name", &config.bPlayerNameESP);
					ImGui::Checkbox("Health", &config.bPlayerHealthESP);
					ImGui::Checkbox("Dist", &config.bDistanceESP);
					ImGui::Checkbox("Line", &config.bLineESP);
					ImGui::Checkbox("Radar", &config.bRadarESP);
					ImGui::Checkbox("Sk3l3ton", &config.bSkeletonESP);
					if (config.bSkeletonESP)
					{
						ImGui::SliderFloat("Max Skel Dist", &config.MaxSkeletonDistance, 1.0f, 1024.f);
					}

					ImGui::Checkbox("Box", &config.BoxESP);
					if (config.BoxESP)
					{
						ImGui::SliderFloat("Max Box Dist", &config.BoxDistance, 1.0f, 1024.f);
					}

					const char* items[] = { "Normal", "Corner", "3D" };
					ImGui::Combo("Box Type", &config.BoxType, items, IM_ARRAYSIZE(items));

					ImGui::Checkbox("Downed", &config.bDowned);

					if (config.bDowned)
					{
						if (ImGui::TreeNode("Downed Options"))
						{
							ImGui::Checkbox("Show Dist", &config.bDownedDistance);
							ImGui::Checkbox("Show HP", &config.bDownedHP);
							ImGui::Checkbox("Show Time to death", &config.bDownedTimeToDie);
							ImGui::TreePop();
						}
					}

					ImGui::Checkbox("ShowLocalMarker", &config.bShowLocalMarker);
					ImGui::Checkbox("ShowTeamMarker", &config.bShowTeamMarker);
					ImGui::Checkbox("Ded Loot", &config.bShowLoot);
					ImGui::Checkbox("Draw Bones", &config.bDrawBones);
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("W0rld"))
				{
					ImGui::Checkbox("AirDrop", &config.bAirDrop);
					if(config.bAirDrop)
					{
						ImGui::Checkbox("Show airdrop contents", &config.bAirdropContents);
					}

					ImGui::Checkbox("Vehicle", &config.bVehicleESP);
					ImGui::Checkbox("Loot", &config.bLootESP);
					ImGui::Checkbox("Loot Dist", &config.bLootDistanceESP);
					if (ImGui::TreeNode("L00t Filter"))
					{
						ImGui::Checkbox("No Filter", &config.bLootFilterNone);
						ImGui::Checkbox("Level 1", &config.bLootFilterL1);
						ImGui::Checkbox("Level 2", &config.bLootFilterL2);
						ImGui::Checkbox("Level 3", &config.bLootFilterL3);

						ImGui::Checkbox("Guns", &config.bLootFilterGuns);
						if (config.bLootFilterGuns)
						{
							if (ImGui::TreeNode("Filter Guns"))
							{
								for (int i = 0; i < Weapons.size(); i++)
								{
									ImGui::Checkbox(Utils::ws2s(Weapons[i]).c_str(), &bWeapon[i]);
								}
								ImGui::TreePop();
							}
						}

						ImGui::Checkbox("Ammo", &config.bAmmoLoot);
						if (config.bAmmoLoot)
						{
							if (ImGui::TreeNode("Filter Ammo"))
							{
								for (int i = 0; i < Ammo.size(); i++)
								{
									ImGui::Checkbox(Utils::ws2s(Ammo[i]).c_str(), &bAmmo[i]);
								}
								ImGui::TreePop();
							}
						}

						ImGui::Checkbox("Attachments", &config.bLootFilterAttachments);
						if (config.bLootFilterAttachments)
						{
							if (ImGui::TreeNode("Filter Attachments"))
							{
								for (int i = 0; i < Attachements.size(); i++)
								{
									ImGui::Checkbox(Utils::ws2s(Attachements[i]).c_str(), &bAttachements[i]);
								}

								ImGui::TreePop();
							}
						}

						ImGui::Checkbox("Medical", &config.bMedicalLoot);
						if (config.bMedicalLoot)
						{
							if (ImGui::TreeNode("Filter Medical"))
							{
								for (int i = 0; i < Medical.size(); i++)
								{
									ImGui::Checkbox(Utils::ws2s(Medical[i]).c_str(), &bMedical[i]);
								}
								ImGui::TreePop();
							}
						}

						ImGui::Checkbox("Scopes", &config.bScopesLoot);
						if (config.bScopesLoot)
						{
							if (ImGui::TreeNode("Filter Scopes"))
							{
								for (int i = 0; i < Scopes.size(); i++)
								{
									ImGui::Checkbox(Utils::ws2s(Scopes[i]).c_str(), &bScopes[i]);
								}
								ImGui::TreePop();
							}
						}

						ImGui::Checkbox("Custom", &config.bCustomFilter);
						if (config.bCustomFilter)
						{
							ImGui::InputText("Name", buff, sizeof(buff));
							static bool badd = false;
							if (ImGui::Button("Add"))
							{
								badd = !badd;
							}

							if (badd)
							{
								static char* temp = buff;
								CustomLoot_Items.push_back(temp);
								badd = false;
							}

							ImGui::SameLine();

							static bool bShowList = false;
							if (ImGui::Button("Show List"))
							{
								bShowList = !bShowList;
							}

							if (bShowList)
							{
								ImGui::ListBox("Custom filters", &CustomLoot_SelectedIndex, CustomLoot_Items.data(), CustomLoot_Items.size(), 4);
								if (ImGui::Button("Remove"))
								{
									CustomLoot_Items.erase(CustomLoot_Items.begin() + CustomLoot_SelectedIndex);
								}
							}
						}

						ImGui::TreePop();
					}
					ImGui::TreePop();
				}

			}

			if (ImGui::CollapsingHeader("R3movals"))
			{
				ImGui::Checkbox("NoRec", &config.bNoRecoil);
				ImGui::Checkbox("NoSpread", &config.bNoSpread);
				ImGui::Checkbox("NoSway", &config.bNoSway);
				ImGui::Checkbox("Fast Fire Rate", &config.bFastFireRate);
				ImGui::Checkbox("Fast Bu11et", &config.bFastBullets);
				ImGui::Checkbox("NoMuzzleFlash", &config.bNoMuzzle);
			}

			if (ImGui::CollapsingHeader("Visual Modifications"))
			{
				ImGui::ColorEdit3("Skelet0n Color", (float*)&config.SkelColorIMGUI);
				ImGui::SliderInt("Skelet0n Width", &config.SkeletonWidth, 1, 10);
				ImGui::ColorEdit3("Box Color", (float*)&config.BoxColorIMGUI);
				ImGui::SliderInt("Box Width", &config.BoxWidth, 1, 10);
				ImGui::ColorEdit3("FOV Ring", (float*)&config.DrawFOVIMGUI);

				if (ImGui::TreeNode("Distance"))
				{
					ImGui::ColorEdit3("Up to 280m", (float*)&config.Distance280IMGUI);
					ImGui::ColorEdit3("Up to 560m", (float*)&config.Distance560IMGUI);
					ImGui::ColorEdit3("More than 560m", (float*)&config.DistanceMorethan560IMGUI);
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("L00t Options"))
				{
					ImGui::ColorEdit3("Default Colour", (float*)&config.LootDefaultColor);
					ImGui::ColorEdit3("L1", (float*)&config.LootL1);
					ImGui::ColorEdit3("L2", (float*)&config.LootL2);
					ImGui::ColorEdit3("L3", (float*)&config.LootL3);
					ImGui::ColorEdit3("Guns", (float*)&config.LootGuns);
					ImGui::ColorEdit3("Attachments", (float*)&config.LootAttachements);
					ImGui::ColorEdit3("Medical", (float*)&config.LootMedical);
					ImGui::ColorEdit3("Ammo", (float*)&config.LootAmmo);
					ImGui::ColorEdit3("Scopes", (float*)&config.LootScopes);
					ImGui::ColorEdit3("Custom", (float*)&config.LootCustom);
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Text"))
				{
					ImGui::SliderFloat("Multiplier", &config.TextMultiplier, 0.25, 10, "%.2f");

					ImGui::Combo("Font", &config.TextFont, Fonts, IM_ARRAYSIZE(Fonts));

					if (ImGui::Button("Update Text"))
					{
						if (config.TextFont != -1)
						{
							int ilength = strlen(Fonts[config.TextFont]);
							WCHAR    str3[255];
							ZeroMemory(str3, 255);
							MultiByteToWideChar(0, 0, Fonts[config.TextFont], ilength, str3, ilength + 1);
							g_pRender->CreateFonts(config.TextMultiplier, str3);
						}
						else {
							g_pRender->CreateFonts(config.TextMultiplier, L"Tahoma");
						}
					}

					ImGui::TreePop();
				}
			}

			if (ImGui::CollapsingHeader("PAim"))
			{
				ImGui::Checkbox("Enabled", &config.bAimbot);
				ImGui::Checkbox("Lock Target", &config.locktarget);
				if (config.locktarget)
				{
					ImGui::Checkbox("Locked Indication", &config.ShowLockIndication);
					if (config.ShowLockIndication)
					{
						if (ImGui::TreeNode("Indication Settings"))
						{
							const char* LockType[] = { "Player", "Static" };
							ImGui::Combo("Lock Type", &config.LockIndicationType, LockType, IM_ARRAYSIZE(LockType));
							ImGui::ColorEdit3("Text Colour", (float*)&config.AimbotLockColour);

							ImGui::Spacing();

							ImGui::Checkbox("Show locked Bone", &config.ShowAimLockBone); //config.bonerad
							ImGui::SliderFloat("Bone Radius", &config.BoneLockrad, 0.0f, 10.f);
							ImGui::ColorEdit3("Bone Colour", (float*)&config.AimbotLockBoneColour);

							ImGui::Spacing();

							ImGui::TreePop();
						}
					}
				}

				ImGui::InputText("Aimkey", aimkey, 256); //, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_AutoSelectAll
				config.aimkey = std::string(aimkey);

				if (ImGui::IsItemHovered())
				{
					ImGui::Text(" List of other keys - \n lshift/shift \n lcontrol/lctr \n lmenu/lalt/alt \n Left mouse - lmouse/mouse1\n Middle Mouse - mmouse/mouse3");
					ImGui::Text(" Right Mouse - rmouse / mouse2 \n Mouse Side Button 1 - sidebtn / mousex1 \n Mouse Side Button 2 - sidebtn2 / mousex2 \n If i missed some send me a DM");
					ImGui::Text(" Capslock - cap/caps \n Tab - tab");
				}

				ImGui::SliderFloat("F0V", &config.FOV, 0.0f, 180.f);
				ImGui::Checkbox("Draw FOV", &config.bDrawFOV);

				ImGui::SliderFloat("Sm00th", &config.Smooth, 0.f, 100.f);
				ImGui::SliderFloat("MaxDist", &config.MaxDist, 0.f, 1024.f);
				if (ImGui::IsItemHovered())
				{
					ImGui::Text("0 = Infinite\n1024m is max dist in game");
				}
				ImGui::Checkbox("VelocityPred", &config.bVelocityPrediction);
				ImGui::Checkbox("BulletDropPred", &config.bBulletDropPrediction);

				const char* items[] = { "Head", "Neck", "Chest", "Waist", "Dick" };
				ImGui::Combo("A1mBone", &config.aimbotBone, items, IM_ARRAYSIZE(items));
			}

			if (ImGui::CollapsingHeader("Debug"))
			{
				ImGui::Checkbox("Debug Mode", &config.bDebugMode);
				ImGui::Checkbox("Log1Ds", &config.bLogIds);

				if (ImGui::Button("DumpNames"))
				{
					// Dump all GNames
					std::ofstream myfile;
					myfile.open("lelnames.txt");

					for (int i = 0; i < 200000; i++)
					{
						if (getNameFromId(i) != "FAIL")
						{
							myfile << i << "  |  " << getNameFromId(i) << "\n";
						}
					}

					myfile.close();
				}

				ImGui::Text("Debug info");

				ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
				ImGui::Text("IMGUI Delay: %.1f", 1000.0f / ImGui::GetIO().Framerate);
				ImGui::Text("Draw delay: %dms", (int)duration);

				if (iCount >= 0 && iCount <= 2147483)
				{
					ImGui::Text("Act0r Size: %d", (int)iCount);
				}
				else {
					ImGui::Text("Act0r Size: LOL");
				}
				ImGui::Text("Base: 0x%X", global->baseaddress);
				ImGui::Text("Uw0rld0ffset: 0x%X", UWorldOffSet);
				ImGui::Text("GNam3s0ffset: 0x%X", gNamesOffSet);
				ImGui::Text("Width: %d", config.ScreenW);
				ImGui::Text("Height: %d", config.ScreenH);

				if (ImGui::Button("Attach Console"))
				{
					AllocConsole();
					freopen("CONIN$", "r", stdin);
					freopen("CONOUT$", "w", stdout);
					freopen("CONOUT$", "w", stderr);
				}

				ImGui::SameLine();

				if (ImGui::Button("Deattach Console"))
				{
					FreeConsole();
				}

				if (ImGui::Button("Refresh"))
				{
					std::cout << "Getting UWorld" << std::endl;
					global->pUWorld = GetUWorld(GWorldAddress, dUWorldOffset);
				}

				//	static float values[7] = { 0.0f, 0.60f, 0.35f, 0.9f, 0.70f, 0.20f, 0.0f };
				//	static int values_offset = 0;
				//	ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0, 80));

			}

			if (ImGui::CollapsingHeader("Config"))
			{
				if (ImGui::Button("Save"))
					SaveSettings();

				ImGui::SameLine();

				if (ImGui::Button("Load"))
					LoadSettings();


				ImGui::SliderInt("SleepRate", &config.iSleepRate, 1, 1000);
				if (ImGui::IsItemHovered())
				{
					ImGui::Text("Higher values will put less stress on CPU but will draw slower.");
				}
				ImGui::SliderFloat("OV Offset", &config.flOverlayOffset, 0.01, 1);
			}

			ImGui::End();
			ImGui::Render();
		}

		g_pRender->End();
		g_pRender->Present();

		Sleep(config.iSleepRate);

		if (DrawMenu)
		{
			RenderEnd = high_resolution_clock::now();
			duration = duration_cast<milliseconds>(RenderEnd - RenderBegin).count();
		}
	}
}

void PollSystem()
{
	VMProtectBeginVirtualization("PollSystem");
	ImGuiIO& io = ImGui::GetIO();
	io.ImeWindowHandle = global->hWnd;
	int mousex, mousey;
	io.DeltaTime = 1.0f / 60.0f;
	bool keys[256];
	BYTE KeyStates[256];

	int TemScreenW = 0, TempScreenH = 0;

	while (true)
	{
		g_pMaths->GetScreenSize(TemScreenW, TempScreenH);
		if (config.ScreenW != TemScreenW || config.ScreenH != TempScreenH)
		{
			int ScreenResW, ScreenResH;
			Utils::GetDesktopResolution(ScreenResW, ScreenResH);
			g_pMaths->UpdateScreenResolution(ScreenResH, ScreenResW);
		}

		if (config.bLogIds && config.bDebugMode)
		{
			namesDump.open("namesdump.txt");
			lootDump.open("lootdump.txt");

			config.bLogIds = false;
		}

		g_pInputSystem->UpdateMousePos(Diffx, Diffy);
		g_pInputSystem->GetMousePos(mousex, mousey);
		g_pInputSystem->PollInputState(true);

		io.MousePos.x = (float)(mousex);
		io.MousePos.y = (float)(mousey);

		GetKeyboardState(KeyStates);

		for (int i = 0; i < 256; i++)
		{
			if (KeyStates[i] & 0x80 && !io.KeysDown[i]) //key is down
			{
				io.KeysDown[i] = true;

				char res[2] = { 0 };
				if (ToAscii(i, MapVirtualKey(i, 0), (const BYTE*)KeyStates, (LPWORD)res, 0) == 1)
				{
					io.AddInputCharactersUTF8(res);
				}
			}
			else if (!(KeyStates[i] & 0x80) && io.KeysDown[i])
				io.KeysDown[i] = false;
		}

		if (g_pInputSystem->InputStates[LBUTTON_DOWN] == true)
		{
			io.MouseDown[0] = true;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (g_pInputSystem->InputStates[LBUTTON_CLICKED] == true)
		{
			io.MouseClicked[0] = true;
		}
		else {
			io.MouseClicked[0] = false;
		}

		if (g_pInputSystem->InputStates[KEY_INSERT_PRESSED])
		{
			DrawMenu = !DrawMenu;
		}
		//io.KeysDown += g_pInputSystem->GetKeyStates();

		Sleep(config.iSleepRate);
	}

	VMProtectEnd();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	VMProtectBeginUltra("WinMain");
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	MacAddress = std::string(Utils::getMAC());
	HDSerial = Utils::ws2s(Utils::GetHdSerial().GetString());
	HDSerial.erase(std::remove_if(HDSerial.begin(), HDSerial.end(), isspace), HDSerial.end());
	
	std::string OverlayWindowName = Utils::random_string(Utils::rangeRandomAlg2(5, 30));

	for (int i = 0; i < 6; i++)
	{
		bAmmo[i] = true;
		bMedical[i] = true;
		bScopes[i] = true;
	}

	for (int i = 0; i < 30; i++)
	{
		bAttachements[i] = true;
	}

	for (int i = 0; i < 20; i++)
	{
		bWeapon[i] = true;
	}

	if(Utils::File_Exsists("config.cfg"))
	{
		LoadSettings();
	} else
	{
		SaveSettings();
		LoadSettings();
	}

	if (!config.bAutoLogin)
	{
		std::cout << "Username: ";
		std::cin >> username;
		std::cout << std::endl << "Password: ";
		std::cin >> password;
	}
	else {
		username = config.username;
		password = config.password;
	}

	std::string reps = requestUpdate();

	if (reps.find("-") != std::string::npos)
	{
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "LOL NOPE", NULL, NULL);
		Utils::Selfdestruct();
		ExitProcess(0);
	}
	if (reps.find("*") != std::string::npos)
	{
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "Invalid login information", NULL, NULL);
		ExitProcess(0);
	}
	if (reps.find("%") != std::string::npos)
	{
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "Unknown ERROR", NULL, NULL);
		ExitProcess(0);
	}
	if (reps.find(")(") != std::string::npos)
	{
		Utils::ErasePEHeaderFromMemory();
		MessageBoxA(NULL, "No subscription", NULL, NULL);
		Utils::Selfdestruct();
		ExitProcess(0);
	}

	if (reps.find("()") != std::string::npos)
	{
		bVerified = true;
		LPWSTR *szArgList;
		int argCount;

		szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
		if (szArgList == NULL)
		{
			std::cout << "Parsing of command lines failed" << std::endl;
		}

		if (argCount == 2)
		{
			system("cls");
			std::cout << "Starting Intilization: " << std::endl;

			global = new Globals();
			global->hProcess = reinterpret_cast<void*>(atoi(Utils::ws2s(szArgList[1]).c_str()));
			LocalFree(szArgList); // Clean up no longer required. 

			mem = new Memory(global->hProcess);

			WNDCLASSEX wc;

			ZeroMemory(&wc, sizeof(WNDCLASSEX));

			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WindowProc;
			wc.hInstance = GetModuleHandle(0);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
			wc.lpszClassName = Utils::StringToWString(OverlayWindowName.c_str()).c_str();

			//Extras
			//wc.cbClsExtra = NULL;
			//wc.cbWndExtra = NULL;

			if (!RegisterClassEx(&wc))
				exit(1);

			float modifier = config.flOverlayOffset;

			global->hWnd = CreateWindowEx(NULL,
				Utils::StringToWString(OverlayWindowName).c_str(),
				Utils::StringToWString(OverlayWindowName).c_str(),
				WS_EX_TOPMOST | WS_POPUP,
				(config.ScreenW * modifier) / 2, (config.ScreenH * modifier) / 2,
				config.ScreenW - (config.ScreenW * modifier), config.ScreenH - (config.ScreenH * modifier),
				NULL,
				NULL,
				hInstance,
				NULL);


			/*SetLayeredWindowAttributes(global->hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
			SetLayeredWindowAttributes(global->hWnd, 0, 255, LWA_ALPHA);
			*/

			SetWindowLong(global->hWnd, GWL_EXSTYLE, (int)GetWindowLong(global->hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
			SetLayeredWindowAttributes(global->hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
			SetLayeredWindowAttributes(global->hWnd, 0, 255, LWA_ALPHA);

			ShowWindow(global->hWnd, nShowCmd);
			UpdateWindow(global->hWnd);

			g_pRender = new CRender(global->hWnd, true, config.ScreenW, config.ScreenH);
			
			if (!g_pRender->isCreated())
			{
				int ilength = strlen(Fonts[config.TextFont]);
				WCHAR    str3[255];
				ZeroMemory(str3, 255);
				MultiByteToWideChar(0, 0, Fonts[config.TextFont], ilength, str3, ilength + 1);
				g_pRender->CreateFonts(config.TextMultiplier, str3);

				ImGui_ImplDX9_Init(global->hWnd, g_pRender->GetDevice()); // ImGui
			}

			global->baseaddress = mem->GetModule(GameName);
			std::cout << std::hex  << global->baseaddress << std::endl;

			GWorldAddress = mem->FindPatternEx(global->hProcess, GameName, (BYTE*)"\x48\x8B\x1D\x00\x00\x00\x00\x74\x40", "xxx????xx", global->baseaddress);
			dUWorldOffset = mem->Read<DWORD>(GWorldAddress + 3) + 7;

		
			global->pUWorld = GetUWorld(GWorldAddress, dUWorldOffset);
			std::cout << std::hex << "global->pUWorld " << global->pUWorld << std::endl;

			UWorldOffSet = (GWorldAddress + dUWorldOffset) - global->baseaddress;
			std::cout << "UWorldOffSet " << UWorldOffSet << std::endl;

			global->pULevel = mem->Read<DWORD_PTR>((DWORD_PTR)global->pUWorld + off_ULevel);

			DWORD_PTR GNamesAddress = mem->FindPatternEx(global->hProcess, GameName,
				(BYTE*)"\x48\x89\x1D\x00\x00\x00\x00\x48\x8B\x5C\x24\x00\x48\x83\xC4\x28\xC3\x48\x8B\x5C\x24\x00\x48\x89\x05\x00\x00\x00\x00\x48\x83\xC4\x28\xC3",
				"xxx????xxxx?xxxxxxxxx?xxx????xxxxx", global->baseaddress);

			DWORD tGNamesOffset = mem->Read<DWORD>(GNamesAddress + 3);
			GNamesAddress += tGNamesOffset + 7;
			GNamesOffset = mem->Read<DWORD_PTR>(GNamesAddress);

			gNamesOffSet = GNamesAddress - global->baseaddress;
			
			//Local Player initialization
			g_pLocalPlayer.LocalPlayerController = mem->Read<DWORD_PTR>((DWORD_PTR)global->pLocalPlayer + off_ULocalPlayer);
			g_pLocalPlayer.pLocalPlayerCameraManager = mem->Read<DWORD_PTR>((DWORD_PTR)g_pLocalPlayer.LocalPlayerController + off_CameraManager);

			ImGuiStyle& style = ImGui::GetStyle();
			style.AntiAliasedLines = true;
			style.AntiAliasedShapes = true;

			style.WindowPadding = ImVec2(15,15);
			style.WindowRounding = 3;
			style.ChildWindowRounding = 0;
			style.FramePadding = ImVec2(5, 5);
			style.FrameRounding = 3;
			style.ItemSpacing = ImVec2(12, 8);
			style.ItemInnerSpacing = ImVec2(8, 6);
			style.TouchExtraPadding = ImVec2(0, 0);
			style.IndentSpacing = 20;
			style.ScrollbarSize = 20;
			style.ScrollbarRounding = 3;
			style.GrabMinSize = 5;
			style.FrameRounding = 3;

			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);

			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
			style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.10f, 0.10f, 0.10f, 0.88f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.13f, 0.13f, 0.13f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
			style.Colors[ImGuiCol_ComboBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.96f, 0.00f, 1.00f, 0.00f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.93f, 0.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_CloseButton] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.73f, 0.11f, 0.09f, 0.59f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.41f, 0.41f, 0.41f, 0.63f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.11f, 0.09f, 1.00f);
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 1.00f, 1.00f, 0.73f);

			std::cout << "Style" << std::endl;

			//Get Actor IDS
			{
				//Cache stuff
				g_pCache.DroppedItemInteractionComponent = GetClassNameIndex("DroppedItemInteractionComponent");
				g_pCache.DroppedItemGroup = GetClassNameIndex("DroppedItemGroup");
				g_pCache.PlayerFemale_A_C = GetClassNameIndex("PlayerFemale_A_C");
				g_pCache.PlayerMale_A = GetClassNameIndex("PlayerMale_A");
				g_pCache.PlayerMale_A_C = GetClassNameIndex("PlayerMale_A_C");
				g_pCache.PlayerFemale_A = GetClassNameIndex("PlayerFemale_A");
				g_pCache.DeathDropItemPackage_C = GetClassNameIndex("DeathDropItemPackage_C");
				g_pCache.Boat_PG117_C = GetClassNameIndex("Boat_PG117_C");
				g_pCache.Dacia_A_01_C = GetClassNameIndex("Dacia_A_01_C");
				g_pCache.Dacia_A_02_C = GetClassNameIndex("Dacia_A_02_C");
				g_pCache.Dacia_A_03_C = GetClassNameIndex("Dacia_A_03_C");
				g_pCache.BP_Motorbike_04_SideCar_C = GetClassNameIndex("BP_Motorbike_04_SideCar_C");
				g_pCache.BP_Motorbike_03_C = GetClassNameIndex("BP_Motorbike_03_C");
				g_pCache.BP_Motorbike_04_C = GetClassNameIndex("BP_Motorbike_04_C");
				g_pCache.Buggy_A_01_C = GetClassNameIndex("Buggy_A_01_C");
				g_pCache.Buggy_A_03_C = GetClassNameIndex("Buggy_A_03_C");
				g_pCache.Buggy_A_02_C = GetClassNameIndex("Buggy_A_02_C");
				g_pCache.Dacia_A_04_C = GetClassNameIndex("Dacia_A_04_C");
				g_pCache.Uaz_B_01_C = GetClassNameIndex("Uaz_B_01_C");
				g_pCache.Uaz_A_01_C = GetClassNameIndex("Uaz_A_01_C");
				g_pCache.Uaz_C_01_C = GetClassNameIndex("Uaz_C_01_C");
				g_pCache.Carapackage_RedBox_C = GetClassNameIndex("Carapackage_RedBox_C");
			}

			/*
			config.bNoRecoil = false;
			config.bNoSway = false;
			config.bFastFireRate = false;
			config.bFastBullets = false;
			config.bNoMuzzle = false;
			config.bNoSpread = false;
			*/

			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RenderAndUpdateThread, 0, 0, 0); // Start Worker Thread
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)OtherStuffThread, 0, 0, 0); // Start Worker Thread
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TickSystem, 0, 0, 0); // Start Worker Thread
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)PollSystem, 0, 0, 0); // Start Worker Thread
			std::cout << "initialization Successful" << std::endl;

			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				HWND hwnd2 = GetForegroundWindow();
				HWND hwnd3 = GetWindow(hwnd2, GW_HWNDPREV);
				SetWindowPos(global->hWnd, hwnd3, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				Sleep(config.iSleepRate); // optimization 
			}

		}
	}

	

	return msg.wParam;
	VMProtectEnd();
}