function [ combinedMAT ] = combineMAT( matFileList )
%COMBINEMAT Summary of this function goes here
%   @author: Syed Shabih Hasan

combinedMAT = {};

for P=1:length(matFileList)
    if 0 == mod(P,10000)
        disp(P)
    end
    if strcmp(matFileList{P,2},'skipped') | strcmp(matFileList{P,2},'error')
        continue;
    end
    load(matFileList{P,2});
    combinedMAT(end+1,:) = ipArray;
end

end

