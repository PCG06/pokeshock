import json


#todo: test with origin/upcoming

#C string vars
define                = "#define"
ENCOUNTER_CHANCE      = "ENCOUNTER_CHANCE"
SLOT                  = "SLOT"
TOTAL                 = "TOTAL"
NULL                  = "NULL"
UNDEFINED             = "UNDEFINED"
MAP_UNDEFINED         = "MAP_UNDEFINED"

#encounter group header types
WILD_MON              = "gWildMon"
BATTLE_PIKE_MON       = "gBattlePikeWildMon"
BATTLE_PYRAMID_MON    = "gBattlePyramidWildMon"

#mon encounter group types
LAND_MONS             = "land_mons"
LAND_MONS_LABEL       = "LandMons"
LAND_MONS_INDEX       = 0
WATER_MONS            = "water_mons"
WATER_MONS_LABEL      = "WaterMons"
WATER_MONS_INDEX      = 1
ROCK_SMASH_MONS       = "rock_smash_mons"
ROCK_SMASH_MONS_LABEL = "RockSmashMons"
ROCK_SMASH_MONS_INDEX = 2
FISHING_MONS          = "fishing_mons"
FISHING_MONS_LABEL    = "FishingMons"
FISHING_MONS_INDEX    = 3
HIDDEN_MONS           = "hidden_mons"
HIDDEN_MONS_LABEL     = "HiddenMons"
HIDDEN_MONS_INDEX     = 4
MONS_INFO_TOTAL       = HIDDEN_MONS_INDEX + 1

#fishing encounter data
GOOD_ROD              = "good_rod"
GOOD_ROD_FIRST_INDEX  = 2
GOOD_ROD_LAST_INDEX   = 4
OLD_ROD               = "old_rod"
OLD_ROD_FIRST_INDEX   = 0
OLD_ROD_LAST_INDEX    = 1
SUPER_ROD             = "super_rod"
SUPER_ROD_FIRST_INDEX = 5
SUPER_ROD_LAST_INDEX  = 9

TIME_MORNING       = "time_morning"
TIME_MORNING_LABEL = "Morning"
TIME_MORNING_INDEX = 0
TIME_DAY           = "time_day"
TIME_DAY_LABEL     = "Day"
TIME_DAY_INDEX     = 1
TIME_EVENING       = "time_evening"
TIME_EVENING_LABEL = "Evening"
TIME_EVENING_INDEX = 2
TIME_NIGHT         = "time_night"
TIME_NIGHT_LABEL   = "Night"
TIME_NIGHT_INDEX   = 3
TOTAL_TIME_STAGES  = TIME_NIGHT_INDEX + 1

#struct building blocks
baseStruct    = "const struct WildPokemon"
structLabel   = ""
structMonType = ""
structTime    = ""
structMap     = ""

structInfo    = "Info"
structHeader  = "Header"
structArrayAssign   = "[] ="

baseStructLabel     = ""
baseStructContent   = []
infoStructString     = ""
infoStructRate      = 0
infoStructContent   = []
headerStructLabel   = ""
headerStructContent = {}
headerStructTable   = {}

headerIndex = 0

#map header data variables
hLabel = ""
hForMaps = True
headersArray = [headerIndex]

#headersArrayItems
landMonsInfo      = ""
waterMonsInfo     = ""
rockSmashMonsInfo = ""
fishingMonsInfo   = ""

#encounter rate variables
eLandMons      = []
eWaterMons     = []
eRockSmashMons = []
eFishingMons   = []


#debug output control
printEncounterHeaders = True
printEncounterRateMacros = True
printEncounterStructsInfoString = True
printEncounterStructs = True


def ImportWildEncounterFile():
    global landMonsInfo
    global waterMonsInfo 
    global rockSmashMonsInfo 
    global fishingMonsInfo
    global hiddenMonsInfo
    global structLabel 
    global structMonType 
    global structTime
    global structMap 
    global baseStructLabel 
    global baseStructContent 
    global infoStructString 
    global infoStructRate
    global headerStructLabel
    global headerStructContent
    global hLabel
    global headersArray
    global eLandMons
    global eWaterMons
    global eRockSmashMons
    global eFishingMons
    global encounterTotalCount
    global encounterCount
    global headerIndex
    global tabStr
    tabStr = "    "

    wFile = open("wild_encounters.json")
    wData = json.load(wFile)

    encounterTotalCount = []
    encounterCount = []
    groupCount = 0
    while groupCount < len(wData["wild_encounter_groups"]):
        encounterTotalCount.append(0)
        encounterCount.append(0)
        groupCount += 1
        
    #print(len(encounterTotalCount))

    for data in wData["wild_encounter_groups"]:
        #print(data)

        wEncounters = wData["wild_encounter_groups"][headerIndex]["encounters"]

        if data == "label":
            hLabel = wData["wild_encounter_groups"][headerIndex]["label"]
        if data == "for_maps":
            hForMaps = wData["wild_encounter_groups"][headerIndex]["for_maps"]
    
        if headerIndex == 0:
            wFields = wData["wild_encounter_groups"][headerIndex]["fields"]
            for field in wFields:
                if field["type"] == LAND_MONS:
                    eLandMons = field["encounter_rates"]
                elif field["type"] == WATER_MONS:
                    eWaterMons = field["encounter_rates"]
                elif field["type"] == ROCK_SMASH_MONS:
                    eRockSmashMons = field["encounter_rates"]
                elif field["type"] == FISHING_MONS:
                    eFishingMons = field["encounter_rates"]
                    eFishingMons.append(field["groups"])

            print('#include "rtc.h" \n')
            PrintEncounterRateMacros()
            print()
        for encounter in wEncounters:
            if "map" in encounter:
                structMap = encounter["map"]
            else:
                structMap = encounter["base_label"]

            structLabel = encounter["base_label"]
            
            if encounterTotalCount[headerIndex] != len(wEncounters):
                encounterTotalCount[headerIndex] = len(wEncounters)
            
            encounterCount[headerIndex] += 1

            headersArray = []
            for time in encounter["encounter_times"]:
                if TIME_MORNING in time:
                    structTime = TIME_MORNING_LABEL
                elif TIME_DAY in time:
                    structTime = TIME_DAY_LABEL
                elif TIME_EVENING in time:
                    structTime = TIME_EVENING_LABEL
                elif TIME_NIGHT in time:
                    structTime = TIME_NIGHT_LABEL
                else:
                    structTime = ""

                landMonsInfo      = ""
                waterMonsInfo     = ""
                rockSmashMonsInfo = ""
                fishingMonsInfo   = ""
                hiddenMonsInfo    = ""
                for encounterTable in time:
                    for areaTable in time[encounterTable]:
                        if LAND_MONS in areaTable:
                            structMonType = LAND_MONS_LABEL
                            landMonsInfo = f"{structLabel}_{structMonType}{structInfo}_{structTime}"
                        elif WATER_MONS in areaTable:
                            structMonType = WATER_MONS_LABEL
                            waterMonsInfo = f"{structLabel}_{structMonType}{structInfo}_{structTime}"
                        elif ROCK_SMASH_MONS in areaTable:
                            structMonType = ROCK_SMASH_MONS_LABEL
                            rockSmashMonsInfo = f"{structLabel}_{structMonType}{structInfo}_{structTime}"
                        elif FISHING_MONS in areaTable:
                            structMonType = FISHING_MONS_LABEL
                            fishingMonsInfo = f"{structLabel}_{structMonType}{structInfo}_{structTime}"
                        else:
                            hiddenMonsInfo = ""
                            structMonType = ""
                        
                        baseStructContent = []
                        for monTable in areaTable:
                            for group in areaTable[monTable]:
                                if "mons" in group:
                                    for mon in areaTable[monTable][group]:
                                        baseStructContent.append(list(mon.values()))
                                if "encounter_rate" in group:
                                    infoStructRate = areaTable[monTable][group]
                        
                        baseStructLabel = f"{baseStruct} {structLabel}_{structMonType}_{structTime}{structArrayAssign}"
                        if printEncounterStructs:
                            print()
                            print(baseStructLabel)
                            print("{")
                            PrintStructContent(baseStructContent)
                            print("};")
                        if printEncounterStructsInfoString:
                            infoStructString = f"{baseStruct}{structInfo} {structLabel}_{structMonType}{structInfo}_{structTime} = {{ {infoStructRate}, {structLabel}_{structMonType}_{structTime} }};"
                            print(infoStructString)

                    AssembleMonHeaderContent()
                
        headerIndex += 1
    PrintWildMonHeadersContent()
    """
    for group in headerStructTable:
        for label in headerStructTable[group]:
            print(headerStructTable[group][label])
    """


def PrintStructContent(contentList):
    for monList in contentList:
        print(f"{tabStr}{{ {monList[0]}, {monList[1]}, {monList[2]} }},")
    return


def AssembleMonHeaderContent():
    global structLabel

    SetupMonInfoVars()
    tempHeaderLabel = GetWildMonHeadersLabel()
    tempHeaderTimeIndex = GetTimeIndexFromString(structTime)
    
    if tempHeaderLabel not in headerStructTable:
        headerStructTable[tempHeaderLabel] = {}
        headerStructTable[tempHeaderLabel]["groupNum"] = headerIndex

    if structLabel not in headerStructTable[tempHeaderLabel]:
        #print(structLabel)
        headerStructTable[tempHeaderLabel][structLabel] = {}
        headerStructTable[tempHeaderLabel][structLabel]["headerType"] = GetWildMonHeadersLabel()
        headerStructTable[tempHeaderLabel][structLabel]["mapGroup"] = structMap
        headerStructTable[tempHeaderLabel][structLabel]["mapNum"] = structMap
        headerStructTable[tempHeaderLabel][structLabel]["encounterTotalCount"] = encounterTotalCount[headerIndex]
        headerStructTable[tempHeaderLabel][structLabel]["encounter_types"] = []

        i = 0
        while i <= TIME_NIGHT_INDEX:
            headerStructTable[tempHeaderLabel][structLabel]["encounter_types"].append([])
            i += 1

    headerStructTable[tempHeaderLabel][structLabel]["encounter_types"][tempHeaderTimeIndex].append(landMonsInfo)
    headerStructTable[tempHeaderLabel][structLabel]["encounter_types"][tempHeaderTimeIndex].append(waterMonsInfo)
    headerStructTable[tempHeaderLabel][structLabel]["encounter_types"][tempHeaderTimeIndex].append(rockSmashMonsInfo)
    headerStructTable[tempHeaderLabel][structLabel]["encounter_types"][tempHeaderTimeIndex].append(fishingMonsInfo)
    headerStructTable[tempHeaderLabel][structLabel]["encounter_types"][tempHeaderTimeIndex].append(hiddenMonsInfo)

    #headerStructTable[tempHeaderLabel].update(headerStructContent[structLabel])


def SetupMonInfoVars():
    global landMonsInfo
    global waterMonsInfo 
    global rockSmashMonsInfo 
    global fishingMonsInfo
    global hiddenMonsInfo

    if landMonsInfo == "":
        landMonsInfo = NULL
    else:
        landMonsInfo = f"&{landMonsInfo}"
    if waterMonsInfo == "":
        waterMonsInfo = NULL
    else:
        waterMonsInfo = f"&{waterMonsInfo}"
    if rockSmashMonsInfo == "":
        rockSmashMonsInfo = NULL
    else:
        rockSmashMonsInfo = f"&{rockSmashMonsInfo}"
    if fishingMonsInfo == "":
        fishingMonsInfo = NULL
    else:
        fishingMonsInfo = f"&{fishingMonsInfo}"
    if hiddenMonsInfo == "":
        hiddenMonsInfo = NULL
    else:
        hiddenMonsInfo = f"&{hiddenMonsInfo}"

    

def PrintWildMonHeadersContent():
    global tabStr

    groupCount = 0
    for group in headerStructTable:
        labelCount = 0
        for label in headerStructTable[group]:
            if label != "groupNum":
                if labelCount == 0:
                    PrintEncounterHeaders("\n")
                    PrintEncounterHeaders(headerStructTable[group][label]["headerType"])

                PrintEncounterHeaders(tabStr + "{")
                for stat in headerStructTable[group][label]:
                    mapData = headerStructTable[group][label][stat]

                    if stat == "mapGroup":
                        PrintEncounterHeaders(f"{tabStr}{tabStr}.mapGroup = {GetMapGroupEnum(mapData)},")
                    elif stat == "mapNum":
                        PrintEncounterHeaders(f"{tabStr}{tabStr}.mapNum = {GetMapGroupEnum(mapData, labelCount + 1)},")

                    if type(headerStructTable[group][label][stat]) == list:
                        PrintEncounterHeaders(f"{tabStr}{tabStr}.encounterTypes =")
                        PrintEncounterHeaders(tabStr + tabStr + "{")

                        infoCount = 0
                        for monInfo in headerStructTable[group][label][stat]:
                            infoIndex = 0
                            PrintEncounterHeaders(f"{tabStr}{tabStr}{tabStr}[{GetTimeStrFromIndex(infoCount)}] = ")
                            for timeGroup in headerStructTable[group][label][stat][infoIndex]:
                                if infoIndex == 0:
                                    PrintEncounterHeaders(tabStr + tabStr + tabStr + "{")
                                PrintEncounterHeaders(f"{tabStr}{tabStr}{tabStr}{tabStr}{GetIMonInfoStringFromIndex(infoIndex)} = {monInfo[infoIndex]},")
                                if infoIndex == MONS_INFO_TOTAL - 1:
                                    PrintEncounterHeaders(tabStr + tabStr + tabStr + "},")
                                infoIndex += 1
                            infoCount += 1
                        PrintEncounterHeaders(tabStr + tabStr + "},")
                PrintEncounterHeaders(tabStr + "},")
                if labelCount + 1 == headerStructTable[group][label]["encounterTotalCount"]:
                    PrintEncounterHeaders(tabStr + "{")
                    PrintEncounterHeaders(f"{tabStr}{tabStr}.mapGroup = {GetMapGroupEnum(MAP_UNDEFINED)},")
                    PrintEncounterHeaders(f"{tabStr}{tabStr}.mapNum = {GetMapGroupEnum(MAP_UNDEFINED, labelCount + 1)},")

                    nullCount = 0
                    while nullCount <= TIME_NIGHT_INDEX:
                        if nullCount == 0:
                            PrintEncounterHeaders(f"{tabStr}{tabStr}.encounterTypes =")
                            PrintEncounterHeaders(tabStr + tabStr + "{")
                        nullIndex = 0
                        PrintEncounterHeaders(f"{tabStr}{tabStr}{tabStr}[{GetTimeStrFromIndex(nullCount)}] = ")
                        while nullIndex <= MONS_INFO_TOTAL - 1:
                            if nullIndex == 0:
                                PrintEncounterHeaders(tabStr + tabStr + tabStr + "{")
                            PrintEncounterHeaders(f"{tabStr}{tabStr}{tabStr}{tabStr}{GetIMonInfoStringFromIndex(nullIndex)} = NULL,")
                            if nullIndex == MONS_INFO_TOTAL - 1:
                                PrintEncounterHeaders(tabStr + tabStr + tabStr + "},")
                            nullIndex += 1
                        nullCount += 1
                    PrintEncounterHeaders(tabStr + tabStr + "},")
                    PrintEncounterHeaders(tabStr + "},")
                labelCount += 1
        groupCount += 1
        PrintEncounterHeaders("};")


def GetWildMonHeadersLabel():
    if headerIndex == 0:
        return f"{baseStruct}{structHeader} {WILD_MON}{structHeader}s{structArrayAssign}" + "\n{"
    elif headerIndex == 1:
        return f"{baseStruct}{structHeader} {BATTLE_PYRAMID_MON}{structHeader}s{structArrayAssign}" + "\n{"
    elif headerIndex == 2:
        return f"{baseStruct}{structHeader} {BATTLE_PIKE_MON}{structHeader}s{structArrayAssign}" + "\n{"
    

def PrintEncounterHeaders(content):
    if printEncounterHeaders:
        print(content)


def PrintEncounterRateMacros():
    if printEncounterRateMacros:
        rateCount = 0
        for percent in eLandMons:
            if rateCount == 0:
                print(f"{define} {ENCOUNTER_CHANCE}_{LAND_MONS.upper()}_{SLOT}_{rateCount} {percent}")
            else:
                print(
                    f"{define} {ENCOUNTER_CHANCE}_{LAND_MONS.upper()}_{SLOT}_{rateCount} {ENCOUNTER_CHANCE}_{LAND_MONS.upper()}_{SLOT}_{rateCount - 1} + {percent}"
                )

            if rateCount + 1 == len(eLandMons):
                print(
                    f"{define} {ENCOUNTER_CHANCE}_{LAND_MONS.upper()}_{TOTAL} ({ENCOUNTER_CHANCE}_{LAND_MONS.upper()}_{SLOT}_{rateCount})"
                )
            rateCount += 1

        rateCount = 0
        for percent in eWaterMons:
            if rateCount == 0:
                print(f"{define} {ENCOUNTER_CHANCE}_{WATER_MONS.upper()}_{SLOT}_{rateCount} {percent}")
            else:
                print(
                    f"{define} {ENCOUNTER_CHANCE}_{WATER_MONS.upper()}_{SLOT}_{rateCount} {ENCOUNTER_CHANCE}_{WATER_MONS.upper()}_{SLOT}_{rateCount - 1} + {percent}"
                )

            if rateCount + 1 == len(eWaterMons):
                print(
                    f"{define} {ENCOUNTER_CHANCE}_{WATER_MONS.upper()}_{TOTAL} ({ENCOUNTER_CHANCE}_{WATER_MONS.upper()}_{SLOT}_{rateCount})"
                )
            rateCount += 1

        rateCount = 0
        for percent in eRockSmashMons:
            if rateCount == 0:
                print(f"{define} {ENCOUNTER_CHANCE}_{ROCK_SMASH_MONS.upper()}_{SLOT}_{rateCount} {percent}")
            else:
                print(
                    f"{define} {ENCOUNTER_CHANCE}_{ROCK_SMASH_MONS.upper()}_{SLOT}_{rateCount} {ENCOUNTER_CHANCE}_{ROCK_SMASH_MONS.upper()}_{SLOT}_{rateCount - 1} + {percent}"
                )
            
            if rateCount + 1 == len(eRockSmashMons):
                print(
                    f"{define} {ENCOUNTER_CHANCE}_{ROCK_SMASH_MONS.upper()}_{TOTAL} ({ENCOUNTER_CHANCE}_{ROCK_SMASH_MONS.upper()}_{SLOT}_{rateCount})"
                )
            rateCount += 1

        for rodRate in eFishingMons[-1]:
            for rodPercentIndex in eFishingMons[-1][rodRate]:
                if rodPercentIndex == OLD_ROD_FIRST_INDEX or rodPercentIndex == GOOD_ROD_FIRST_INDEX or rodPercentIndex == SUPER_ROD_FIRST_INDEX:
                    print(
                        f"{define} {ENCOUNTER_CHANCE}_{FISHING_MONS.upper()}_{rodRate.upper()}_{SLOT}_{rodPercentIndex} {eFishingMons[rodPercentIndex]}"
                    )
                else:
                    print(
                        f"{define} {ENCOUNTER_CHANCE}_{FISHING_MONS.upper()}_{rodRate.upper()}_{SLOT}_{rodPercentIndex} {ENCOUNTER_CHANCE}_{FISHING_MONS.upper()}_{rodRate.upper()}_{SLOT}_{rodPercentIndex - 1} + {eFishingMons[rodPercentIndex]}"
                    )
                
                if rodPercentIndex == OLD_ROD_LAST_INDEX or rodPercentIndex == GOOD_ROD_LAST_INDEX or rodPercentIndex == SUPER_ROD_LAST_INDEX:
                    print(
                        f"{define} {ENCOUNTER_CHANCE}_{FISHING_MONS.upper()}_{rodRate.upper()}_{TOTAL} ({ENCOUNTER_CHANCE}_{FISHING_MONS.upper()}_{rodRate.upper()}_{SLOT}_{rodPercentIndex})"
                    )


def GetTimeStrFromIndex(index):
    if index == TIME_MORNING_INDEX:
        return TIME_MORNING.upper()
    elif index == TIME_DAY_INDEX:
        return TIME_DAY.upper()
    elif index == TIME_EVENING_INDEX:
        return TIME_EVENING.upper()
    elif index == TIME_NIGHT_INDEX:
        return TIME_NIGHT.upper()
    return index


def GetTimeIndexFromString(string):
    if string.lower() == TIME_MORNING or string == TIME_MORNING_LABEL:
        return TIME_MORNING_INDEX
    elif string.lower() == TIME_DAY or string == TIME_DAY_LABEL:
        return TIME_DAY_INDEX
    elif string.lower() == TIME_EVENING or string == TIME_EVENING_LABEL:
        return TIME_EVENING_INDEX
    elif string.lower() == TIME_NIGHT or string == TIME_NIGHT_LABEL:
        return TIME_NIGHT_INDEX
    return string


def GetIMonInfoStringFromIndex(index):
    if index == LAND_MONS_INDEX:
        return ".landMonsInfo"
    elif index == WATER_MONS_INDEX:
        return ".waterMonsInfo"
    elif index == ROCK_SMASH_MONS_INDEX:
        return ".rockSmashMonsInfo"
    elif index == FISHING_MONS_INDEX:
        return ".fishingMonsInfo"
    elif index == HIDDEN_MONS_INDEX:
        return ".hiddenMonsInfo"
    return index

def GetMapGroupEnum(string, index = 0):
    if "MAP_" in string and index == 0:
        return "MAP_GROUP(" + string[4:len(string)] + ")"
    elif "MAP_" in string and index != 0:
        return "MAP_NUM(" + string[4:len(string)] + ")"
    return index


ImportWildEncounterFile()


""" !!!! EXAMPLE TEXT !!!!
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_0 20 
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_1 ENCOUNTER_CHANCE_LAND_MONS_SLOT_0 + 20
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_2 ENCOUNTER_CHANCE_LAND_MONS_SLOT_1 + 10
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_3 ENCOUNTER_CHANCE_LAND_MONS_SLOT_2 + 10
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_4 ENCOUNTER_CHANCE_LAND_MONS_SLOT_3 + 10
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_5 ENCOUNTER_CHANCE_LAND_MONS_SLOT_4 + 10
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_6 ENCOUNTER_CHANCE_LAND_MONS_SLOT_5 + 5
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_7 ENCOUNTER_CHANCE_LAND_MONS_SLOT_6 + 5
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_8 ENCOUNTER_CHANCE_LAND_MONS_SLOT_7 + 4
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_9 ENCOUNTER_CHANCE_LAND_MONS_SLOT_8 + 4
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_10 ENCOUNTER_CHANCE_LAND_MONS_SLOT_9 + 1
#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_11 ENCOUNTER_CHANCE_LAND_MONS_SLOT_10 + 1
#define ENCOUNTER_CHANCE_LAND_MONS_TOTAL (ENCOUNTER_CHANCE_LAND_MONS_SLOT_11)
#define ENCOUNTER_CHANCE_WATER_MONS_SLOT_0 60 
#define ENCOUNTER_CHANCE_WATER_MONS_SLOT_1 ENCOUNTER_CHANCE_WATER_MONS_SLOT_0 + 30
#define ENCOUNTER_CHANCE_WATER_MONS_SLOT_2 ENCOUNTER_CHANCE_WATER_MONS_SLOT_1 + 5
#define ENCOUNTER_CHANCE_WATER_MONS_SLOT_3 ENCOUNTER_CHANCE_WATER_MONS_SLOT_2 + 4
#define ENCOUNTER_CHANCE_WATER_MONS_SLOT_4 ENCOUNTER_CHANCE_WATER_MONS_SLOT_3 + 1
#define ENCOUNTER_CHANCE_WATER_MONS_TOTAL (ENCOUNTER_CHANCE_WATER_MONS_SLOT_4)
#define ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_0 60 
#define ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_1 ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_0 + 30
#define ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_2 ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_1 + 5
#define ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_3 ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_2 + 4
#define ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_4 ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_3 + 1
#define ENCOUNTER_CHANCE_ROCK_SMASH_MONS_TOTAL (ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_4)
#define ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2 60 
#define ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3 ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2 + 20
#define ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4 ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3 + 20
#define ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_TOTAL (ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4)
#define ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0 70 
#define ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_1 ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0 + 30
#define ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_TOTAL (ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_1)
#define ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5 40 
#define ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6 ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5 + 40
#define ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7 ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6 + 15
#define ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8 ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7 + 4
#define ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9 ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8 + 1
#define ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_TOTAL (ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9)

const struct WildPokemon gRoute101_LandMons_Day[] =
{
    { 2, 2, SPECIES_WURMPLE },
    { 2, 2, SPECIES_POOCHYENA },
    { 2, 2, SPECIES_WURMPLE },
    { 3, 3, SPECIES_WURMPLE },
    { 3, 3, SPECIES_POOCHYENA },
    { 3, 3, SPECIES_POOCHYENA },
    { 3, 3, SPECIES_WURMPLE },
    { 3, 3, SPECIES_POOCHYENA },
    { 2, 2, SPECIES_ZIGZAGOON },
    { 2, 2, SPECIES_ZIGZAGOON },
    { 3, 3, SPECIES_ZIGZAGOON },
    { 3, 3, SPECIES_ZIGZAGOON },
};

const struct WildPokemonInfo gRoute101_LandMonsInfo_Day= { 20, gRoute101_LandMons_Day };
const struct WildPokemonHeader gWildMonHeaders[] =
{
    {
        .mapGroup = MAP(ROUTE101),
        .mapNum = MAP_NUM(ROUTE101),
        .encounterTypes = 
            [TIME_DAY] = 
            {
                .landMonsInfo = &gRoute101_LandMonsInfo_Day,
                .waterMonsInfo = NULL,
                .rockSmashMonsInfo = NULL,
                .fishingMonsInfo = NULL,
                .hiddenMonsInfo = NULL,
            }
    },
}
"""