#pragma once

// このファイルは、clap_plugin_descriptor.features内で使用されることを
// 意図した標準プラグイン機能のセットを提供します。
//
// 実用的な理由で、スペースを避けて代わりに`-`を使用し、
// 機能配列を生成するスクリプトを促進します。
//
// 非標準機能は次のようにフォーマットされるべきです: "$namespace:$feature"

/////////////////////
// プラグインカテゴリ //
/////////////////////

// プラグインがノートイベントを処理してオーディオを生成できる場合、この機能を追加してください
#define CLAP_PLUGIN_FEATURE_INSTRUMENT "instrument"

// プラグインがオーディオエフェクトである場合、この機能を追加してください
#define CLAP_PLUGIN_FEATURE_AUDIO_EFFECT "audio-effect"

// プラグインがノートエフェクトまたはノートジェネレーター/シーケンサーである場合、この機能を追加してください
#define CLAP_PLUGIN_FEATURE_NOTE_EFFECT "note-effect"

// プラグインがオーディオをノートに変換する場合、この機能を追加してください
#define CLAP_PLUGIN_FEATURE_NOTE_DETECTOR "note-detector"

// プラグインがアナライザーである場合、この機能を追加してください
#define CLAP_PLUGIN_FEATURE_ANALYZER "analyzer"

/////////////////////////
// プラグインサブカテゴリ //
/////////////////////////

#define CLAP_PLUGIN_FEATURE_SYNTHESIZER "synthesizer"
#define CLAP_PLUGIN_FEATURE_SAMPLER "sampler"
#define CLAP_PLUGIN_FEATURE_DRUM "drum" // 単一ドラム用
#define CLAP_PLUGIN_FEATURE_DRUM_MACHINE "drum-machine"

#define CLAP_PLUGIN_FEATURE_FILTER "filter"
#define CLAP_PLUGIN_FEATURE_PHASER "phaser"
#define CLAP_PLUGIN_FEATURE_EQUALIZER "equalizer"
#define CLAP_PLUGIN_FEATURE_DEESSER "de-esser"
#define CLAP_PLUGIN_FEATURE_PHASE_VOCODER "phase-vocoder"
#define CLAP_PLUGIN_FEATURE_GRANULAR "granular"
#define CLAP_PLUGIN_FEATURE_FREQUENCY_SHIFTER "frequency-shifter"
#define CLAP_PLUGIN_FEATURE_PITCH_SHIFTER "pitch-shifter"

#define CLAP_PLUGIN_FEATURE_DISTORTION "distortion"
#define CLAP_PLUGIN_FEATURE_TRANSIENT_SHAPER "transient-shaper"
#define CLAP_PLUGIN_FEATURE_COMPRESSOR "compressor"
#define CLAP_PLUGIN_FEATURE_EXPANDER "expander"
#define CLAP_PLUGIN_FEATURE_GATE "gate"
#define CLAP_PLUGIN_FEATURE_LIMITER "limiter"

#define CLAP_PLUGIN_FEATURE_FLANGER "flanger"
#define CLAP_PLUGIN_FEATURE_CHORUS "chorus"
#define CLAP_PLUGIN_FEATURE_DELAY "delay"
#define CLAP_PLUGIN_FEATURE_REVERB "reverb"

#define CLAP_PLUGIN_FEATURE_TREMOLO "tremolo"
#define CLAP_PLUGIN_FEATURE_GLITCH "glitch"

#define CLAP_PLUGIN_FEATURE_UTILITY "utility"
#define CLAP_PLUGIN_FEATURE_PITCH_CORRECTION "pitch-correction"
#define CLAP_PLUGIN_FEATURE_RESTORATION "restoration" // 音を修復
#define CLAP_PLUGIN_FEATURE_MULTI_EFFECTS "multi-effects"

#define CLAP_PLUGIN_FEATURE_MIXING "mixing"
#define CLAP_PLUGIN_FEATURE_MASTERING "mastering"

////////////////////////
// オーディオ機能      //
////////////////////////

#define CLAP_PLUGIN_FEATURE_MONO "mono"
#define CLAP_PLUGIN_FEATURE_STEREO "stereo"
#define CLAP_PLUGIN_FEATURE_SURROUND "surround"
#define CLAP_PLUGIN_FEATURE_AMBISONIC "ambisonic"