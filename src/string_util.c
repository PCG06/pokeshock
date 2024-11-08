#include "global.h"
#include "string_util.h"
#include "text.h"
#include "strings.h"
#include "malloc.h"
#include "union_room_chat.h"

EWRAM_DATA u8 gStringVar1[0x100] = {0};
EWRAM_DATA u8 gStringVar2[0x100] = {0};
EWRAM_DATA u8 gStringVar3[0x100] = {0};
EWRAM_DATA u8 gStringVar4[0x3E8] = {0};
EWRAM_DATA static u8 sUnknownStringVar[16] = {0};

static const u8 sDigits[] = __("0123456789ABCDEF");

static const s32 sPowersOfTen[] =
{
             1,
            10,
           100,
          1000,
         10000,
        100000,
       1000000,
      10000000,
     100000000,
    1000000000,
};

u8 *StringCopy_Nickname(u8 *dest, const u8 *src)
{
    u8 i;
    u32 limit = POKEMON_NAME_LENGTH;

    for (i = 0; i < limit; i++)
    {
        dest[i] = src[i];

        if (dest[i] == EOS)
            return &dest[i];
    }

    dest[i] = EOS;
    return &dest[i];
}

u8 *StringGet_Nickname(u8 *str)
{
    u32 i;
    u32 limit = POKEMON_NAME_LENGTH;

    for (i = 0; i < limit; i++)
        if (str[i] == EOS)
            return &str[i];

    str[i] = EOS;
    return &str[i];
}

u8 *StringCopy_PlayerName(u8 *dest, const u8 *src)
{
    s32 i;
    s32 limit = PLAYER_NAME_LENGTH;

    for (i = 0; i < limit; i++)
    {
        dest[i] = src[i];

        if (dest[i] == EOS)
            return &dest[i];
    }

    dest[i] = EOS;
    return &dest[i];
}

u8 *StringCopy(u8 *dest, const u8 *src)
{
    while (*src != EOS)
    {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = EOS;
    return dest;
}

u8 *StringAppend(u8 *dest, const u8 *src)
{
    while (*dest != EOS)
        dest++;

    return StringCopy(dest, src);
}

u8 *StringCopyN(u8 *dest, const u8 *src, u8 n)
{
    u32 i;

    for (i = 0; i < n; i++)
        dest[i] = src[i];

    return &dest[n];
}

u8 *StringAppendN(u8 *dest, const u8 *src, u8 n)
{
    while (*dest != EOS)
        dest++;

    return StringCopyN(dest, src, n);
}

u16 StringLength(const u8 *str)
{
    u16 length = 0;

    while (str[length] != EOS)
        length++;

    return length;
}

u16 StringLineLength(const u8 *str)
{
    u16 i = 0, length = 0;

    while (str[length] != EOS)
    {
        switch (str[length])
        {
        case CHAR_PROMPT_SCROLL:
        case CHAR_PROMPT_CLEAR:
        case CHAR_NEWLINE:
            return length;
        default:
            i++;
            length++;
            break;
        }
    }

    return length;
}

s32 StringCompare(const u8 *str1, const u8 *str2)
{
    while (*str1 == *str2)
    {
        if (*str1 == EOS)
            return 0;
        str1++;
        str2++;
    }

    return *str1 - *str2;
}

s32 StringCompareN(const u8 *str1, const u8 *str2, u32 n)
{
    while (*str1 == *str2)
    {
        if (*str1 == EOS)
            return 0;
        str1++;
        str2++;
        if (--n == 0)
            return 0;
    }

    return *str1 - *str2;
}

bool8 IsStringLengthAtLeast(const u8 *str, s32 n)
{
    u32 i;

    for (i = 0; i < n; i++)
        if (str[i] && str[i] != EOS)
            return TRUE;

    return FALSE;
}

u8 *ConvertIntToDecimalStringN(u8 *dest, s32 value, enum StringConvertMode mode, u8 n)
{
    enum { WAITING_FOR_NONZERO_DIGIT, WRITING_DIGITS, WRITING_SPACES } state;
    s32 powerOfTen;
    s32 largestPowerOfTen = sPowersOfTen[n - 1];

    state = WAITING_FOR_NONZERO_DIGIT;

    if (mode == STR_CONV_MODE_RIGHT_ALIGN)
        state = WRITING_SPACES;

    if (mode == STR_CONV_MODE_LEADING_ZEROS)
        state = WRITING_DIGITS;

    for (powerOfTen = largestPowerOfTen; powerOfTen > 0; powerOfTen /= 10)
    {
        u8 c;
        u16 digit = value / powerOfTen;
        s32 temp = value - (powerOfTen * digit);

        if (state == WRITING_DIGITS)
        {
            u8 *out = dest++;

            if (digit <= 9)
                c = sDigits[digit];
            else
                c = CHAR_QUESTION_MARK;

            *out = c;
        }
        else if (digit != 0 || powerOfTen == 1)
        {
            u8 *out;
            state = WRITING_DIGITS;
            out = dest++;

            if (digit <= 9)
                c = sDigits[digit];
            else
                c = CHAR_QUESTION_MARK;

            *out = c;
        }
        else if (state == WRITING_SPACES)
        {
            *dest++ = CHAR_SPACER;
        }

        value = temp;
    }

    *dest = EOS;
    return dest;
}

u8 *ConvertUIntToDecimalStringN(u8 *dest, u32 value, enum StringConvertMode mode, u8 n)
{
    enum { WAITING_FOR_NONZERO_DIGIT, WRITING_DIGITS, WRITING_SPACES } state;
    s32 powerOfTen;
    s32 largestPowerOfTen = sPowersOfTen[n - 1];

    state = WAITING_FOR_NONZERO_DIGIT;

    if (mode == STR_CONV_MODE_RIGHT_ALIGN)
        state = WRITING_SPACES;

    if (mode == STR_CONV_MODE_LEADING_ZEROS)
        state = WRITING_DIGITS;

    for (powerOfTen = largestPowerOfTen; powerOfTen > 0; powerOfTen /= 10)
    {
        u8 c;
        u16 digit = value / powerOfTen;
        u32 temp = value - (powerOfTen * digit);

        if (state == WRITING_DIGITS)
        {
            u8 *out = dest++;

            if (digit <= 9)
                c = sDigits[digit];
            else
                c = CHAR_QUESTION_MARK;

            *out = c;
        }
        else if (digit != 0 || powerOfTen == 1)
        {
            u8 *out;
            state = WRITING_DIGITS;
            out = dest++;

            if (digit <= 9)
                c = sDigits[digit];
            else
                c = CHAR_QUESTION_MARK;

            *out = c;
        }
        else if (state == WRITING_SPACES)
        {
            *dest++ = CHAR_SPACER;
        }

        value = temp;
    }

    *dest = EOS;
    return dest;
}

u8 *ConvertIntToHexStringN(u8 *dest, s32 value, enum StringConvertMode mode, u8 n)
{
    enum { WAITING_FOR_NONZERO_DIGIT, WRITING_DIGITS, WRITING_SPACES } state;
    u32 i;
    s32 powerOfSixteen;
    s32 largestPowerOfSixteen = 1;

    for (i = 1; i < n; i++)
        largestPowerOfSixteen *= 16;

    state = WAITING_FOR_NONZERO_DIGIT;

    if (mode == STR_CONV_MODE_RIGHT_ALIGN)
        state = WRITING_SPACES;

    if (mode == STR_CONV_MODE_LEADING_ZEROS)
        state = WRITING_DIGITS;

    for (powerOfSixteen = largestPowerOfSixteen; powerOfSixteen > 0; powerOfSixteen /= 16)
    {
        u8 c;
        u32 digit = value / powerOfSixteen;
        s32 temp = value % powerOfSixteen;

        if (state == WRITING_DIGITS)
        {
            u8 *out = dest++;

            if (digit <= 0xF)
                c = sDigits[digit];
            else
                c = CHAR_QUESTION_MARK;

            *out = c;
        }
        else if (digit != 0 || powerOfSixteen == 1)
        {
            u8 *out;
            state = WRITING_DIGITS;
            out = dest++;

            if (digit <= 0xF)
                c = sDigits[digit];
            else
                c = CHAR_QUESTION_MARK;

            *out = c;
        }
        else if (state == WRITING_SPACES)
        {
            *dest++ = CHAR_SPACER;
        }

        value = temp;
    }

    *dest = EOS;
    return dest;
}

u8 *StringExpandPlaceholders(u8 *dest, const u8 *src)
{
    for (;;)
    {
        u8 c = *src++;
        u8 placeholderId;
        const u8 *expandedString;

        switch (c)
        {
        case PLACEHOLDER_BEGIN:
            placeholderId = *src++;
            expandedString = GetExpandedPlaceholder(placeholderId);
            dest = StringExpandPlaceholders(dest, expandedString);
            break;
        case EXT_CTRL_CODE_BEGIN:
            *dest++ = c;
            c = *src++;
            *dest++ = c;

            switch (c)
            {
            case EXT_CTRL_CODE_RESET_FONT:
            case EXT_CTRL_CODE_PAUSE_UNTIL_PRESS:
            case EXT_CTRL_CODE_FILL_WINDOW:
            case EXT_CTRL_CODE_JPN:
            case EXT_CTRL_CODE_ENG:
            case EXT_CTRL_CODE_PAUSE_MUSIC:
            case EXT_CTRL_CODE_RESUME_MUSIC:
                break;
            case EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW:
                *dest++ = *src++;
            case EXT_CTRL_CODE_PLAY_BGM:
                *dest++ = *src++;
            default:
                *dest++ = *src++;
            }
            break;
        case EOS:
            *dest = EOS;
            return dest;
        case CHAR_PROMPT_SCROLL:
        case CHAR_PROMPT_CLEAR:
        case CHAR_NEWLINE:
        default:
            *dest++ = c;
        }
    }
}

u8 *StringBraille(u8 *dest, const u8 *src)
{
    const u8 setBrailleFont[] = {
        EXT_CTRL_CODE_BEGIN,
        EXT_CTRL_CODE_FONT,
        FONT_BRAILLE,
        EOS
    };
    const u8 gotoLine2[] = {
        CHAR_NEWLINE,
        EXT_CTRL_CODE_BEGIN,
        EXT_CTRL_CODE_SHIFT_DOWN,
        2,
        EOS
    };

    dest = StringCopy(dest, setBrailleFont);

    for (;;)
    {
        u8 c = *src++;

        switch (c)
        {
        case EOS:
            *dest = c;
            return dest;
        case CHAR_NEWLINE:
            dest = StringCopy(dest, gotoLine2);
            break;
        default:
            *dest++ = c;
            *dest++ = c + NUM_BRAILLE_CHARS;
            break;
        }
    }
}

static const u8 *ExpandPlaceholder_UnknownStringVar(void)
{
    return sUnknownStringVar;
}

static const u8 *ExpandPlaceholder_PlayerName(void)
{
    return gSaveBlock2Ptr->playerName;
}

static const u8 *ExpandPlaceholder_StringVar1(void)
{
    return gStringVar1;
}

static const u8 *ExpandPlaceholder_StringVar2(void)
{
    return gStringVar2;
}

static const u8 *ExpandPlaceholder_StringVar3(void)
{
    return gStringVar3;
}

static const u8 *ExpandPlaceholder_KunChan(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        return gText_ExpandedPlaceholder_Kun;
    else
        return gText_ExpandedPlaceholder_Chan;
}

static const u8 *ExpandPlaceholder_RivalName(void)
{
    if (gSaveBlock2Ptr->playerGender == MALE)
        return gText_ExpandedPlaceholder_May;
    else
        return gText_ExpandedPlaceholder_Brendan;
}

static const u8 *ExpandPlaceholder_Version(void)
{
    return gText_ExpandedPlaceholder_Emerald;
}

static const u8 *ExpandPlaceholder_Aqua(void)
{
    return gText_ExpandedPlaceholder_Aqua;
}

static const u8 *ExpandPlaceholder_Magma(void)
{
    return gText_ExpandedPlaceholder_Magma;
}

static const u8 *ExpandPlaceholder_Archie(void)
{
    return gText_ExpandedPlaceholder_Archie;
}

static const u8 *ExpandPlaceholder_Maxie(void)
{
    return gText_ExpandedPlaceholder_Maxie;
}

static const u8 *ExpandPlaceholder_Kyogre(void)
{
    return gText_ExpandedPlaceholder_Kyogre;
}

static const u8 *ExpandPlaceholder_Groudon(void)
{
    return gText_ExpandedPlaceholder_Groudon;
}

const u8 *GetExpandedPlaceholder(u32 id)
{
    typedef const u8 *(*ExpandPlaceholderFunc)(void);

    static const ExpandPlaceholderFunc funcs[] =
    {
        [PLACEHOLDER_ID_UNKNOWN]      = ExpandPlaceholder_UnknownStringVar,
        [PLACEHOLDER_ID_PLAYER]       = ExpandPlaceholder_PlayerName,
        [PLACEHOLDER_ID_STRING_VAR_1] = ExpandPlaceholder_StringVar1,
        [PLACEHOLDER_ID_STRING_VAR_2] = ExpandPlaceholder_StringVar2,
        [PLACEHOLDER_ID_STRING_VAR_3] = ExpandPlaceholder_StringVar3,
        [PLACEHOLDER_ID_KUN]          = ExpandPlaceholder_KunChan,
        [PLACEHOLDER_ID_RIVAL]        = ExpandPlaceholder_RivalName,
        [PLACEHOLDER_ID_VERSION]      = ExpandPlaceholder_Version,
        [PLACEHOLDER_ID_AQUA]         = ExpandPlaceholder_Aqua,
        [PLACEHOLDER_ID_MAGMA]        = ExpandPlaceholder_Magma,
        [PLACEHOLDER_ID_ARCHIE]       = ExpandPlaceholder_Archie,
        [PLACEHOLDER_ID_MAXIE]        = ExpandPlaceholder_Maxie,
        [PLACEHOLDER_ID_KYOGRE]       = ExpandPlaceholder_Kyogre,
        [PLACEHOLDER_ID_GROUDON]      = ExpandPlaceholder_Groudon,
    };

    if (id >= ARRAY_COUNT(funcs))
        return gText_ExpandedPlaceholder_Empty;
    else
        return funcs[id]();
}

u8 *StringFill(u8 *dest, u8 c, u16 n)
{
    u32 i;

    for (i = 0; i < n; i++)
        *dest++ = c;

    *dest = EOS;
    return dest;
}

u8 *StringCopyPadded(u8 *dest, const u8 *src, u8 c, u16 n)
{
    while (*src != EOS)
    {
        *dest++ = *src++;

        if (n)
            n--;
    }

    n--;

    while (n != (u16)-1)
    {
        *dest++ = c;
        n--;
    }

    *dest = EOS;
    return dest;
}

u8 *StringFillWithTerminator(u8 *dest, u16 n)
{
    return StringFill(dest, EOS, n);
}

u8 *StringCopyN_Multibyte(u8 *dest, u8 *src, u32 n)
{
    u32 i;

    for (i = n - 1; i != (u32)-1; i--)
    {
        if (*src == EOS)
        {
            break;
        }
        else
        {
            *dest++ = *src++;
            if (*(src - 1) == CHAR_EXTRA_SYMBOL)
                *dest++ = *src++;
        }
    }

    *dest = EOS;
    return dest;
}

u32 StringLength_Multibyte(const u8 *str)
{
    u32 length = 0;

    while (*str != EOS)
    {
        if (*str == CHAR_EXTRA_SYMBOL)
            str++;
        str++;
        length++;
    }

    return length;
}

u8 *WriteColorChangeControlCode(u8 *dest, u32 colorType, u8 color)
{
    *dest = EXT_CTRL_CODE_BEGIN;
    dest++;

    switch (colorType)
    {
    case 0:
        *dest = EXT_CTRL_CODE_COLOR;
        dest++;
        break;
    case 1:
        *dest = EXT_CTRL_CODE_SHADOW;
        dest++;
        break;
    case 2:
        *dest = EXT_CTRL_CODE_HIGHLIGHT;
        dest++;
        break;
    }

    *dest = color;
    dest++;
    *dest = EOS;
    return dest;
}

bool32 IsStringJapanese(u8 *str)
{
    while (*str != EOS)
    {
        if (*str <= JAPANESE_CHAR_END)
            if (*str != CHAR_SPACE)
                return TRUE;
        str++;
    }

    return FALSE;
}

bool32 IsStringNJapanese(u8 *str, s32 n)
{
    s32 i;

    for (i = 0; *str != EOS && i < n; i++)
    {
        if (*str <= JAPANESE_CHAR_END)
            if (*str != CHAR_SPACE)
                return TRUE;
        str++;
    }

    return FALSE;
}

u8 GetExtCtrlCodeLength(u8 code)
{
    static const u8 lengths[] =
    {
        [0]                                    = 1,
        [EXT_CTRL_CODE_COLOR]                  = 2,
        [EXT_CTRL_CODE_HIGHLIGHT]              = 2,
        [EXT_CTRL_CODE_SHADOW]                 = 2,
        [EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW] = 4,
        [EXT_CTRL_CODE_PALETTE]                = 2,
        [EXT_CTRL_CODE_FONT]                   = 2,
        [EXT_CTRL_CODE_RESET_FONT]             = 1,
        [EXT_CTRL_CODE_PAUSE]                  = 2,
        [EXT_CTRL_CODE_PAUSE_UNTIL_PRESS]      = 1,
        [EXT_CTRL_CODE_WAIT_SE]                = 1,
        [EXT_CTRL_CODE_PLAY_BGM]               = 3,
        [EXT_CTRL_CODE_ESCAPE]                 = 2,
        [EXT_CTRL_CODE_SHIFT_RIGHT]            = 2,
        [EXT_CTRL_CODE_SHIFT_DOWN]             = 2,
        [EXT_CTRL_CODE_FILL_WINDOW]            = 1,
        [EXT_CTRL_CODE_PLAY_SE]                = 3,
        [EXT_CTRL_CODE_CLEAR]                  = 2,
        [EXT_CTRL_CODE_SKIP]                   = 2,
        [EXT_CTRL_CODE_CLEAR_TO]               = 2,
        [EXT_CTRL_CODE_MIN_LETTER_SPACING]     = 2,
        [EXT_CTRL_CODE_JPN]                    = 1,
        [EXT_CTRL_CODE_ENG]                    = 1,
        [EXT_CTRL_CODE_PAUSE_MUSIC]            = 1,
        [EXT_CTRL_CODE_RESUME_MUSIC]           = 1,
    };

    u8 length = 0;
    if (code < ARRAY_COUNT(lengths))
        length = lengths[code];
    return length;
}

static const u8 *SkipExtCtrlCode(const u8 *s)
{
    while (*s == EXT_CTRL_CODE_BEGIN)
    {
        s++;
        s += GetExtCtrlCodeLength(*s);
    }

    return s;
}

s32 StringCompareWithoutExtCtrlCodes(const u8 *str1, const u8 *str2)
{
    s32 retVal = 0;

    while (1)
    {
        str1 = SkipExtCtrlCode(str1);
        str2 = SkipExtCtrlCode(str2);

        if (*str1 > *str2)
            break;

        if (*str1 < *str2)
        {
            retVal = -1;
            if (*str2 == EOS)
                retVal = 1;
        }

        if (*str1 == EOS)
            return retVal;

        str1++;
        str2++;
    }

    retVal = 1;

    if (*str1 == EOS)
        retVal = -1;

    return retVal;
}

void ConvertInternationalString(u8 *s, u8 language)
{
    if (language == LANGUAGE_JAPANESE)
    {
        u32 i;

        StripExtCtrlCodes(s);
        i = StringLength(s);
        s[i++] = EXT_CTRL_CODE_BEGIN;
        s[i++] = EXT_CTRL_CODE_ENG;
        s[i++] = EOS;

        i--;

        while (i != -1)
        {
            s[i + 2] = s[i];
            i--;
        }

        s[0] = EXT_CTRL_CODE_BEGIN;
        s[1] = EXT_CTRL_CODE_JPN;
    }
}

void StripExtCtrlCodes(u8 *str)
{
    u16 srcIndex = 0;
    u16 destIndex = 0;
    while (str[srcIndex] != EOS)
    {
        if (str[srcIndex] == EXT_CTRL_CODE_BEGIN)
        {
            srcIndex++;
            srcIndex += GetExtCtrlCodeLength(str[srcIndex]);
        }
        else
        {
            str[destIndex++] = str[srcIndex++];
        }
    }
    str[destIndex] = EOS;
}

u8 *StringCopyUppercase(u8 *dest, const u8 *src)
{
    while (*src != EOS)
    {
        if (*src >= CHAR_a && *src <= CHAR_z)
            *dest = gCaseToggleTable[*src];
        else
            *dest = *src;
        dest++;
        src++;
    }

    *dest = EOS;
    return dest;
}

void BreakStringKnuth(u8 *src, u32 maxWidth, u8 screenLines, u8 fontId)
{
    if (StringHasManualBreaks(src))
        return;
    s32 bestBadness = -1;
    u16 numChars = 1;
    u16 numWords = 1;
    u16 currWordIndex = 0;
    u8 currWordLength = 1;
    if (src[0] == EOS)
        return;
    bool32 isPrevCharSplitting = FALSE;
    bool32 isCurrCharSplitting;
    //  Get numbers of chars in string and count words
    while (src[numChars] != EOS)
    {
        isCurrCharSplitting = IsWordSplittingChar(src, &numChars);
        if (isCurrCharSplitting == TRUE && isPrevCharSplitting == FALSE)
            numWords++;
        isPrevCharSplitting = isCurrCharSplitting;
        numChars++;
    }
    //  Allocate enough space for word data
    struct StringWord *allWords = Alloc(numWords*sizeof(struct StringWord));

    allWords[currWordIndex].startIndex = 0;
    allWords[currWordIndex].width = 0;
    isPrevCharSplitting = FALSE;
    //  Fill in word begin index and lengths
    for (u16 i = 1; i < numChars; i++)
    {
        isCurrCharSplitting = IsWordSplittingChar(src, &i);
        if (isCurrCharSplitting == TRUE && isPrevCharSplitting == FALSE)
        {
            allWords[currWordIndex].length = currWordLength;
            currWordIndex++;
            currWordLength = 0;
        }
        else if (isCurrCharSplitting == FALSE && isPrevCharSplitting == TRUE)
        {
            allWords[currWordIndex].startIndex = i;
            allWords[currWordIndex].width = 0;
            currWordLength++;
        }
        else
        {
            currWordLength++;
        }
        isPrevCharSplitting = isCurrCharSplitting;
    }
    allWords[currWordIndex].length = currWordLength;

    //  Fill in individual word widths
    for (u32 i = 0; i < numWords; i++)
        for (u32 j = 0; j < allWords[i].length; j++)
            allWords[i].width += GetGlyphWidth(src[allWords[i].startIndex + j], FALSE, fontId);

    //  Step 1: Does it all fit one one line? Then no break
    //  Step 2: Try to split across minimum number of lines
    u32 spaceWidth = GetGlyphWidth(0, FALSE, fontId);
    u32 totalWidth = allWords[0].width;
    //  Calculate total widths without any line breaks
    for (u32 i = 1; i < numWords; i++)
        totalWidth += allWords[i].width + spaceWidth;

    //  If it doesn't fit on 1 line, do fancy line break calculation
    if (totalWidth > maxWidth)
    {
        //  Figure out how many lines are needed with naive method
        u16 totalLines = 1;
        u16 currLineWidth = 0;
        for (currWordIndex = 0; currWordIndex < numWords; currWordIndex++)
        {
            if (currLineWidth + allWords[currWordIndex].length > maxWidth)
            {
                totalLines++;
                currLineWidth = allWords[currWordIndex].width;
            }
            else
            {
                currLineWidth += allWords[currWordIndex].width + spaceWidth;
            }
        }
        u16 targetLineWidth = totalWidth/totalLines;
        MgbaPrintf(MGBA_LOG_WARN, "Target: %u", targetLineWidth);
        struct StringLine *stringLines = Alloc(totalLines*sizeof(struct StringLine));
        for (u32 lineIndex = 0; lineIndex < totalLines; lineIndex++)
        {
            stringLines[lineIndex].numWords = 0;
            stringLines[lineIndex].spaceWidth = spaceWidth;
            stringLines[lineIndex].extraSpaceWidth = 0;
        }
        currWordIndex = 0;
        u16 currLineIndex = 0;
        stringLines[currLineIndex].words = &allWords[currWordIndex];
        stringLines[currLineIndex].numWords = 1;
        currLineWidth = allWords[currWordIndex].width;
        currWordIndex++;
        while (currWordIndex < numWords)
        {
            if (currLineWidth + spaceWidth + allWords[currWordIndex].width > maxWidth)
            {
                //  go to next line
                currLineIndex++;
                stringLines[currLineIndex].words = &allWords[currWordIndex];
                stringLines[currLineIndex].numWords = 1;
                currLineWidth = allWords[currWordIndex].width;
                currWordIndex++;
            }
            else if (currLineWidth > targetLineWidth)
            {
                //  go to next line
                currLineIndex++;
                stringLines[currLineIndex].words = &allWords[currWordIndex];
                stringLines[currLineIndex].numWords = 1;
                currLineWidth = allWords[currWordIndex].width;
                currWordIndex++;
            }
            else
            {
                //  continue on current line
                //  add word and space width
                currLineWidth += spaceWidth + allWords[currWordIndex].width;
                stringLines[currLineIndex].numWords++;
                currWordIndex++;
            }
            //MgbaPrintf(MGBA_LOG_WARN, "%u",allWords[currWordIndex].length);
        }
        u32 currBadness = GetStringBadness(stringLines, totalLines, maxWidth);
        MgbaPrintf(MGBA_LOG_WARN, "Badness: %u", currBadness);
        BuildNewString(stringLines, totalLines, src);
        MgbaPrintf(MGBA_LOG_WARN, "%S", src);
        Free(stringLines);
    }

    Free(allWords);
}

//  Only allow word splitting on allowed chars
bool32 IsWordSplittingChar(const u8 *src, u16 *index)
{
    switch (src[*index])
    {
        case 0:             //  " "
            return TRUE;
        default:
            return FALSE;
    }
}

//  Badness calculation
//  unfilled lines scale linerarly
//  jagged lines scales by the square
//  runts scale linearly
//  numbers not final
u32 GetStringBadness(struct StringLine *stringLines, u16 numLines, u16 maxWidth)
{
    u32 badness = 0;
    u16 *lineWidths = Alloc(numLines*2);
    u16 widestWidth = 0;
    for (u32 i = 0; i < numLines; i++)
    {
        lineWidths[i] = 0;
        for (u32 j = 0; j < stringLines[i].numWords; j++)
            lineWidths[i] += stringLines[i].words[j].width;
        lineWidths[i] += (stringLines[i].numWords-1)*stringLines[i].spaceWidth;
        if (lineWidths[i] > widestWidth)
            widestWidth = lineWidths[i];
        if (stringLines[i].numWords == 1)
            badness += BADNESS_RUNT;
    }
    for (u32 i = 0; i < numLines; i++)
    {
        u32 extraSpaceWidth = 0;
        if (lineWidths[i] != widestWidth)
        {
            //  Not the best way to do this, ideally a line should be allowed to get longer than current widest
            //  line. But then the widest line has to be recalculated.
            while (lineWidths[i] + (extraSpaceWidth + 1)*(stringLines[i].numWords-1) < widestWidth && extraSpaceWidth < MAX_SPACE_WIDTH)
                extraSpaceWidth++;
            lineWidths[i] += extraSpaceWidth*(stringLines[i].numWords-1);
        }
        badness += (maxWidth - lineWidths[i])*BADNESS_UNFILLED;
        u32 baseBadness = (widestWidth - lineWidths[i])*BADNESS_JAGGED;
        badness += baseBadness*baseBadness;
        stringLines[i].extraSpaceWidth = extraSpaceWidth;
    }
    Free(lineWidths);
    return badness;
}

//  Build the new string from the data stored in the StringLine structs
void BuildNewString(struct StringLine *stringLines, u16 numLines, u8 *str)
{
    u16 numCharsNeeded = 1;
    u16 newStrIndex = 0;
    for (u32 i = 0; i < numLines; i++)
    {
        //  Words
        for (u32 j = 0; j < stringLines[i].numWords; j++)
            numCharsNeeded += stringLines[i].words[j].length;
        //  Spaces
        if (stringLines[i].extraSpaceWidth == 0)
            numCharsNeeded += stringLines[i].numWords - 1;
        else
            numCharsNeeded += (stringLines[i].numWords - 1) * 4;
        //  Line breaks
        numCharsNeeded++;
    }
    u8 *newStr = Alloc(numCharsNeeded);
    newStrIndex += 3;
    for (u32 lineIndex = 0; lineIndex < numLines; lineIndex++)
    {
        for (u32 wordIndex = 0; wordIndex < stringLines[lineIndex].numWords; wordIndex++)
        {
            //  Add space if not first word
            if (wordIndex != 0)
            {
                newStr[newStrIndex] = 0;
                newStrIndex++;
                //  Widen space if needed
                //  current SHIFT_RIGHT doesn't seem to work
                /*
                if (stringLines[lineIndex].extraSpaceWidth != 0)
                {
                    newStr[newStrIndex] = 0xFC;
                    newStr[newStrIndex + 1] = 0x0D;
                    newStr[newStrIndex + 2] = stringLines[lineIndex].extraSpaceWidth;
                    newStrIndex += 3;
                }
                */
            }
            //  Add word characters
            for (u32 charIndex = 0; charIndex < stringLines[lineIndex].words[wordIndex].length; charIndex++)
            {
                newStr[newStrIndex] = str[stringLines[lineIndex].words[wordIndex].startIndex + charIndex];
                newStrIndex++;
            }
        }
        if (lineIndex + 1 < numLines)
        {
            newStr[newStrIndex] = 0xFE;
            newStrIndex++;
        }
    }
    newStr[newStrIndex] = EOS;
    u16 currIndex = 0;
    while (newStr[currIndex] != EOS)
    {
        str[currIndex] = newStr[currIndex];
        currIndex++;
    }
    Free(newStr);
}

bool32 StringHasManualBreaks(u8 *src)
{
    u32 charIndex = 0;
    while (src[charIndex] != EOS)
    {
        if (src[charIndex] == 0xFA || src[charIndex] == 0xFE)
            return TRUE;
        charIndex++;
    }
    return FALSE;
}
