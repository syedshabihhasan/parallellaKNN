function [ mapper ] = reduceDataset( fileList, opFolder )
%REDUCEDATASET script to reduce the dataset size
%   @author: Syed Shabih Hasan

parObj = parpool;
n = length(fileList);
mapper = cell(n,1);
parfor P=1:n
    try
    %% load file, see if everything is present, if not skip file
    [shouldSkip, key, seg_pitch, seg_timbre, tatum_start, tempo, ...
        time_sig, seg_conf,sec_conf,key_conf,seg_loud_max] ...
        = parallelLoad(fileList{P});
    if shouldSkip
        disp(sprintf('Either NaN or non existent field for %s',...
            fileList{P}));
        mapper{P} = 'skipped';
        continue;
    end
    %% calculate the stats of segment pitches and tatums start
    [r_p,c_p] = size(seg_pitch);
    if 12 ~= r_p
        disp(sprintf('not 12 (%d,%d) seg_pitches rows, %s',r_p,c_p,...
            fileList{P}));
    end
    seg_pitch_stat = [];
    for Q=1:r_p
        temp = arrayStats(seg_pitch(Q,:));
        seg_pitch_stat(end+1:end+length(temp)) = temp;
    end
    [r_t,c_t] = size(seg_timbre);
    if 12 ~= r_t
        disp(sprintf('uh oh, not 12 (%d,%d) rows for %s',r_p,c_p,...
            fileList{P}));
    end
    seg_timbre_stat = [];
    for Q=1:r_t
        temp = arrayStats(seg_timbre(Q,:));
        seg_timbre_stat(end+1:end+length(temp)) = temp;
    end
    tatum_start_stats = arrayStats(tatum_start);
    segments_confidence_stats = arrayStats(seg_conf);
    sections_confidence_stats = arrayStats(sec_conf);
    segments_loudness_max_stats = arrayStats(seg_loud_max);
    actualFilename = getActualFilename(fileList{P});
    fnameToSave = strcat(opFolder,'/',actualFilename,'.bin');
    %% write the values
    f = fopen(fnameToSave,'w');
    % write key, an integer
    fwrite(f,key,'int',0,'l');
    % write segment pitches stats, doubles
    fwrite(f,seg_pitch_stat,'double',0,'l');
    % write segment timbre stats, doubles
    fwrite(f,seg_timbre_stat,'double',0,'l');
    % write tatum start stats, doubles
    fwrite(f,tatum_start_stats,'double',0,'l');
    % write the tempo, a double
    fwrite(f,tempo,'double',0,'l');
    % write the time signature, an integer
    fwrite(f,time_sig,'int',0,'l');
    % write the segments confidence, doubles
    fwrite(f,segments_confidence_stats,'double',0,'l');
    % write the sections confidence, doubles
    fwrite(f,sections_confidence_stats,'double',0,'l');
    % write the segments loudness max, doubles
    fwrite(f,segments_loudness_max_stats,'double',0,'l');
    % write the key confidence, double
    fwrite(f,key_conf,'double',0,'l');
    fclose(f);
    disp(sprintf('Done writing binary file, '));
    mapper{P} = fnameToSave;
    catch err
        disp(sprintf('There was an error processing file %s',fileList{P}));
        mapper{P} = 'error';
        continue;
    end
end
delete(parObj);

end

