function [ nearestNeighbors ] = doKNNSearch( flatFilePath, K )
%DOKNNSEARCH Summary of this function goes here
%   @author: Syed Shabih Hasan

load(flatFilePath);
[r,~]  = size(flatFile);
nearestNeighbors = zeros(r,K);
parObj = parpool;
parfor P=1:r
    disp(sprintf('P=%d',P));
    nearestNeighbors(P,:) = knnsearch(flatFile,flatFilePath(P,:),'K',K,...
                                        'Distance','hamming');
end
disp('done!');
delete(parObj);

end

