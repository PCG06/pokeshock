#include "global.h"
#include "main.h"
#include "bg.h"
#include "gpu_regs.h"
#include "sprite.h"
#include "window.h"
#include "decompress.h"
#include "text.h"
#include "characters.h"
#include "string_util.h"
#include "palette.h"
#include "palette_util.h"
#include "malloc.h"
#include "task.h"
#include "menu.h"
#include "scanline_effect.h"
#include "sound.h"
#include "strings.h"
#include "trainer_pokemon_sprites.h"
#include "pokeball.h"
#include "kaba_speech.h"
#include "constants/rgb.h"
#include "constants/songs.h"

/*! @brief
 * This file controls Elder Kaba's speech sequence, made
 * seperately from src/main_menu.c to avoid too big of
 * conflicts and just makes the sequence much cleaner to
 * read.
 *
 * Below is the order of functions that'll run the intro.
 * - DoKabaSpeech
 *   - Move the game state into Elder Kaba's Speech.
 *
 * - Task_KabaSpeech_Begin
 *   - Start the process of loading the intro resources.
 */

// Define, enum, struct definitions

// If you want to edit these pos defines, dont touch the `+ 32`!
// positions for joltik + pokeball
#define MON_POS_X 54 + 32
#define MON_POS_Y 66 + 32
// positions for platform (they're split into two sprites!)
#define PLAT_POS_X 56 + 32
#define PLAT_POS_Y 83 + 16
// this is pos difference for the second platform sprite
#define PLAT_POS_X2 64

enum SpriteTags
{
    TAG_PLATFORM = 0x9000,
};

enum Bgs
{
    // self explanatory
    BG_TEXT = 0,
    // generic names as their usage is both for
    // kaba and player/rival sprites
    BG_PIC_1,
    BG_PIC_2,
    // BG_BG feels stupid but this isn't any better either,
    // suggestions welcome (for context this is for the red bg)
    BG_INTRO,
    BG_COUNT,
};

enum WindowIds
{
    WIN_TEXT = 0,
    WIN_YESNO,
    WIN_AO,
    WIN_AKA,
    WIN_COUNT,
};

enum SpeechLoadStates
{
    STATE_RESET = 0,
    STATE_ALLOC,
    STATE_REGS,
    STATE_BGS,
    STATE_BG_GFX,
    STATE_WINDOWS,
    STATE_FINISH,
    STATE_COUNT,
};

enum PicIds
{
    PIC_AKA = 0,
    PIC_AO,
    PIC_KABA,
    PIC_COUNT
};

struct KabaSpeech
{
    u16 introTilemapBuffer[0x800];
    u16 pic1TilemapBuffer[0x800];
    u16 pic2TilemapBuffer[0x800];
    u8 monSpriteId;
    u8 ballSpriteId;
    u8 platformSpriteIds[2]; // the platform is made out of 2 64x32sprites
    u16 alphaCoeff;
    u16 timer;
};

// EWRAM data
static EWRAM_DATA struct KabaSpeech *sKabaSpeech = NULL;

// Function declarations
static void Task_KabaSpeech_Begin(u8);
static void Task_KabaSpeech_FadeInEverything(u8);
static void Task_KabaSpeech_GreetingsTraveler(u8);
static void Task_KabaSpeech_AndThis(u8);
static void Task_KabaSpeech_ReleaseJoltikFromPokeball(u8);
static void Task_KabaSpeech_JoltikAPokemon(u8);
static void Task_KabaSpeech_MainTalk(u8);
static void Task_KabaSpeech_ReturnJoltik(u8);
static void Task_KabaSpeech_FadeOutPokeball(u8);
static void Task_KabaSpeech_FadeOutPlatform(u8);
static void Task_KabaSpeech_SpawnPlayerChoice(u8);
static void Task_KabaSpeech_WaitForChoice(u8);

static void KabaSpeech_DrawCharacterPic(u8);
static inline void KabaSpeech_PrintMessageBox(const u8 *);
static void KabaSpeech_CreateMonSprite(void);
static void KabaSpeech_CreatePlatformSprites(void);

// Const data
static const u8 sKabaSpeech_Greetings[] = _(
    "Greetings, young traveler.\p"
    "And welcome to the world of Pokémon.\p"
    "I am Kaba, one of the four elders of\n"
    "the Toku Region.\p"
    "However, you can call me Elder Kaba\n"
    "instead.\p"
);
static const u8 sKabaSpeech_AndThis[] = _(
    "And this…"
);
static const u8 sKabaSpeech_JoltikAPokemon[] = _(
    "…is Joltik, a Pokémon creature.{PAUSE 30}\p"
);
static const u8 sKabaSpeech_MainTalk[] = _(
    "Joltik, and other Pokémon creatures,\n"
    "initially lives seperately from us,\l"
    "humans.\p"
    "However, thanks to a latest discovery,\n"
    "we're now able capture and befriend\l"
    "these creatures with this special\l"
    "device we call Pokéball.\p"
    "More here blah blah i ran out of ideas\p"
);
static const u8 sKabaSpeech_GenderChoice[] = _(
    "boy or girl ?"
);

static const u16 sKabaSpeech_BgGfx[] = INCBIN_U16("graphics/kaba_speech/bg.4bpp");
static const u16 sKabaSpeech_BgPal[] = INCBIN_U16("graphics/kaba_speech/bg.gbapal");
static const u32 sKabaSpeech_BgMap[] = INCBIN_U32("graphics/kaba_speech/bg.bin.lz");

static const u16 sKabaSpeech_KabaPicGfx[] = INCBIN_U16("graphics/kaba_speech/pics/kaba.4bpp");
static const u16 sKabaSpeech_KabaPicPal[] = INCBIN_U16("graphics/kaba_speech/pics/kaba.gbapal");
static const u32 sKabaSpeech_KabaPicMap[] = INCBIN_U32("graphics/kaba_speech/pics/kaba.bin.lz");

//! NOTE: both aka and ao has the same palette as of writing
static const u16 sKabaSpeech_PlayerPicPal[] = INCBIN_U16("graphics/kaba_speech/pics/ao.gbapal");

static const u16 sKabaSpeech_AkaPicGfx[] = INCBIN_U16("graphics/kaba_speech/pics/aka.4bpp");
static const u8 sKabaSpeech_AkaPicMap[] = INCBIN_U8("graphics/kaba_speech/pics/aka.bin");

static const u16 sKabaSpeech_AoPicGfx[] = INCBIN_U16("graphics/kaba_speech/pics/ao.4bpp");
static const u8 sKabaSpeech_AoPicMap[] = INCBIN_U8("graphics/kaba_speech/pics/ao.bin");

static const u16 sKabaSpeech_PlatformGfx[] = INCBIN_U16("graphics/kaba_speech/platform.4bpp");
static const u16 sKabaSpeech_PlatformPal[] = INCBIN_U16("graphics/kaba_speech/platform.gbapal");

static const struct BgTemplate sKabaSpeech_BgTemplates[BG_COUNT] =
{
    [BG_TEXT] = {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 0,
        // other unspecified attributes defaults to 0
    },
    [BG_PIC_1] = {
        .bg = 1,
        .charBaseIndex = 2,
        .mapBaseIndex = 30,
        .priority = 1,
    },
    [BG_PIC_2] = {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 28,
        .priority = 2,
    },
    [BG_INTRO] = {
        .bg = 3,
        .charBaseIndex = 3,
        .mapBaseIndex = 27,
        .priority = 3,
    },
};

static const struct WindowTemplate sKabaSpeech_WindowTemplates[] =
{
    [WIN_TEXT] =
    {
        .bg = BG_TEXT,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x1,
    },
    [WIN_YESNO] = 
    {
        .bg = BG_TEXT,
        .tilemapLeft = 21,
        .tilemapTop = 9,
        .width = 5,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = (26*4)+1,
    },
    DUMMY_WIN_TEMPLATE,
};

static const struct SpriteSheet sKabaSpeech_PlatformSpriteSheet = 
{
    .data = sKabaSpeech_PlatformGfx,
    .size = 2048, // 64x128/2
    .tag = TAG_PLATFORM,
};

static const struct SpritePalette sKabaSpeech_PlatformSpritePalette =
{
    .data = sKabaSpeech_PlatformPal,
    .tag = TAG_PLATFORM,
};

static const struct OamData sKabaSpeech_PlatformOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x32),
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sKabaSpeech_PlatformLeft[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sKabaSpeech_PlatformRight[] =
{
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sKabaSpeech_PlatformAnims[] =
{
    sKabaSpeech_PlatformLeft,
    sKabaSpeech_PlatformRight
};

static const struct SpriteTemplate sKabaSpeech_PlatformTemplate =
{
    .tileTag = TAG_PLATFORM,
    .paletteTag = TAG_PLATFORM,
    .oam = &sKabaSpeech_PlatformOamData,
    .anims = sKabaSpeech_PlatformAnims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// Code
void CB2_KabaSpeech(void)
{
    RunTasks();
    RunTextPrinters();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void VBlankCB_KabaSpeech(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void DoKabaSpeech(void)
{
    CreateTask(Task_KabaSpeech_Begin, 0);
    SetMainCallback2(CB2_KabaSpeech);
}

static void Task_KabaSpeech_Begin(u8 taskId)
{
    switch(gMain.state)
    {
        case STATE_RESET:
            SetVBlankCallback(NULL);
            SetHBlankCallback(NULL);
            DmaFill16(3, 0, VRAM, VRAM_SIZE);
            ResetPaletteFade();
            ScanlineEffect_Stop();
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTempTileDataBuffers();
            break;
        case STATE_ALLOC:
            sKabaSpeech = AllocZeroed(sizeof(*sKabaSpeech));
            break;
        case STATE_REGS:
            SetGpuReg(REG_OFFSET_WIN0H, 0);
            SetGpuReg(REG_OFFSET_WIN0V, 0);
            SetGpuReg(REG_OFFSET_WIN1H, 0);
            SetGpuReg(REG_OFFSET_WIN1V, 0);
            SetGpuReg(REG_OFFSET_WININ, 0);
            SetGpuReg(REG_OFFSET_WINOUT, 0);
            SetGpuReg(REG_OFFSET_BLDCNT, 0);
            SetGpuReg(REG_OFFSET_BLDALPHA, 0);
            SetGpuReg(REG_OFFSET_BLDY, 0);
            SetGpuReg(REG_OFFSET_DISPCNT, 0);
            break;
        case STATE_BGS:
            ResetBgsAndClearDma3BusyFlags(0);
            InitBgsFromTemplates(0, sKabaSpeech_BgTemplates, ARRAY_COUNT(sKabaSpeech_BgTemplates));
            SetBgTilemapBuffer(BG_INTRO, sKabaSpeech->introTilemapBuffer);
            SetBgTilemapBuffer(BG_PIC_1, sKabaSpeech->pic1TilemapBuffer);
            SetBgTilemapBuffer(BG_PIC_2, sKabaSpeech->pic2TilemapBuffer);
            ResetBgPositions();
            break;
        case STATE_BG_GFX:
            KabaSpeech_CreateMonSprite();
            KabaSpeech_CreatePlatformSprites();
            LoadPalette(sKabaSpeech_BgPal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
            LoadBgTiles(BG_INTRO, sKabaSpeech_BgGfx, sizeof(sKabaSpeech_BgGfx), 0);
            CopyToBgTilemapBuffer(BG_INTRO, sKabaSpeech_BgMap, 0, 0);
            CopyBgTilemapBufferToVram(BG_INTRO);
            KabaSpeech_DrawCharacterPic(PIC_KABA);
            break;
        case STATE_WINDOWS:
            InitWindows(sKabaSpeech_WindowTemplates);
            InitTextBoxGfxAndPrinters();
            Menu_LoadStdPalAt(BG_PLTT_ID(13));
            DrawDialogueFrame(WIN_TEXT, TRUE);
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
            break;
        case STATE_FINISH:
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
            SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_TGT1_BG1 | BLDCNT_TGT1_BG2 | BLDCNT_TGT1_OBJ | BLDCNT_TGT2_BG_ALL | BLDCNT_EFFECT_BLEND);
            SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, 16));
            SetGpuReg(REG_OFFSET_BLDY, 0);
            ShowBg(BG_INTRO);
            ShowBg(BG_PIC_1);
            ShowBg(BG_PIC_2);
            ShowBg(BG_TEXT);
            SetVBlankCallback(VBlankCB_KabaSpeech);
            PlayBGM(MUS_ROUTE122);
            sKabaSpeech->alphaCoeff = 0;
            sKabaSpeech->timer = 0xD8;
            gTasks[taskId].func = Task_KabaSpeech_FadeInEverything;
            gMain.state = 0;
            return;
    }
    DebugPrintf("state: %d", gMain.state);
    gMain.state++;
}

static void Task_KabaSpeech_FadeInEverything(u8 taskId)
{
    if (sKabaSpeech->timer)
    {
        sKabaSpeech->timer--;
    }
    else if (sKabaSpeech->alphaCoeff >= 16)
    {
        sKabaSpeech->alphaCoeff = 16;
        sKabaSpeech->timer = 80;
        gTasks[taskId].func = Task_KabaSpeech_GreetingsTraveler;
        return;
    }
    else
    {
        sKabaSpeech->alphaCoeff++;
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(sKabaSpeech->alphaCoeff, 16 - sKabaSpeech->alphaCoeff));
    }
}

static void Task_KabaSpeech_GreetingsTraveler(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (sKabaSpeech->timer)
        {
            sKabaSpeech->timer--;
        }
        else
        {
            KabaSpeech_PrintMessageBox(sKabaSpeech_Greetings);
            gTasks[taskId].func = Task_KabaSpeech_AndThis;
        }
    }
}

static void Task_KabaSpeech_AndThis(u8 taskId)
{
    if (!IsTextPrinterActive(WIN_TEXT))
    {
        KabaSpeech_PrintMessageBox(sKabaSpeech_AndThis);
        sKabaSpeech->timer = 30;
        gTasks[taskId].func = Task_KabaSpeech_ReleaseJoltikFromPokeball;
    }
}

static void Task_KabaSpeech_ReleaseJoltikFromPokeball(u8 taskId)
{
    u32 spriteId;
    if (!IsTextPrinterActive(WIN_TEXT))
    {
        if (sKabaSpeech->timer)
        {
            sKabaSpeech->timer--;
        }
        else
        {
            spriteId = sKabaSpeech->monSpriteId;
            gSprites[spriteId].invisible = FALSE;
            CreatePokeballSpriteToReleaseMon(spriteId, gSprites[spriteId].oam.paletteNum, MON_POS_X, MON_POS_Y, 0, 0, 32, 0x00007FFF, SPECIES_JOLTIK);
            gTasks[taskId].func = Task_KabaSpeech_JoltikAPokemon;
            sKabaSpeech->timer = 0;
        }
    }
}

static void Task_KabaSpeech_JoltikAPokemon(u8 taskId)
{
    sKabaSpeech->timer++;
    if (IsCryFinished())
    {
        if (sKabaSpeech->timer >= 192)
        {
            KabaSpeech_PrintMessageBox(sKabaSpeech_JoltikAPokemon);
            gTasks[taskId].func = Task_KabaSpeech_MainTalk;
        }
    }
}

static void Task_KabaSpeech_MainTalk(u8 taskId)
{
    if (!IsTextPrinterActive(WIN_TEXT))
    {
        KabaSpeech_PrintMessageBox(sKabaSpeech_MainTalk);
        gTasks[taskId].func = Task_KabaSpeech_ReturnJoltik;
    }
}

static void Task_KabaSpeech_ReturnJoltik(u8 taskId)
{
    if (!IsTextPrinterActive(WIN_TEXT))
    {
        u32 spriteId = sKabaSpeech->monSpriteId;
        sKabaSpeech->ballSpriteId = CreateTradePokeballSprite(spriteId, gSprites[spriteId].oam.paletteNum, MON_POS_X, MON_POS_Y, 0, 0, 32, 0x00007FFF);
        gTasks[taskId].func = Task_KabaSpeech_FadeOutPokeball;
    }
}

static void Task_KabaSpeech_FadeOutPokeball(u8 taskId)
{
    // the ball callback ends its functionality with SpriteCallbackDummy
    if (gSprites[sKabaSpeech->ballSpriteId].callback == SpriteCallbackDummy)
    {
        gSprites[sKabaSpeech->monSpriteId].invisible = TRUE;
        gSprites[sKabaSpeech->ballSpriteId].invisible = TRUE;
        DestroySprite(&gSprites[sKabaSpeech->monSpriteId]);
        DestroySprite(&gSprites[sKabaSpeech->ballSpriteId]);
        sKabaSpeech->alphaCoeff = 16;
        gTasks[taskId].func = Task_KabaSpeech_FadeOutPlatform;
    }
}

static void Task_KabaSpeech_FadeOutPlatform(u8 taskId)
{
    if (sKabaSpeech->alphaCoeff <= 0)
    {
        sKabaSpeech->alphaCoeff = 0;
        sKabaSpeech->timer = 0xD8;
        gSprites[sKabaSpeech->platformSpriteIds[0]].invisible = FALSE;
        gSprites[sKabaSpeech->platformSpriteIds[1]].invisible = FALSE;
        ClearDialogWindowAndFrameToTransparent(WIN_TEXT, TRUE);
        FillBgTilemapBufferRect_Palette0(BG_PIC_1, 0, 0, 0, 32, 32);
        CopyBgTilemapBufferToVram(BG_PIC_1);
        KabaSpeech_DrawCharacterPic(PIC_AO);
        KabaSpeech_DrawCharacterPic(PIC_AKA);
        gTasks[taskId].func = Task_KabaSpeech_SpawnPlayerChoice;
    }
    else
    {
        sKabaSpeech->alphaCoeff--;
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(sKabaSpeech->alphaCoeff, 16 - sKabaSpeech->alphaCoeff));
    }
}

static void Task_KabaSpeech_FadeOutPlayerChoice(u8 taskId)
{
    if (sKabaSpeech->alphaCoeff <= 0)
    {
        sKabaSpeech->alphaCoeff = 0;
        sKabaSpeech->timer = 0xD8;
        gSprites[sKabaSpeech->platformSpriteIds[0]].invisible = FALSE;
        gSprites[sKabaSpeech->platformSpriteIds[1]].invisible = FALSE;
        ClearDialogWindowAndFrameToTransparent(WIN_TEXT, TRUE);
        FillBgTilemapBufferRect_Palette0(BG_PIC_1, 0, 0, 0, 32, 32);
        CopyBgTilemapBufferToVram(BG_PIC_1);
        KabaSpeech_DrawCharacterPic(PIC_AO);
        KabaSpeech_DrawCharacterPic(PIC_AKA);
        gTasks[taskId].func = Task_KabaSpeech_SpawnPlayerChoice;
    }
    else
    {
        sKabaSpeech->alphaCoeff--;
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(sKabaSpeech->alphaCoeff, 16 - sKabaSpeech->alphaCoeff));
    }
}

static void Task_KabaSpeech_SpawnPlayerChoice(u8 taskId)
{
    if (sKabaSpeech->timer)
    {
        sKabaSpeech->timer--;
    }
    else if (sKabaSpeech->alphaCoeff >= 16)
    {
        sKabaSpeech->alphaCoeff = 16;
        sKabaSpeech->timer = 0;
        KabaSpeech_PrintMessageBox(sKabaSpeech_GenderChoice);
        gTasks[taskId].func = Task_KabaSpeech_WaitForChoice;
    }
    else
    {
        sKabaSpeech->alphaCoeff++;
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(sKabaSpeech->alphaCoeff, 16 - sKabaSpeech->alphaCoeff));
    }
}

static void Task_KabaSpeech_WaitForChoice(u8 taskId)
{

}

// misc. helper functions

static void KabaSpeech_DrawCharacterPic(u8 id)
{
    switch(id)
    {
        case PIC_AKA: // male
            LoadPalette(sKabaSpeech_PlayerPicPal, BG_PLTT_ID(3), PLTT_SIZE_4BPP);
            LoadBgTiles(BG_PIC_2, sKabaSpeech_AkaPicGfx, sizeof(sKabaSpeech_AkaPicGfx), 0);
            CopyToBgTilemapBufferRect(BG_PIC_2, sKabaSpeech_AkaPicMap, 14, 0, 9, 13);
            CopyBgTilemapBufferToVram(BG_PIC_2);
            return;
        case PIC_AO: // female
            LoadPalette(sKabaSpeech_PlayerPicPal, BG_PLTT_ID(3), PLTT_SIZE_4BPP);
            LoadBgTiles(BG_PIC_1, sKabaSpeech_AoPicGfx, sizeof(sKabaSpeech_AoPicGfx), 0x50);
            CopyToBgTilemapBufferRect(BG_PIC_1, sKabaSpeech_AoPicMap, 7, 0, 9, 13);
            CopyBgTilemapBufferToVram(BG_PIC_1);
            return;
        case PIC_KABA:
            LoadPalette(sKabaSpeech_KabaPicPal, BG_PLTT_ID(1), PLTT_SIZE_4BPP);
            LoadBgTiles(BG_PIC_1, sKabaSpeech_KabaPicGfx, sizeof(sKabaSpeech_KabaPicGfx), 0);
            CopyToBgTilemapBuffer(BG_PIC_1, sKabaSpeech_KabaPicMap, 0, 0);
            CopyBgTilemapBufferToVram(BG_PIC_1);
            return;
        default:
            return;
    }
}

static inline void KabaSpeech_PrintMessageBox(const u8 *str)
{
    DrawDialogueFrame(WIN_TEXT, FALSE);
    if (str != gStringVar4)
    {
        StringExpandPlaceholders(gStringVar4, str);
        AddTextPrinterParameterized2(WIN_TEXT, FONT_NORMAL, gStringVar4, GetPlayerTextSpeed(), NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
    }
    else
    {
        AddTextPrinterParameterized2(WIN_TEXT, FONT_NORMAL, str, GetPlayerTextSpeed(), NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
    }
    CopyWindowToVram(WIN_TEXT, COPYWIN_FULL);
}

static void KabaSpeech_CreateMonSprite(void)
{
    sKabaSpeech->monSpriteId = CreateMonPicSprite_Affine(SPECIES_JOLTIK, FALSE, 0, MON_PIC_AFFINE_FRONT, MON_POS_X, MON_POS_Y, 14, TAG_NONE);
    gSprites[sKabaSpeech->monSpriteId].callback = SpriteCallbackDummy;
    gSprites[sKabaSpeech->monSpriteId].oam.priority = 0;
    gSprites[sKabaSpeech->monSpriteId].invisible = TRUE;
}

static void KabaSpeech_CreatePlatformSprites(void)
{
    u32 leftSpriteId, rightSpriteId;
    LoadSpriteSheet(&sKabaSpeech_PlatformSpriteSheet);
    LoadSpritePalette(&sKabaSpeech_PlatformSpritePalette);
    leftSpriteId = CreateSprite(&sKabaSpeech_PlatformTemplate, PLAT_POS_X, PLAT_POS_Y, 0);
    rightSpriteId = CreateSprite(&sKabaSpeech_PlatformTemplate, PLAT_POS_X, PLAT_POS_Y, 0);
    gSprites[rightSpriteId].x2 = PLAT_POS_X2;
    StartSpriteAnim(&gSprites[leftSpriteId], 0);
    StartSpriteAnim(&gSprites[rightSpriteId], 1);
    gSprites[leftSpriteId].invisible = TRUE;
    gSprites[rightSpriteId].invisible = TRUE;
    sKabaSpeech->platformSpriteIds[0] = leftSpriteId;
    sKabaSpeech->platformSpriteIds[1] = rightSpriteId;
}
