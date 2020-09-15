/*
 * MIDI Controller - MIDI General MIDI 1
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#include <ctype.h>
#include <string.h>

#include "program_memory.h"
#include "smart_string.h"

#include "midi_defs.h"
#include "midi_gm1.h"

#ifdef _MIDI_SHORT_PROGRAM_NAME
#define NAME_SELECT(reg_name, short_name) \
  (short_name)
#else
#define NAME_SELECT(reg_name, short_name) \
  (reg_name)
#endif  /* _MIDI_SHORT_PROGRAM_NAME */

/* Program Names */
static char const kMidiAcousticGrandPiano[] __ROM_SECTION =
    NAME_SELECT("Acoustic Grand Piano", "Grand Piano");
static char const kMidiBrightAcousticPiano[] __ROM_SECTION =
    NAME_SELECT("Bright Acoustic Piano", "Bright Piano");
static char const kMidiElectricGrandPiano[] __ROM_SECTION =
    "Electric Grand Piano";
static char const kMidiHonkyTonkPiano[] __ROM_SECTION = "Honky Tonk Piano";
static char const kMidiElectricPianoOne[] __ROM_SECTION =
    NAME_SELECT("Electric Piano 1", "EP 1");
static char const kMidiElectricPianoTwo[] __ROM_SECTION =
    NAME_SELECT("Electric Piano 2", "EP 2");
static char const kMidiHarpsichord[] __ROM_SECTION = "Harpsichord";
static char const kMidiClavi[] __ROM_SECTION = "Clavi";
static char const kMidiCelesta[] __ROM_SECTION = "Celesta";
static char const kMidiGlockenspiel[] __ROM_SECTION = "Glockenspiel";
static char const kMidiMusicBox[] __ROM_SECTION = "Music Box";
static char const kMidiVibraphone[] __ROM_SECTION = "Vibraphone";
static char const kMidiMarimba[] __ROM_SECTION = "Marimba";
static char const kMidiXylophone[] __ROM_SECTION = "Xylophone";
static char const kMidiTubularBells[] __ROM_SECTION = "Tubular Bells";
static char const kMidiDuclimer[] __ROM_SECTION = "Duclimer";
static char const kMidiDrawbarOrgan[] __ROM_SECTION = "Drawbar Organ";
static char const kMidiPercussiveOrgan[] __ROM_SECTION = "Percussion Organ";
static char const kMidiRockOrgan[] __ROM_SECTION = "Rock Organ";
static char const kMidiChurchOrgan[] __ROM_SECTION = "Church Organ";
static char const kMidiReedOrgan[] __ROM_SECTION = "Reed Organ";
static char const kMidiAccordion[] __ROM_SECTION = "Accordion";
static char const kMidiHarmonic[] __ROM_SECTION = "Harmonic";
static char const kMidiTangoAccordion[] __ROM_SECTION = "Tango Accordion";
static char const kMidiAcousticGuitarNylon[] __ROM_SECTION =
    NAME_SELECT("Acoustic Guitar Nylon", "Nylon Guitar");
static char const kMidiAcousticGuitarSteel[] __ROM_SECTION =
    NAME_SELECT("Acoustic Guitar Steel", "Steel Guitar");
static char const kMidiElectricGuitarJazz[] __ROM_SECTION =
    NAME_SELECT("Electric Guitar Jazz", "Jazz Guitar");
static char const kMidiElectricGuitarClean[] __ROM_SECTION =
    NAME_SELECT("Electric Guitar Clean", "Clean Guitar");
static char const kMidiElectricGuitarMuted[] __ROM_SECTION =
    NAME_SELECT("Electric Guitar Muted", "Muted Guitar");
static char const kMidiOverdriveGuitar[] __ROM_SECTION = "Overdrive Guitar";
static char const kMidiDistortionGuitar[] __ROM_SECTION = "Distortion Guitar";
static char const kMidiGuitarHarmonics[] __ROM_SECTION = "Guitar Harmonics";
static char const kMidiAcousticBass[] __ROM_SECTION = "Acoustic Bass";
static char const kMidiElectricBassFinger[] __ROM_SECTION =
    "Electric Bass Finger";
static char const kMidiElectricBassPick[] __ROM_SECTION = "Electric Bass Pick";
static char const kMidiFretlessBass[] __ROM_SECTION = "Fretless Bass";
static char const kMidiSlapBaseOne[] __ROM_SECTION = "Slap Bass 1";
static char const kMidiSlapBaseTwo[] __ROM_SECTION = "Slap Bass 2";
static char const kMidiSynthBassOne[] __ROM_SECTION = "Synth Bass 1";
static char const kMidiSynthBaseTwo[] __ROM_SECTION = "Synth Bass 2";
static char const kMidiViolin[] __ROM_SECTION = "Violin";
static char const kMidiViola[] __ROM_SECTION = "Viola";
static char const kMidiCello[] __ROM_SECTION = "Cello";
static char const kMidiContrabass[] __ROM_SECTION = "Contrabass";
static char const kMidiTremoloStrings[] __ROM_SECTION = "Tremolo Strings";
static char const kMidiPizzicatoStrings[] __ROM_SECTION = "Pizzicato Strings";
static char const kMidiOrchestraHarp[] __ROM_SECTION =
    NAME_SELECT("Orchestra Harp", "Harp");
static char const kMidiTimpani[] __ROM_SECTION = "Timpani";
static char const kMidiStringEnsembleOne[] __ROM_SECTION = "String Ensemble 1";
static char const kMidiStringEnsembleTwo[] __ROM_SECTION = "String Ensemble 2";
static char const kMidiSynthStringsOne[] __ROM_SECTION = "Synth Strings 1";
static char const kMidiSynthStringsTwo[] __ROM_SECTION = "Synth Strings 2";
static char const kMidiChoirAahs[] __ROM_SECTION =
    NAME_SELECT("Choir Aahs", "Aahs");
static char const kMidiVoiceOohs[] __ROM_SECTION =
    NAME_SELECT("Voice Oohs", "Oohs");
static char const kMidiSynthVoice[] __ROM_SECTION = "Synth Voice";
static char const kMidiOrchestraHit[] __ROM_SECTION = "Orchestra Hit";
static char const kMidiTrumpet[] __ROM_SECTION = "Trumpet";
static char const kMidiTrombone[] __ROM_SECTION = "Trombone";
static char const kMidiTuba[] __ROM_SECTION = "Tuba";
static char const kMidiMutedTrumpet[] __ROM_SECTION = "Muted Trumpet";
static char const kMidiFrenchHorn[] __ROM_SECTION = "French Horn";
static char const kMidiBrassSection[] __ROM_SECTION = "Brass Section";
static char const kMidiSynthBrassOne[] __ROM_SECTION = "Synth Brass 1";
static char const kMidiSynthBrassTwo[] __ROM_SECTION = "Synth Brass 2";
static char const kMidiSopranoSax[] __ROM_SECTION = "Soprano Sax";
static char const kMidiAltoSax[] __ROM_SECTION = "Alto Sax";
static char const kMidiTenorSax[] __ROM_SECTION = "Tenor Sax";
static char const kMidiBaritonSax[] __ROM_SECTION = "Bariton Sax";
static char const kMidiOboe[] __ROM_SECTION = "Oboe";
static char const kMidiEnglishHorn[] __ROM_SECTION = "English Horn";
static char const kMidiBassoon[] __ROM_SECTION = "Bassoon";
static char const kMidiClarinet[] __ROM_SECTION = "Clarinet";
static char const kMidiPiccolo[] __ROM_SECTION = "Piccolo";
static char const kMidiFlute[] __ROM_SECTION = "Flute";
static char const kMidiRecorder[] __ROM_SECTION = "Recorder";
static char const kMidiPanFlute[] __ROM_SECTION = "Pan Flute";
static char const kMidiBlownBottle[] __ROM_SECTION =
    NAME_SELECT("Blown Bottle", "Bottle");
static char const kMidiShakuhachi[] __ROM_SECTION = "Shakuhachi";
static char const kMidiWhistle[] __ROM_SECTION = "Whistle";
static char const kMidiOcarina[] __ROM_SECTION = "Ocarina";
static char const kMidiLeadOneSquare[] __ROM_SECTION =
    NAME_SELECT("Lead 1 Square", "Square Lead");
static char const kMidiLeadTwoSawTooth[] __ROM_SECTION =
    NAME_SELECT("Lead 2 Sawtooth", "Sawtooth Lead");
static char const kMidiLeadThreeCalliope[] __ROM_SECTION =
    NAME_SELECT("Lead 3 Calliope", "Calliope Lead");
static char const kMidiLeadFourChiff[] __ROM_SECTION =
    NAME_SELECT("Lead 4 Chiff", "Chiff Lead");
static char const kMidiLeadFiveCharang[] __ROM_SECTION =
    NAME_SELECT("Lead 5 Charang", "Charang Lead");
static char const kMidiLeadSixVoice[] __ROM_SECTION =
    NAME_SELECT("Lead 6 Voice", "Voice Lead");
static char const kMidiLeadSevenFifths[] __ROM_SECTION =
    NAME_SELECT("Lead 7 Fifths", "Fifths Lead");
static char const kMidiLeadEightBassLead[] __ROM_SECTION =
    NAME_SELECT("Lead 8 Bass Lead", "Bass Lead");
static char const kMidiPadOneNewAge[] __ROM_SECTION =
    NAME_SELECT("Pad 1 New Age", "New Age Pad");
static char const kMidiPadTwoWarm[] __ROM_SECTION =
    NAME_SELECT("Pad 2 Warm", "Warm Pad");
static char const kMidiPadThreePolysynth[] __ROM_SECTION =
    NAME_SELECT("Pad 3 Polysynth", "Polysynth Pad");
static char const kMidiPadFourChoir[] __ROM_SECTION =
    NAME_SELECT("Pad 4 Choir", "Choir Pad");
static char const kMidiPadFiveBowed[] __ROM_SECTION =
    NAME_SELECT("Pad 5 Powed", "Powed Pad");
static char const kMidiPadSixMetallic[] __ROM_SECTION =
    NAME_SELECT("Pad 6 Metallic", "Metallic Pad");
static char const kMidiPadSevenHalo[] __ROM_SECTION =
    NAME_SELECT("Pad 7 Halo", "Halo Pad");
static char const kMidiPadEightSweep[] __ROM_SECTION =
    NAME_SELECT("Pad 8 Sweep", "Sweep Pad");
static char const kMidiFxOneRain[] __ROM_SECTION =
    NAME_SELECT("FX 1 Rain", "Rain");
static char const kMidiFxTwoSoundtrack[] __ROM_SECTION =
    NAME_SELECT("FX 2 Soundtrack", "Soundtrack");
static char const kMidiFxThreeCrystal[] __ROM_SECTION =
    NAME_SELECT("FX 3 Crystal", "Crystal");
static char const kMidiFxFourAtmosphere[] __ROM_SECTION =
    NAME_SELECT("FX 4 Atmosphere", "Atmosphere");
static char const kMidiFxFiveBrightness[] __ROM_SECTION =
    NAME_SELECT("FX 5 Brightness", "Brightness");
static char const kMidiFxSixGoblins[] __ROM_SECTION =
    NAME_SELECT("FX 6 Goblins", "Goblins");
static char const kMidiFxSevenEchoes[] __ROM_SECTION =
    NAME_SELECT("FX 7 Echoes", "Echoes");
static char const kMidiFxEightSciFi[] __ROM_SECTION =
    NAME_SELECT("FX 8 Sci Fi", "Sci Fi");
static char const kMidiSitar[] __ROM_SECTION = "Sitar";
static char const kMidiBanjo[] __ROM_SECTION = "Banjo";
static char const kMidiShamisen[] __ROM_SECTION = "Shamisen";
static char const kMidiKoto[] __ROM_SECTION = "Koto";
static char const kMidiKalimba[] __ROM_SECTION = "Kalimba";
static char const kMidiBagPipe[] __ROM_SECTION = "Bag pipe";
static char const kMidiFiddle[] __ROM_SECTION = "Fiddle";
static char const kMidiGhanai[] __ROM_SECTION = "Ghanai";
static char const kMidiTinkleBell[] __ROM_SECTION = "Tinkle Bell";
static char const kMidiAgogo[] __ROM_SECTION = "Agogo";
static char const kMidiSteelDrums[] __ROM_SECTION = "Steel Drums";
static char const kMidiWoodblock[] __ROM_SECTION = "Woodblock";
static char const kMidiTaikoDrum[] __ROM_SECTION = "Taiko Drum";
static char const kMidiMelodicTom[] __ROM_SECTION = "Melodic Tom";
static char const kMidiSynthDrum[] __ROM_SECTION = "Synth Drum";
static char const kMidiReverseCymbal[] __ROM_SECTION = "Cymbal";
static char const kMidiGuitarFretNoise[] __ROM_SECTION =
    NAME_SELECT("Guitar Fret Noise", "Fret Noise");
static char const kMidiBreathNoise[] __ROM_SECTION =
    NAME_SELECT("Breath Noise", "Breath");
static char const kMidiSeashore[] __ROM_SECTION = "Seashore";
static char const kMidiBirdTweet[] __ROM_SECTION =
    NAME_SELECT("Bird Tweet", "Tweet");
static char const kMidiTelephoneRing[] __ROM_SECTION =
    NAME_SELECT("Telephone Ring", "Telephone");
static char const kMidiHelicopter[] __ROM_SECTION = "Helicopter";
static char const kMidiApplause[] __ROM_SECTION = "Applause";
static char const kMidiGunshot[] __ROM_SECTION = "Gunshot";

static char const * const kMidiGm1ProgramNameMap[128] __ROM_SECTION = {
  [MIDI_ACOUSTIC_GRAND_PIANO] = kMidiAcousticGrandPiano,
  [MIDI_BRIGHT_ACOUSTIC_PIANO] = kMidiBrightAcousticPiano,
  [MIDI_ELECTRIC_GRAND_PIANO] = kMidiElectricGrandPiano,
  [MIDI_HONKY_TONK_PIANO] = kMidiHonkyTonkPiano,
  [MIDI_ELECTRIC_PIANO_1] = kMidiElectricPianoOne,
  [MIDI_ELECTRIC_PIANO_2] = kMidiElectricPianoTwo,
  [MIDI_HARPSICHORD] = kMidiHarpsichord,
  [MIDI_CLAVI] = kMidiClavi,
  [MIDI_CELESTA] = kMidiCelesta,
  [MIDI_GLOCKENSPIEL] = kMidiGlockenspiel,
  [MIDI_MUSIC_BOX] = kMidiMusicBox,
  [MIDI_VIBRAPHONE] = kMidiVibraphone,
  [MIDI_MARIMBA] = kMidiMarimba,
  [MIDI_XYLOPHONE] = kMidiXylophone,
  [MIDI_TUBULAR_BELLS] = kMidiTubularBells,
  [MIDI_DUCLIMER] = kMidiDuclimer,
  [MIDI_DRAWBAR_ORGAN] = kMidiDrawbarOrgan,
  [MIDI_PERCUSSIVE_ORGAN] = kMidiPercussiveOrgan,
  [MIDI_ROCK_ORGAN] = kMidiRockOrgan,
  [MIDI_CHURCH_ORGAN] = kMidiChurchOrgan,
  [MIDI_REED_ORGAN] = kMidiReedOrgan,
  [MIDI_ACCORDION] = kMidiAccordion,
  [MIDI_HARMONIC] = kMidiHarmonic,
  [MIDI_TANGO_ACCORDION] = kMidiTangoAccordion,
  [MIDI_ACOUSTIC_GUITAR_NYLON] = kMidiAcousticGuitarNylon,
  [MIDI_ACOUSTIC_GUITAR_STEEL] = kMidiAcousticGuitarSteel,
  [MIDI_ELECTRIC_GUITAR_JAZZ] = kMidiElectricGuitarJazz,
  [MIDI_ELECTRIC_GUITAR_CLEAN] = kMidiElectricGuitarClean,
  [MIDI_ELECTRIC_GUITAR_MUTED] = kMidiElectricGuitarMuted,
  [MIDI_OVERDRIVE_GUITAR] = kMidiOverdriveGuitar,
  [MIDI_DISTORTION_GUITAR] = kMidiDistortionGuitar,
  [MIDI_GUITAR_HARMONICS] = kMidiGuitarHarmonics,
  [MIDI_ACOUSTIC_BASS] = kMidiAcousticBass,
  [MIDI_ELECTRIC_BASS_FINGER] = kMidiElectricBassFinger,
  [MIDI_ELECTRIC_BASS_PICK] = kMidiElectricBassPick,
  [MIDI_FRETLESS_BASS] = kMidiFretlessBass,
  [MIDI_SLAP_BASS_1] = kMidiSlapBaseOne,
  [MIDI_SLAP_BASS_2] = kMidiSlapBaseTwo,
  [MIDI_SYNTH_BASS_1] = kMidiSynthBassOne,
  [MIDI_SYNTH_BASS_2] = kMidiSynthBaseTwo,
  [MIDI_VIOLIN] = kMidiViolin,
  [MIDI_VIOLA] = kMidiViola,
  [MIDI_CELLO] = kMidiCello,
  [MIDI_CONTRABASS] = kMidiContrabass,
  [MIDI_TREMOLO_STRINGS] = kMidiTremoloStrings,
  [MIDI_PIZZICATO_STRINGS] = kMidiPizzicatoStrings,
  [MIDI_ORCHESTRA_HARP] = kMidiOrchestraHarp,
  [MIDI_TIMPANI] = kMidiTimpani,
  [MIDI_STRING_ENSEMBLE_1] = kMidiStringEnsembleOne,
  [MIDI_STRING_ENSEMBLE_2] = kMidiStringEnsembleTwo,
  [MIDI_SYNTH_STRINGS_1] = kMidiSynthStringsOne,
  [MIDI_SYNTH_STRINGS_2] = kMidiSynthStringsTwo,
  [MIDI_CHOIR_AAHS] = kMidiChoirAahs,
  [MIDI_VOICE_OOHS] = kMidiVoiceOohs,
  [MIDI_SYNTH_VOICE] = kMidiSynthVoice,
  [MIDI_ORCHESTRA_HIT] = kMidiOrchestraHit,
  [MIDI_TRUMPET] = kMidiTrumpet,
  [MIDI_TROMBONE] = kMidiTrombone,
  [MIDI_TUBA] = kMidiTuba,
  [MIDI_MUTED_TRUMPET] = kMidiMutedTrumpet,
  [MIDI_FRENCH_HORN] = kMidiFrenchHorn,
  [MIDI_BRASS_SECTION] = kMidiBrassSection,
  [MIDI_SYNTH_BRASS_1] = kMidiSynthBrassOne,
  [MIDI_SYNTH_BRASS_2] = kMidiSynthBrassTwo,
  [MIDI_SOPRANO_SAX] = kMidiSopranoSax,
  [MIDI_ALTO_SAX] = kMidiAltoSax,
  [MIDI_TENOR_SAX] = kMidiTenorSax,
  [MIDI_BARITONE_SAX] = kMidiBaritonSax,
  [MIDI_OBOE] = kMidiOboe,
  [MIDI_ENGLISH_HORN] = kMidiEnglishHorn,
  [MIDI_BASSOON] = kMidiBassoon,
  [MIDI_CLARINET] = kMidiClarinet,
  [MIDI_PICCOLO] = kMidiPiccolo,
  [MIDI_FLUTE] = kMidiFlute,
  [MIDI_RECORDER] = kMidiRecorder,
  [MIDI_PAN_FLUTE] = kMidiPanFlute,
  [MIDI_BLOWN_BOTTLE] = kMidiBlownBottle,
  [MIDI_SHAKUHACHI] = kMidiShakuhachi,
  [MIDI_WHISTLE] = kMidiWhistle,
  [MIDI_OCARINA] = kMidiOcarina,
  [MIDI_LEAD_1_SQUARE] = kMidiLeadOneSquare,
  [MIDI_LEAD_2_SAWTOOTH] = kMidiLeadTwoSawTooth,
  [MIDI_LEAD_3_CALLIOPE] = kMidiLeadThreeCalliope,
  [MIDI_LEAD_4_CHIFF] = kMidiLeadFourChiff,
  [MIDI_LEAD_5_CHARANG] = kMidiLeadFiveCharang,
  [MIDI_LEAD_6_VOICE] = kMidiLeadSixVoice,
  [MIDI_LEAD_7_FIFTHS] = kMidiLeadSevenFifths,
  [MIDI_LEAD_8_BASS_LEAD] = kMidiLeadEightBassLead,
  [MIDI_PAD_1_NEW_AGE] = kMidiPadOneNewAge,
  [MIDI_PAD_2_WARM] = kMidiPadTwoWarm,
  [MIDI_PAD_3_POLYSYNTH] = kMidiPadThreePolysynth,
  [MIDI_PAD_4_CHOIR] = kMidiPadFourChoir,
  [MIDI_PAD_5_BOWED] = kMidiPadFiveBowed,
  [MIDI_PAD_6_METALLIC] = kMidiPadSixMetallic,
  [MIDI_PAD_7_HALO] = kMidiPadSevenHalo,
  [MIDI_PAD_8_SWEEP] = kMidiPadEightSweep,
  [MIDI_FX_1_RAIN] = kMidiFxOneRain,
  [MIDI_FX_2_SOUNDTRACK] = kMidiFxTwoSoundtrack,
  [MIDI_FX_3_CRYSTAL] = kMidiFxThreeCrystal,
  [MIDI_FX_4_ATMOSPHERE] = kMidiFxFourAtmosphere,
  [MIDI_FX_5_BRIGHTNESS] = kMidiFxFiveBrightness,
  [MIDI_FX_6_GOBLINS] = kMidiFxSixGoblins,
  [MIDI_FX_7_ECHOES] = kMidiFxSevenEchoes,
  [MIDI_FX_8_SCI_FI] = kMidiFxEightSciFi,
  [MIDI_SITAR] = kMidiSitar,
  [MIDI_BANJO] = kMidiBanjo,
  [MIDI_SHAMISEN] = kMidiShamisen,
  [MIDI_KOTO] = kMidiKoto,
  [MIDI_KALIMBA] = kMidiKalimba,
  [MIDI_BAG_PIPE] = kMidiBagPipe,
  [MIDI_FIDDLE] = kMidiFiddle,
  [MIDI_SHANAI] = kMidiGhanai,
  [MIDI_TINKLE_BELL] = kMidiTinkleBell,
  [MIDI_AGOGO] = kMidiAgogo,
  [MIDI_STEEL_DRUMS] = kMidiSteelDrums,
  [MIDI_WOODBLOCK] = kMidiWoodblock,
  [MIDI_TAIKO_DRUM] = kMidiTaikoDrum,
  [MIDI_MELODIC_TOM] = kMidiMelodicTom,
  [MIDI_SYNTH_DRUM] = kMidiSynthDrum,
  [MIDI_REVERSE_CYMBAL] = kMidiReverseCymbal,
  [MIDI_GUITAR_FRET_NOISE] = kMidiGuitarFretNoise,
  [MIDI_BREATH_NOISE] = kMidiBreathNoise,
  [MIDI_SEASHORE] = kMidiSeashore,
  [MIDI_BIRD_TWEET] = kMidiBirdTweet,
  [MIDI_TELEPHONE_RING] = kMidiTelephoneRing,
  [MIDI_HELICOPTER] = kMidiHelicopter,
  [MIDI_APPLAUSE] = kMidiApplause,
  [MIDI_GUNSHOT] = kMidiGunshot,
};

/* Percussion Names */
static char const kMidiAcousticBassDrum[] __ROM_SECTION = "Acoustic Bass Drum";
static char const kMidiBassDrumOne[] __ROM_SECTION =
    NAME_SELECT("Bass Drum 1", "Bass Drum");
static char const kMidiSideStick[] __ROM_SECTION =
    NAME_SELECT("Side Stick", "Stick");
static char const kMidiAcousticSnare[] __ROM_SECTION = "Acoustic Snare";
static char const kMidiHandClap[] __ROM_SECTION =
    NAME_SELECT("Hand Clap", "Clap");
static char const kMidiElectricSnare[] __ROM_SECTION = "Electric Snare";
static char const kMidiLowFloorTom[] __ROM_SECTION = "Low Floor Tom";
static char const kMidiClosedHiHat[] __ROM_SECTION = "Closed Hi Hat";
static char const kMidiHighFloorTom[] __ROM_SECTION = "High Floor Tom";
static char const kMidiPedalHiHat[] __ROM_SECTION = "Pedal Hi Hat";
static char const kMidiLowTom[] __ROM_SECTION = "Low Tom";
static char const kMidiOpenHiHat[] __ROM_SECTION = "Open Hi Hat";
static char const kMidiLowMidTom[] __ROM_SECTION = "Low Mid Tom";
static char const kMidiHiMidTom[] __ROM_SECTION = "Hi Mid Tom";
static char const kMidiCrashCymbalOne[] __ROM_SECTION = "Crash Cymbal 1";
static char const kMidiHighTom[] __ROM_SECTION = "High Tom";
static char const kMidiRideCymbalOne[] __ROM_SECTION = "Ride Cymbal 1";
static char const kMidiChineseCymbal[] __ROM_SECTION = "Chinese Cymbal";
static char const kMidiRideBell[] __ROM_SECTION = "Ride Bell";
static char const kMidiTambourine[] __ROM_SECTION = "Tambourine";
static char const kMidiSplashCymbal[] __ROM_SECTION = "Splash Cymbal";
static char const kMidiCowbell[] __ROM_SECTION = "Cowbell";
static char const kMidiCrashCymbalTwo[] __ROM_SECTION = "Crash Cymbal 2";
static char const kMidiVibraslap[] __ROM_SECTION = "Vibraslap";
static char const kMidiRideCymbalTwo[] __ROM_SECTION = "Ride Cymbal 2";
static char const kMidiHiBongo[] __ROM_SECTION = "Hi Bongo";
static char const kMidiLowBongo[] __ROM_SECTION = "Low Bongo";
static char const kMidiMuteHiConga[] __ROM_SECTION = "Mute Hi Conga";
static char const kMidiOpenHiConga[] __ROM_SECTION = "Open Hi Conga";
static char const kMidiLowConga[] __ROM_SECTION = "Low Conga";
static char const kMidiHighTimbale[] __ROM_SECTION = "High Timbale";
static char const kMidiLowTimbale[] __ROM_SECTION = "Low Timbale";
static char const kMidiHighAgogo[] __ROM_SECTION = "High Agogo";
static char const kMidiLowAgogo[] __ROM_SECTION = "Low Agogo";
static char const kMidiCabasa[] __ROM_SECTION = "Cabasa";
static char const kMidiMaracas[] __ROM_SECTION = "Maracas";
static char const kMidiShortWhistle[] __ROM_SECTION = "Short Whistle";
static char const kMidiLongWhistle[] __ROM_SECTION = "Long Whistle";
static char const kMidiShortGuiro[] __ROM_SECTION = "Short Guiro";
static char const kMidiLongGuiro[] __ROM_SECTION = "Long Guiro";
static char const kMidiClaves[] __ROM_SECTION = "Claves";
static char const kMidiHiWoodBlock[] __ROM_SECTION = "Hi Wood Block";
static char const kMidiLowWoodBlock[] __ROM_SECTION = "Low Wood Block";
static char const kMidiMuteCuica[] __ROM_SECTION = "Mute Cuica";
static char const kMidiOpenCuica[] __ROM_SECTION = "Open Cuica";
static char const kMidiMuteTriangle[] __ROM_SECTION = "Mute Triangle";
static char const kMidiOpenTriangle[] __ROM_SECTION = "Open Triangle";

#define PERCUSSION_INDEX(percussion_id) \
  (percussion_id - MIDI_ACOUSTIC_BASS_DRUM)

#define PERCUSSION_MAP_SIZE (MIDI_OPEN_TRIANGLE - MIDI_ACOUSTIC_BASS_DRUM + 1)

#define MidiIsPercussionNote(note) \
    ((note) >= MIDI_ACOUSTIC_BASS_DRUM && (note) <= MIDI_OPEN_TRIANGLE)

static char const * const
kMidiGm1PercussionNameMap[PERCUSSION_MAP_SIZE] __ROM_SECTION = {
  [PERCUSSION_INDEX(MIDI_ACOUSTIC_BASS_DRUM)] = kMidiAcousticBassDrum,
  [PERCUSSION_INDEX(MIDI_BASS_DRUM_1)] = kMidiBassDrumOne,
  [PERCUSSION_INDEX(MIDI_SIDE_STICK)] = kMidiSideStick,
  [PERCUSSION_INDEX(MIDI_ACOUSTIC_SNARE)] = kMidiAcousticSnare,
  [PERCUSSION_INDEX(MIDI_HAND_CLAP)] = kMidiHandClap,
  [PERCUSSION_INDEX(MIDI_ELECTRIC_SNARE)] = kMidiElectricSnare,
  [PERCUSSION_INDEX(MIDI_LOW_FLOOR_TOM)] = kMidiLowFloorTom,
  [PERCUSSION_INDEX(MIDI_CLOSED_HI_HAT)] = kMidiClosedHiHat,
  [PERCUSSION_INDEX(MIDI_HIGH_FLOOR_TOM)] = kMidiHighFloorTom,
  [PERCUSSION_INDEX(MIDI_PEDAL_HI_HAT)] = kMidiPedalHiHat,
  [PERCUSSION_INDEX(MIDI_LOW_TOM)] = kMidiLowTom,
  [PERCUSSION_INDEX(MIDI_OPEN_HI_HAT)] = kMidiOpenHiHat,
  [PERCUSSION_INDEX(MIDI_LOW_MID_TOM)] = kMidiLowMidTom,
  [PERCUSSION_INDEX(MIDI_HI_MID_TOM)] = kMidiHiMidTom,
  [PERCUSSION_INDEX(MIDI_CRASH_CYMBAL_1)] = kMidiCrashCymbalOne,
  [PERCUSSION_INDEX(MIDI_HIGH_TOM)] = kMidiHighTom,
  [PERCUSSION_INDEX(MIDI_RIDE_CYMBAL_1)] = kMidiRideCymbalOne,
  [PERCUSSION_INDEX(MIDI_CHINESE_CYMBAL)] = kMidiChineseCymbal,
  [PERCUSSION_INDEX(MIDI_RIDE_BELL)] = kMidiRideBell,
  [PERCUSSION_INDEX(MIDI_TAMBOURINE)] = kMidiTambourine,
  [PERCUSSION_INDEX(MIDI_SPLASH_CYMBAL)] = kMidiSplashCymbal,
  [PERCUSSION_INDEX(MIDI_COWBELL)] = kMidiCowbell,
  [PERCUSSION_INDEX(MIDI_CRASH_CYMBAL_2)] = kMidiCrashCymbalTwo,
  [PERCUSSION_INDEX(MIDI_VIBRASLAP)] = kMidiVibraslap,
  [PERCUSSION_INDEX(MIDI_RIDE_CYMBAL_2)] = kMidiRideCymbalTwo,
  [PERCUSSION_INDEX(MIDI_HI_BONGO)] = kMidiHiBongo,
  [PERCUSSION_INDEX(MIDI_LOW_BONGO)] = kMidiLowBongo,
  [PERCUSSION_INDEX(MIDI_MUTE_HI_CONGO)] = kMidiMuteHiConga,
  [PERCUSSION_INDEX(MIDI_OPEN_HI_CONGO)] = kMidiOpenHiConga,
  [PERCUSSION_INDEX(MIDI_LOW_CONGO)] = kMidiLowConga,
  [PERCUSSION_INDEX(MIDI_HIGH_TIMBALE)] = kMidiHighTimbale,
  [PERCUSSION_INDEX(MIDI_LOW_TIMBALE)] = kMidiLowTimbale,
  [PERCUSSION_INDEX(MIDI_HIGH_AGOGO)] = kMidiHighAgogo,
  [PERCUSSION_INDEX(MIDI_LOW_AGOGO)] = kMidiLowAgogo,
  [PERCUSSION_INDEX(MIDI_CABASA)] = kMidiCabasa,
  [PERCUSSION_INDEX(MIDI_MARACAS)] = kMidiMaracas,
  [PERCUSSION_INDEX(MIDI_SHORT_WHISTLE)] = kMidiShortWhistle,
  [PERCUSSION_INDEX(MIDI_LONG_WHISTLE)] = kMidiLongWhistle,
  [PERCUSSION_INDEX(MIDI_SHORT_GUIRO)] = kMidiShortGuiro,
  [PERCUSSION_INDEX(MIDI_LONG_GUIRO)] = kMidiLongGuiro,
  [PERCUSSION_INDEX(MIDI_CLAVES)] = kMidiClaves,
  [PERCUSSION_INDEX(MIDI_HI_WOOD_BLOCK)] = kMidiHiWoodBlock,
  [PERCUSSION_INDEX(MIDI_LOW_WOOD_BLOCK)] = kMidiLowWoodBlock,
  [PERCUSSION_INDEX(MIDI_MUTE_CUICA)] = kMidiMuteCuica,
  [PERCUSSION_INDEX(MIDI_OPEN_CUICA)] = kMidiOpenCuica,
  [PERCUSSION_INDEX(MIDI_MUTE_TRIANGLE)] = kMidiMuteTriangle,
  [PERCUSSION_INDEX(MIDI_OPEN_TRIANGLE)] = kMidiOpenTriangle
};

#define IsValidFlags(flags) (((flags) & 0x07) == (flags))

/* Length of longest program number "XXX " + null term */
#define NUMBER_BUFFER_SIZE  6

static size_t FormatString(
    uint8_t number, uint8_t flags, char const *pm_src,
    char *buffer, size_t buffer_size) {
  size_t i = 0;
  if (MIDI_GM1_WITH_NUMBER & flags) {
    i = SmartStringDecFormat(
        ((uint32_t) number) + 1u, buffer, buffer_size);
    buffer[i++] = ' ';
  }
  switch (flags & MIDI_GM1_CAPITALIZATION) {
    case MIDI_GM1_CAPITALIZATION: {
      /* Ex. Synth Guitar */
      i = ProgMemoryAppendString(pm_src, buffer, buffer_size);
    } break;
    case MIDI_GM1_UPPER_CASE: {
      /* Ex. ALTO SAX */
      char const *pm_ptr = pm_src;
      char c = 0;
      ProgMemoryCopy(pm_ptr, &c, sizeof(char));
      while (c) {
        if (islower(c)) {
          buffer[i++] = toupper(c);
        } else {
          buffer[i++] = c;
        }
        ProgMemoryCopy(++pm_ptr, &c, sizeof(char));
      }
    } break;
    case MIDI_GM1_LOWER_CASE: {
      /* Ex. electric piano 1 */
      char const *pm_ptr = pm_src;
      char c = 0;
      ProgMemoryCopy(pm_ptr, &c, sizeof(char));
      while (c) {
        if (isupper(c)) {
          buffer[i++] = tolower(c);
        } else {
          buffer[i++] = c;
        }
        ProgMemoryCopy(++pm_ptr, &c, sizeof(char));
      }
    } break;
    default: {
      char const *pm_ptr = pm_src;
      char c = 0;
      ProgMemoryCopy(pm_ptr, &c, sizeof(char));
      /* Ex. SLAP_BASS_1 */
      while (c) {
        if (islower(c)) {
          buffer[i++] = toupper(c);
        } else if (c == ' ') {
          buffer[i++] = '_';
        } else {
          buffer[i++] = c;
        }
        ProgMemoryCopy(++pm_ptr, &c, sizeof(char));
      }
    }
  }
  return i;
}

/* Length of longest name + longest program number "XXX " + null term */
#define PROGRAM_NAME_BUFFER_SIZE  (22 + NUMBER_BUFFER_SIZE)
#define PERCUSSION_NAME_BUFFER_SIZE  (18 + NUMBER_BUFFER_SIZE)

size_t MidiGetGeneralMidiProgramName(
    midi_program_number_t program, uint8_t flags,
    char *name, size_t name_size) {
  if (!MidiIsValidProgramNumber(program) || !IsValidFlags(flags)) return 0;
  if (name == NULL && name_size > 0) return 0;
  char buffer[PROGRAM_NAME_BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  const char *name_ptr = NULL;
  ProgMemoryCopy(&kMidiGm1ProgramNameMap[program], &name_ptr, sizeof(char*));
  size_t const char_count = FormatString(
      program, flags, name_ptr, buffer, sizeof(buffer));
  if (name == NULL) {
    return char_count;
  }
  return SmartStringCopy(buffer, name, name_size);
}

size_t MidiGetGeneralMidiPercussionName(
    uint8_t key, uint8_t flags, char *name, size_t name_size) {
  if (!MidiIsPercussionNote(key) || !IsValidFlags(flags)) return 0;
  if (name == NULL && name_size > 0) return 0;
  char buffer[PERCUSSION_NAME_BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  const char *name_ptr = NULL;
  ProgMemoryCopy(
      &kMidiGm1PercussionNameMap[PERCUSSION_INDEX(key)],
      &name_ptr, sizeof(char*));
  size_t const char_count = FormatString(
      key, flags, name_ptr,
      buffer, sizeof(buffer));
  if (name == NULL) {
    return char_count;
  }
  return SmartStringCopy(buffer, name, name_size);
}
