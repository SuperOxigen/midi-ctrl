/*
 * MIDI Controller - MIDI Standard Definitions
 *
 * Copyright (c) 2020 Alex Dale
 * This project is licensed under the terms of the MIT license.
 * See LICENSE for details.
 */
#ifndef _MIDI_DEFS_H_
#define _MIDI_DEFS_H_

/*
 *    General MIDI Values
 */
#define MIDI_CHANNEL_1                    0x00
#define MIDI_CHANNEL_2                    0x01
#define MIDI_CHANNEL_3                    0x02
#define MIDI_CHANNEL_4                    0x03
#define MIDI_CHANNEL_5                    0x04
#define MIDI_CHANNEL_6                    0x05
#define MIDI_CHANNEL_7                    0x06
#define MIDI_CHANNEL_8                    0x07
#define MIDI_CHANNEL_9                    0x08
#define MIDI_CHANNEL_10                   0x09
#define MIDI_CHANNEL_11                   0x0A
#define MIDI_CHANNEL_12                   0x0B
#define MIDI_CHANNEL_13                   0x0C
#define MIDI_CHANNEL_14                   0x0D
#define MIDI_CHANNEL_15                   0x0E
#define MIDI_CHANNEL_16                   0x0F
/* Default channel for drums. */
#define MIDI_DRUM_CHANNEL                 MIDI_CHANNEL_10

/*
 *    MIDI Message Types
 */
#define MIDI_NONE                         0x00
/* Channel-Based Message */
#define MIDI_NOTE_OFF                     0x80
#define MIDI_NOTE_ON                      0x90
#define MIDI_KEY_PRESSURE                 0xA0
#define MIDI_CONTROL_CHANGE               0xB0
#define MIDI_PROGRAM_CHANGE               0xC0
#define MIDI_CHANNEL_PRESSURE             0xD0
#define MIDI_PITCH_WHEEL                  0xE0
/* System Exclusive */
#define MIDI_SYSTEM_EXCLUSIVE             0xF0
/* System Common Messages */
#define MIDI_TIME_CODE                    0xF1
#define MIDI_SONG_POSITION_POINTER        0xF2
#define MIDI_SONG_SELECT                  0xF3
#define MIDI_TUNE_REQUEST                 0xF6
#define MIDI_END_SYSTEM_EXCLUSIVE         0xF7
/* System Real-Time Messages */
#define MIDI_TIMING_CLOCK                 0xF8
#define MIDI_START                        0xFA
#define MIDI_CONTINUE                     0xFB
#define MIDI_STOP                         0xFC
#define MIDI_ACTIVE_SENSING               0xFE
#define MIDI_SYSTEM_RESET                 0xFF

/*
 *    MIDI Notes
 */
#define MIDI_NOTE_ON_VELOCITY             0x40
#define MIDI_MIDDLE_C                     0x58

/*
 *    MIDI Control Numbers
 */
#define MIDI_BANK_SELECT_MSB              0x00
#define MIDI_BANK_SELECT_LSB              0x20
#define MIDI_MODULATION_WHEEL_MSB         0x01
#define MIDI_MODULATION_WHEEL_LSB         0x21
#define MIDI_BREATH_CONTROLLER_MSB        0x02
#define MIDI_BREATH_CONTROLLER_LSB        0x22
#define MIDI_FOOT_CONTROLLER_MSB          0x40
#define MIDI_FOOT_CONTROLLER_LSB          0x20
#define MIDI_PORTAMENTO_TIME_MSB          0x05
#define MIDI_PORTAMENTO_TIME_LSB          0x25
#define MIDI_CHANNEL_VOLUME_MSB           0x07
#define MIDI_CHANNEL_VOLUME_LSB           0x27
#define MIDI_MAIN_VOLUME_MSB              MIDI_CHANNEL_VOLUME_MSB
#define MIDI_MAIN_VOLUME_LSB              MIDI_CHANNEL_VOLUME_LSB
/* Special effects controller */
#define MIDI_BALANCE_MSB                  0x08
#define MIDI_BALANCE_LSB                  0x28
#define MIDI_PAN_MSB                      0x0A
#define MIDI_PAN_LSB                      0x2A
#define MIDI_EXPRESSION_CONTROLLER_MSB    0x0B
#define MIDI_EXPRESSION_CONTROLLER_LSB    0x2B
/* General effects controllers */
#define MIDI_EFFECT_CONTROLLER_1_MSB      0x0C
#define MIDI_EFFECT_CONTROLLER_1_LSB      0x2C
#define MIDI_EFFECT_CONTROLLER_2_MSB      0x0D
#define MIDI_EFFECT_CONTROLLER_2_LSB      0x2D
/* General Purpose, high precision */
#define MIDI_GENERAL_CONTROLLER_1_MSB     0x10
#define MIDI_GENERAL_CONTROLLER_1_LSB     0x30
#define MIDI_GENERAL_CONTROLLER_2_MSB     0x11
#define MIDI_GENERAL_CONTROLLER_2_LSB     0x31
#define MIDI_GENERAL_CONTROLLER_3_MSB     0x12
#define MIDI_GENERAL_CONTROLLER_3_LSB     0x32
#define MIDI_GENERAL_CONTROLLER_4_MSB     0x13
#define MIDI_GENERAL_CONTROLLER_4_LSB     0x33
/* General Purpose, low precision */
#define MIDI_GENERAL_CONTROLLER_5         0x50
#define MIDI_GENERAL_CONTROLLER_6         0x51
#define MIDI_GENERAL_CONTROLLER_7         0x52
#define MIDI_GENERAL_CONTROLLER_8         0x53
/* Toggled.  See MIDI_CONTROLLER_(ON|OFF) */
#define MIDI_DAMBER_PEDAL                 0x40
#define MIDI_PORTAMENTO                   0x41
#define MIDI_SOSTENUTO                    0x42
#define MIDI_SOFT_PEDAL                   0x43
#define MIDI_LEGATO_FOOTSWITCH            0x44
#define MIDI_HOLD_2                       0x45
/* Sound Controller */
#define MIDI_SOUND_CONTROLLER_1           0x46
#define MIDI_SOUND_CONTROLLER_2           0x47
#define MIDI_SOUND_CONTROLLER_3           0x48
#define MIDI_SOUND_CONTROLLER_4           0x49
#define MIDI_SOUND_CONTROLLER_5           0x4A
#define MIDI_SOUND_CONTROLLER_6           0x4B
#define MIDI_SOUND_CONTROLLER_7           0x4C
#define MIDI_SOUND_CONTROLLER_8           0x4D
#define MIDI_SOUND_CONTROLLER_9           0x4E
#define MIDI_SOUND_CONTROLLER_10          0x4F
/* Sound Controller, dafaults */
#define MIDI_SOUND_VARIATION              MIDI_SOUND_CONTROLLER_1
#define MIDI_HARMONIC_INTENSITY           MIDI_SOUND_CONTROLLER_2
#define MIDI_TIMBER_INTENSITY             MIDI_SOUND_CONTROLLER_2
#define MIDI_RELEASE_TIME                 MIDI_SOUND_CONTROLLER_3
#define MIDI_ATTACH_TIME                  MIDI_SOUND_CONTROLLER_4
#define MIDI_BRIGHTNESS                   MIDI_SOUND_CONTROLLER_5
/* Sound Controller, unofficial defaults. */
#define MIDI_DECAY_TIME                   MIDI_SOUND_CONTROLLER_6
#define MIDI_VIBRATO_RATE                 MIDI_SOUND_CONTROLLER_7
#define MIDI_VIBRATO_DEPTH                MIDI_SOUND_CONTROLLER_8
#define MIDI_VIBRATO_DELAY                MIDI_SOUND_CONTROLLER_9
/* Portamento Controller */
#define MIDI_PORTAMENTO_CONTROL           0x53
#define MIDI_HIGH_RESOLUTION_VELOCITY_PREFIX 0x58
/* Effects Depth */
#define MIDI_EFFECTS_1_DEPTH              0x5B
#define MIDI_EFFECTS_2_DEPTH              0x5C
#define MIDI_EFFECTS_3_DEPTH              0x5D
#define MIDI_EFFECTS_4_DEPTH              0x5E
#define MIDI_EFFECTS_5_DEPTH              0x5F
/* Effects Depth, defaults */
#define MIDI_REVERB_SEND_LEVEL            MIDI_EFFECTS_1_DEPTH
#define MIDI_EXTERNAL_EFFECTS_DEPTH       MIDI_EFFECTS_1_DEPTH  /* depricated */
#define MIDI_TREMOLO_DEPTH                MIDI_EFFECTS_2_DEPTH  /* depricated */
#define MIDI_CHORUS_SEND_LEVEL            MIDI_EFFECTS_3_DEPTH
#define MIDI_CHORUS_DEPTH                 MIDI_EFFECTS_3_DEPTH  /* depricated */
#define MIDI_CELESTE_DEPTH                MIDI_EFFECTS_4_DEPTH  /* depricated */
#define MIDI_PHASER_DEPTH                 MIDI_EFFECTS_5_DEPTH  /* depricated */
/* Parameter Data */
#define MIDI_DATA_ENTRY_MSB               0x06
#define MIDI_DATA_ENTRY_LSB               0x26
#define MIDI_DATA_INCREMENT               0x60
#define MIDI_DATA_DECREMENT               0x61
/* Parameter Numbers */
#define MIDI_NON_REGISTERED_PARAMETER_NUMBER_LSB \
                                          0x62
#define MIDI_NON_REGISTERED_PARAMETER_NUMBER_MSB \
                                          0x63
#define MIDI_NRPN_LSB MIDI_NON_REGISTERED_PARAMETER_NUMBER_LSB
#define MIDI_NRPN_MSB MIDI_NON_REGISTERED_PARAMETER_NUMBER_MSB
#define MIDI_REGISTERED_PARAMETER_NUMBER_LSB \
                                          0x64
#define MIDI_REGISTERED_PARAMETER_NUMBER_MSB \
                                          0x65
#define MIDI_RPN_LSB MIDI_REGISTERED_PARAMETER_NUMBER_LSB
#define MIDI_RPN_MSB MIDI_REGISTERED_PARAMETER_NUMBER_MSB
/* Channel Mode Controls */
#define MIDI_ALL_SOUND_OFF                0x78
#define MIDI_RESET_ALL_CONTROLLERS        0x79
#define MIDI_LOCAL_CONTROL                0x7A
#define MIDI_ALL_NOTES_OFF                0x7B
#define MIDI_OMNI_MODE_OFF                0x7C
#define MIDI_OMNI_MODE_ON                 0x7D
#define MIDI_MONO_MODE_ON                 0x7E
#define MIDI_POLY_MODE_ON                 0x7F

/* On/Off arguments for certain control numbers. */
#define MIDI_CONTROL_ON                    127
#define MIDI_CONTROL_OFF                     0
/* Ranges for on/off */
#define MIDI_CONTROL_ON_MIN                 64
#define MIDI_CONTROL_ON_MAX                127
#define MIDI_CONTROL_OFF_MIN                 0
#define MIDI_CONTROL_OFF_MAX                63

/* Balance effect level */
#define MIDI_BALANCE_FULL_LEFT            0x00
#define MIDI_BALANCE_EQUAL                0x40
#define MIDI_BALANCE_FULL_RIGHT           0x7F
#define MIDI_PAN_HARD_LEFT                0x00
#define MIDI_PAN_CENTER                   0x40
#define MIDI_PAN_HARD_RIGHT               0x7F

/* Foot switch */

/*
 *    Registered Parameter Numbers
 */
#define MIDI_PITCH_BEND_SENSITIVITY_MSB   0x00
#define MIDI_PITCH_BEND_SENSITIVITY_LSB   0x00
#define MIDI_CHANNEL_FINE_TUNING_MSB      0x00
#define MIDI_CHANNEL_FINE_TUNING_LSB      0x01
#define MIDI_CHANNEL_COARSE_TUNING_MSB    0x00
#define MIDI_CHANNEL_COARSE_TUNING_LSB    0x02
#define MIDI_TUNING_PROGRAM_CHANGE_MSB    0x00
#define MIDI_TUNING_PROGRAM_CHANGE_LSB    0x03
#define MIDI_TUNING_BANK_SELECT_MSB       0x00
#define MIDI_TUNING_BANK_SELECT_LSB       0x04
#define MIDI_MODULATION_DEPTH_RANGE_MSB   0x00
#define MIDI_MODULATION_DEPTH_RANGE_LSB   0x05
#define MIDI_MPE_CONFIGURATION_MESSAGE_MSB 0x00
#define MIDI_MPE_CONFIGURATION_MESSAGE_LSB 0x06
/* 3D Sound Controllers */
#define MIDI_AZIMUTH_ANGLE_MSB            0x3D
#define MIDI_AZIMUTH_ANGLE_LSB            0x00
#define MIDI_ELEVATION_MSB                0x3D
#define MIDI_ELEVATION_LSB                0x01
#define MIDI_GAIN_MSB                     0x3D
#define MIDI_GAIN_LSB                     0x02
#define MIDI_DISTANCE_RATIO_MSB           0x3D
#define MIDI_DISTANCE_RATIO_LSB           0x03
#define MIDI_MAXIMUM_DISTANCE_MSB         0x3D
#define MIDI_MAXIMUM_DISTANCE_LSB         0x04
#define MIDI_GAIN_AT_MAXIMUM_DISTANCE_MSB 0x3D
#define MIDI_GAIN_AT_MAXIMUM_DISTANCE_LSB 0x05
#define MIDI_REFERENCE_DISTANCE_RATIO_MSB 0x3D
#define MIDI_REFERENCE_DISTANCE_RATIO_LSB 0x06
#define MIDI_PAN_SPREAD_ANGLE_MSB         0x3D
#define MIDI_PAN_SPREAD_ANGLE_LSB         0x07
#define MIDI_ROLL_ANGLE_MSB               0x3D
#define MIDI_ROLL_ANGLE_LSB               0x08
/* Set Function Number to NULL */
#define MIDI_NULL_PARAMETER_MSB           0x7F
#define MIDI_NULL_PARAMETER_LSB           0x7F

/*
 *    MIDI Programs / Instruments
 */
/* GM1 Instrument "Program" Numbers */
#define MIDI_ACOUSTIC_GRAND_PIANO         0
#define MIDI_BRIGHT_ACOUSTIC_PIANO        1
#define MIDI_ELECTRIC_GRAND_PIANO         2
#define MIDI_HONKY_TONK_PIANO             3
#define MIDI_ELECTRIC_PIANO_1             4
#define MIDI_ELECTRIC_PIANO_2             5
#define MIDI_HARPSICHORD                  6
#define MIDI_CLAVI                        7
#define MIDI_CELESTA                      8
#define MIDI_GLOCKENSPIEL                 9
#define MIDI_MUSIC_BOX                    10
#define MIDI_VIBRAPHONE                   11
#define MIDI_MARIMBA                      12
#define MIDI_XYLOPHONE                    13
#define MIDI_TUBULAR_BELLS                14
#define MIDI_DUCLIMER                     15
#define MIDI_DRAWBAR_ORGAN                16
#define MIDI_PERCUSSIVE_ORGAN             17
#define MIDI_ROCK_ORGAN                   18
#define MIDI_CHURCH_ORGAN                 19
#define MIDI_REED_ORGAN                   20
#define MIDI_ACCORDION                    21
#define MIDI_HARMONIC                     22
#define MIDI_TANGO_ACCORDION              23
#define MIDI_ACOUSTIC_GUITAR_NYLON        24
#define MIDI_ACOUSTIC_GUITAR_STEEL        25
#define MIDI_ELECTRIC_GUITAR_JAZZ         26
#define MIDI_ELECTRIC_GUITAR_CLEAN        27
#define MIDI_ELECTRIC_GUITAR_MUTED        28
#define MIDI_OVERDRIVE_GUITAR             29
#define MIDI_DISTORTION_GUITAR            30
#define MIDI_GUITAR_HARMONICS             31
#define MIDI_ACOUSTIC_BASS                32
#define MIDI_ELECTRIC_BASS_FINGER         33
#define MIDI_ELECTRIC_BASS_PICK           34
#define MIDI_FRETLESS_BASS                35
#define MIDI_SLAP_BASS_1                  36
#define MIDI_SLAP_BASS_2                  37
#define MIDI_SYNTH_BASS_1                 38
#define MIDI_SYNTH_BASS_2                 39
#define MIDI_VIOLIN                       40
#define MIDI_VIOLA                        41
#define MIDI_CELLO                        42
#define MIDI_CONTRABASS                   43
#define MIDI_TREMOLO_STRINGS              44
#define MIDI_PIZZICATO_STRINGS            45
#define MIDI_ORCHESTRA_HARP               46
#define MIDI_TIMPANI                      47
#define MIDI_STRING_ENSEMBLE_1            48
#define MIDI_STRING_ENSEMBLE_2            49
#define MIDI_SYNTH_STRINGS_1              50
#define MIDI_SYNTH_STRINGS_2              51
#define MIDI_CHOIR_AAHS                   52
#define MIDI_VOICE_OOHS                   53
#define MIDI_SYNTH_VOICE                  54
#define MIDI_ORCHESTRA_HIT                55
#define MIDI_TRUMPET                      56
#define MIDI_TROMBONE                     57
#define MIDI_TUBA                         58
#define MIDI_MUTED_TRUMPET                59
#define MIDI_FRENCH_HORN                  60
#define MIDI_BRASS_SECTION                61
#define MIDI_SYNTH_BRASS_1                62
#define MIDI_SYNTH_BRASS_2                63
#define MIDI_SOPRANO_SAX                  64
#define MIDI_ALTO_SAX                     65
#define MIDI_TENOR_SAX                    66
#define MIDI_BARITONE_SAX                 67
#define MIDI_OBOE                         68
#define MIDI_ENGLISH_HORN                 69
#define MIDI_BASSOON                      70
#define MIDI_CLARINET                     71
#define MIDI_PICCOLO                      72
#define MIDI_FLUTE                        73
#define MIDI_RECORDER                     74
#define MIDI_PAN_FLUTE                    75
#define MIDI_BLOWN_BOTTLE                 76
#define MIDI_SHAKUHACHI                   77
#define MIDI_WHISTLE                      78
#define MIDI_OCARINA                      79
#define MIDI_LEAD_1_SQUARE                80
#define MIDI_LEAD_2_SAWTOOTH              81
#define MIDI_LEAD_3_CALLIOPE              82
#define MIDI_LEAD_4_CHIFF                 83
#define MIDI_LEAD_5_CHARANG               84
#define MIDI_LEAD_6_VOICE                 85
#define MIDI_LEAD_7_FIFTHS                86
#define MIDI_LEAD_8_BASS_LEAD             87
#define MIDI_PAD_1_NEW_AGE                88
#define MIDI_PAD_2_WARM                   89
#define MIDI_PAD_3_POLYSYNTH              90
#define MIDI_PAD_4_CHOIR                  91
#define MIDI_PAD_5_BOWED                  92
#define MIDI_PAD_6_METALLIC               93
#define MIDI_PAD_7_HALO                   94
#define MIDI_PAD_8_SWEEP                  95
#define MIDI_FX_1_RAIN                    96
#define MIDI_FX_2_SOUNDTRACK              97
#define MIDI_FX_3_CRYSTAL                 98
#define MIDI_FX_4_ATMOSPHERE              99
#define MIDI_FX_5_BRIGHTNESS              100
#define MIDI_FX_6_GOBLINS                 101
#define MIDI_FX_7_ECHOES                  102
#define MIDI_FX_8_SCI_FI                  103
#define MIDI_SITAR                        104
#define MIDI_BANJO                        105
#define MIDI_SHAMISEN                     106
#define MIDI_KOTO                         107
#define MIDI_KALIMBA                      108
#define MIDI_BAG_PIPE                     109
#define MIDI_FIDDLE                       110
#define MIDI_SHANAI                       111
#define MIDI_TINKLE_BELL                  112
#define MIDI_AGOGO                        113
#define MIDI_STEEL_DRUMS                  114
#define MIDI_WOODBLOCK                    115
#define MIDI_TAIKO_DRUM                   116
#define MIDI_MELODIC_TOM                  117
#define MIDI_SYNTH_DRUM                   118
#define MIDI_REVERSE_CYMBAL               119
#define MIDI_GUITAR_FRET_NOISE            120
#define MIDI_BREATH_NOISE                 121
#define MIDI_SEASHORE                     122
#define MIDI_BIRD_TWEET                   123
#define MIDI_TELEPHONE_RING               124
#define MIDI_HELICOPTER                   125
#define MIDI_APPLAUSE                     126
#define MIDI_GUNSHOT                      127

/* GM1 Percussion Map */
#define MIDI_ACOUSTIC_BASS_DRUM           34
#define MIDI_BASS_DRUM_1                  35
#define MIDI_SIDE_STICK                   36
#define MIDI_ACOUSTIC_SNARE               37
#define MIDI_HAND_CLAP                    38
#define MIDI_ELECTRIC_SNARE               39
#define MIDI_LOW_FLOOR_TOM                40
#define MIDI_CLOSED_HI_HAT                41
#define MIDI_HIGH_FLOOR_TOM               42
#define MIDI_PEDAL_HI_HAT                 43
#define MIDI_LOW_TOM                      44
#define MIDI_OPEN_HI_HAT                  45
#define MIDI_LOW_MID_TOM                  46
#define MIDI_HI_MID_TOM                   47
#define MIDI_CRASH_CYMBAL_1               48
#define MIDI_HIGH_TOM                     49
#define MIDI_RIDE_CYMBAL_1                50
#define MIDI_CHINESE_CYMBAL               51
#define MIDI_RIDE_BELL                    52
#define MIDI_TAMBOURINE                   53
#define MIDI_SPLASH_CYMBAL                54
#define MIDI_COWBELL                      55
#define MIDI_CRASH_CYMBAL_2               56
#define MIDI_VIBRASLAP                    57
#define MIDI_RIDE_CYMBAL_2                58
#define MIDI_HI_BONGO                     59
#define MIDI_LOW_BONGO                    60
#define MIDI_MUTE_HI_CONGO                61
#define MIDI_OPEN_HI_CONGO                62
#define MIDI_LOW_CONGO                    63
#define MIDI_HIGH_TIMBALE                 64
#define MIDI_LOW_TIMBALE                  65
#define MIDI_HIGH_AGOGO                   66
#define MIDI_LOW_AGOGO                    67
#define MIDI_CABASA                       68
#define MIDI_MARACAS                      69
#define MIDI_SHORT_WHISTLE                70
#define MIDI_LONG_WHISTLE                 71
#define MIDI_SHORT_GUIRO                  72
#define MIDI_LONG_GUIRO                   73
#define MIDI_CLAVES                       74
#define MIDI_HI_WOOD_BLOCK                75
#define MIDI_LOW_WOOD_BLOCK               76
#define MIDI_MUTE_CUICA                   77
#define MIDI_OPEN_CUICA                   78
#define MIDI_MUTE_TRIANGLE                79
#define MIDI_OPEN_TRIANGLE                80

/*
 *    MIDI Time Code
 */
/* Time code types.  (LSN = Lease significant nibble) */
#define MIDI_FRAME_COUNT_LSN              0x00
#define MIDI_FRAME_COUNT_MSN              0x10
#define MIDI_SECONDS_COUNT_LSN            0x20
#define MIDI_SECONDS_COUNT_MSN            0x30
#define MIDI_MINUTES_COUNT_LSN            0x40
#define MIDI_MINUTES_COUNT_MSN            0x50
#define MIDI_HOURS_COUNT_LSN              0x60
#define MIDI_HOURS_COUNT_MSN              0x70
/* Time code value for frame rate. */
#define MIDI_24_FPS                       0x00
#define MIDI_25_FPS                       0x20
#define MIDI_30_FPS_DROP_FRAME            0x40
#define MIDI_30_FPS_NON_DROP              0x60

/*
 *  MIDI Device Control
 */
#define MIDI_MASTER_VOLUME_OFF            0x0000
#define MIDI_MASTER_VOLUME_MAX            0x3FFF
#define MIDI_MASTER_BALANCE_HARD_LEFT     0x0000
#define MIDI_MASTER_BALANCE_EQUAL         0x2000
#define MIDI_MASTER_BALANCE_HARD_RIGHT    0x3FFF

/*
 *  System Univeral
 */
/* Special Manufacturer IDs */
#define MIDI_SPECIAL_ID                   0x7D
#define MIDI_NON_REAL_TIME_ID             0x7E
#define MIDI_REAL_TIME_ID                 0x7F

/* Special Device ID */
#define MIDI_ALL_CALL                     0x7F

/* Non-Realtime Sub Id */
#define MIDI_DUMP_HEADER                  0x01
#define MIDI_DATA_PACKET                  0x02
#define MIDI_DUMP_REQUEST                 0x03
#define MIDI_NRT_TIME_CODE                0x04
#define MIDI_SAMPLE_DUMP_EXT              0x05
#define MIDI_GENERAL_INFO                 0x06
#define MIDI_FILE_DUMP                    0x07
#define MIDI_TUNING_DUMP                  0x08
#define MIDI_GENERAL_MIDI                 0x09
#define MIDI_DOWNLOADABLE_SOUNDS          0x0A
#define MIDI_FILE_REFERENCE               0x0B
#define MIDI_VISUAL_CONTROL               0x0C
#define MIDI_CAPABILITY_INFO              0x0D
#define MIDI_EOF                          0x7B
#define MIDI_WAIT                         0x7C
#define MIDI_CANCEL                       0x7D
#define MIDI_NAK                          0x7E
#define MIDI_ACK                          0x7F

/* Sample Dump Ext Sub-ID */
#define MIDI_SAMPLE_LOOP_RESPONSE         0x01
#define MIDI_SAMPLE_LOOP_REQUEST          0x02
#define MIDI_SAMPLE_NAME_RESPONSE         0x03
#define MIDI_SAMPLE_NAME_REQUEST          0x04
#define MIDI_EXT_DUMP_HEADER              0x05
#define MIDI_EXT_LOOP_RESPONSE            0x06
#define MIDI_EXT_LOOP_REQUEST             0x07

/* General Information Sub ID */
#define MIDI_DEVICE_INQUIRY_RESPONSE      0x01
#define MIDI_DEVICE_INQUIRY_REQUEST       0x02

/* Loop types */
#define MIDI_LOOP_FORWARD_ONLY            0x00
#define MIDI_LOOP_BIDIRECTIONAL           0x01
#define MIDI_LOOP_OFF                     0x7F

/* General MIDI Sub ID */
#define MIDI_GENERAL_MIDI_ON              0x01
#define MIDI_GENERAL_MIDI_OFF             0x02

/* Realtime Sub Id */
#define MIDI_RT_TIME_CODE                 0x01
#define MIDI_SHOW_CONTROL                 0x02
#define MIDI_NOTATION_INFO                0x03
#define MIDI_DEVICE_CONTROL               0x04
#define MIDI_CUEING                       0x05
#define MIDI_MMC_COMMANDS                 0x06
#define MIDI_MMC_RESPONSES                0x07
#define MIDI_TUNING_STANDARD              0x08
#define MIDI_CONTROLLER_DEST              0x09
#define MIDI_KEY_BASED_CONTROL            0x0A
#define MIDI_SCALABLE_POLYPHONY           0x0B
#define MIDI_MOBILE_PHONE_CONTROL         0x0C

/* RT Time Code Messages */
#define MIDI_FULL_TIME_CODE               0x01
#define MIDI_USER_BITS                    0x02

/* Notation Info Sub ID */
#define MIDI_BAR_NUMBER                   0x01
#define MIDI_TIME_SIGNATURE_IMMEDIATE     0x02
#define MIDI_TIME_SIGNATURE_DELAY         0x42

/* Device Control Sub ID */
#define MIDI_MASTER_VOLUME                0x01
#define MIDI_MASTER_BALANCE               0x02

#endif  /* _MIDI_DEFS_H_ */
