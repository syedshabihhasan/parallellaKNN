function [ shouldSkip, key, seg_pitch, seg_timbre, tatum_start, tempo, ...
    time_sig,seg_conf,sec_conf,key_conf,seg_loud_max ] = ...
    parallelLoad( filename )
%PARALLELLOAD Summary of this function goes here
%   Detailed explanation goes here

toCheck = {'key','segments_pitches','segments_timbre','tatums_start',...
    'tempo','time_signature','segments_confidence',...
    'sections_confidence','key_confidence','segments_loudness_max'};
shouldSkip = false;
vals = load(filename);
key = [];
seg_pitch = [];
seg_timbre = [];
tatum_start = [];
tempo = [];
time_sig = [];
seg_conf = [];
sec_conf = [];
key_conf = [];
seg_loud_max = [];

%% check if the fields exist, if they do not, skip flag is set to true
for P=1:length(toCheck)
    if ~isfield(vals,toCheck{P})
        shouldSkip = true;
        return;
    else
        valsHasNAN = eval(sprintf('isnan(vals.%s)',toCheck{P}));
        if valsHasNAN
            shouldSkip = true;
            return;
        end
    end
end

%% every field we require exists and is not nan, return the values
key = vals.key;
seg_pitch = vals.segments_pitches;
seg_timbre = vals.segments_timbre;
tatum_start = vals.tatums_start;
tempo = vals.tempo;
time_sig = vals.time_signature;
seg_conf = vals.segments_confidence;
sec_conf = vals.sections_confidence;
key_conf = vals.key_confidence;
seg_loud_max = vals.segments_loudness_max;
end

