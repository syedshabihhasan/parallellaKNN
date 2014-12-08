function [ knnres ] = doParKNN( flatFilePath, K )
%DOPARKNN Summary of this function goes here
%   @author: Syed Shabih Hasan

load(flatFilePath);
[r,~] = size(flatFile);
parObj = parpool;
knnres = zeros(r,K);
parfor P=1:r
    knnres(P,:) = knnsearch(flatFile,flatFile(P,:),'K',K,'Distance','hamming');
end
delete(parObj);

end

