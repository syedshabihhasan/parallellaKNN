function [ combinedMAT ] = combineMAT( matFileList )
%COMBINEMAT Summary of this function goes here
%   @author: Syed Shabih Hasan

combinedMAT = {};

for P=1:length(matFileList)
    if 0 == mod(P,10000)
        disp(P)
    end
    if 0 == mod(P,100000)
        save(sprintf('comb%d',P),'combinedMAT');
        combinedMAT = {};
    end
    if strcmp(matFileList{P,2},'skipped') ||...
            strcmp(matFileList{P,2},'error')
        continue;
    else
        load(matFileList{P,2});
        combinedMAT(end+1,:) = ipArray;
    end
end

end

