%% add the path containing the files
addpath ../;

%% load the file list
load('../dataVariables/fileList.mat');

%% start parallel pool
parObj = parpool;
n = length(fileList);
parfor P=1:n
    hdfFname = fileList{P};
    actualFname = strsplit(hdfFname,'/');
    actualFname = actualFname{end};
    actualFname = strsplit(actualFname,'.');
    actualFname = actualFname{1};
    toSaveMAT = sprintf('../dataVariables/matFiles/%s.mat',actualFname);
    hdf5_to_matfile(hdfFname,toSaveMAT);
end
delete(parObj);
