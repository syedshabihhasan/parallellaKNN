function [ combinedMAT ] = combineMAT( matFileList )
%COMBINEMAT Summary of this function goes here
%   Detailed explanation goes here

combinedMAT = {};

for P=1:length(matFileList)
    if strcmp(matFileList{P,2},'skipped') | strcmp(matFileList{P,2},'error')
        continue;
    end
    load(matFileList{P,2});
    combinedMAT(end+1,:) = ipArray;
end

end

