function [ nearestNeighbors ] = doKNNSearch( flatFilePath, K )
%DOKNNSEARCH Summary of this function goes here
%   @author: Syed Shabih Hasan

load(flatFilePath);
nearestNeighbors = knnsearch(flatFile,flatFile,'K',K,...
                                        'Distance','hamming');

end

