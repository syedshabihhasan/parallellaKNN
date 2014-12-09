function [ knnres ] = doParKNN( flatFilePath, K, N )
%DOPARKNN Summary of this function goes here
%   @author: Syed Shabih Hasan

t = load(flatFilePath);
flatFile = t.flatFile;
clear t;
if 1 == N
    tempFlatFile = flatFile(1:250000,:);
elseif 2 == N
    tempFlatFile = flatFile(251000:500000,:);
elseif 3 == N
    tempFlatFile = flatFile(500001:750000,:);
elseif 4 == N
    tempFlatFile = flatFile(750000:end,:);
end
[r,~] = size(tempFlatFile);
parObj = parpool;
knnres = zeros(r,K);
parfor P=1:r
    disp(sprintf('LoopPos=%d',P));
    temp = tempFlatFile(P,:);
    knnres(P,:) = knnsearch(flatFile,temp,'K',K,'Distance','hamming');
end
delete(parObj);

end

